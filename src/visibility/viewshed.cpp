#include <functional>

#include <QElapsedTimer>

#include "viewshed.h"
#include "visibility.h"

using viewshed::IPoint;
using viewshed::LoSEvaluator;
using viewshed::MemoryRaster;
using viewshed::SharedStatusList;
using viewshed::StatusNode;
using viewshed::ViewPoint;
using viewshed::Viewshed;
using viewshed::ViewshedAlgorithms;
using viewshed::ViewshedValues;

Viewshed::Viewshed( std::shared_ptr<IPoint> point, std::shared_ptr<QgsRasterLayer> dem, ViewshedAlgorithms algs,
                    double minimalAngle, double maximalAngle )
    : mPoint( std::move( point ) ), mInputDem( std::move( dem ) ), mAlgs( algs )
{
    mValid = false;

    if ( !std::isnan( minimalAngle ) )
    {
        mMinAngle = minimalAngle * ( M_PI / 180 );
    }

    if ( !std::isnan( maximalAngle ) )
    {
        mMaxAngle = maximalAngle * ( M_PI / 180 );
    }

    if ( mMinAngle > mMaxAngle || mMinAngle < -360 || 720 < mMaxAngle )
    {
        return;
    }

    mCellSize = mInputDem->rasterUnitsPerPixelX();

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
}

void Viewshed::prepareMemoryRasters()
{
    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResults.push_back( std::make_shared<MemoryRaster>( mInputDem, Qgis::DataType::Float32,
                                                            mInputDem->dataProvider()->sourceNoDataValue( 1 ) ) );
    }
}

void Viewshed::initEventList()
{

    std::unique_ptr<QgsRasterInterface> rInterface;
    rInterface.reset( mInputDem->dataProvider()->clone() );

    QgsRasterIterator iter( rInterface.get() );
    iter.setMaximumTileHeight( rInterface->ySize() + 1 );
    iter.setMaximumTileWidth( rInterface->xSize() + 1 );

    iter.startRasterRead( mDefaultBand, mInputDem->width(), mInputDem->height(), mInputDem->extent() );
    int iterLeft = 0;
    int iterTop = 0;
    int iterCols = 0;
    int iterRows = 0;
    bool isNoData = false;
    std::unique_ptr<QgsRasterBlock> rasterBlock;

    while ( iter.readNextRasterPart( mDefaultBand, iterCols, iterRows, rasterBlock, iterLeft, iterTop ) )
    {
        for ( int blockRow = 0; blockRow < iterRows; blockRow++ )
        {
            for ( int blockColumn = 0; blockColumn < iterCols; blockColumn++ )
            {
                const double pixelValue = rasterBlock->valueAndNoData( blockRow, blockColumn, isNoData );

                if ( !isNoData )
                {
                    int column = blockColumn + iterLeft;
                    int row = blockRow + iterTop;

                    double elevs[3];
                    elevs[CellPosition::CENTER] = pixelValue;
                    Position tempPosEnter =
                        Visibility::calculateEventPosition( CellPosition::ENTER, row, column, mPoint );
                    elevs[CellPosition::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
                    Position tempPosExit =
                        Visibility::calculateEventPosition( CellPosition::EXIT, row, column, mPoint );
                    elevs[CellPosition::EXIT] = getCornerValue( tempPosExit, rasterBlock, pixelValue );

                    double angleCenter = Visibility::calculateAngle( row, column, mPoint );
                    double angleEnter = Visibility::calculateAngle( &tempPosEnter, mPoint );
                    double angleExit = Visibility::calculateAngle( &tempPosExit, mPoint );

                    // ??? probably not necessary here ???
                    if ( angleEnter > angleCenter )
                    {
                        angleEnter -= 2 * M_PI;
                    }

                    double eventDistance = Visibility::calculateDistance( row, column, mPoint, mCellSize );

                    if ( eventDistance < mMaxDistance && checkInsideAngle( angleEnter, angleExit ) )
                    {
                        Event eCenter = Event( CellPosition::CENTER, row, column, eventDistance, angleCenter, elevs );
                        Event eEnter = Event( CellPosition::ENTER, row, column,
                                              Visibility::calculateDistance( &tempPosEnter, mPoint, mCellSize ),
                                              angleEnter, elevs );
                        Event eExit =
                            Event( CellPosition::EXIT, row, column,
                                   Visibility::calculateDistance( &tempPosExit, mPoint, mCellSize ), angleExit, elevs );

                        if ( mPoint->row == row && mPoint->col < column )
                        {
                            viewPointRowEventList.push_back( eCenter );
                        }
                        else
                        {
                            eventList.push_back( eEnter );
                        }

                        eventList.push_back( eCenter );
                        eventList.push_back( eExit );
                    }
                }
            }
        }
    }
}

void Viewshed::sortEventList() { std::sort( eventList.begin(), eventList.end() ); }

void Viewshed::prefillStatusList()
{
    if ( 0 < statusList.size() )
    {
        statusList.clear();
    }

    for ( Event e : viewPointRowEventList )
    {
        StatusNode sn( mPoint, &e, mCellSize );
        statusList.push_back( sn );
    }
}

void Viewshed::extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                           std::function<double( StatusNode )> func )
{
    MemoryRaster result = MemoryRaster( dem_ );

    int i = 0;
    for ( Event event : eventList )
    {
        if ( event.eventType == CellPosition::CENTER )
        {
            StatusNode sn( mPoint, &event, mCellSize );
            result.setValue( func( sn ), sn.col, sn.row );
        }
        i++;
    }

    result.save( fileName );
}

void Viewshed::parseEventList( std::function<void( int size, int current )> progressCallback )
{
    prepareMemoryRasters();

    ViewshedValues rasterValues;

    int i = 0;
    for ( Event e : eventList )
    {
        progressCallback( eventList.size(), i );

        StatusNode sn;
        switch ( e.eventType )
        {
            case CellPosition::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }

                sn = StatusNode( mPoint, &e, mCellSize );
                statusList.push_back( sn );
                break;
            }
            case CellPosition::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }

                sn = StatusNode( e.row, e.col );

                std::vector<StatusNode>::iterator index = std::find( statusList.begin(), statusList.end(), sn );
                if ( index != statusList.end() )
                {
                    statusList.erase( index );
                }
                break;
            }
            case CellPosition::CENTER:
            {
                std::shared_ptr<StatusNode> poi = std::make_shared<StatusNode>( mPoint, &e, mCellSize );

                mResultPixels.push_back( mThreadPool.submit( taskVisibility, mAlgs, statusList, poi, mPoint ) );

                break;
            }
        }

        if ( mMaxNumberOfTasks < mThreadPool.get_tasks_total() || mMaxNumberOfResults < mResultPixels.size() )
        {
            // parse result to rasters to avoid clutching in memory
            parseCalculatedResults();
        }

        i++;
    }

    // parse results left after the algorithm finished
    parseCalculatedResults();

    for ( int j = 0; j < mAlgs.size(); j++ )
    {
        mResults.at( j )->setValue( mAlgs.at( j )->viewpointValue(), mPoint->col, mPoint->row );
    }
}

void Viewshed::parseCalculatedResults()
{
    mThreadPool.wait_for_tasks();

    for ( int j = 0; j < mResultPixels.size(); j++ )
    {
        setPixelData( mResultPixels[j].get() );
    }

    mResultPixels = BS::multi_future<ViewshedValues>();
}

void Viewshed::setPixelData( ViewshedValues values )
{
    for ( int j = 0; j < values.values.size(); j++ )
    {
        mResults.at( j )->setValue( values.values.at( j ), values.col, values.row );
    }
}

ViewshedValues viewshed::taskVisibility( ViewshedAlgorithms algs, std::vector<StatusNode> statusList,
                                         std::shared_ptr<StatusNode> poi, std::shared_ptr<IPoint> point )
{
    std::sort( statusList.begin(), statusList.end() );

    LoSEvaluator losEval;

    losEval.calculate( algs, statusList, poi, point );

    return losEval.mResultValues;
}

StatusNode Viewshed::statusNodeFromPoint( QgsPoint point )
{
    QgsPoint pointRaster = mInputDem->dataProvider()->transformCoordinates(
        point, QgsRasterDataProvider::TransformType::TransformLayerToImage );

    int row = pointRaster.y();
    int col = pointRaster.x();

    StatusNode sn;

    for ( Event e : eventList )
    {
        if ( e.eventType == CellPosition::CENTER && e.col == col && e.row == row )
        {
            sn = StatusNode( mPoint, &e, mCellSize );
            return sn;
        }
    }

    return sn;
}

std::shared_ptr<std::vector<StatusNode>> Viewshed::LoSToPoint( QgsPoint point, bool onlyToPoint )
{
    prefillStatusList();

    StatusNode poi = statusNodeFromPoint( point );
    StatusNode sn;

    for ( Event e : eventList )
    {
        switch ( e.eventType )
        {
            case CellPosition::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                sn = StatusNode( mPoint, &e, mCellSize );
                statusList.push_back( sn );
                break;
            }

            case CellPosition::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                sn = StatusNode( e.row, e.col );
                std::vector<StatusNode>::iterator index = std::find( statusList.begin(), statusList.end(), sn );
                if ( index != statusList.end() )
                {
                    statusList.erase( index );
                }
                break;
            }

            case CellPosition::CENTER:
            {
                sn = StatusNode( mPoint, &e, mCellSize );
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

SharedStatusList Viewshed::getLoS( StatusNode poi, bool onlyToPoi )
{
    SharedStatusList losToReturn = std::make_shared<StatusList>();

    for ( int j = 0; j < statusList.size(); j++ )
    {
        StatusNode node = statusList.at( j );

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

std::shared_ptr<MemoryRaster> Viewshed::resultRaster( int index ) { return mResults.at( index ); }

double Viewshed::getCornerValue( const Position &pos, const std::unique_ptr<QgsRasterBlock> &block,
                                 double defaultValue )
{

    double xL = pos.col - 0.5;
    double yL = pos.row - 0.5;
    double xU = pos.col + 0.5;
    double yU = pos.row + 0.5;

    bool noData;
    double value1 = block->valueAndNoData( (int)( yL ), (int)( xL ), noData );
    if ( noData )
    {
        return defaultValue;
    }
    double value2 = block->valueAndNoData( (int)( yL ), (int)( xU ), noData );
    if ( noData )
    {
        return defaultValue;
    }
    double value3 = block->valueAndNoData( (int)( yU ), (int)( xL ), noData );
    if ( noData )
    {
        return defaultValue;
    }
    double value4 = block->valueAndNoData( (int)( yU ), (int)( xU ), noData );
    if ( noData )
    {
        return defaultValue;
    }

    return ( value1 + value2 + value3 + value4 ) / 4;
}

void Viewshed::saveResults( QString location )
{
    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        QString file = QString( "%1/%2.tif" ).arg( location ).arg( mAlgs.at( i )->name() );
        mResults.at( i )->save( file );
    }
}

void Viewshed::setMaximalDistance( double distance ) { mMaxDistance = distance; }

bool Viewshed::checkInsideAngle( double eventEnterAngle, double eventExitAngle )
{

    if ( mMinAngle <= eventEnterAngle && eventExitAngle <= mMaxAngle )
    {
        return true;
    }

    if ( mMinAngle < 0 )
    {
        if ( mMinAngle <= ( eventEnterAngle - 2 * M_PI ) && ( eventExitAngle - 2 * M_PI ) <= mMaxAngle )
        {
            return true;
        }
    }

    return false;
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
