#include "visibilityelevationdifference.h"

using viewshed::visibilityalgorithm::ElevationDifference;

double ElevationDifference::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetElevation() - los->vp()->totalElevation();
}

double ElevationDifference::pointValue() { return 0.0; }

const QString ElevationDifference::name() { return QString( "Elevation_Difference" ); }