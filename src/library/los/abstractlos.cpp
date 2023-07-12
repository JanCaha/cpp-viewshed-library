#include "abstractlos.h"
#include "cellevent.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::Point;
using viewshed::Visibility;

AbstractLoS::AbstractLoS(){};

void AbstractLoS::setViewPoint( std::shared_ptr<Point> vp ) { mVp = vp; }

void AbstractLoS::setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset )
{
    mVp = std::make_shared<Point>( poi->mRow, poi->mCol, poi->centreElevation(), observerOffset, 0 );
}

double AbstractLoS::targetDistance() { return mPointDistance; }

double AbstractLoS::targetGradient() { return Visibility::gradient( mVp, mTp->totalElevation(), mPointDistance ); }

double AbstractLoS::targetElevation() { return mTp->totalElevation(); }

void AbstractLoS::setTargetPoint( std::shared_ptr<Point> tp )
{
    mTp = tp;
    mPointDistance = mTp->distance( mVp );
}

void AbstractLoS::setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset )
{
    mAngleHorizontal = poi->centreAngle();
    mTp = std::make_shared<Point>( poi->mRow, poi->mCol, poi->centreElevation(), targetOffset, 0 );
    mPointDistance = mVp->distance( mTp );
}

double AbstractLoS::horizontalAngle() { return mAngleHorizontal; }

void AbstractLoS::setAngle( double angle ) { mAngleHorizontal = angle; }

int AbstractLoS::targetCol() { return mTp->mCol; }

int AbstractLoS::targetRow() { return mTp->mRow; }

std::shared_ptr<Point> AbstractLoS::vp() { return mVp; }

double AbstractLoS::viewPointElevation() { return mVp->mElevation; }

double AbstractLoS::viewPointTotalElevation() { return mVp->totalElevation(); }
