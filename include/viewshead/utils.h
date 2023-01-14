#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "statusnode.h"
#include "viewshed.h"

class Utils
{
  public:
    static void saveToCsv( std::vector<std::pair<double, double>> distanceElevation, std::string fileName );
    static std::vector<std::pair<double, double>> distanceElevation( SharedStatusList los, StatusNode poi );
};

#endif