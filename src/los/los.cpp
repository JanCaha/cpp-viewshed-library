#include "los.h"
#include "visibility.h"

using viewshed::ILoS;
using viewshed::InverseLoS;
using viewshed::LoS;
using viewshed::Visibility;

ILoS::ILoS(){};

void ILoS::setViewPoint( std::shared_ptr<Point> vp ) { mVp = vp; }

void ILoS::setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset )
{
    mVp = std::make_shared<Point>( poi->row, poi->col, poi->centreElevation(), observerOffset, 0 );
}

double ILoS::targetDistance() { return mPointDistance; }

double ILoS::targetGradient() { return Visibility::calculateGradient( mVp, mTp->totalElevation(), mPointDistance ); }

double ILoS::targetElevation() { return mTp->totalElevation(); }

void ILoS::setTargetPoint( std::shared_ptr<Point> tp )
{
    mTp = tp;
    mPointDistance = mTp->distance( mVp );
}

void ILoS::setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset )
{
    mAngleHorizontal = poi->centreAngle();
    mTp = std::make_shared<Point>( poi->row, poi->col, poi->centreElevation(), targetOffset, 0 );
    mPointDistance = mVp->distance( mTp );
}

double ILoS::horizontalAngle() { return mAngleHorizontal; }

void ILoS::setAngle( double angle ) { mAngleHorizontal = angle; }

int ILoS::targetCol() { return mTp->col; }

int ILoS::targetRow() { return mTp->row; }

std::shared_ptr<Point> ILoS::vp() { return mVp; }

LoS::LoS() : std::vector<LoSNode>() {}

LoS::LoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

void LoS::sort() { std::sort( begin(), end() ); }

void LoS::setLoSNodes( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

double LoS::gradient( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Gradient ); }

double LoS::distance( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double LoS::elevation( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Elevation ); }

bool LoS::isValid() { return mVp->isValid(); }

void LoS::setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset )
{
    std::vector<LoSNode>::iterator index = std::find( begin(), end(), *poi );
    if ( index != end() )
    {
        mTargetIndex = std::distance( begin(), index );
    }

    mAngleHorizontal = poi->centreAngle();
    mTp = std::make_shared<Point>( poi->row, poi->col, poi->centreElevation(), targetOffset, 0 );
    mPointDistance = mVp->distance( mTp );
}

int LoS::targetPointIndex() { return mTargetIndex; }

InverseLoS::InverseLoS( std::shared_ptr<Point> vp, std::shared_ptr<Point> tp )
{
    mVp = vp;
    mTp = tp;
    mPointDistance = mVp->distance( mTp );
}

double InverseLoS::distance( int i )
{
    return mPointDistance - at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance );
}

double InverseLoS::gradient( int i )
{
    double dist = distance( i );
    return Visibility::calculateGradient( mVp, at( i ).valueAtAngle( mAngleHorizontal, ValueType::Elevation ), dist );
}
