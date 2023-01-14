#ifndef VIEWSHEDLIB_POINTS_H
#define VIEWSHEDLIB_POINTS_H

#include "qgspoint.h"
#include "qgsrasterlayer.h"

struct ViewPoint
{
    int row, col;
    double x, y;
    double elevation, offset;

    ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset = 1.6, int rasterBand = 1 );
    ViewPoint( int row_, int col_, double elevation_, double offset_ );

    double totalElevation();
    bool isValid();

  private:
    bool mValid;
};

#endif