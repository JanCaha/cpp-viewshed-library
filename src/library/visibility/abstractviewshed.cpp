#include "abstractviewshed.h"
#include "threadtasks.h"
#include "visibility.h"

using viewshed::AbstractViewshed;
using viewshed::LoSNode;

void AbstractViewshed::prepareMemoryRasters()
{
    mResults->clear();
    mResults->reserve( mVisibilityIndices->size() );

    GDALDataType dataType = GDALDataType::GDT_Float64;

#if ( CELL_EVENT_USE_FLOAT )
    dataType = GDALDataType::GDT_Float32;
#endif

    for ( std::size_t i = 0; i < mVisibilityIndices->size(); i++ )
    {
        mResults->push_back( std::make_shared<SingleBandRaster>( *mInputDsm.get(), dataType ) );
    }
}

void AbstractViewshed::setDefaultResultDataType( GDALDataType dataType )
{
    mDataType = dataType;
    if ( !mResults->empty() )
    {
        prepareMemoryRasters();
    }
}

void AbstractViewshed::initEventList()
{
    std::chrono::_V2::steady_clock::time_point startTime = std::chrono::_V2::steady_clock::now();

    mCellEvents.clear();
    mLosNodes.clear();
    mValidCells = 0;
    mTotalLosNodesCount = 0;
    mNumberOfLos = 0;

    if ( mInverseViewshed )
    {
        mCellEvents.reserve( mInputDsm->height() * mInputDsm->width() * 5 );
    }
    else
    {
        mCellEvents.reserve( mInputDsm->height() * mInputDsm->width() * 3 );
    }

    bool isNoData = false;
    bool isMaskNoData = false;

    bool solveCell;

    for ( int row = 0; row < mInputDsm->height(); row++ )
    {
        for ( int column = 0; column < mInputDsm->width(); column++ )
        {
            isNoData = mInputDsm->isNoData( row, column );
            const double pixelValue = mInputDsm->value( row, column );

            solveCell = true;
            if ( mVisibilityMask )
            {
                isMaskNoData = mVisibilityMask->isNoData( row, column );
                const double maskValue = mVisibilityMask->value( row, column );

                if ( isMaskNoData || maskValue == 0 )
                {
                    solveCell = false;
                }
            }

            if ( !isNoData )
            {
                mValidCells++;
                addEventsFromCell( row, column, pixelValue, solveCell );
            }
        }
    }

    mTimeInit =
        std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::steady_clock::now() - startTime );
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

bool AbstractViewshed::isInsideAngles( const double &eventEnterAngle, const double &eventExitAngle )
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
    std::chrono::_V2::steady_clock::time_point startTime = std::chrono::_V2::steady_clock::now();

    std::sort( mCellEvents.begin(), mCellEvents.end() );

    mTimeSort =
        std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::steady_clock::now() - startTime );
}

void AbstractViewshed::parseEventList( std::function<void( int size, int current )> progressCallback )
{
    prepareMemoryRasters();

    std::chrono::_V2::steady_clock::time_point startTime = std::chrono::_V2::steady_clock::now();

    ViewshedValues rasterValues;

    std::size_t i = 0;
    for ( CellEvent e : mCellEvents )
    {
        progressCallback( mCellEvents.size(), i );

        switch ( e.mEventType )
        {
            case CellEventPositionType::ENTER:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                mLosNodes.push_back( mLoSNodeTemp );
                break;
            }
            case CellEventPositionType::EXIT:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }

                mLoSNodeTemp = LoSNode( e.mRow, e.mCol );

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

        if ( mMaxNumberOfTasks < mThreadPool.get_tasks_total() )
        {
            // parse result to rasters to avoid clutching in memory
            mThreadPool.wait_for_tasks();
        }

        i++;
    }

    // parse results left after the algorithm finished
    mThreadPool.wait_for_tasks();

    for ( int j = 0; j < mVisibilityIndices->size(); j++ )
    {
        mResults->at( j )->writeValue( mPoint->mRow, mPoint->mCol, mVisibilityIndices->at( j )->pointValue() );
    }

    mTimeParse =
        std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::_V2::steady_clock::now() - startTime );
}

void AbstractViewshed::setPixelData( ViewshedValues values )
{
    for ( int j = 0; j < values.values.size(); j++ )
    {
        mResults->at( j )->writeValue( values.mRow, values.mCol, values.values.at( j ) );
    }
}

void AbstractViewshed::extractValuesFromEventList( std::shared_ptr<ProjectedSquareCellRaster> dem_,
                                                   std::string fileName, std::function<double( LoSNode )> func )
{
    SingleBandRaster result = SingleBandRaster( *dem_.get() );

    std::size_t i = 0;
    for ( CellEvent event : mCellEvents )
    {
        if ( event.mEventType == CellEventPositionType::CENTER )
        {
            mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &event, mCellSize );
            result.writeValue( mLoSNodeTemp.mRow, mLoSNodeTemp.mCol, func( mLoSNodeTemp ) );
        }
        i++;
    }

    result.saveFile( fileName );
}

std::shared_ptr<SingleBandRaster> AbstractViewshed::resultRaster( int index ) { return mResults->at( index ); }

void AbstractViewshed::saveResults( std::string location, std::string fileNamePrefix )
{
    std::string filePath;
    std::string fileName;

    for ( std::size_t i = 0; i < mVisibilityIndices->size(); i++ )
    {
        if ( fileNamePrefix.empty() )
        {
            fileName = mVisibilityIndices->at( i )->name() + ".tif";
        }
        else
        {
            fileName = mVisibilityIndices->at( i )->name() + ".tif";
        }
        filePath = location + "/" + fileName;

        mResults->at( i )->saveFile( filePath );
    }
};

OGRPoint AbstractViewshed::point( int row, int col )
{
    double x, y;
    double rowD = row + 0.5;
    double colD = col + 0.5;
    mInputDsm->transformCoordinatesToWorld( rowD, colD, x, y );

    return OGRPoint( x, y );
}

LoSNode AbstractViewshed::statusNodeFromPoint( OGRPoint point )
{
    double x = point.getX();
    double y = point.getY();

    double rowD, colD;
    mInputDsm->transformCoordinatesToRaster( x, y, rowD, colD );

    int row = rowD;
    int col = colD;

    LoSNode ln;

    for ( CellEvent e : mCellEvents )
    {
        if ( e.mEventType == CellEventPositionType::CENTER && e.mCol == col && e.mRow == row )
        {
            ln = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
            return ln;
        }
    }

    return ln;
}

void AbstractViewshed::setMaxConcurentTaks( int maxTasks ) { mMaxNumberOfTasks = maxTasks; }

void AbstractViewshed::setMaxThreads( int threads )
{
    if ( std::thread::hardware_concurrency() < threads )
    {
        threads = std::thread::hardware_concurrency();
    }

    mThreadPool.reset( threads );
}

std::vector<LoSNode> AbstractViewshed::prepareLoSWithPoint( OGRPoint point )
{

    LoSNode poi = statusNodeFromPoint( point );

    std::vector<LoSNode> losNodes;

    for ( CellEvent e : mCellEvents )
    {
        switch ( e.mEventType )
        {
            case CellEventPositionType::ENTER:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }
                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                losNodes.push_back( mLoSNodeTemp );
                break;
            }

            case CellEventPositionType::EXIT:
            {
                if ( mPoint->mRow == e.mRow && mPoint->mCol == e.mCol )
                {
                    break;
                }
                mLoSNodeTemp = LoSNode( e.mRow, e.mCol );
                std::vector<LoSNode>::iterator index = std::find( losNodes.begin(), losNodes.end(), mLoSNodeTemp );
                if ( index != losNodes.end() )
                {
                    losNodes.erase( index );
                }
                break;
            }

            case CellEventPositionType::CENTER:
            {
                mLoSNodeTemp = LoSNode( mPoint->mRow, mPoint->mCol, &e, mCellSize );
                if ( mLoSNodeTemp.mCol == poi.mCol && mLoSNodeTemp.mRow == poi.mRow )
                {
                    return losNodes;
                }
                break;
            }
        }
    }
    return losNodes;
}