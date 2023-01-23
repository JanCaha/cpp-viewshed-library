#ifndef VIEWSHEDLIB_INVERSEVIEWSHED_H
#define VIEWSHEDLIB_INVERSEVIEWSHED_H

#include <functional>
#include <limits>

#include "enums.h"
#include "iviewshed.h"
#include "los.h"
#include "losevaluator.h"
#include "points.h"

namespace viewshed
{
    class InverseViewshed : public IViewshed
    {
      public:
        InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset, std::shared_ptr<QgsRasterLayer> dem,
                         std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs,
                         double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                         double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} );

        std::shared_ptr<InverseLoS> getLoS( QgsPoint point, bool onlyToPoint = false );

        void submitToThreadpool( CellEvent &e ) override;

        void initEventList();

      private:
        double mObserverOffset;
    };

} // namespace viewshed

#endif