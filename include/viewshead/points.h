#ifndef VIEWSHEDLIB_POINTS_H
#define VIEWSHEDLIB_POINTS_H

#include "qgspoint.h"
#include "qgsrasterlayer.h"

namespace viewshed
{

    class RasterPosition
    {
      public:
        RasterPosition(){};

        int row, col;
    };

    class Point : public RasterPosition
    {
      public:
        Point();
        Point( int row_, int col_, double elevation_, double offset_, double cellSize_ );
        Point( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offsetAtPoint = 1.6, int rasterBand = 1 );

        double x, y;
        double elevation, offset;
        double cellSize;

        double totalElevation();
        double distance( std::shared_ptr<Point> point );
        bool isValid();

      protected:
        bool mValid;

        void setUp( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, int rasterBand = 1 );
        void setUp( int row_, int col_, std::shared_ptr<QgsRasterLayer> dem, int rasterBand = 1 );
    };

} // namespace viewshed

#endif