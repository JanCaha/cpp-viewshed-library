#ifndef VIEWSHEDLIB_VIEWSHED_H
#define VIEWSHEDLIB_VIEWSHED_H

#include <cmath>
#include <limits>

#include <qgsrasterlayer.h>

#include "BS_thread_pool.hpp"

#include "cellevent.h"
#include "enums.h"
#include "iviewshed.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "losnode.h"
#include "memoryraster.h"
#include "points.h"
#include "rasterposition.h"
#include "viewshedvalues.h"

namespace viewshed
{
    class Viewshed : public IViewshed
    {
      public:
        Viewshed( std::shared_ptr<IPoint> point, std::shared_ptr<QgsRasterLayer> dem,
                  std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs,
                  double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                  double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} );

        std::shared_ptr<std::vector<LoSNode>> LoSToPoint( QgsPoint point, bool onlyToPoint = false );

      private:
        std::shared_ptr<std::vector<LoSNode>> getLoS( LoSNode poi, bool onlyToPoi = false );
    };

} // namespace viewshed

#endif