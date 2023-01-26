#include <cmath>

#include <QElapsedTimer>

#include "inverseviewshed.h"
#include "los.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::InverseLoS;
using viewshed::InverseViewshed;
using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::Point;
using viewshed::ViewshedValues;

InverseViewshed::InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset,
                                  std::shared_ptr<QgsRasterLayer> dem,
                                  std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                                  double minimalAngle, double maximalAngle )
{
    mValid = false;

    mPoint = targetPoint;
    mInputDem = dem;
    mAlgs = algs;

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

    if ( onlyToPoi )
    {
        double poiDistance = poi->centreDistance();

        losNodes.erase( std::remove_if( losNodes.begin(), losNodes.end(),
                                        [&poiDistance]( LoSNode &node )
                                        { return poiDistance <= node.centreDistance(); } ),
                        losNodes.end() );
    }

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( losNodes );
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );
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
                    int column = blockColumn + iterLeft;
                    int row = blockRow + iterTop;

                    double elevs[3];
                    elevs[CellEventPositionType::CENTER] = pixelValue;
                    CellEventPosition tempPosEnter =
                        Visibility::calculateEventPosition( CellEventPositionType::ENTER, row, column, mPoint );
                    elevs[CellEventPositionType::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
                    CellEventPosition tempPosExit =
                        Visibility::calculateEventPosition( CellEventPositionType::EXIT, row, column, mPoint );
                    elevs[CellEventPositionType::EXIT] = getCornerValue( tempPosExit, rasterBlock, pixelValue );

                    double angleCenter = Visibility::calculateAngle( row, column, mPoint );
                    double angleEnter = Visibility::calculateAngle( &tempPosEnter, mPoint );
                    double angleExit = Visibility::calculateAngle( &tempPosExit, mPoint );

                    if ( angleEnter > angleCenter )
                    {
                        angleEnter -= 2 * M_PI;
                    }

                    double eventDistance = Visibility::calculateDistance( row, column, mPoint, mCellSize );

                    if ( eventDistance < mMaxDistance && isInsideAngles( angleEnter, angleExit ) )
                    {
                        CellEvent eCenter =
                            CellEvent( CellEventPositionType::CENTER, row, column, eventDistance, angleCenter, elevs );
                        CellEvent eEnter = CellEvent( CellEventPositionType::ENTER, row, column,
                                                      Visibility::calculateDistance( &tempPosEnter, mPoint, mCellSize ),
                                                      angleEnter, elevs );
                        CellEvent eExit = CellEvent( CellEventPositionType::EXIT, row, column,
                                                     Visibility::calculateDistance( &tempPosExit, mPoint, mCellSize ),
                                                     angleExit, elevs );

                        double addValue = M_PI;

                        if ( angleCenter >= M_PI )
                        {
                            addValue = -M_PI;
                        }

                        CellEvent eEnterOpposite =
                            CellEvent( CellEventPositionType::ENTER, row, column,
                                       -Visibility::calculateDistance( &tempPosEnter, mPoint, mCellSize ),
                                       addValue + angleEnter, elevs );
                        CellEvent eExitOpposite =
                            CellEvent( CellEventPositionType::EXIT, row, column,
                                       -Visibility::calculateDistance( &tempPosExit, mPoint, mCellSize ),
                                       addValue + angleExit, elevs );

                        // // TODO is this it?
                        // if ( eventEnter1.distance < 0 && eventEnter1.angle > 2 * Math.PI )
                        // {
                        //     eventEnter1.angle = eventEnter1.angle - 2 * Math.PI;
                        // }

                        // Target or ViewPoint are not part CellEvents - handled separately
                        if ( mPoint->row == row && mPoint->col == column )
                        {
                            mLosNodePoint = LoSNode( mPoint, &eCenter, mCellSize );
                            continue;
                        }

                        mCellEvents.push_back( eEnterOpposite );
                        mCellEvents.push_back( eExitOpposite );

                        mCellEvents.push_back( eEnter );
                        mCellEvents.push_back( eCenter );
                        mCellEvents.push_back( eExit );

                        if ( mPoint->row == row && mPoint->col < column )
                        {
                            CellEvent eEnter2 =
                                CellEvent( CellEventPositionType::ENTER, row, column,
                                           Visibility::calculateDistance( &tempPosEnter, mPoint, mCellSize ),
                                           angleEnter + ( 2 * M_PI ), elevs );
                            mCellEvents.push_back( eEnter2 );
                        }
                    }
                }
            }
        }
    }
}