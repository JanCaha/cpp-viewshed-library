#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "abstractlos.h"
#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{

    struct DataTriplet
    {
        DataTriplet( double f, double s, bool t )
        {
            first = f;
            second = s;
            third = t;
        };

        double first;
        double second;
        bool third;
    };

    class Utils
    {
      public:
        static void saveToCsv( std::vector<DataTriplet> data, std::string header, std::string fileName );

        static std::vector<DataTriplet> distanceElevation( std::shared_ptr<AbstractLoS> los );

        static std::vector<std::pair<double, double>> rasterCoordinates( std::shared_ptr<std::vector<LoSNode>> los,
                                                                         LoSNode poi );
        static double earthDiameter( QgsCoordinateReferenceSystem crs );
    };
} // namespace viewshed

#endif