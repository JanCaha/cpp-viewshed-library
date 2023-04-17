#ifndef VIEWSHEDLIB_UTILS_H
#define VIEWSHEDLIB_UTILS_H

#include "abstractlos.h"
#include "abstractviewshedalgorithm.h"
#include "losnode.h"
#include "viewshed.h"
#include "visibilityalgorithms.h"

using namespace viewshed::visibilityalgorithm;

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

    /**
     * @brief Class that contains mostly static helper functions.
     *
     */
    class ViewshedUtils
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

        /**
         * @brief Validate the raster for viewshed calculation, checks if it fulfills all the requirements.
         *
         * @param r1
         * @param error Returned error message.
         * @return true
         * @return false
         */
        static bool validateRaster( std::shared_ptr<QgsRasterLayer> r1, std::string &error );

        /**
         * @brief Checks if the two rasters have the same properties and that one can be used visibility calculation
         * mask for the second raster, which should be raster of digital surface model.
         *
         * @param r1
         * @param r2
         * @param error Returned error message.
         * @return true
         * @return false
         */
        static bool compareRasters( std::shared_ptr<QgsRasterLayer> r1, std::shared_ptr<QgsRasterLayer> r2,
                                    std::string &error );

        /**
         * @brief Returns list of all implemented viewshed indices as algorithms to be used in viewshed of inverse
         * viewshed calculation.
         *
         * @return std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>
         */
        static std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> allAlgorithms();

        /**
         * @brief Returns list of all implemented viewshed indices as algorithms to be used in viewshed of inverse
         * viewshed calculation. Allows setting specific value to be used in some algorithms as invisible value.
         *
         * @param invisibleValue
         * @return std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>
         */
        static std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>
        allAlgorithms( double invisibleValue );
    };
} // namespace viewshed

#endif