#pragma once
#include "viewshed_export.h"

#include <limits>

#include "BS_thread_pool.hpp"

#include "simplerasters.h"

#include "abstractviewshedalgorithm.h"
#include "cellevent.h"
#include "celleventposition.h"
#include "defaultdatatypes.h"
#include "los.h"
#include "losnode.h"
#include "viewshedtypes.h"
#include "viewshedvalues.h"
#include "visibility.h"

using viewshed::LoS;
using viewshed::ViewshedAlgorithms;

namespace viewshed
{
    /**
     * @brief Abstract class that represents viewshed calculation from this class specific implementations ( \a Viewshed
     * and \a InverseViewshed) are derived. Class provides all the base functions for those algorithms, including
     * implementation of plane sweep algorithm for viewshed according to Van Kreveld(1996) and Haverkort et al(2007), as
     implemented in GRASS GIS described by Toma et al. (2022).
     *
     * VAN KREVELD, M. Variations on Sweep Algorithms: efficient computation of extended viewsheds and class intervals.
     * Utrecht: Utrecht University: Information and Computing Sciences, 1996.
     *
     * HAVERKORT, H., TOMA, L., ZHUANG, Y. Computing Visibility on Terrains in External Memory. ACM Journal
     * on Experimental Algorithmics, 13, 2009.
     *
     * TOMA, L., ZHUANG, Y., RICHARD, W., METZ, M.  Grass gis manual: r.viewshed, 2022.
     * https://grass.osgeo.org/grass82/manuals/r.viewshed.html.
     */
    class DLL_API AbstractViewshed
    {
      public:
        /**
         * @brief Create event list for line sweep algorithm from input raster.
         *
         */
        void initEventList();

        /**
         * @brief  Sort event list for parsing.
         *
         */
        void sortEventList();

        /**
         * @brief Parse event list by individual events.
         *
         * @param progressCallback Call function with arguments of elements done from total elements (both as
         * intereger).
         */
        void parseEventList( std::function<void( int, int )> progressCallback = []( int, int ) {} );

        void extractValuesFromEventList();
        void extractValuesFromEventList( std::shared_ptr<ProjectedSquareCellRaster> dem_, std::string fileName,
                                         std::function<double( LoSNode )> func );

        /**
         * @brief Checks if cell with these enter and exit angles falls inside the angle limit.
         *
         * @param eventEnterAngle
         * @param eventExitAngle
         * @return true
         * @return false
         */
        bool isInsideAngles( const double &eventEnterAngle, const double &eventExitAngle );

        /**
         * @brief Set the Maximal Distance limit value. Limits event list creation to cells within this distance limit.
         * Value is in the same units as CRS of input raster.
         *
         * @param distance
         */
        void setMaximalDistance( double distance );

        /**
         * @brief Set horizontal angle limits for viewshed calculation.
         *0
         * @param minAngle
         * @param maxAngle
         */
        void setAngles( double minAngle, double maxAngle );

        /**
         * @brief Set the number of maximum Concurent Taks that can be prepared in thread pool for processing.
         *
         * @param maxTasks
         */
        void setMaxConcurentTaks( int maxTasks );

        /**
         * @brief Set the maximal number of threads that can be used for calculation.
         *
         * @param threads
         */
        void setMaxThreads( int threads );

        /**
         * @brief Initialized output rasters in memory.
         *
         */
        void prepareMemoryRasters();

        /**
         * @brief Calculate the viewshed. Covers all the steps - create list of events, sort list of events, parse list
         * of events.
         *
         * @param stepsTimingCallback Callback function outputing message how long individual step takes.
         * @param progressCallback Callback function outputing processes part of event list.
         */
        virtual void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time ) {},
            std::function<void( int, int )> progressCallback = []( int, int ) {} ) = 0;

        /**
         * @brief Function that creates events for event list from given cell.
         *
         * @param row
         * @param column
         * @param pixelValue
         * @param rasterBlock
         * @param solveCell Based on mask value, information whether center event for this cell should be created.
         */
        virtual void addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell ) = 0;

        /**
         * @brief Submit LoS with this \a CellEvent to threadpool for solving.
         *
         * @param e
         */
        virtual void submitToThreadpool( CellEvent &e ) = 0;

        /**
         * @brief Extract individual result raster.
         *
         * @param index
         * @return std::shared_ptr<SingleBandRaster>
         */
        std::shared_ptr<SingleBandRaster> resultRaster( int index = 0 );

        /**
         * @brief Save all result rasters into the folder, optionally using name prefix.
         *
         * @param location
         * @param fileNamePrefix
         */
        void saveResults( std::string location, std::string fileNamePrefix = "" );

        /**
         * @brief Create \a LoSNode from \a OGRPoint.
         *
         * @param point
         * @return LoSNode
         */
        LoSNode statusNodeFromPoint( OGRPoint point );

        /**
         * @brief Create \a OGRPoint with coordinates of given row and column in the raster.
         *
         * @param row
         * @param col
         * @return OGRPoint
         */
        OGRPoint point( int row, int col );

        /**
         * @brief Set the Default Result Data Type to use in output rasters.
         *
         * @param dataType
         */
        void setDefaultResultDataType( GDALDataType dataType );

        /**
         * @brief Number of valid cells in raster - cells not containing no data.
         *
         * @return long
         */
        long numberOfValidCells() { return mValidCells; };

        /**
         * @brief Number of events in event list.
         *
         * @return long
         */
        long numberOfCellEvents() { return mCellEvents.size(); };

        /**
         * @brief Set the Visibility Mask for viewshed calculation.
         *
         * @param mask
         */
        void setVisibilityMask( std::shared_ptr<ProjectedSquareCellRaster> mask ) { mVisibilityMask = mask; }

        /**
         * @brief Overall number of solved LoSNodes in all evaluated LoS.
         *
         * @return long long
         */
        long long totalCountOfLoSNodes() { return mTotalLosNodesCount; };

        /**
         * @brief Size of cell event list in bytes.
         *
         * @return long long
         */
        long long sizeOfEvents() { return sizeof( CellEvent ) * mCellEvents.size(); };

        /**
         * @brief Overall size of all solved LoS in bytes.
         *
         * @return long long
         */
        long long totalSizeOfLoS() { return sizeof( LoSNode ) * mTotalLosNodesCount; };

        /**
         * @brief Mean size of solved LoS in bytes.
         *
         * @return long long
         */
        long long meanSizeOfLoS() { return ( totalSizeOfLoS() ) / mNumberOfLos; };

        /**
         * @brief Number of seconds the creation of event list lasted.
         *
         * @return double
         */
        double initLastedSeconds() { return mTimeInit.count() / (double)1e9; }

        /**
         * @brief Number of seconds the sort of event list lasted.
         *
         * @return double
         */
        double sortLastedSeconds() { return mTimeSort.count() / (double)1e9; }

        /**
         * @brief Number of seconds the parsing of event list lasted.
         *
         * @return double
         */
        double parseLastedSeconds() { return mTimeParse.count() / (double)1e9; }

        /**
         * @brief Number of seconds the calculation of viewshed indexes lasted.
         *
         * @return double
         */
        double processingLastedSeconds() { return initLastedSeconds() + sortLastedSeconds() + parseLastedSeconds(); }

        /**
         * @brief Size in bytes that one output raster takes.
         *
         * @return long long
         */
        long long sizeOfOutputRaster()
        {
            if ( mResults->size() > 0 )
            {
                return mResults->at( 0 )->dataSize();
            }
            return 0;
        }

        /**
         * @brief Number of output rasters. Equals number of input algorithms.
         *
         * @return int
         */
        int numberOfResultRasters() { return mResults->size(); }

        /**
         * @brief Size in bytes that all output raster takes.
         *
         * @return long long
         */
        long long sizeOfOutputRasters() { return mVisibilityIndices->size() * sizeOfOutputRaster(); }

        /**
         * @brief Extract individual cell event from cell events.
         *
         * @param i
         * @return CellEvent
         */
        CellEvent cellEvent( size_t i ) { return mCellEvents.at( i ); }

        /**
         * @brief Calculate visibility mask of areas which are visible from point (visibility), or from which the points
         * is visible (inverse visibility). The visibility raster is calculated using fast algorithm, significantly
         * faster than algorithms with visibility indices.
         *
         */
        virtual void calculateVisibilityRaster() = 0;

        /**
         * @brief Save visibility raster to file.
         *
         * @param filePath
         */
        void saveVisibilityRaster( std::string filePath );

        /**
         * @brief Calculate visibility mask raster.
         *
         */
        void calculateVisibilityMask();

      protected:
        /**
         * @brief LoSNodes in currently solved LoS while parsing event list.
         *
         */
        std::vector<LoSNode> mLosNodes;

        /**
         * @brief Event list. All cell events for the input raster.
         *
         */
        std::vector<CellEvent> mCellEvents;

        /**
         * @brief Input raster with digital surface model.
         *
         */
        std::shared_ptr<ProjectedSquareCellRaster> mInputDsm;

        /**
         * @brief Input raster with visibility calculation mask.
         *
         */
        std::shared_ptr<ProjectedSquareCellRaster> mVisibilityMask = nullptr;

        /**
         * @brief Important point for visibility calculation. Either viewpoint (normal viewshed) or target point
         * (inverse viewshed).
         *
         */
        std::shared_ptr<Point> mPoint;

        /**
         * @brief Visibility indexes to calculate while calculating this viewshed. As \a AbstractViewshedAlgorithm.
         *
         */
        std::shared_ptr<ViewshedAlgorithms> mVisibilityIndices;

        /**
         * @brief Data type of output rasters.
         *
         */
        GDALDataType mDataType = OUTPUT_RASTER_DATA_TYPE;

        /**
         * @brief Default band to read from rasters.
         *
         */
        int mDefaultBand = 1;
        long mValidCells = 0;
        long mTotalLosNodesCount = 0;
        long mNumberOfLos = 0;

        bool mInverseViewshed = false;

        double mMaxDistance = std::numeric_limits<double>::max();
        double mMinAngle = std::numeric_limits<double>::quiet_NaN();
        double mMaxAngle = std::numeric_limits<double>::quiet_NaN();

        /**
         * @brief Maximal number of LoS to solve, that can be stored in threadpool.
         *
         */
        int mMaxNumberOfTasks = 100;

        double mCellSize;
        double mValid;
        bool mCurvatureCorrections = false;
        double mEarthDiameter = EARTH_DIAMETER;
        double mRefractionCoefficient = REFRACTION_COEFFICIENT;

        LoSNode mLosNodePoint;

        std::vector<LoSNode> prepareLoSWithPoint( OGRPoint point );
        bool validAngles();

        /**
         * @brief Threadpool that takes care of preparing individual threads with LoS to be solved.
         *
         */
        BS::thread_pool mThreadPool;

        std::shared_ptr<ResultRasters> mResults = std::make_shared<ResultRasters>();
        std::shared_ptr<ProjectedSquareCellRaster> mVisibilityRaster = nullptr;

        std::chrono::nanoseconds mTimeInit;
        std::chrono::nanoseconds mTimeSort;
        std::chrono::nanoseconds mTimeParse;

        double mCellElevs[3];
        double mAngleCenter, mAngleEnter, mAngleExit;
        double mEventDistance;
        CellEvent mEventCenter, mEventEnter, mEventExit = CellEvent();
        CellEvent mEventEnterOpposite, mEventExitOpposite = CellEvent();
        double mOppositeAngleEnter, mOppositeAngleExit;
        LoSNode mLoSNodeTemp = LoSNode();
    };

} // namespace viewshed
