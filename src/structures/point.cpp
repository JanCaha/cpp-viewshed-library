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

Point::Point( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offsetAtPoint, int rasterBand )
{

    setUp( point, dem, rasterBand );
    offset = offsetAtPoint;
}

void Point::setUp( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, int rasterBand )
{
    x = point.x();
    y = point.y();

    bool ok;
    elevation = dem->dataProvider()->sample( QgsPointXY( point.x(), point.y() ), rasterBand, &ok );

    QgsPoint pointRaster =
        dem->dataProvider()->transformCoordinates( point, QgsRasterDataProvider::TransformType::TransformLayerToImage );

    mValid = ok && !pointRaster.isEmpty();

    col = pointRaster.x();
    row = pointRaster.y();

    cellSize = dem->rasterUnitsPerPixelX();
}

void Point::setUp( int row_, int col_, std::shared_ptr<QgsRasterLayer> dem, int rasterBand )
{
    row = row_;
    col = col_;

    cellSize = dem->rasterUnitsPerPixelX();

    QgsPoint pointRaster( col_, row_ );

    QgsPoint point = dem->dataProvider()->transformCoordinates(
        pointRaster, QgsRasterDataProvider::TransformType::TransformImageToLayer );

    bool ok;
    elevation = dem->dataProvider()->sample( QgsPointXY( point.x(), point.y() ), rasterBand, &ok );

    mValid = ok;
}

double Point::totalElevation() { return elevation + offset; }

bool Point::isValid() { return mValid; }

double Point::distance( std::shared_ptr<Point> point ) { return Visibility::distance( row, col, point, cellSize ); }