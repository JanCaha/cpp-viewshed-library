#ifndef VIEWSHEDLIB_VIEWSHED_H
#define VIEWSHEDLIB_VIEWSHED_H

#include <functional>
#include <limits>

#include "abstractviewshed.h"
#include "enums.h"
#include "los.h"
#include "losevaluator.h"
#include "point.h"
#include "visibility.h"

namespace viewshed
{
    class Viewshed : public AbstractViewshed
    {
      public:
        Viewshed( std::shared_ptr<Point> viewPoint, std::shared_ptr<QgsRasterLayer> dem,
                  std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                  bool applyCurvatureCorrections = true, double earthDiameter = EARTH_DIAMETER,
                  double reffractionCoeff = REFRACTION_COEFFICIENT,
                  double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                  double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} );

        std::shared_ptr<LoS> getLoS( QgsPoint point, bool onlyToPoint = false );

        void submitToThreadpool( CellEvent &e ) override;
    };

} // namespace viewshed

#endif