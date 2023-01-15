#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{
    class Utils
    {
      public:
        static void saveToCsv( std::vector<std::pair<double, double>> distanceElevation, std::string fileName );
        static std::vector<std::pair<double, double>> distanceElevation( SharedLoSNodeList los, LoSNode poi );
    };
} // namespace viewshed

#endif