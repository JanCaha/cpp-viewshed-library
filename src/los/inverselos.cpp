#include "inverselos.h"
#include "abstractlos.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::InverseLoS;
using viewshed::Visibility;

InverseLoS::InverseLoS() {}

InverseLoS::InverseLoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

double InverseLoS::distance( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double InverseLoS::gradient( int i )
{
    double dist = distance( i );
    return Visibility::calculateGradient( mVp, at( i ).valueAtAngle( mAngleHorizontal, ValueType::Elevation ), dist );
}

double InverseLoS::elevation( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Elevation ); }

void InverseLoS::setTargetPoint( std::shared_ptr<Point> tp, double targetOffset )
{
    mTargetIndex = -1;
    mTp = std::make_shared<Point>( tp->row, tp->col, tp->elevation, targetOffset, tp->cellSize );
}

void InverseLoS::setViewPoint( std::shared_ptr<LoSNode> vp, double observerOffset )
{
    mTargetIndex = -1;
    mVp = std::make_shared<Point>( vp->row, vp->col, vp->centreElevation(), observerOffset, 0 );
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
    fixDistancesAngles();
    sort();
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

int InverseLoS::resultRow() { return mVp->row; }

int InverseLoS::resultCol() { return mVp->col; }

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
            if ( at( i ).distances[CellEventPositionType::CENTER] < targetDistance() &&
                 targetDistance() < at( i + 1 ).distances[CellEventPositionType::CENTER] )
            {
                mTargetIndex = i;
            }
        }
        else
        {
            mTargetIndex = i;
        }
    }
}