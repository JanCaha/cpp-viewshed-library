#include <cmath>
#include <numbers>

#include "abstractlos.h"
#include "cellevent.h"
#include "inverselos.h"
#include "los.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::InverseLoS;
using viewshed::LoSNode;
using viewshed::Visibility;

InverseLoS::InverseLoS() {}

InverseLoS::InverseLoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

void InverseLoS::setTargetPoint( std::shared_ptr<Point> tp, double targetOffset )
{
    mTargetIndex = -1;
    mTp = std::make_shared<Point>( tp->mRow, tp->mCol, tp->mElevation, targetOffset, tp->mCellSize );
}

void InverseLoS::setUpTargetLoSNode()
{
    LoSNode ln = LoSNode( mTp->mRow, mTp->mCol );
    double angle = Visibility::angle( mVp->mRow, mVp->mCol, mTp->mRow, mTp->mCol );
    ln.mAngle[CellEventPositionType::ENTER] = angle;
    ln.mAngle[CellEventPositionType::CENTER] = angle;
    ln.mAngle[CellEventPositionType::EXIT] = angle;
    ln.mElevs[CellEventPositionType::ENTER] = mTp->mElevation;
    ln.mElevs[CellEventPositionType::CENTER] = mTp->mElevation;
    ln.mElevs[CellEventPositionType::EXIT] = mTp->mElevation;
    ln.mDistances[CellEventPositionType::ENTER] = 0;
    ln.mDistances[CellEventPositionType::CENTER] = 0;
    ln.mDistances[CellEventPositionType::EXIT] = 0;
    push_back( ln );
}

void InverseLoS::setViewPoint( std::shared_ptr<LoSNode> vp, double observerOffset )
{
    mTargetIndex = -1;
    mVp = std::make_shared<Point>( vp->mRow, vp->mCol, vp->centreElevation(), observerOffset, 0 );
    mPointDistance = mTp->distance( mVp );
    mAngleHorizontal = vp->centreAngle();
}

void InverseLoS::setLoSNodes( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

void InverseLoS::setLoSNodes( LoS los ) { assign( los.begin(), los.end() ); }

int InverseLoS::numberOfNodes() { return size(); };

int InverseLoS::targetPointIndex() { return mTargetIndex; }

void InverseLoS::sort() { std::sort( begin(), end() ); }

void InverseLoS::prepareForCalculation()
{
    removePointsAfterViewPoint();
    setUpTargetLoSNode();
    fixDistancesAngles();
    sort();

    if ( mRemovePointsAfterTarget )
    {
        removePointsAfterTarget();
    }

    findTargetPointIndex();
};

void InverseLoS::removePointsAfterViewPoint()
{
    double dist = mPointDistance;
    erase( std::remove_if( begin(), end(), [&dist]( LoSNode &node )
                           { return ( dist <= node.centreDistance() && !node.mInverseLoSBehindTarget ); } ),
           end() );
}

LoSNode InverseLoS::nodeAt( std::size_t i ) { return this->operator[]( i ); }

int InverseLoS::resultRow() { return mVp->mRow; }

int InverseLoS::resultCol() { return mVp->mCol; }

// TODO fix !!!!!
bool InverseLoS::isValid() { return true; }

void InverseLoS::fixDistancesAngles()
{
    for ( std::size_t i = 0; i < size(); i++ )
    {
        if ( this->operator[]( i ).mInverseLoSBehindTarget )
        {
            this->operator[]( i ).mDistances[CellEventPositionType::ENTER] += mPointDistance;
            this->operator[]( i ).mDistances[CellEventPositionType::CENTER] += mPointDistance;
            this->operator[]( i ).mDistances[CellEventPositionType::EXIT] += mPointDistance;

            double addValue = -std::numbers::pi;

            if ( this->operator[]( i ).mAngle[CellEventPositionType::CENTER] < std::numbers::pi ||
                 this->operator[]( i ).mAngle[CellEventPositionType::CENTER] == 0 )
            {
                addValue = +std::numbers::pi;
            }

            this->operator[]( i ).mAngle[CellEventPositionType::ENTER] += addValue;
            this->operator[]( i ).mAngle[CellEventPositionType::CENTER] += addValue;
            this->operator[]( i ).mAngle[CellEventPositionType::EXIT] += addValue;
        }
        else
        {
            this->operator[]( i ).mDistances[CellEventPositionType::ENTER] =
                mPointDistance - this->operator[]( i ).mDistances[CellEventPositionType::ENTER];
            this->operator[]( i ).mDistances[CellEventPositionType::CENTER] =
                mPointDistance - this->operator[]( i ).mDistances[CellEventPositionType::CENTER];
            this->operator[]( i ).mDistances[CellEventPositionType::EXIT] =
                mPointDistance - this->operator[]( i ).mDistances[CellEventPositionType::EXIT];
        }
    }
}

void InverseLoS::findTargetPointIndex()
{
    for ( std::size_t i = 0; i < numberOfNodes(); i++ )
    {
        if ( i + 1 < numberOfNodes() )
        {
            if ( this->operator[]( i ).centreDistance() == targetDistance() )
            {
                mTargetIndex = i;
                break;
            }
        }
    }

    if ( mTargetIndex == -1 )
    {
        mTargetIndex = numberOfNodes() - 1;
    }
}

void InverseLoS::setRemovePointsAfterTarget( bool remove ) { mRemovePointsAfterTarget = remove; }

void InverseLoS::removePointsAfterTarget()
{

    erase(
        std::remove_if( begin(), end(), [this]( LoSNode &node ) { return mPointDistance <= node.centreDistance(); } ),
        end() );
}