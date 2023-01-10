#ifndef POINTS_H
#define POINTS_H

#include "qgspoint.h"
#include "qgsrasterlayer.h"

struct ViewPoint
{
    int row, col;
    double x, y;
    double elevation, offset;

    ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset = 1.6, int rasterBand = 1 );
    ViewPoint( int row, int col, double elevation, double offset );

    double totalElevation();
    bool isValid();

  private:
    bool mValid;
};

#endif