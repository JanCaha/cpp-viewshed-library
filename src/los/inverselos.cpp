#include "los.h"
#include "visibility.h"

using viewshed::ILoS;
using viewshed::InverseLoS;
using viewshed::Visibility;

InverseLoS::InverseLoS() {}

InverseLoS::InverseLoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

double InverseLoS::distance( int i )
{
    return mPointDistance - at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance );
}

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

void InverseLoS::prepareForCalculation() { removePointsAfterViewPoint(); };

void InverseLoS::removePointsAfterViewPoint()
{
    double dist = mPointDistance;
    erase( std::remove_if( begin(), end(), [&dist]( LoSNode &node ) { return dist <= node.centreDistance(); } ),
           end() );
}

LoSNode InverseLoS::nodeAt( int i ) { return at( i ); }

// TODO fix !!!!!
bool InverseLoS::isValid() { return true; }
