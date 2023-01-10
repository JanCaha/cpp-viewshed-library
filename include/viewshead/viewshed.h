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

class Viewshed
{
  public:
    Viewshed( std::shared_ptr<ViewPoint> vp_, std::shared_ptr<QgsRasterLayer> dem_,
              std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
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

  private:
    std::vector<Event> viewPointRowEventList;
    std::vector<StatusNode> statusList;
    std::vector<Event> eventList;
    std::shared_ptr<QgsRasterLayer> mInputDem;
    std::shared_ptr<ViewPoint> mVp;
    std::vector<std::shared_ptr<IViewshedAlgorithm>> mAlgs;
    Qgis::DataType dataType = Qgis::DataType::Float64;
    int mDefaultBand = 1;

    double mMinAngle = -360.0;
    double mMaxAngle = 720.0;
    double mMaxDistance = std::numeric_limits<double>::max();
    double mCellSize;
    double mValid;

    BS::thread_pool mThreadPool;
    BS::multi_future<ViewshedValues> mResultPixels;

    std::vector<std::shared_ptr<MemoryRaster>> mResults;

    double getCornerValue( const Position &pos, const std::unique_ptr<QgsRasterBlock> &block, double defaultValue );
};

ViewshedValues taskVisibility( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                               std::vector<StatusNode> statusList, std::shared_ptr<StatusNode> poi,
                               std::shared_ptr<ViewPoint> vp );
