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

LoS::LoS() {}

LoS::LoS( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

void LoS::sort() { std::sort( begin(), end() ); }

void LoS::setLoSNodes( std::vector<LoSNode> losNodes ) { assign( losNodes.begin(), losNodes.end() ); }

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

LoSNode LoS::nodeAt( std::size_t i ) { return this->operator[]( i ); }

int LoS::resultRow() { return mTp->mRow; }

int LoS::resultCol() { return mTp->mCol; }

void LoS::removePointsAfterTarget()
{

    erase(
        std::remove_if( begin(), end(), [this]( LoSNode &node ) { return mPointDistance <= node.centreDistance(); } ),
        end() );
}