#include <cmath>

#include <QElapsedTimer>

#include "los.h"
#include "threadtasks.h"
#include "viewshed.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::Point;
using viewshed::Viewshed;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<Point> viewPoint, std::shared_ptr<QgsRasterLayer> dem,
                    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                    bool applyCurvatureCorrections, double earthDiameter, double refractionCoeff, double minimalAngle,
                    double maximalAngle )
{
    mValid = false;

    mPoint = viewPoint;
    mInputDem = dem;
    mAlgs = algs;

    mCurvatureCorrections = applyCurvatureCorrections;
    mEarthDiameter = earthDiameter;
    mRefractionCoefficient = refractionCoeff;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
}

std::shared_ptr<LoS> Viewshed::getLoS( QgsPoint point, bool onlyToPoi )
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

    std::shared_ptr<LoS> los = std::make_shared<LoS>( losNodes );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );
    los->prepareForCalculation();

    return los;
}

void Viewshed::calculate( std::function<void( std::string, double )> stepsTimingCallback,
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

void Viewshed::submitToThreadpool( CellEvent &e )
{
    std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );

    std::shared_ptr<LoS> los = std::make_shared<LoS>( mLosNodes );
    los->setViewPoint( mPoint );
    los->setTargetPoint( poi );

    mResultPixels.push_back( mThreadPool.submit( viewshed::evaluateLoSForPoI, los, mAlgs ) );
}