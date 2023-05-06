#include "visibilityelevationdifference.h"

using viewshed::visibilityalgorithm::ElevationDifference;

double ElevationDifference::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetElevation() - los->vp()->totalElevation();
}

double ElevationDifference::pointValue() { return 0.0; }

const std::string ElevationDifference::name() { return "Elevation_Difference"; }