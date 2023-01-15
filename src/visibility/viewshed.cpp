#include <functional>

#include <QElapsedTimer>

#include "viewshed.h"
#include "visibility.h"

using viewshed::IPoint;
using viewshed::LoSEvaluator;
using viewshed::MemoryRaster;
using viewshed::SharedStatusList;
using viewshed::LoSNode;
using viewshed::ViewPoint;
using viewshed::Viewshed;
using viewshed::ViewshedAlgorithms;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<IPoint> point, std::shared_ptr<QgsRasterLayer> dem, ViewshedAlgorithms algs,
                    double minimalAngle, double maximalAngle )
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
    prefillStatusList();

    LoSNode poi = statusNodeFromPoint( point );
    LoSNode sn;

    for ( CellEvent e : eventList )
    {
        switch ( e.eventType )
        {
            case CellPosition::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                sn = LoSNode( mPoint, &e, mCellSize );
                statusList.push_back( sn );
                break;
            }

            case CellPosition::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                sn = LoSNode( e.row, e.col );
                std::vector<LoSNode>::iterator index = std::find( statusList.begin(), statusList.end(), sn );
                if ( index != statusList.end() )
                {
                    statusList.erase( index );
                }
                break;
            }

            case CellPosition::CENTER:
            {
                sn = LoSNode( mPoint, &e, mCellSize );
                if ( sn.col == poi.col && sn.row == poi.row )
                {
                    return getLoS( poi, onlyToPoint );
                }
                break;
            }
        }
    }

    SharedStatusList los;
    return los;
}

SharedStatusList Viewshed::getLoS( LoSNode poi, bool onlyToPoi )
{
    SharedStatusList losToReturn = std::make_shared<StatusList>();

    for ( int j = 0; j < statusList.size(); j++ )
    {
        LoSNode node = statusList.at( j );

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

    prefillStatusList();

    timer.restart();

    parseEventList( progressCallback );

    stepsTimingCallback( "Parsing of event list lasted: ", timer.elapsed() / 1000.0 );
}
