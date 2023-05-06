#include "chrono"
#include <cmath>

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

InverseViewshed::InverseViewshed(
    std::shared_ptr<Point> targetPoint, double observerOffset, std::shared_ptr<ProjectedSquareCellRaster> dem,
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> visibilityIndices,
    bool applyCurvatureCorrections, double earthDiameter, double refractionCoeff, double minimalAngle,
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
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );
    los->setRemovePointsAfterTarget( onlyToPoi );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );
    los->prepareForCalculation();

    return los;
}

void InverseViewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
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

void InverseViewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( mLosNodes );
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );
    los->applyCurvatureCorrections( mCurvatureCorrections, mRefractionCoefficient, mEarthDiameter );

    mThreadPool.push_task( viewshed::evaluateLoS, los, mVisibilityIndices, mResults );
}

void InverseViewshed::addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell )
{
    mCellElevs[CellEventPositionType::CENTER] = pixelValue;
    CellEventPosition tempPosEnter = Visibility::eventPosition( CellEventPositionType::ENTER, row, column, mPoint );
    mCellElevs[CellEventPositionType::ENTER] = mInputDsm->cornerValue( tempPosEnter.row, tempPosEnter.col );
    CellEventPosition tempPosExit = Visibility::eventPosition( CellEventPositionType::EXIT, row, column, mPoint );
    mCellElevs[CellEventPositionType::EXIT] = mInputDsm->cornerValue( tempPosExit.row, tempPosExit.col );

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

        mOppositeAngleEnter = mAngleEnter;
        mOppositeAngleExit = mAngleExit;

        if ( mOppositeAngleEnter < M_PI )
        {
            mOppositeAngleEnter = M_PI + mOppositeAngleEnter;
        }
        else
        {
            mOppositeAngleEnter = mOppositeAngleEnter - M_PI;
        }

        if ( mOppositeAngleExit < M_PI )
        {
            mOppositeAngleExit = M_PI + mOppositeAngleExit;
        }
        else
        {
            mOppositeAngleExit = mOppositeAngleExit - M_PI;
        }

        mEventEnterOpposite =
            CellEvent( CellEventPositionType::ENTER, row, column,
                       Visibility::distance( &tempPosEnter, mPoint, mCellSize ), mOppositeAngleEnter, mCellElevs );
        mEventEnterOpposite.behindTargetForInverseLoS = true;

        mEventExitOpposite =
            CellEvent( CellEventPositionType::EXIT, row, column,
                       Visibility::distance( &tempPosExit, mPoint, mCellSize ), mOppositeAngleExit, mCellElevs );
        mEventExitOpposite.behindTargetForInverseLoS = true;

        // Target or ViewPoint are not part CellEvents - handled separately
        if ( mPoint->row == row && mPoint->col == column )
        {
            mLosNodePoint = LoSNode( mPoint, &mEventCenter, mCellSize );
            return;
        }

        // LosNode prefill
        if ( mPoint->row == row )
        {
            if ( mPoint->col < column )
            {
                mLoSNodeTemp = LoSNode( mPoint, &mEventCenter, mCellSize );
                mLosNodes.push_back( mLoSNodeTemp );
            }
            else
            {
                mLoSNodeTemp = LoSNode( mPoint, &mEventEnterOpposite, mCellSize );
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