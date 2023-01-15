#include "points.h"

using viewshed::IPoint;
using viewshed::ViewPoint;

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
