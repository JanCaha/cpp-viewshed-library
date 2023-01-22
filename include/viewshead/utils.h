#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "los.h"
#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{
    class Utils
    {
      public:
        static void saveToCsv( std::vector<std::pair<double, double>> data, std::string header, std::string fileName );
        static std::vector<std::pair<double, double>> distanceElevation( std::shared_ptr<LoS> los );
        static std::vector<std::pair<double, double>> rasterCoordinates( std::shared_ptr<std::vector<LoSNode>> los,
                                                                         LoSNode poi );
    };
} // namespace viewshed

#endif