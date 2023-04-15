#include <cmath>
#include <limits>

#include "visibilityhorizons.h"

using viewshed::visibilityalgorithm::Horizons;

Horizons::Horizons( double horizon, double notHorizon, double globalHorizon )
    : mHorizon( horizon ), mNotHorizon( notHorizon )
{
    if ( std::isnan( globalHorizon ) )
    {
        mGlobalHorizon = horizon;
    }
    else
    {
        mGlobalHorizon = globalHorizon;
    }
}

double Horizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    if ( losValues->isTargetGlobalHorizon() )
    {
        return mGlobalHorizon;
    }

    if ( losValues->isTargetHorizon() )
    {
        return mHorizon;
    }

    return mNotHorizon;
}

const QString Horizons::name() { return QString( "Horizons" ); }