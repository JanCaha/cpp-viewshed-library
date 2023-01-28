#include "viewshedelevationdifference.h"

using viewshed::ViewshedElevationDifference;

double ViewshedElevationDifference::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetElevation() - los->vp()->totalElevation();
}

const double ViewshedElevationDifference::invisible() { return 0.0; }

const double ViewshedElevationDifference::completlyVisible() { return 0.0; }

const double ViewshedElevationDifference::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifference::name() { return QString( "Elevation Difference" ); }