#include <exception>

#include "abstractlos.h"
#include "cellevent.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::LoSNode;
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

double AbstractLoS::gradient( int i ) { return Visibility::gradient( mVp, elevation( i ), distance( i ) ); }

double AbstractLoS::distance( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double AbstractLoS::elevation( int i )
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

LoSNode AbstractLoS::nodeAt( int i ) { return at( i ); }

int AbstractLoS::numberOfNodes() { return size(); };

void AbstractLoS::sort()
{
    if ( !isValid() )
    {
        throw std::logic_error( "LoS is not valid." );
    }

    std::sort( begin(), end() );
}
