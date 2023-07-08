#include "los.h"
#include "abstractlos.h"
#include "cellevent.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::AbstractLoS;
using viewshed::LoS;
using viewshed::LoSNode;
using viewshed::Visibility;

LoS::LoS() : std::vector<LoSNode>() {}

LoS::LoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

void LoS::sort() { std::sort( begin(), end() ); }

void LoS::setLoSNodes( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

double LoS::gradient( int i ) { return Visibility::gradient( elevation( i ) - mVp->totalElevation(), distance( i ) ); }

double LoS::distance( int i ) { return at( i ).valueAtAngle( mAngleHorizontal, ValueType::Distance ); }

double LoS::elevation( int i )
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

bool LoS::isValid() { return mVp->isValid(); }

void LoS::setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset )
{
    mAngleHorizontal = poi->centreAngle();
    mTp = std::make_shared<Point>( poi->mRow, poi->mCol, poi->centreElevation(), targetOffset, 0 );
    mPointDistance = mVp->distance( mTp );
}

void LoS::findTargetPointIndex()
{
    LoSNode ln = LoSNode( mTp->mRow, mTp->mCol );

    std::vector<LoSNode>::iterator index = std::find( begin(), end(), ln );
    if ( index != end() )
    {
        mTargetIndex = std::distance( begin(), index );
    }
}

int LoS::targetPointIndex() { return mTargetIndex; }

void LoS::prepareForCalculation()
{
    sort();
    findTargetPointIndex();
};

int LoS::numberOfNodes() { return size(); };

LoSNode LoS::nodeAt( int i ) { return at( i ); }

int LoS::resultRow() { return mTp->mRow; }

int LoS::resultCol() { return mTp->mCol; }

void LoS::removePointsAfterTarget()
{

    erase( std::remove_if( begin(), end(), [=]( LoSNode &node ) { return mPointDistance <= node.centreDistance(); } ),
           end() );
}