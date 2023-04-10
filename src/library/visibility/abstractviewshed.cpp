#include "abstractviewshed.h"
#include "memoryraster.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::AbstractViewshed;
using viewshed::LoSNode;
using viewshed::MemoryRaster;

void AbstractViewshed::prepareMemoryRasters()
{
    mResults.clear();
    mResults.reserve( mAlgs->size() );

    for ( int i = 0; i < mAlgs->size(); i++ )
    {
        mResults.push_back( std::make_shared<MemoryRaster>(
            mInputDem, mDataType, mInputDem->dataProvider()->sourceNoDataValue( mDefaultBand ) ) );
    }
}

void AbstractViewshed::setDefaultResultDataType( Qgis::DataType dataType )
{
    mDataType = dataType;
    if ( !mResults.empty() )
    {
        prepareMemoryRasters();
    }
}

void AbstractViewshed::initEventList()
{
    std::chrono::_V2::system_clock::time_point startTime = std::chrono::_V2::high_resolution_clock::now();

    mCellEvents.clear();
    mLosNodes.clear();
    mValidCells = 0;

    if ( mInverseViewshed )
    {
        mCellEvents.reserve( mInputDem->height() * mInputDem->width() * 5 );
    }
    else
    {
        mCellEvents.reserve( mInputDem->height() * mInputDem->width() * 3 );
    }

    std::unique_ptr<QgsRasterBlock> rasterBlock( mInputDem->dataProvider()->block(
        mDefaultBand, mInputDem->extent(), mInputDem->width(), mInputDem->height() ) );

    bool isNoData = false;
    bool isMaskNoData = false;
    std::unique_ptr<QgsRasterBlock> maskBlock;

    bool solveCell;

    if ( mVisibilityMask )
    {
        maskBlock = std::unique_ptr<QgsRasterBlock>( mVisibilityMask->dataProvider()->block(
            mDefaultBand, mInputDem->extent(), mInputDem->width(), mInputDem->height() ) );
    }

    for ( int row = 0; row < rasterBlock->height(); row++ )
    {
        for ( int column = 0; column < rasterBlock->width(); column++ )
        {
            const double pixelValue = rasterBlock->valueAndNoData( row, column, isNoData );

            solveCell = true;
            if ( mVisibilityMask )
            {
                const double maskValue = maskBlock->valueAndNoData( row, column, isMaskNoData );
                if ( isMaskNoData || maskValue == 0 )
                {
                    solveCell = false;
                }
            }

            if ( !isNoData )
            {
                mValidCells++;
                addEventsFromCell( row, column, pixelValue, rasterBlock, solveCell );
            }
        }
    }

    mTimeInit = std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::high_resolution_clock::now() -
                                                                      startTime );
}

void AbstractViewshed::setMaximalDistance( double distance ) { mMaxDistance = distance; }

void AbstractViewshed::setAngles( double minAngle, double maxAngle )
{
    if ( !std::isnan( minAngle ) )
    {
        mMinAngle = minAngle * ( M_PI / 180 );

        if ( mMinAngle < -M_PI )
        {
            mMinAngle = -M_PI;
        }
    }

    if ( !std::isnan( maxAngle ) )
    {
        mMaxAngle = maxAngle * ( M_PI / 180 );

        if ( M_2_PI < mMaxAngle )
        {
            mMaxAngle = M_2_PI;
        }
    }

    if ( !std::isnan( mMinAngle ) && !std::isnan( mMaxAngle ) )
    {
        if ( mMinAngle > mMaxAngle )
        {
            mMinAngle = std::numeric_limits<double>::quiet_NaN();
            mMaxAngle = std::numeric_limits<double>::quiet_NaN();
        }
    }
}

bool AbstractViewshed::validAngles() { return !( std::isnan( mMinAngle ) || std::isnan( mMaxAngle ) ); }

bool AbstractViewshed::isInsideAngles( double eventEnterAngle, double eventExitAngle )
{
    if ( validAngles() )
    {
        if ( ( mMinAngle <= eventExitAngle && eventExitAngle <= mMaxAngle ) ||
             ( mMinAngle <= eventEnterAngle && eventEnterAngle <= mMaxAngle ) )
        {
            return true;
        }

        if ( mMinAngle < 0 && M_PI < eventEnterAngle )
        {
            if ( mMinAngle <= ( eventExitAngle - 2 * M_PI ) )
            {
                return true;
            }
        }

        return false;
    }

    return true;
}

void AbstractViewshed::sortEventList()
{
    std::chrono::_V2::system_clock::time_point startTime = std::chrono::_V2::high_resolution_clock::now();

    std::sort( mCellEvents.begin(), mCellEvents.end() );

    mTimeSort = std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::high_resolution_clock::now() -
                                                                      startTime );
}

void AbstractViewshed::parseEventList( std::function<void( int size, int current )> progressCallback )
{
    prepareMemoryRasters();

    std::chrono::_V2::system_clock::time_point startTime = std::chrono::_V2::high_resolution_clock::now();

    ViewshedValues rasterValues;

    int i = 0;
    for ( CellEvent e : mCellEvents )
    {
        progressCallback( mCellEvents.size(), i );

        switch ( e.eventType )
        {
            case CellEventPositionType::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( mPoint, &e, mCellSize );
                mLosNodes.push_back( mLoSNodeTemp );
                break;
            }
            case CellEventPositionType::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( e.row, e.col );

                std::vector<LoSNode>::iterator index = std::find( mLosNodes.begin(), mLosNodes.end(), mLoSNodeTemp );
                if ( index != mLosNodes.end() )
                {
                    mLosNodes.erase( index );
                }
                break;
            }
            case CellEventPositionType::CENTER:
            {
                mTotalLosNodesCount += mLosNodes.size();
                mNumberOfLos++;

                submitToThreadpool( e );

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
        mResults.at( j )->setValue( mAlgs->at( j )->pointValue(), mPoint->col, mPoint->row );
    }

    mTimeParse = std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::high_resolution_clock::now() -
                                                                       startTime );
}

void AbstractViewshed::parseCalculatedResults()
{
    mThreadPool.wait_for_tasks();

    for ( int j = 0; j < mResultPixels.size(); j++ )
    {
        setPixelData( mResultPixels[j].get() );
    }

    mResultPixels = BS::multi_future<ViewshedValues>();
}

void AbstractViewshed::setPixelData( ViewshedValues values )
{
    for ( int j = 0; j < values.values.size(); j++ )
    {
        mResults.at( j )->setValue( values.values.at( j ), values.col, values.row );
    }
}

void AbstractViewshed::extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                                   std::function<double( LoSNode )> func )
{
    MemoryRaster result = MemoryRaster( dem_, mDataType );

    int i = 0;
    for ( CellEvent event : mCellEvents )
    {
        if ( event.eventType == CellEventPositionType::CENTER )
        {
            mLoSNodeTemp = LoSNode( mPoint, &event, mCellSize );
            result.setValue( func( mLoSNodeTemp ), mLoSNodeTemp.col, mLoSNodeTemp.row );
        }
        i++;
    }

    result.save( fileName );
}

double AbstractViewshed::getCornerValue( const CellEventPosition &pos, const std::unique_ptr<QgsRasterBlock> &block,
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

std::shared_ptr<MemoryRaster> AbstractViewshed::resultRaster( int index ) { return mResults.at( index ); }

void AbstractViewshed::saveResults( std::string location, std::string fileNamePrefix = "" )
{
    saveResults( QString::fromStdString( location ), QString::fromStdString( fileNamePrefix ) );
};

void AbstractViewshed::saveResults( QString location, QString fileNamePrefix )
{
    for ( int i = 0; i < mAlgs->size(); i++ )
    {
        QString filePath = QString( "%1/%2" );

        QString fileName;

        if ( fileNamePrefix.isEmpty() )
        {
            fileName = QString( "%1.tif" ).arg( mAlgs->at( i )->name() );
        }
        else
        {
            fileName = QString( "%1 %2.tif" ).arg( fileNamePrefix, mAlgs->at( i )->name() );
        }
        mResults.at( i )->save( filePath.arg( location, fileName ) );
    }
}

QgsPoint AbstractViewshed::point( int row, int col )
{
    QgsPoint p = mInputDem->dataProvider()->transformCoordinates(
        QgsPoint( col + 0.5, row + 0.5 ), QgsRasterDataProvider::TransformType::TransformImageToLayer );

    return p;
}

LoSNode AbstractViewshed::statusNodeFromPoint( QgsPoint point )
{
    QgsPoint pointRaster = mInputDem->dataProvider()->transformCoordinates(
        point, QgsRasterDataProvider::TransformType::TransformLayerToImage );

    int row = pointRaster.y();
    int col = pointRaster.x();

    LoSNode ln;

    for ( CellEvent e : mCellEvents )
    {
        if ( e.eventType == CellEventPositionType::CENTER && e.col == col && e.row == row )
        {
            ln = LoSNode( mPoint, &e, mCellSize );
            return ln;
        }
    }

    return ln;
}

void AbstractViewshed::setMaxConcurentTaks( int maxTasks ) { mMaxNumberOfTasks = maxTasks; }

void AbstractViewshed::setMaxResultsInMemory( int maxResults ) { mMaxNumberOfResults = maxResults; }

void AbstractViewshed::setMaxThreads( int threads )
{
    if ( std::thread::hardware_concurrency() < threads )
    {
        threads = std::thread::hardware_concurrency();
    }

    mThreadPool.reset( threads );
}

std::vector<LoSNode> AbstractViewshed::prepareLoSWithPoint( QgsPoint point )
{

    LoSNode poi = statusNodeFromPoint( point );

    std::vector<LoSNode> losNodes;

    for ( CellEvent e : mCellEvents )
    {
        switch ( e.eventType )
        {
            case CellEventPositionType::ENTER:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                mLoSNodeTemp = LoSNode( mPoint, &e, mCellSize );
                losNodes.push_back( mLoSNodeTemp );
                break;
            }

            case CellEventPositionType::EXIT:
            {
                if ( mPoint->row == e.row && mPoint->col == e.col )
                {
                    break;
                }
                mLoSNodeTemp = LoSNode( e.row, e.col );
                std::vector<LoSNode>::iterator index = std::find( losNodes.begin(), losNodes.end(), mLoSNodeTemp );
                if ( index != losNodes.end() )
                {
                    losNodes.erase( index );
                }
                break;
            }

            case CellEventPositionType::CENTER:
            {
                mLoSNodeTemp = LoSNode( mPoint, &e, mCellSize );
                if ( mLoSNodeTemp.col == poi.col && mLoSNodeTemp.row == poi.row )
                {
                    return losNodes;
                }
                break;
            }
        }
    }
    return losNodes;
}