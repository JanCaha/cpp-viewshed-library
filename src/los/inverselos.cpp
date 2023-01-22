#include "los.h"
#include "visibility.h"

using viewshed::ILoS;
using viewshed::InverseLoS;
using viewshed::Visibility;

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
    mTargetIndex = 0;
    mTp = std::make_shared<Point>( tp->row, tp->col, tp->elevation, targetOffset, tp->cellSize );
}

void InverseLoS::setViewPoint( std::shared_ptr<LoSNode> vp, double observerOffset )
{
    mTargetIndex = 0;
    mVp = std::make_shared<Point>( vp->row, vp->col, vp->centreElevation(), observerOffset, 0 );
    mPointDistance = mTp->distance( mVp );
    mAngleHorizontal = vp->centreAngle();
}