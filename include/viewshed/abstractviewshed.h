#ifndef VIEWSHEDLIB_ABSTRACTVIEWSHED_H
#define VIEWSHEDLIB_ABSTRACTVIEWSHED_H

#include <limits>

#include "qgspoint.h"
#include "qgsrasterlayer.h"

#include "BS_thread_pool.hpp"

#include "abstractviewshedalgorithm.h"
#include "cellevent.h"
#include "celleventposition.h"
#include "defaultdatatypes.h"
#include "los.h"
#include "losnode.h"
#include "memoryraster.h"
#include "viewshedvalues.h"
#include "visibility.h"

using viewshed::LoS;

namespace viewshed
{
    class AbstractViewshed
    {
      public:
        void initEventList();
        void sortEventList();
        void parseEventList( std::function<void( int, int )> progressCallback = []( int, int ) {} );
        void extractValuesFromEventList();
        void extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                         std::function<double( LoSNode )> func );
        bool isInsideAngles( double eventEnterAngle, double eventExitAngle );

        void setMaximalDistance( double distance );
        void setAngles( double minAngle, double maxAngle );
        void setMaxConcurentTaks( int maxTasks );
        void setMaxResultsInMemory( int maxResults );
        void setMaxThreads( int threads );

        void prepareMemoryRasters();

        virtual void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time ) {},
            std::function<void( int, int )> progressCallback = []( int, int ) {} ) = 0;

        virtual void addEventsFromCell( int &row, int &column, const double &pixelValue,
                                        std::unique_ptr<QgsRasterBlock> &rasterBlock, bool &solveCell ) = 0;

        virtual void submitToThreadpool( CellEvent &e ) = 0;

        std::shared_ptr<MemoryRaster> resultRaster( int index = 0 );

        void saveResults( QString location, QString fileNamePrefix = QString() );

        void saveResultsStdString( std::string location, std::string fileNamePrefix = "" );

        LoSNode statusNodeFromPoint( QgsPoint point );
        QgsPoint point( int row, int col );

        void setDefaultResultDataType( Qgis::DataType dataType );

        long numberOfValidCells() { return mValidCells; };

        long numberOfCellEvents() { return mCellEvents.size(); };

        void setVisibilityMask( std::shared_ptr<QgsRasterLayer> mask ) { mVisibilityMask = mask; }

        long long totalCountOfLoSNodes() { return mTotalLosNodesCount; };

        long long sizeOfEvents() { return sizeof( CellEvent ) * mCellEvents.size(); };

        long long totalSizeOfLoS() { return sizeof( LoSNode ) * mTotalLosNodesCount; };

        long long meanSizeOfLoS() { return ( totalSizeOfLoS() ) / mNumberOfLos; };

        double initLastedSeconds() { return mTimeInit.count() / (double)1e9; }

        double sortLastedSeconds() { return mTimeSort.count() / (double)1e9; }

        double parseLastedSeconds() { return mTimeParse.count() / (double)1e9; }

        double processingLastedSeconds() { return initLastedSeconds() + sortLastedSeconds() + parseLastedSeconds(); }

        long long sizeOfOutputRaster()
        {
            if ( mResults.size() > 0 )
            {
                return mResults[0]->dataSize();
            }
            return 0;
        }

        int numberOfResultRasters() { return mResults.size(); }

        long long sizeOfOutputRasters() { return mAlgs->size() * sizeOfOutputRaster(); }

      protected:
        std::vector<LoSNode> mLosNodes;
        std::vector<CellEvent> mCellEvents;
        std::shared_ptr<QgsRasterLayer> mInputDem;
        std::shared_ptr<QgsRasterLayer> mVisibilityMask = nullptr;
        std::shared_ptr<Point> mPoint;
        std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> mAlgs;
        Qgis::DataType mDataType = OUTPUT_RASTER_DATA_TYPE;
        int mDefaultBand = 1;
        long mValidCells = 0;
        long mTotalLosNodesCount = 0;
        long mNumberOfLos = 0;

        bool mInverseViewshed = false;

        double mMaxDistance = std::numeric_limits<double>::max();
        double mMinAngle = std::numeric_limits<double>::quiet_NaN();
        double mMaxAngle = std::numeric_limits<double>::quiet_NaN();
        int mMaxNumberOfTasks = 100;
        int mMaxNumberOfResults = 500;
        double mCellSize;
        double mValid;
        bool mCurvatureCorrections = false;
        double mEarthDiameter = EARTH_DIAMETER;
        double mRefractionCoefficient = REFRACTION_COEFFICIENT;

        LoSNode mLosNodePoint;

        std::vector<LoSNode> prepareLoSWithPoint( QgsPoint point );
        bool validAngles();
        void setPixelData( ViewshedValues values );
        void parseCalculatedResults();

        BS::thread_pool mThreadPool;
        BS::multi_future<ViewshedValues> mResultPixels;

        std::vector<std::shared_ptr<MemoryRaster>> mResults;

        std::chrono::nanoseconds mTimeInit;
        std::chrono::nanoseconds mTimeSort;
        std::chrono::nanoseconds mTimeParse;

        double getCornerValue( const CellEventPosition &pos, const std::unique_ptr<QgsRasterBlock> &block,
                               double defaultValue );

        double mCellElevs[3];
        double mAngleCenter, mAngleEnter, mAngleExit;
        double mEventDistance;
        CellEvent mEventCenter, mEventEnter, mEventExit = CellEvent();
        CellEvent mEventEnterOpposite, mEventExitOpposite = CellEvent();
        double mOppositeAngleEnter, mOppositeAngleExit;
        LoSNode mLoSNodeTemp = LoSNode();
    };

} // namespace viewshed

#endif