#include "los.h"
#include "visibility.h"

using viewshed::ILoS;
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