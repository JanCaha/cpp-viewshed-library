#include "simplerasters.h"

#include "point.h"
#include "visibility.h"

using viewshed::Point;
using viewshed::Visibility;

Point::Point() {}

Point::Point( int row, int col, double elevation, double offset, double cellSize )
{
    mRow = row;
    mCol = col;
    mElevation = elevation;
    mOffset = offset;
    mCellSize = cellSize;

    mValid = true;
}

Point::Point( OGRPoint point, std::shared_ptr<SingleBandRaster> dem, double offsetAtPoint )
{

    setUp( point, dem );
    mOffset = offsetAtPoint;
}

void Point::setUp( OGRPoint point, std::shared_ptr<SingleBandRaster> dem )
{
    mX = point.getX();
    mY = point.getY();

    double rowD, colD;

    dem->transformCoordinatesToRaster( mX, mY, rowD, colD );

    bool ok = dem->isNoData( rowD, colD );
    mElevation = dem->value( rowD, colD );

    mValid = !ok && dem->isInside( point );

    mCol = static_cast<int>( colD );
    mRow = static_cast<int>( rowD );

    mCellSize = dem->xCellSize();
}

void Point::setUp( int row, int col, std::shared_ptr<SingleBandRaster> dem )
{
    mRow = row;
    mCol = col;

    mCellSize = dem->xCellSize();

    dem->transformCoordinatesToWorld( mRow, mCol, mX, mY );

    mElevation = dem->value( mRow, mCol );
    mValid = dem->isNoData( mRow, mCol );
}

double Point::totalElevation() { return mElevation + mOffset; }

bool Point::isValid() { return mValid; }

double Point::distance( std::shared_ptr<Point> point ) { return Visibility::distance( mRow, mCol, point, mCellSize ); }