#ifndef VIEWSHEDLIB_IVIEWSHED_H
#define VIEWSHEDLIB_IVIEWSHED_H

#include <limits>

#include "qgspoint.h"
#include "qgsrasterlayer.h"

#include "BS_thread_pool.hpp"

#include "cellevent.h"
#include "celleventposition.h"
#include "iviewshedalgorithm.h"
#include "los.h"
#include "losnode.h"
#include "memoryraster.h"
#include "viewshedvalues.h"

using viewshed::LoS;

namespace viewshed
{
    class IViewshed
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

        virtual void submitToThreadpool( CellEvent &e ) = 0;

        std::shared_ptr<MemoryRaster> resultRaster( int index = 0 );

        void saveResults( QString location, QString fileNamePrefix = QString() );
        LoSNode statusNodeFromPoint( QgsPoint point );
        QgsPoint point( int row, int col );

      protected:
        std::vector<LoSNode> mLosNodes;
        std::vector<CellEvent> mCellEvents;
        std::shared_ptr<QgsRasterLayer> mInputDem;
        std::shared_ptr<Point> mPoint;
        std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> mAlgs;
        Qgis::DataType mDataType = Qgis::DataType::Float64;
        int mDefaultBand = 1;

        double mMaxDistance = std::numeric_limits<double>::max();
        double mMinAngle = std::numeric_limits<double>::quiet_NaN();
        double mMaxAngle = std::numeric_limits<double>::quiet_NaN();
        int mMaxNumberOfTasks = 100;
        int mMaxNumberOfResults = 500;
        double mCellSize;
        double mValid;

        LoSNode mLosNodePoint;

        std::vector<LoSNode> prepareLoSWithPoint( QgsPoint point );
        bool validAngles();
        void prepareMemoryRasters();
        void setPixelData( ViewshedValues values );
        void parseCalculatedResults();

        BS::thread_pool mThreadPool;
        BS::multi_future<ViewshedValues> mResultPixels;

        std::vector<std::shared_ptr<MemoryRaster>> mResults;

        double getCornerValue( const CellEventPosition &pos, const std::unique_ptr<QgsRasterBlock> &block,
                               double defaultValue );
    };

} // namespace viewshed

#endif