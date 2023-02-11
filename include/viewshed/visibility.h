#ifndef VIEWSHEDLIB_VISIBILITY_H
#define VIEWSHEDLIB_VISIBILITY_H

#include "celleventposition.h"
#include "enums.h"
#include "point.h"
#include "rasterposition.h"

#define REFRACTION_COEFFICIENT 0.142860
#define EARTH_DIAMETER 12740000

namespace viewshed
{
    /**
     * @brief Class that provides static functions to assist with viewshed calculation.
     *
     */
    class Visibility
    {
      public:
        /**
         * @brief Obtain cell event postion based on cell event position type, row, column and important point.
         *
         * @param eventType
         * @param row
         * @param col
         * @param point
         * @return CellEventPosition
         */
        static CellEventPosition eventPosition( CellEventPositionType eventType, int row, int col,
                                                std::shared_ptr<Point> point );

        /**
         * @brief Calculate horizontal angle.
         *
         * @param row
         * @param column
         * @param point
         * @return double
         */
        static double angle( double row, double column, std::shared_ptr<Point> point );

        /**
         * @brief Calculate horizontal angle.
         *
         * @param pos
         * @param point
         * @return double
         */
        static double angle( CellEventPosition *pos, std::shared_ptr<Point> point );

        /**
         * @brief Calculate horizontal angle.
         *
         * @param pos
         * @param point
         * @return double
         */
        static double angle( RasterPosition *pos, std::shared_ptr<Point> point );

        /**
         * @brief Calculate distance.
         *
         * @param x1
         * @param y1
         * @param x2
         * @param y2
         * @param cellSize
         * @return double
         */
        static double distance( const double &x1, const double &y1, const double &x2, const double &y2,
                                double &cellSize );

        /**
         * @brief Calculate distance.
         *
         * @param row
         * @param column
         * @param point
         * @param cellSize
         * @return double
         */
        static double distance( int &row, int &column, std::shared_ptr<Point> point, double &cellSize );

        /**
         * @brief Calculate distance.
         *
         * @param point1
         * @param point2
         * @param cellSize
         * @return double
         */
        static double distance( std::shared_ptr<Point> point1, std::shared_ptr<Point> point2, double &cellSize );

        /**
         * @brief Calculate distance.
         *
         * @param row
         * @param column
         * @param point
         * @param cellSize
         * @return double
         */
        static double distance( double &row, double &column, std::shared_ptr<Point> point, double &cellSize );

        /**
         * @brief Calculate distance.
         *
         * @param pos
         * @param point
         * @param cellSize
         * @return double
         */
        static double distance( CellEventPosition *pos, std::shared_ptr<Point> point, double &cellSize );

        /**
         * @brief Calculate gradient.
         *
         * @param point
         * @param elevation
         * @param distance
         * @return double
         */
        static double gradient( std::shared_ptr<Point> point, double elevation, double &distance );

        /**
         * @brief Recalculate elevation with inclusion of atmospheric refraction and earth curvature;
         *
         * @param elevation
         * @param distance
         * @param refractionCoeff Refraction coefficient. Default value is 0.142860.
         * @param earthDiameter Diameter of Earth to use. Default value is 12740000, the units should be the same as for
         * raster DEM.
         * @return double
         */
        static double curvatureCorrections( double elevation, double distance,
                                            double refractionCoeff = REFRACTION_COEFFICIENT,
                                            double earthDiameter = EARTH_DIAMETER );
    };
} // namespace viewshed

#endif