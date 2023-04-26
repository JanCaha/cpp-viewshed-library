#ifndef VIEWSHEDLIB_INVERSEVIEWSHED_H
#define VIEWSHEDLIB_INVERSEVIEWSHED_H

#include <functional>
#include <limits>

#include "abstractviewshed.h"
#include "enums.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "point.h"
#include "visibility.h"

namespace viewshed
{
    class InverseViewshed : public AbstractViewshed
    {
      public:
        InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset, std::shared_ptr<QgsRasterLayer> dem,
                         std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> visibilityIndices,
                         bool applyCurvatureCorrections = true, double earthDiameter = EARTH_DIAMETER,
                         double refractionCoeff = REFRACTION_COEFFICIENT,
                         double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                         double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} ) override;

        std::shared_ptr<InverseLoS> getLoS( QgsPoint point, bool onlyToPoint = false );

        void submitToThreadpool( CellEvent &e ) override;

        void addEventsFromCell( int &row, int &column, const double &pixelValue,
                                std::unique_ptr<QgsRasterBlock> &rasterBlock, bool &solveCell ) override;

      private:
        double mObserverOffset;
    };

} // namespace viewshed

#endif