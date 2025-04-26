#include <chrono>
#include <cmath>

#include "los.h"
#include "threadtasks.h"
#include "viewshed.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::Point;
using viewshed::Viewshed;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<Point> viewPoint, std::shared_ptr<ProjectedSquareCellRaster> dem,
                    std::shared_ptr<ViewshedAlgorithms> visibilityIndices, bool applyCurvatureCorrections,
                    double earthDiameter, double refractionCoeff, double minimalAngle, double maximalAngle )
{
    mValid = false;

    mPoint = viewPoint;
    mInputDsm = dem;
    mVisibilityIndices = visibilityIndices;

    mCurvatureCorrections = applyCurvatureCorrections;
    mEarthDiameter = earthDiameter;
    mRefractionCoefficient = refractionCoeff;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDsm->xCellSize();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
}

std::shared_ptr<LoS> Viewshed::getLoS( OGRPoint point, bool onlyToPoi )
{

    std::vector<LoSNode> losNodes = prepareLoSWithPoint( point );

    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( statusNodeFromPoint( point ) );

    if ( onlyToPoi )
    {
        double poiDistance = poi->centreDistance();

        losNodes.erase( std::remove_if( losNodes.begin(), losNodes.end(), [&poiDistance]( LoSNode &node )
                                        { return poiDistance <= node.centreDistance(); } ),
                        losNodes.end() );
    }

    std::shared_ptr<LoS> los = std::make_shared<LoS>( losNodes );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );
    los->prepareForCalculation();

    return los;
}

void Viewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
                          std::function<void( int size, int current )> progressCallback )
{
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
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint->mRow, mPoint->mCol, &e, mCellSize );

    std::shared_ptr<LoS> los = std::make_shared<LoS>( mLosNodes );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );

    mThreadPool.detach_task( std::bind( viewshed::evaluateLoS, los, mVisibilityIndices, mResults ) );
}

void Viewshed::addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell )
{
    mCellElevs[CellEventPositionType::CENTER] = pixelValue;
    CellEventPosition tempPosEnter =
        Visibility::eventPosition( CellEventPositionType::ENTER, row, column, mPoint->mRow, mPoint->mCol );
    mCellElevs[CellEventPositionType::ENTER] = mInputDsm->cornerValue( tempPosEnter.mRow, tempPosEnter.mCol );
    CellEventPosition tempPosExit =
        Visibility::eventPosition( CellEventPositionType::EXIT, row, column, mPoint->mRow, mPoint->mCol );
    mCellElevs[CellEventPositionType::EXIT] = mInputDsm->cornerValue( tempPosExit.mRow, tempPosExit.mCol );

    mAngleCenter = Visibility::angle( row, column, mPoint->mRow, mPoint->mCol );
    mAngleEnter = Visibility::angle( tempPosEnter.mRow, tempPosEnter.mCol, mPoint->mRow, mPoint->mCol );
    mAngleExit = Visibility::angle( tempPosExit.mRow, tempPosExit.mCol, mPoint->mRow, mPoint->mCol );

    mEventDistance = Visibility::distance( row, column, static_cast<double>( mPoint->mRow ),
                                           static_cast<double>( mPoint->mCol ), mCellSize );

    if ( mEventDistance < mMaxDistance && isInsideAngles( mAngleEnter, mAngleExit ) )
    {
        mEventCenter =
            CellEvent( CellEventPositionType::CENTER, row, column, mEventDistance, mAngleCenter, mCellElevs );
        mEventEnter =
            CellEvent( CellEventPositionType::ENTER, row, column,
                       Visibility::distance( tempPosEnter.mRow, tempPosEnter.mCol, static_cast<double>( mPoint->mRow ),
                                             static_cast<double>( mPoint->mCol ), mCellSize ),
                       mAngleEnter, mCellElevs );
        mEventExit =
            CellEvent( CellEventPositionType::EXIT, row, column,
                       Visibility::distance( tempPosExit.mRow, tempPosExit.mCol, static_cast<double>( mPoint->mRow ),
                                             static_cast<double>( mPoint->mCol ), mCellSize ),
                       mAngleExit, mCellElevs );

        // Target or ViewPoint are not part CellEvents - handled separately
        if ( mPoint->mRow == row && mPoint->mCol == column )
        {
            mLosNodePoint = LoSNode( mPoint->mRow, mPoint->mCol, &mEventCenter, mCellSize );
            return;
        }

        // LosNode prefill
        if ( mPoint->mRow == row && mPoint->mCol < column )
        {
            mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &mEventEnter, mCellSize );
            mLosNodes.push_back( mLoSNodeTemp );
        }

        mCellEvents.push_back( mEventEnter );
        mCellEvents.push_back( mEventExit );
        if ( solveCell )
        {
            mCellEvents.push_back( mEventCenter );
        }
    }
}

void Viewshed::calculateVisibilityRaster()
{

    mVisibilityRaster = std::make_shared<ProjectedSquareCellRaster>( *mInputDsm.get(), GDALDataType::GDT_Int32, false );
    double noDataValue = -999;
    mVisibilityRaster->setNoData( noDataValue );
    mVisibilityRaster->prefillValues( noDataValue );

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    std::map<double, LoSNode> los;

    for ( LoSNode ln : mLosNodes )
    {
        los.insert( std::pair<double, LoSNode>( ln.centreDistance(), ln ) );
    }

    // observer point is always visible
    mVisibilityRaster->writeValue( mPoint->mRow, mPoint->mCol, 1 );

    std::size_t i = 0;
    for ( CellEvent e : mCellEvents )
    {
        // progressCallback( mCellEvents.size(), i );

        switch ( e.mEventType )
        {
            case CellEventPositionType::ENTER:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                los.insert( std::pair<double, LoSNode>( mLoSNodeTemp.centreDistance(), mLoSNodeTemp ) );
                break;
            }
            case CellEventPositionType::EXIT:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                los.erase( mLoSNodeTemp.centreDistance() );
                break;
            }
            case CellEventPositionType::CENTER:
            {
                double maxGradient = -99;
                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                double curCorr, gradient, elevation;

                curCorr = Visibility::curvatureCorrections( mLoSNodeTemp.centreDistance(), mRefractionCoefficient,
                                                            mEarthDiameter );
                elevation = mLoSNodeTemp.centreElevation() + curCorr;

                gradient = Visibility::gradient( elevation - mPoint->totalElevation(), mLoSNodeTemp.centreDistance() );

                double nodeGradient = gradient;

                for ( auto it = los.begin(); it != los.lower_bound( mLoSNodeTemp.centreDistance() ); ++it )
                {
                    curCorr =
                        Visibility::curvatureCorrections( it->second.distanceAtAngle( mLoSNodeTemp.centreAngle() ),
                                                          mRefractionCoefficient, mEarthDiameter );
                    elevation = it->second.elevationAtAngle( mLoSNodeTemp.centreAngle() ) + curCorr;

                    gradient = Visibility::gradient( elevation - mPoint->totalElevation(),
                                                     it->second.distanceAtAngle( mLoSNodeTemp.centreAngle() ) );

                    if ( gradient > maxGradient )
                    {
                        maxGradient = gradient;
                    }
                }

                double visible = 0;
                if ( maxGradient < nodeGradient )
                {
                    visible = 1;
                };
                mVisibilityRaster->writeValue( e.mRow, e.mCol, visible );

                break;
            }
        }

        i++;
    }

    mTimeParse = std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::steady_clock::now() - startTime );
}