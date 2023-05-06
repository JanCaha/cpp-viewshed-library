#include <limits>

#include "visibilitydistancelocalhorizon.h"

using viewshed::visibilityalgorithm::DistanceLocalHorizon;

DistanceLocalHorizon::DistanceLocalHorizon( double noHorizon ) : mNoHorizon( noHorizon ) {}

double DistanceLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->horizonBeforeExist() )
    {
        return los->targetDistance() - los->distance( losValues->indexHorizonBefore );
    }
    else
    {
        return mNoHorizon;
    }
}

const std::string DistanceLocalHorizon::name() { return "Distance From Local Horizon"; }