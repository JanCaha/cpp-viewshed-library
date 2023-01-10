#include <functional>

#include <QElapsedTimer>

#include "viewshed.h"
#include "visibility.h"

Viewshed::Viewshed( std::shared_ptr<ViewPoint> vp, std::shared_ptr<QgsRasterLayer> dem,
                    std::vector<std::shared_ptr<IViewshedAlgorithm>> algs, double minimalAngle, double maximalAngle )
    : mVp( std::move( vp ) ), mInputDem( std::move( dem ) ), mAlgs( algs )
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

    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResults.push_back( std::make_shared<MemoryRaster>( mInputDem, Qgis::DataType::Float32,
                                                            mInputDem->dataProvider()->sourceNoDataValue( 1 ) ) );
    }

    mThreadPool.reset( mThreadPool.get_thread_count() - 1 );

    mValid = true;
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
                        Visibility::calculateEventPosition( CellPosition::ENTER, row, column, *mVp );
                    elevs[CellPosition::ENTER] = getCornerValue( tempPosEnter, rasterBlock, pixelValue );
                    Position tempPosExit = Visibility::calculateEventPosition( CellPosition::EXIT, row, column, *mVp );
                    elevs[CellPosition::EXIT] = getCornerValue( tempPosExit, rasterBlock, pixelValue );

                    double angleCenter = Visibility::calculateAngle( row, column, mVp );
                    double angleEnter = Visibility::calculateAngle( &tempPosEnter, mVp );
                    double angleExit = Visibility::calculateAngle( &tempPosExit, mVp );

                    // ??? probably not necessary here ???
                    if ( angleEnter > angleCenter )
                    {
                        angleEnter -= 2 * M_PI;
                    }

                    double eventDistance = Visibility::calculateDistance( row, column, mVp, mCellSize );

                    if ( eventDistance < mMaxDistance && checkInsideAngle( angleEnter, angleExit ) )
                    {
                        Event eCenter = Event( CellPosition::CENTER, row, column, eventDistance, angleCenter, elevs );
                        Event eEnter =
                            Event( CellPosition::ENTER, row, column,
                                   Visibility::calculateDistance( &tempPosEnter, mVp, mCellSize ), angleEnter, elevs );
                        Event eExit =
                            Event( CellPosition::EXIT, row, column,
                                   Visibility::calculateDistance( &tempPosExit, mVp, mCellSize ), angleExit, elevs );

                        if ( mVp->row == row && mVp->col < column )
                        {
                            // qDebug() << "Sn prefill " << row << ":" << column;
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
    for ( Event e : viewPointRowEventList )
    {
        StatusNode sn( mVp, &e, mCellSize );
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
        if ( i % 10'000 == 0 )
            qDebug() << ( (double)i / (double)eventList.size() ) * 100.0 << "%.";

        if ( event.eventType == CellPosition::CENTER )
        {
            StatusNode sn( mVp, &event, mCellSize );
            result.setValue( func( sn ), sn.col, sn.row );
        }
        i++;
    }

    result.save( fileName );
}

void Viewshed::parseEventList( std::function<void( int size, int current )> progressCallback )
{
    int i = 0;
    for ( Event e : eventList )
    {
        progressCallback( eventList.size(), i );

        StatusNode sn;
        switch ( e.eventType )
        {
            case CellPosition::ENTER:
            {
                if ( mVp->row == e.row && mVp->col == e.col )
                {
                    break;
                }

                sn = StatusNode( mVp, &e, mCellSize );
                statusList.push_back( sn );
                break;
            }
            case CellPosition::EXIT:
            {
                if ( mVp->row == e.row && mVp->col == e.col )
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
                // sn = StatusNode( mVp, &e, mCellSize );
                std::shared_ptr<StatusNode> poi = std::make_shared<StatusNode>( mVp, &e, mCellSize );

                if ( mVp->row == sn.row && mVp->col == sn.col )
                {
                    for ( int j = 0; j < mAlgs.size(); j++ )
                    {
                        mResults.at( j )->setValue( mAlgs.at( j )->viewpointValue(), sn.col, sn.row );
                    }
                    break;
                }

                mResultPixels.push_back( mThreadPool.submit( taskVisibility, mAlgs, statusList, poi, mVp ) );

                break;
            }
        }
        i++;
    }

    ViewshedValues rasterValues;

    for ( int i = 0; i < mResultPixels.size(); i++ )
    {
        rasterValues = mResultPixels[i].get();

        for ( int j = 0; j < rasterValues.values.size(); j++ )
        {
            mResults.at( j )->setValue( rasterValues.values.at( j ), rasterValues.col, rasterValues.row );
        }
    }
}

ViewshedValues taskVisibility( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                               std::vector<StatusNode> statusList, std::shared_ptr<StatusNode> poi,
                               std::shared_ptr<ViewPoint> vp )
{
    std::sort( statusList.begin(), statusList.end() );

    LoSEvaluator losEval;

    losEval.calculate( algs, statusList, poi, vp );

    return losEval.mResultValues;
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

    stepsTimingCallback( "Parese of event list lasted: ", timer.elapsed() / 1000.0 );
}
