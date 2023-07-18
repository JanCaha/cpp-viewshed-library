#ifndef VIEWSHEDLIB_POINT_H
#define VIEWSHEDLIB_POINT_H

#include <memory>

#include "rasterposition.h"

class SingleBandRaster;
class OGRPoint;

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
         * @param row
         * @param col
         * @param elevation
         * @param offset
         * @param cellSize
         */
        Point( int row_, int col_, double elevation, double offset, double cellSize );
        /**
         * @brief Construct a new Point object from OGRPoint, SingleBandRaster and offset.
         *
         * @param point
         * @param dem
         * @param offsetAtPoint
         * @param rasterBand
         */
        Point( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, double offsetAtPoint = 1.6 );

        double mX, mY;
        double mElevation, mOffset;
        double mCellSize;

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

        void setUp( OGRPoint point, std::shared_ptr<SingleBandRaster> dem );
        void setUp( int row, int col, std::shared_ptr<SingleBandRaster> dem );
    };

} // namespace viewshed

#endif