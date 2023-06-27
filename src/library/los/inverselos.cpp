#include <cmath>

#include "abstractlos.h"
#include "inverselos.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::InverseLoS;
using viewshed::Visibility;

InverseLoS::InverseLoS() {}

InverseLoS::InverseLoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

double InverseLoS::distance( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double InverseLoS::gradient( int i ) { return Visibility::gradient( mVp, elevation( i ), distance( i ) ); }

double InverseLoS::elevation( int i )
{

    double elevation = at( i ).valueAtAngle( mAngleHorizontal, ValueType::Elevation );

    if ( mCurvatureCorrections )
    {
        return elevation +
               Visibility::curvatureCorrections( at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ),
                                                 mRefractionCoefficient, mEarthDiameter );
    }
    else
    {
        return elevation;
    }
}

void InverseLoS::setTargetPoint( std::shared_ptr<Point> tp, double targetOffset )
{
    mTargetIndex = -1;
    mTp = std::make_shared<Point>( tp->mRow, tp->mCol, tp->mElevation, targetOffset, tp->mCellSize );
}

void InverseLoS::setUpTargetLoSNode()
{
    LoSNode ln = LoSNode( mTp->mRow, mTp->mCol );
    double angle = Visibility::angle( mVp->mRow, mVp->mCol, mTp );
    ln.angle[CellEventPositionType::ENTER] = angle;
    ln.angle[CellEventPositionType::CENTER] = angle;
    ln.angle[CellEventPositionType::EXIT] = angle;
    ln.elevs[CellEventPositionType::ENTER] = mTp->mElevation;
    ln.elevs[CellEventPositionType::CENTER] = mTp->mElevation;
    ln.elevs[CellEventPositionType::EXIT] = mTp->mElevation;
    ln.distances[CellEventPositionType::ENTER] = 0;
    ln.distances[CellEventPositionType::CENTER] = 0;
    ln.distances[CellEventPositionType::EXIT] = 0;
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
    erase( std::remove_if( begin(), end(),
                           [&dist]( LoSNode &node )
                           { return ( dist <= node.centreDistance() && !node.inverseLoSBehindTarget ); } ),
           end() );
}

LoSNode InverseLoS::nodeAt( int i ) { return at( i ); }

int InverseLoS::resultRow() { return mVp->mRow; }

int InverseLoS::resultCol() { return mVp->mCol; }

// TODO fix !!!!!
bool InverseLoS::isValid() { return true; }

void InverseLoS::fixDistancesAngles()
{
    for ( int i = 0; i < size(); i++ )
    {
        if ( at( i ).inverseLoSBehindTarget )
        {
            at( i ).distances[CellEventPositionType::ENTER] += mPointDistance;
            at( i ).distances[CellEventPositionType::CENTER] += mPointDistance;
            at( i ).distances[CellEventPositionType::EXIT] += mPointDistance;

            double addValue = -M_PI;

            if ( at( i ).angle[CellEventPositionType::CENTER] < M_PI ||
                 at( i ).angle[CellEventPositionType::CENTER] == 0 )
            {
                addValue = +M_PI;
            }

            at( i ).angle[CellEventPositionType::ENTER] += addValue;
            at( i ).angle[CellEventPositionType::CENTER] += addValue;
            at( i ).angle[CellEventPositionType::EXIT] += addValue;
        }
        else
        {
            at( i ).distances[CellEventPositionType::ENTER] =
                mPointDistance - at( i ).distances[CellEventPositionType::ENTER];
            at( i ).distances[CellEventPositionType::CENTER] =
                mPointDistance - at( i ).distances[CellEventPositionType::CENTER];
            at( i ).distances[CellEventPositionType::EXIT] =
                mPointDistance - at( i ).distances[CellEventPositionType::EXIT];
        }
    }
}

void InverseLoS::findTargetPointIndex()
{
    for ( int i = 0; i < numberOfNodes(); i++ )
    {
        if ( i + 1 < numberOfNodes() )
        {
            if ( at( i ).centreDistance() == targetDistance() )
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

    erase( std::remove_if( begin(), end(), [=]( LoSNode &node ) { return mPointDistance <= node.centreDistance(); } ),
           end() );
}