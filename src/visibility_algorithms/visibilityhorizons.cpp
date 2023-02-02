#include <cmath>
#include <limits>

#include "visibilityhorizons.h"

using viewshed::VisibilityHorizons;

VisibilityHorizons::VisibilityHorizons( double horizon, double notHorizon )
    : mHorizon( horizon ), mNotHorizon( notHorizon )
{
}

double VisibilityHorizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    if ( losValues->isTargetHorizon() )
    {
        return mHorizon;
    }

    return mNotHorizon;
}

const QString VisibilityHorizons::name() { return QString( "Horizons" ); }