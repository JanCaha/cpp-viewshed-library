#include <cmath>

#include <QElapsedTimer>

#include "abstractviewshedalgorithm.h"
#include "inverselos.h"
#include "inverseviewshed.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::InverseLoS;
using viewshed::InverseViewshed;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::Point;
using viewshed::ViewshedValues;

InverseViewshed::InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset,
                                  std::shared_ptr<QgsRasterLayer> dem,
                                  std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                                  bool applyCurvatureCorrections, double earthDiameter, double refractionCoeff,
                                  double minimalAngle, double maximalAngle )
{
    mValid = false;

    mPoint = targetPoint;
    mInputDem = dem;
    mAlgs = algs;

    mCurvatureCorrections = applyCurvatureCorrections;
    mEarthDiameter = earthDiameter;
    mRefractionCoefficient = refractionCoeff;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mObserverOffset = observerOffset;

    mValid = true;
}

std::shared_ptr<InverseLoS> InverseViewshed::getLoS( QgsPoint point, bool onlyToPoi )
{

    std::vector<LoSNode> losNodes = prepareLoSWithPoint( point );

    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( statusNodeFromPoint( point ) );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( losNodes );
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );

    los->setRemovePointsAfterTarget( onlyToPoi );

    los->prepareForCalculation();

    return los;
}

void InverseViewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
                                 std::function<void( int size, int current )> progressCallback )
{
    QElapsedTimer timer;

    timer.start();

    initEventList();

    stepsTimingCallback( "Init event list lasted: ", timer.elapsed() / 1000.0 );

    timer.restart();

    sortEventList();

    stepsTimingCallback( "Sort event list lasted: ", timer.elapsed() / 1000.0 );

    timer.restart();

    parseEventList( progressCallback );

    stepsTimingCallback( "Parsing of event list lasted: ", timer.elapsed() / 1000.0 );
}

void InverseViewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( mLosNodes );
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );

    mResultPixels.push_back( mThreadPool.submit( viewshed::evaluateLoSForPoI, los, mAlgs ) );
}

void InverseViewshed::initEventList()
{

    std::unique_ptr<QgsRasterInterface> rInterface;
    rInterface.reset( mInputDem->dataProvider()->clone() );

    QgsRasterIterator iter( rInterface.get() );
    iter.setMaximumTileHeight( rInterface->ySize() + 1 );
    iter.setMaximumTileWidth( rInterface->xSize() + 1 );

    iter.startRasterRead( mDefaultBand, mInputDem->width(), mInputDem->height(), mInputDem->extent() );
    int iterLeft = 0;
    int iterTop = 0;
    int iterCols = 0;
    int iterRows = 0;
    bool isNoData = false;
    std::unique_ptr<QgsRasterBlock> rasterBlock; // = std::make_shared<QgsRasterBlock>();

    while ( iter.readNextRasterPart( mDefaultBand, iterCols, iterRows, rasterBlock, iterLeft, iterTop ) )
    {
        for ( int blockRow = 0; blockRow < iterRows; blockRow++ )
        {
            for ( int blockColumn = 0; blockColumn < iterCols; blockColumn++ )
            {
                const double pixelValue = rasterBlock->valueAndNoData( blockRow, blockColumn, isNoData );

                if ( !isNoData )
                {
                    mValidCells++;

                    int column = blockColumn + iterLeft;
                    int row = blockRow + iterTop;

                    double elevs[3];
                    elevs[CellEventPositionType::CENTER] = pixelValue;
                    CellEventPosition tempPosEnter =
                        Visibility::eventPosition( CellEventPositionType::ENTER, row, column, mPoint );
                    elevs[CellEventPositionType::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
                    CellEventPosition tempPosExit =
                        Visibility::eventPosition( CellEventPositionType::EXIT, row, column, mPoint );
                    elevs[CellEventPositionType::EXIT] = getCornerValue( tempPosExit, rasterBlock, pixelValue );

                    double angleCenter = Visibility::angle( row, column, mPoint );
                    double angleEnter = Visibility::angle( &tempPosEnter, mPoint );
                    double angleExit = Visibility::angle( &tempPosExit, mPoint );

                    double eventDistance = Visibility::distance( row, column, mPoint, mCellSize );

                    if ( eventDistance < mMaxDistance && isInsideAngles( angleEnter, angleExit ) )
                    {
                        CellEvent eCenter =
                            CellEvent( CellEventPositionType::CENTER, row, column, eventDistance, angleCenter, elevs );
                        CellEvent eEnter =
                            CellEvent( CellEventPositionType::ENTER, row, column,
                                       Visibility::distance( &tempPosEnter, mPoint, mCellSize ), angleEnter, elevs );
                        CellEvent eExit =
                            CellEvent( CellEventPositionType::EXIT, row, column,
                                       Visibility::distance( &tempPosExit, mPoint, mCellSize ), angleExit, elevs );

                        double oppositeAngleEnter = angleEnter;
                        double oppositeAngleExit = angleExit;

                        if ( oppositeAngleEnter < M_PI )
                        {
                            oppositeAngleEnter = M_PI + oppositeAngleEnter;
                        }
                        else
                        {
                            oppositeAngleEnter = oppositeAngleEnter - M_PI;
                        }

                        if ( oppositeAngleExit < M_PI )
                        {
                            oppositeAngleExit = M_PI + oppositeAngleExit;
                        }
                        else
                        {
                            oppositeAngleExit = oppositeAngleExit - M_PI;
                        }

                        CellEvent eEnterOpposite = CellEvent( CellEventPositionType::ENTER, row, column,
                                                              Visibility::distance( &tempPosEnter, mPoint, mCellSize ),
                                                              oppositeAngleEnter, elevs );
                        eEnterOpposite.behindTargetForInverseLoS = true;

                        CellEvent eExitOpposite = CellEvent( CellEventPositionType::EXIT, row, column,
                                                             Visibility::distance( &tempPosExit, mPoint, mCellSize ),
                                                             oppositeAngleExit, elevs );
                        eExitOpposite.behindTargetForInverseLoS = true;

                        // Target or ViewPoint are not part CellEvents - handled separately
                        if ( mPoint->row == row && mPoint->col == column )
                        {
                            mLosNodePoint = LoSNode( mPoint, &eCenter, mCellSize );
                            continue;
                        }

                        if ( mPoint->row == row )
                        {
                            LoSNode ln;

                            if ( mPoint->col < column )
                            {
                                ln = LoSNode( mPoint, &eEnter, mCellSize );
                                mLosNodes.push_back( ln );
                            }
                            else
                            {
                                ln = LoSNode( mPoint, &eEnterOpposite, mCellSize );
                                mLosNodes.push_back( ln );
                            }
                        }

                        mCellEvents.push_back( eEnterOpposite );
                        mCellEvents.push_back( eExitOpposite );

                        mCellEvents.push_back( eEnter );
                        mCellEvents.push_back( eCenter );
                        mCellEvents.push_back( eExit );
                    }
                }
            }
        }
    }
}