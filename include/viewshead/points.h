#ifndef VIEWSHEDLIB_POINTS_H
#define VIEWSHEDLIB_POINTS_H

#include "qgspoint.h"
#include "qgsrasterlayer.h"

namespace viewshed
{

    class IPoint
    {
      public:
        int row, col;
        double x, y;
        double elevation, offset;

        double totalElevation();
        bool isValid();

      protected:
        bool mValid;

        void setUp( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, int rasterBand = 1 );
        void setUp( int row_, int col_, std::shared_ptr<QgsRasterLayer> dem, int rasterBand = 1 );
    };

    class ViewPoint : public IPoint
    {
      public:
        ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset = 1.6, int rasterBand = 1 );
        ViewPoint( int row_, int col_, double elevation_, double offset_ );
    };

    class TargetPoint : public IPoint
    {
      public:
        TargetPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double cellSize_, double offset_ = 0,
                     int rasterBand = 1 );
        double distance( std::shared_ptr<IPoint> point );

      private:
        double cellSize;
    };

} // namespace viewshed

#endif