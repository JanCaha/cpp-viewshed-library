#include "points.h"
#include "visibility.h"

using viewshed::IPoint;
using viewshed::TargetPoint;
using viewshed::ViewPoint;
using viewshed::Visibility;

void IPoint::setUp( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, int rasterBand )
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
}

void IPoint::setUp( int row_, int col_, std::shared_ptr<QgsRasterLayer> dem, int rasterBand )
{
    row = row_;
    col = col_;

    QgsPoint pointRaster( col_, row_ );

    QgsPoint point = dem->dataProvider()->transformCoordinates(
        pointRaster, QgsRasterDataProvider::TransformType::TransformImageToLayer );

    bool ok;
    elevation = dem->dataProvider()->sample( QgsPointXY( point.x(), point.y() ), rasterBand, &ok );

    mValid = ok;
}

double IPoint::totalElevation() { return elevation + offset; }

bool IPoint::isValid() { return mValid; }

ViewPoint::ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset_, int rasterBand )
{
    setUp( point, dem, rasterBand );

    offset = offset_;
}

ViewPoint::ViewPoint( int row_, int col_, double elevation_, double offset_ )
{
    row = row_;
    col = col_;
    elevation = elevation_;
    offset = offset_;
    mValid = true;
}

TargetPoint::TargetPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double cellSize_, double offset_,
                          int rasterBand )
{
    setUp( point, dem, rasterBand );

    offset = offset_;
    cellSize = cellSize_;
}

double TargetPoint::distance( std::shared_ptr<IPoint> point )
{
    return Visibility::calculateDistance( row, col, point, cellSize );
}