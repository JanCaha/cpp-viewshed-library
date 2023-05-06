#include "point.h"
#include "visibility.h"

using viewshed::Point;
using viewshed::Visibility;

Point::Point() {}

Point::Point( int row_, int col_, double elevation_, double offset_, double cellSize_ )
{
    row = row_;
    col = col_;
    elevation = elevation_;
    offset = offset_;
    cellSize = cellSize_;

    mValid = true;
}

Point::Point( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, double offsetAtPoint, int rasterBand )
{

    setUp( point, dem, rasterBand );
    offset = offsetAtPoint;
}

void Point::setUp( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, int rasterBand )
{
    x = point.getX();
    y = point.getY();

    double rowD, colD;

    dem->transformCoordinatesToRaster( x, y, rowD, colD );

    bool ok = dem->isNoData( rowD, colD );
    elevation = dem->value( rowD, colD );

    mValid = !ok && dem->isInside( point );

    col = static_cast<int>( colD );
    row = static_cast<int>( rowD );

    cellSize = dem->xCellSize();
}

void Point::setUp( int row_, int col_, std::shared_ptr<SingleBandRaster> dem, int rasterBand )
{
    row = row_;
    col = col_;

    cellSize = dem->xCellSize();

    dem->transformCoordinatesToWorld( row, col, x, y );

    elevation = dem->value( row, col );
    mValid = dem->isNoData( row, col );
}

double Point::totalElevation() { return elevation + offset; }

bool Point::isValid() { return mValid; }

double Point::distance( std::shared_ptr<Point> point ) { return Visibility::distance( row, col, point, cellSize ); }