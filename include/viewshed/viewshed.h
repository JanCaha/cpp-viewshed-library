#pragma once

#include <functional>
#include <iostream>
#include <limits>

#include "abstractviewshed.h"
#include "enums.h"
#include "los.h"
#include "losevaluator.h"
#include "point.h"
#include "viewshedtypes.h"
#include "visibility.h"

using viewshed::ViewshedAlgorithms;

namespace viewshed
{
    class Viewshed : public AbstractViewshed
    {
      public:
        Viewshed( std::shared_ptr<Point> viewPoint, std::shared_ptr<ProjectedSquareCellRaster> dem,
                  std::shared_ptr<ViewshedAlgorithms> visibilityIndices, bool applyCurvatureCorrections = true,
                  double earthDiameter = EARTH_DIAMETER, double refractionCoeff = REFRACTION_COEFFICIENT,
                  double minimalAngle = std::numeric_limits<double>::quiet_NaN(),
                  double maximalAngle = std::numeric_limits<double>::quiet_NaN() );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { std::cout << text << time << std::endl; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} ) override;

        std::shared_ptr<LoS> getLoS( OGRPoint point, bool onlyToPoint = false );

        void addEventsFromCell( int &row, int &column, const double &pixelValue, bool &solveCell ) override;

        void submitToThreadpool( CellEvent &e ) override;

        void calculateVisibilityRaster() override;
    };

} // namespace viewshed
