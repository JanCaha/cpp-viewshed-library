#pragma once
#include "viewshed_export.h"

#include <memory>

#include "constants.h"
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
    class DLL_API Point : public RasterPosition
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
        Point( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, double offsetAtPoint = OBSERVER_OFFSET );

        double mX = 0;
        double mY = 0;
        double mElevation = 0;
        double mOffset = 0;
        double mCellSize = 0;

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
        bool mValid = false;

        void setUp( OGRPoint point, std::shared_ptr<SingleBandRaster> dem );
        void setUp( int row, int col, std::shared_ptr<SingleBandRaster> dem );
    };

} // namespace viewshed
