#include <chrono>
#include <cmath>
#include <numbers>

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
using viewshed::Point;
using viewshed::ViewshedValues;

InverseViewshed::InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset,
                                  std::shared_ptr<ProjectedSquareCellRaster> dem,
                                  std::shared_ptr<ViewshedAlgorithms> visibilityIndices, bool applyCurvatureCorrections,
                                  double earthDiameter, double refractionCoeff, double minimalAngle,
                                  double maximalAngle )
{
    mValid = false;

    mInverseViewshed = true;

    mPoint = targetPoint;
    mInputDsm = dem;
    mVisibilityIndices = visibilityIndices;

    mCurvatureCorrections = applyCurvatureCorrections;
    mEarthDiameter = earthDiameter;
    mRefractionCoefficient = refractionCoeff;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDsm->xCellSize();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mObserverOffset = observerOffset;

    mValid = true;
}

std::shared_ptr<InverseLoS> InverseViewshed::getLoS( OGRPoint point, bool onlyToPoi )
{

    std::vector<LoSNode> losNodes = prepareLoSWithPoint( point );

    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( statusNodeFromPoint( point ) );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( losNodes );
    los->setTargetPoint( mPoint, mPoint->mOffset );
    los->setViewPoint( poi, mObserverOffset );
    los->setRemovePointsAfterTarget( onlyToPoi );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );
    los->prepareForCalculation();

    return los;
}

void InverseViewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
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

void InverseViewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint->mRow, mPoint->mCol, &e, mCellSize );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( mLosNodes );
    los->setTargetPoint( mPoint, mPoint->mOffset );
    los->setViewPoint( poi, mObserverOffset );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );

    mThreadPool.detach_task( std::bind( viewshed::evaluateLoS, los, mVisibilityIndices, mResults ) );
}

void InverseViewshed::addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell )
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

        mOppositeAngleEnter = mAngleEnter;
        mOppositeAngleExit = mAngleExit;

        if ( mOppositeAngleEnter < std::numbers::pi )
        {
            mOppositeAngleEnter = std::numbers::pi + mOppositeAngleEnter;
        }
        else
        {
            mOppositeAngleEnter = mOppositeAngleEnter - std::numbers::pi;
        }

        if ( mOppositeAngleExit < std::numbers::pi )
        {
            mOppositeAngleExit = std::numbers::pi + mOppositeAngleExit;
        }
        else
        {
            mOppositeAngleExit = mOppositeAngleExit - std::numbers::pi;
        }

        mEventEnterOpposite =
            CellEvent( CellEventPositionType::ENTER, row, column,
                       Visibility::distance( tempPosEnter.mRow, tempPosEnter.mCol, static_cast<double>( mPoint->mRow ),
                                             static_cast<double>( mPoint->mCol ), mCellSize ),
                       mOppositeAngleEnter, mCellElevs );
        mEventEnterOpposite.mBehindTargetForInverseLoS = true;

        mEventExitOpposite =
            CellEvent( CellEventPositionType::EXIT, row, column,
                       Visibility::distance( tempPosExit.mRow, tempPosExit.mCol, static_cast<double>( mPoint->mRow ),
                                             static_cast<double>( mPoint->mCol ), mCellSize ),
                       mOppositeAngleExit, mCellElevs );
        mEventExitOpposite.mBehindTargetForInverseLoS = true;

        // Target or ViewPoint are not part CellEvents - handled separately
        if ( mPoint->mRow == row && mPoint->mCol == column )
        {
            mLosNodePoint = LoSNode( mPoint->mRow, mPoint->mCol, &mEventCenter, mCellSize );
            return;
        }

        // LosNode prefill
        if ( mPoint->mRow == row )
        {
            if ( mPoint->mCol < column )
            {
                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &mEventCenter, mCellSize );
                mLosNodes.push_back( mLoSNodeTemp );
            }
            else
            {
                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &mEventEnterOpposite, mCellSize );
                mLosNodes.push_back( mLoSNodeTemp );
            }
        }

        mCellEvents.push_back( mEventEnterOpposite );
        mCellEvents.push_back( mEventExitOpposite );

        mCellEvents.push_back( mEventEnter );
        mCellEvents.push_back( mEventExit );
        if ( solveCell )
        {
            mCellEvents.push_back( mEventCenter );
        }
    }
}

void InverseViewshed::calculateVisibilityRaster()
{

    mVisibilityRaster = std::make_shared<ProjectedSquareCellRaster>( *mInputDsm.get(), GDALDataType::GDT_Float32 );

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    std::map<double, LoSNode> los;

    for ( LoSNode ln : mLosNodes )
    {
        if ( ln.mInverseLoSBehindTarget )
        {
            continue;
        }

        los.insert( std::pair<double, LoSNode>( ln.centreDistance(), ln ) );
    }

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
                if ( e.mBehindTargetForInverseLoS )
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
                if ( e.mBehindTargetForInverseLoS )
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
                double curCorr, gradient, elevation, horizontalAngle;

                double visibilityElevation = mLoSNodeTemp.centreElevation() + mObserverOffset;

                curCorr = Visibility::curvatureCorrections( mLoSNodeTemp.centreDistance(), mRefractionCoefficient,
                                                            mEarthDiameter );
                elevation = visibilityElevation + curCorr;

                gradient = Visibility::gradient( mPoint->totalElevation() - elevation, mLoSNodeTemp.centreDistance() );

                double visibilityGradient = gradient;
                double visibilityDistance = mLoSNodeTemp.centreDistance();

                for ( auto it = los.begin(); it != los.lower_bound( mLoSNodeTemp.centreDistance() ); ++it )
                {
                    horizontalAngle = mLoSNodeTemp.centreAngle(); // + std::numbers::pi;

                    curCorr = Visibility::curvatureCorrections( visibilityDistance -
                                                                    it->second.distanceAtAngle( horizontalAngle ),
                                                                mRefractionCoefficient, mEarthDiameter );

                    elevation = it->second.elevationAtAngle( horizontalAngle ) + curCorr;

                    gradient =
                        Visibility::gradient( elevation - visibilityElevation,
                                              visibilityDistance - it->second.distanceAtAngle( horizontalAngle ) );

                    if ( gradient > maxGradient )
                    {
                        maxGradient = gradient;
                    }
                }

                double visible = 0;
                if ( maxGradient < visibilityGradient )
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