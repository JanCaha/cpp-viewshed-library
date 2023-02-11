#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "abstractlos.h"
#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{

    /**
     * @brief Representation of triplet of data. First two values are doubles and third is boolean. Used to export LoS
     * to CSV where the values are point's distance, elevation and whether the point is target point.
     *
     */
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
        /**
         * @brief Save data vector of DataTriplets as CSV file.
         *
         * @param data
         * @param header
         * @param fileName
         */
        static void saveToCsv( std::vector<DataTriplet> data, std::string header, std::string fileName );

        /**
         * @brief Extracts triplets of distance, elevation and target point (boolean) from provided LoS.
         *
         * @param los
         * @return std::vector<DataTriplet>
         */
        static std::vector<DataTriplet> distanceElevation( std::shared_ptr<AbstractLoS> los );

        static std::vector<std::pair<double, double>> rasterCoordinates( std::shared_ptr<std::vector<LoSNode>> los,
                                                                         LoSNode poi );

        /**
         * @brief Extract Earth's diameter from ellipsoid associated with provided coordinate reference system.
         *
         * @param crs
         * @return double
         */
        static double earthDiameter( QgsCoordinateReferenceSystem crs );
    };
} // namespace viewshed

#endif