#include "visibilityelevationdifference.h"

using viewshed::VisibilityElevationDifference;

double VisibilityElevationDifference::result( std::shared_ptr<LoSImportantValues> losValues,
                                            std::shared_ptr<AbstractLoS> los )
{
    return los->targetElevation() - los->vp()->totalElevation();
}

double VisibilityElevationDifference::pointValue() { return 0.0; }

const QString VisibilityElevationDifference::name() { return QString( "Elevation_Difference" ); }