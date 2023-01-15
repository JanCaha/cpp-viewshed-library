#ifndef VIEWSHEDLIB_VIEWSHED_H
#define VIEWSHEDLIB_VIEWSHED_H

#include <cmath>
#include <limits>

#include <qgsrasterlayer.h>

#include "BS_thread_pool.hpp"

#include "enums.h"
#include "event.h"
#include "iviewshed.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "memoryraster.h"
#include "points.h"
#include "position.h"
#include "statusnode.h"
#include "viewshedvalues.h"

namespace viewshed
{
    // typedef std::vector<Event> EventList;
    // typedef std::vector<StatusNode> StatusList;
    // typedef std::shared_ptr<std::vector<StatusNode>> SharedStatusList;
    // typedef std::vector<std::shared_ptr<IViewshedAlgorithm>> ViewshedAlgorithms;

    class Viewshed : public IViewshed
    {
      public:
        Viewshed( std::shared_ptr<IPoint> point, std::shared_ptr<QgsRasterLayer> dem, ViewshedAlgorithms algs,
                  double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                  double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} );

        SharedStatusList LoSToPoint( QgsPoint point, bool onlyToPoint = false );

      private:
        SharedStatusList getLoS( StatusNode poi, bool onlyToPoi = false );
    };

} // namespace viewshed

#endif