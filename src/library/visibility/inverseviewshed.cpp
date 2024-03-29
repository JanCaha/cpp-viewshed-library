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