#ifndef VIEWSHEDLIB_INVERSEVIEWSHED_H
#define VIEWSHEDLIB_INVERSEVIEWSHED_H

#include <functional>
#include <limits>

#include "abstractviewshed.h"
#include "enums.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "point.h"

namespace viewshed
{
    class InverseViewshed : public AbstractViewshed
    {
      public:
        InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset, std::shared_ptr<QgsRasterLayer> dem,
                         std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
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