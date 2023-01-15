#ifndef VIEWSHEDLIB_IVIEWSHED_H
#define VIEWSHEDLIB_IVIEWSHED_H

#include "qgspoint.h"
#include "qgsrasterlayer.h"

#include "BS_thread_pool.hpp"

#include "cellevent.h"
#include "iviewshedalgorithm.h"
#include "memoryraster.h"
#include "position.h"
#include "losnode.h"
#include "viewshedvalues.h"

namespace viewshed
{
    typedef std::vector<CellEvent> EventList;
    typedef std::vector<LoSNode> StatusList;
    typedef std::shared_ptr<std::vector<LoSNode>> SharedStatusList;
    typedef std::vector<std::shared_ptr<IViewshedAlgorithm>> ViewshedAlgorithms;

    class IViewshed
    {
      public:
        void initEventList();
        void sortEventList();
        void prefillStatusList();
        void parseEventList( std::function<void( int, int )> progressCallback = []( int, int ) {} );
        void extractValuesFromEventList();
        void extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                         std::function<double( LoSNode )> func );
        bool checkInsideAngle( double eventEnterAngle, double eventExitAngle );

        void setMaximalDistance( double distance );
        void setAngles( double minAngle, double maxAngle );

        std::shared_ptr<MemoryRaster> resultRaster( int index = 0 );

        void saveResults( QString location );
        LoSNode statusNodeFromPoint( QgsPoint point );

      protected:
        EventList viewPointRowEventList;
        StatusList statusList;
        EventList eventList;
        std::shared_ptr<QgsRasterLayer> mInputDem;
        std::shared_ptr<IPoint> mPoint;
        ViewshedAlgorithms mAlgs;
        Qgis::DataType dataType = Qgis::DataType::Float64;
        int mDefaultBand = 1;

        double mMinAngle = -360.0;
        double mMaxAngle = 720.0;
        double mMaxDistance = std::numeric_limits<double>::max();
        double mCellSize;
        double mValid;

        int mMaxNumberOfTasks = 100;
        int mMaxNumberOfResults = 500;

        void prepareMemoryRasters();
        void setPixelData( ViewshedValues values );
        void parseCalculatedResults();

        BS::thread_pool mThreadPool;
        BS::multi_future<ViewshedValues> mResultPixels;

        std::vector<std::shared_ptr<MemoryRaster>> mResults;

        double getCornerValue( const Position &pos, const std::unique_ptr<QgsRasterBlock> &block, double defaultValue );
    };

} // namespace viewshed

#endif