#include <cmath>
#include <limits>

#include "visibilityhorizons.h"

using viewshed::visibilityalgorithm::Horizons;

Horizons::Horizons( double horizon, double notHorizon ) : mHorizon( horizon ), mNotHorizon( notHorizon ) {}

double Horizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    if ( losValues->isTargetHorizon() )
    {
        return mHorizon;
    }

    return mNotHorizon;
}

const QString Horizons::name() { return QString( "Horizons" ); }