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

double AbstractLoS::targetGradient()
{
    return Visibility::gradient( mTp->totalElevation() - mVp->totalElevation(), mPointDistance );
}

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

double AbstractLoS::currentDistance() { return mCurrentLoSNode.distanceAtAngle( mAngleHorizontal ); }

double AbstractLoS::currentElevation()
{
    return mCurrentLoSNode.elevationAtAngle( mAngleHorizontal ) + curvatureCorrectionsFix( currentDistance() );
}

double AbstractLoS::currentGradient()
{
    return Visibility::gradient( currentElevation() - mVp->totalElevation(), currentDistance() );
}

double AbstractLoS::curvatureCorrectionsFix( const double distance )
{
    if ( mCurvatureCorrections )
    {
        return Visibility::curvatureCorrections( distance, mRefractionCoefficient, mEarthDiameter );
    }

    return 0;
}

double AbstractLoS::elevation( std::size_t i )
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

double AbstractLoS::distance( std::size_t i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double AbstractLoS::gradient( std::size_t i )
{
    return Visibility::gradient( elevation( i ) - mVp->totalElevation(), distance( i ) );
}

void AbstractLoS::setCurrentLoSNode( std::size_t i ) { mCurrentLoSNode = at( i ); }