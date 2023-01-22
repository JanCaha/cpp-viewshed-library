#include <cmath>

#include <QElapsedTimer>

#include "inverseviewshed.h"
#include "los.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::InverseViewshed;
using viewshed::IViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::Point;
using viewshed::ViewshedValues;

InverseViewshed::InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset,
                                  std::shared_ptr<QgsRasterLayer> dem,
                                  std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs,
                                  double minimalAngle, double maximalAngle )
{
    mValid = false;

    mPoint = targetPoint;
    mInputDem = dem;
    mAlgs = algs;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mObserverOffset = observerOffset;

    mValid = true;
}

std::shared_ptr<LoS> InverseViewshed::getLoS( QgsPoint point, bool onlyToPoi )
{

    std::vector<LoSNode> losNodes = prepareLoSWithPoint( point );

    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( statusNodeFromPoint( point ) );

    if ( onlyToPoi )
    {
        double poiDistance = poi->centreDistance();

        losNodes.erase( std::remove_if( losNodes.begin(), losNodes.end(),
                                        [&poiDistance]( LoSNode &node )
                                        { return poiDistance <= node.centreDistance(); } ),
                        losNodes.end() );
    }

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( losNodes );
    los->sort();
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );

    return los;
}

void InverseViewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
                                 std::function<void( int size, int current )> progressCallback )
{
    QElapsedTimer timer;

    timer.start();

    initEventList();

    stepsTimingCallback( "Init event list lasted: ", timer.elapsed() / 1000.0 );

    timer.restart();

    sortEventList();

    stepsTimingCallback( "Sort event list lasted: ", timer.elapsed() / 1000.0 );

    timer.restart();

    parseEventList( progressCallback );

    stepsTimingCallback( "Parsing of event list lasted: ", timer.elapsed() / 1000.0 );
}

void InverseViewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );

    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>( mLosNodes );
    los->sort();
    los->setTargetPoint( mPoint, mPoint->offset );
    los->setViewPoint( poi, mObserverOffset );

    mResultPixels.push_back( mThreadPool.submit( viewshed::evaluateLoSForPoI, los, mAlgs ) );
}