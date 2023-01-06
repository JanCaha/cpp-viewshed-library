#include <cmath>
#include <limits>

#include <qgsrasterlayer.h>

#include "enums.h"
#include "event.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "memoryraster.h"
#include "points.h"
#include "position.h"
#include "statusnode.h"

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
    void parseEventList();
    void extractValuesFromEventList();
    void extractValuesFromEventList( std::shared_ptr<QgsRasterLayer> dem_, QString fileName,
                                     std::function<double( StatusNode )> func );

    bool checkInsideAngle( double eventEnterAngle, double eventExitAngle );

    std::shared_ptr<MemoryRaster> resultRaster( int index = 0 );

    void saveResults( QString location );
    void setMaximalDistance( double distance );

  private:
    LoSEvaluator mLosEvaluator;
    std::vector<Event> viewPointRowEventList;
    std::vector<StatusNode> statusList;
    std::vector<Event> eventList;
    std::shared_ptr<QgsRasterLayer> mInputDem;
    std::shared_ptr<ViewPoint> mVp;
    Qgis::DataType dataType = Qgis::DataType::Float64;
    int mDefaultBand = 1;

    double mMinAngle = -360.0;
    double mMaxAngle = 720.0;
    double mMaxDistance = std::numeric_limits<double>::max();
    double mCellSize;
    double mValid;

    std::vector<std::shared_ptr<MemoryRaster>> mResults;

    double getCornerValue( const Position &pos, const std::unique_ptr<QgsRasterBlock> &block, double defaultValue );
};
