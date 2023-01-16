#include "iviewshed.h"
#include "memoryraster.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::IViewshed;
using viewshed::LoSNode;
using viewshed::MemoryRaster;

void IViewshed::prepareMemoryRasters()
{
    for ( int i = 0; i < mAlgs->size(); i++ )
    {
        mResults.push_back( std::make_shared<MemoryRaster>( mInputDem, Qgis::DataType::Float32,
                                                            mInputDem->dataProvider()->sourceNoDataValue( 1 ) ) );
    }
}

void IViewshed::initEventList()
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
    std::unique_ptr<QgsRasterBlock> rasterBlock; // = std::make_shared<QgsRasterBlock>();

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
                    RasterPosition tempPosEnter =
                        Visibility::calculateEventPosition( CellPosition::ENTER, row, column, mPoint );
                    elevs[CellPosition::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
                    RasterPosition tempPosExit =
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
                        CellEvent eCenter =
                            CellEvent( CellPosition::CENTER, row, column, eventDistance, angleCenter, elevs );
                        CellEvent eEnter = CellEvent( CellPosition::ENTER, row, column,
                                                      Visibility::calculateDistance( &tempPosEnter, mPoint, mCellSize ),
                                                      angleEnter, elevs );
                        CellEvent eExit = CellEvent( CellPosition::EXIT, row, column,
                                                     Visibility::calculateDistance( &tempPosExit, mPoint, mCellSize ),
                                                     angleExit, elevs );

                        mCellEvents.push_back( eEnter );
                        mCellEvents.push_back( eCenter );
                        mCellEvents.push_back( eExit );
                    }
                }
            }
        }
    }
}

void IViewshed::setMaximalDistance( double distance ) { mMaxDistance = distance; }

void IViewshed::setAngles( double minAngle, double maxAngle )
{
    if ( !std::isnan( minAngle ) )
    {
        mMinAngle = minAngle * ( M_PI / 180 );
    }

    if ( !std::isnan( maxAngle ) )
    {
        mMaxAngle = maxAngle * ( M_PI / 180 );
    }

    if ( mMinAngle > mMaxAngle )
    {
        mMinAngle = -360.0;
        mMaxAngle = 720.0;
    }

    if ( mMinAngle < -360 )
    {
        mMinAngle = -360.0;
    }

    if ( 720 < mMaxAngle )
    {
        mMaxAngle = 720.0;
    }
}

bool IViewshed::checkInsideAngle( double eventEnterAngle, double eventExitAngle )
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

void IViewshed::sortEventList() { std::sort( mCellEvents.begin(), mCellEvents.end() ); }

void IViewshed::parseEventList( std::function<void( int size, int current )> progressCallback )
{
    prepareMemoryRasters();

    ViewshedValues rasterValues;

    int i = 0;
    for ( CellEvent e : mCellEvents )
    {
        progressCallback( mCellEvents.size(), i );

        LoSNode ln;
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
                std::shared_ptr<LoSNode> poi = std::make_shared<LoSNode>( mPoint, &e, mCellSize );
                std::shared_ptr<std::vector<LoSNode>> los =
                    std::make_shared<std::vector<LoSNode>>( mLosNodes.begin(), mLosNodes.end() );

                mResultPixels.push_back( mThreadPool.submit( viewshed::evaluateLoSForPoI, mAlgs, los, poi, mPoint ) );

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

    for ( int j = 0; j < mAlgs->size(); j++ )
    {
        mResults.at( j )->setValue( mAlgs->at( j )->viewpointValue(), mPoint->col, mPoint->row );
    }
}

void IViewshed::parseCalculatedResults()
{
    mThreadPool.wait_for_tasks();

    for ( int j = 0; j < mResultPixels.size(); j++ )
    {
        setPixelData( mResultPixels[j].get() );
    }

    mResultPixels = BS::multi_future<ViewshedValues>();
}

void IViewshed::setPixelData( ViewshedValues values )
{
    for ( int j = 0; j < values.values.size(); j++ )
    {
        mResults.at( j )->setValue( values.values.at( j ), values.col, values.row );
    }
}

void IViewshed::extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                            std::function<double( LoSNode )> func )
{
    MemoryRaster result = MemoryRaster( dem_ );

    int i = 0;
    for ( CellEvent event : mCellEvents )
    {
        if ( event.eventType == CellPosition::CENTER )
        {
            LoSNode ln( mPoint, &event, mCellSize );
            result.setValue( func( ln ), ln.col, ln.row );
        }
        i++;
    }

    result.save( fileName );
}

double IViewshed::getCornerValue( const RasterPosition &pos, const std::unique_ptr<QgsRasterBlock> &block,
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

std::shared_ptr<MemoryRaster> IViewshed::resultRaster( int index ) { return mResults.at( index ); }

void IViewshed::saveResults( QString location )
{
    for ( int i = 0; i < mAlgs->size(); i++ )
    {
        QString file = QString( "%1/%2.tif" ).arg( location ).arg( mAlgs->at( i )->name() );
        mResults.at( i )->save( file );
    }
}

LoSNode IViewshed::statusNodeFromPoint( QgsPoint point )
{
    QgsPoint pointRaster = mInputDem->dataProvider()->transformCoordinates(
        point, QgsRasterDataProvider::TransformType::TransformLayerToImage );

    int row = pointRaster.y();
    int col = pointRaster.x();

    LoSNode ln;

    for ( CellEvent e : mCellEvents )
    {
        if ( e.eventType == CellPosition::CENTER && e.col == col && e.row == row )
        {
            ln = LoSNode( mPoint, &e, mCellSize );
            return ln;
        }
    }

    return ln;
}

void IViewshed::setMaxConcurentTaks( int maxTasks ) { mMaxNumberOfTasks = maxTasks; }

void IViewshed::setMaxResultsInMemory( int maxResults ) { mMaxNumberOfResults = maxResults; }

void IViewshed::setMaxThreads( int threads )
{
    if ( mThreadPool.get_thread_count() < threads )
    {
        return;
    }

    mThreadPool.reset( threads );
}