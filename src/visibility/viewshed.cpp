#include <functional>

#include <QElapsedTimer>

#include "viewshed.h"
#include "visibility.h"

using viewshed::IPoint;
using viewshed::LoSEvaluator;
using viewshed::LoSNode;
using viewshed::MemoryRaster;
using viewshed::ViewPoint;
using viewshed::Viewshed;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<IPoint> point, std::shared_ptr<QgsRasterLayer> dem,
                    std::vector<std::shared_ptr<IViewshedAlgorithm>> algs, double minimalAngle, double maximalAngle )
{
    mValid = false;

    mPoint = point;
    mInputDem = dem;
    mAlgs = algs;

    setAngles( minimalAngle, maximalAngle );

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
}

std::shared_ptr<std::vector<LoSNode>> Viewshed::LoSToPoint( QgsPoint point, bool onlyToPoint )
{
    LoSNode poi = statusNodeFromPoint( point );
    LoSNode ln;

    mLosNodes.clear();

    for ( CellEvent e : mCellEvents )
    {
        switch ( e.eventType )
        {
            case CellPosition::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                ln = LoSNode( mPoint, &e, mCellSize );
                mLosNodes.push_back( ln );
                break;
            }

            case CellPosition::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                ln = LoSNode( e.row, e.col );
                std::vector<LoSNode>::iterator index = std::find( mLosNodes.begin(), mLosNodes.end(), ln );
                if ( index != mLosNodes.end() )
                {
                    mLosNodes.erase( index );
                }
                break;
            }

            case CellPosition::CENTER:
            {
                ln = LoSNode( mPoint, &e, mCellSize );
                if ( ln.col == poi.col && ln.row == poi.row )
                {
                    return getLoS( poi, onlyToPoint );
                }
                break;
            }
        }
    }

    std::shared_ptr<std::vector<LoSNode>> los;
    return los;
}

std::shared_ptr<std::vector<LoSNode>> Viewshed::getLoS( LoSNode poi, bool onlyToPoi )
{
    std::shared_ptr<std::vector<LoSNode>> losToReturn = std::make_shared<std::vector<LoSNode>>();

    for ( int j = 0; j < mLosNodes.size(); j++ )
    {
        LoSNode node = mLosNodes.at( j );

        if ( node.angle[CellPosition::ENTER] <= poi.centreAngle() &&
             poi.centreAngle() <= node.angle[CellPosition::EXIT] )
        {
            if ( onlyToPoi )
            {
                if ( node.centreDistance() <= poi.centreDistance() )
                {
                    losToReturn->push_back( node );
                }
            }
            else
            {
                losToReturn->push_back( node );
            }
        }
    }

    std::sort( losToReturn->begin(), losToReturn->end() );

    return losToReturn;
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
