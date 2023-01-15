#include "points.h"

using viewshed::ViewPoint;

ViewPoint::ViewPoint( QgsPoint point, std::shared_ptr<QgsRasterLayer> dem, double offset_, int rasterBand )
{
    mValid = false;

    x = point.x();
    y = point.y();

    std::shared_ptr<bool> ok = std::make_shared<bool>( true );
    elevation = dem->dataProvider()->sample( QgsPointXY( point.x(), point.y() ), rasterBand, ok.get() );

    if ( *ok == false )
    {
        return;
    }

    QgsPoint pointRaster =
        dem->dataProvider()->transformCoordinates( point, QgsRasterDataProvider::TransformType::TransformLayerToImage );

    if ( pointRaster.isEmpty() )
    {
        return;
    }

    col = pointRaster.x();
    row = pointRaster.y();
    offset = offset_;

    mValid = true;
}

ViewPoint::ViewPoint( int row_, int col_, double elevation_, double offset_ )
{
    row = row_;
    col = col_;
    elevation = elevation_;
    offset = offset_;
    mValid = true;
}

double ViewPoint::totalElevation() { return elevation + offset; }

bool ViewPoint::isValid() { return mValid; }
