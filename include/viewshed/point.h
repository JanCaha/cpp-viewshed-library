#ifndef VIEWSHEDLIB_POINT_H
#define VIEWSHEDLIB_POINT_H

#include "simplerasters.h"

#include "rasterposition.h"

namespace viewshed
{
    /**
     * @brief Class that represents important point for viewshed - view point, or inverse viewshed - target point. The
     * point is specified by cell position in raster coordinates, world coordinates, elevation at point, offset from
     * elevation and cell size.
     *
     */
    class Point : public RasterPosition
    {
      public:
        /**
         * @brief Construct a new Point object which is empty.
         *
         */
        Point();
        /**
         * @brief Construct a new Point object by manually specifying all the necessary information.
         *
         * @param row_
         * @param col_
         * @param elevation_
         * @param offset_
         * @param cellSize_
         */
        Point( int row_, int col_, double elevation_, double offset_, double cellSize_ );
        /**
         * @brief Construct a new Point object from OGRPoint, SingleBandRaster and offset.
         *
         * @param point
         * @param dem
         * @param offsetAtPoint
         * @param rasterBand
         */
        Point( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, double offsetAtPoint = 1.6, int rasterBand = 1 );

        double x, y;
        double elevation, offset;
        double cellSize;

        /**
         * @brief Combination of elevation plus offset;
         *
         * @return double
         */
        double totalElevation();
        /**
         * @brief Distance from another point.
         *
         * @param point
         * @return double
         */
        double distance( std::shared_ptr<Point> point );
        /**
         * @brief Check if the point is valid.
         *
         * @return true
         * @return false
         */
        bool isValid();

      protected:
        bool mValid;

        void setUp( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, int rasterBand = 1 );
        void setUp( int row_, int col_, std::shared_ptr<SingleBandRaster> dem, int rasterBand = 1 );
    };

} // namespace viewshed

#endif