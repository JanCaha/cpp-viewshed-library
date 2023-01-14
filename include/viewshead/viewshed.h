#ifndef VIEWSHEDLIB_VIEWSHED_H
#define VIEWSHEDLIB_VIEWSHED_H

#include <cmath>
#include <limits>

#include <qgsrasterlayer.h>

#include "BS_thread_pool.hpp"

#include "enums.h"
#include "event.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "memoryraster.h"
#include "points.h"
#include "position.h"
#include "statusnode.h"
#include "viewshedvalues.h"

typedef std::vector<Event> EventList;
typedef std::vector<StatusNode> StatusList;
typedef std::shared_ptr<std::vector<StatusNode>> SharedStatusList;
typedef std::vector<std::shared_ptr<IViewshedAlgorithm>> ViewshedAlgorithms;

class Viewshed
{
  public:
    Viewshed( std::shared_ptr<ViewPoint> vp_, std::shared_ptr<QgsRasterLayer> dem_, ViewshedAlgorithms algs,
              double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
              double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

    void initEventList();
    void sortEventList();
    void prefillStatusList();
    void parseEventList( std::function<void( int, int )> progressCallback = []( int, int ) {} );
    void extractValuesFromEventList();
    void extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                     std::function<double( StatusNode )> func );

    void calculate(
        std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
        { qDebug() << QString::fromStdString( text ) << time; },
        std::function<void( int, int )> progressCallback = []( int, int ) {} );

    bool checkInsideAngle( double eventEnterAngle, double eventExitAngle );

    std::shared_ptr<MemoryRaster> resultRaster( int index = 0 );

    void saveResults( QString location );
    void setMaximalDistance( double distance );
    SharedStatusList LoSToPoint( QgsPoint point, bool onlyToPoint = false );
    StatusNode statusNodeFromPoint( QgsPoint point );

  private:
    EventList viewPointRowEventList;
    StatusList statusList;
    EventList eventList;
    std::shared_ptr<QgsRasterLayer> mInputDem;
    std::shared_ptr<ViewPoint> mVp;
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

    SharedStatusList getLoS( StatusNode poi, bool onlyToPoi = false );
};

ViewshedValues taskVisibility( ViewshedAlgorithms algs, std::vector<StatusNode> statusList,
                               std::shared_ptr<StatusNode> poi, std::shared_ptr<ViewPoint> vp );

#endif