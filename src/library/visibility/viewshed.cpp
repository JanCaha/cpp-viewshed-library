#include "chrono"
#include <cmath>

#include "los.h"
#include "threadtasks.h"
#include "viewshed.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::Point;
using viewshed::Viewshed;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<Point> viewPoint, std::shared_ptr<QgsRasterLayer> dem,
                    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                    bool applyCurvatureCorrections, double earthDiameter, double refractionCoeff, double minimalAngle,
                    double maximalAngle )
{
    mValid = false;

    mPoint = viewPoint;
    mInputDem = dem;
    mAlgs = algs;

    mCurvatureCorrections = applyCurvatureCorrections;
    mEarthDiameter = earthDiameter;
    mRefractionCoefficient = refractionCoeff;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
}

std::shared_ptr<LoS> Viewshed::getLoS( QgsPoint point, bool onlyToPoi )
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

    std::shared_ptr<LoS> los = std::make_shared<LoS>( losNodes );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );
    los->prepareForCalculation();

    return los;
}

void Viewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
                          std::function<void( int size, int current )> progressCallback )
{
    using namespace std::chrono::_V2;
    using namespace std::chrono;

    initEventList();

    stepsTimingCallback( "Init event list lasted: ", initLastedSeconds() );

    sortEventList();

    stepsTimingCallback( "Sort event list lasted: ", sortLastedSeconds() );

    parseEventList( progressCallback );

    stepsTimingCallback( "Parsing of event list lasted: ", parseLastedSeconds() );
}

void Viewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );

    auto startTime = std::chrono::high_resolution_clock::now();
    std::shared_ptr<LoS> los = std::make_shared<LoS>( mLosNodes );
    auto endTime = std::chrono::high_resolution_clock::now();
    los->timeToCopy = std::chrono::duration_cast<std::chrono::nanoseconds>( endTime - startTime );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );

    mResultPixels.push_back( mThreadPool.submit( viewshed::evaluateLoSForPoI, los, mAlgs ) );
}

void Viewshed::addEventsFromCell( int &row, int &column, const double &pixelValue,
                                  std::unique_ptr<QgsRasterBlock> &rasterBlock, bool &solveCell )
{
    mCellElevs[CellEventPositionType::CENTER] = pixelValue;
    CellEventPosition tempPosEnter = Visibility::eventPosition( CellEventPositionType::ENTER, row, column, mPoint );
    mCellElevs[CellEventPositionType::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
    CellEventPosition tempPosExit = Visibility::eventPosition( CellEventPositionType::EXIT, row, column, mPoint );
    mCellElevs[CellEventPositionType::EXIT] = getCornerValue( tempPosExit, rasterBlock, pixelValue );

    mAngleCenter = Visibility::angle( row, column, mPoint );
    mAngleEnter = Visibility::angle( &tempPosEnter, mPoint );
    mAngleExit = Visibility::angle( &tempPosExit, mPoint );

    mEventDistance = Visibility::distance( row, column, mPoint, mCellSize );

    if ( mEventDistance < mMaxDistance && isInsideAngles( mAngleEnter, mAngleExit ) )
    {
        mEventCenter =
            CellEvent( CellEventPositionType::CENTER, row, column, mEventDistance, mAngleCenter, mCellElevs );
        mEventEnter = CellEvent( CellEventPositionType::ENTER, row, column,
                                 Visibility::distance( &tempPosEnter, mPoint, mCellSize ), mAngleEnter, mCellElevs );
        mEventExit = CellEvent( CellEventPositionType::EXIT, row, column,
                                Visibility::distance( &tempPosExit, mPoint, mCellSize ), mAngleExit, mCellElevs );

        // Target or ViewPoint are not part CellEvents - handled separately
        if ( mPoint->row == row && mPoint->col == column )
        {
            mLosNodePoint = LoSNode( mPoint, &mEventCenter, mCellSize );
            return;
        }

        // LosNode prefill
        if ( mPoint->row == row && mPoint->col < column )
        {
            mLoSNodeTemp = LoSNode( mPoint, &mEventEnter, mCellSize );
            mLosNodes.push_back( mLoSNodeTemp );
        }

        mCellEvents.push_back( mEventEnter );
        mCellEvents.push_back( mEventExit );
        if ( solveCell )
        {
            mCellEvents.push_back( mEventEnter );
        }
    }
}