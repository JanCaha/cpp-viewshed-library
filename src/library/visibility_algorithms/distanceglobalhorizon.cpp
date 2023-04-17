#include <limits>

#include "visibilitydistanceglobalhorizon.h"

using viewshed::visibilityalgorithm::DistanceGlobalHorizon;

DistanceGlobalHorizon::DistanceGlobalHorizon( double noHorizon ) : mNoHorizon( noHorizon ) {}

double DistanceGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->horizonExist() )
    {
        return los->targetDistance() - los->distance( losValues->mIndexHorizon );
    }
    else
    {
        return mNoHorizon;
    }
}

const QString DistanceGlobalHorizon::name() { return QString( "Distance From Global Horizon" ); }