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
    };

    class ViewPoint : public IPoint
    {
      public:
        ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset = 1.6, int rasterBand = 1 );
        ViewPoint( int row_, int col_, double elevation_, double offset_ );
    };

} // namespace viewshed

#endif