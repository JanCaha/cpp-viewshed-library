#pragma once
#include "viewshed_export.h"

#include <functional>
#include <iostream>
#include <limits>

#include "abstractviewshed.h"
#include "enums.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "point.h"
#include "viewshedtypes.h"
#include "visibility.h"

using viewshed::ViewshedAlgorithms;

namespace viewshed
{
    class DLL_API InverseViewshed : public AbstractViewshed
    {
      public:
        InverseViewshed( std::shared_ptr<Point> targetPoint, double observerOffset,
                         std::shared_ptr<ProjectedSquareCellRaster> dem,
                         std::shared_ptr<ViewshedAlgorithms> visibilityIndices, bool applyCurvatureCorrections = true,
                         double earthDiameter = EARTH_DIAMETER, double refractionCoeff = REFRACTION_COEFFICIENT,
                         double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                         double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { std::cout << text << time << std::endl; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} ) override;

        std::shared_ptr<InverseLoS> getLoS( OGRPoint point, bool onlyToPoint = false );

        void submitToThreadpool( const CellEvent &e ) override;

        void addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell ) override;

        void calculateVisibilityRaster() override;

      private:
        double mObserverOffset;
    };

} // namespace viewshed
