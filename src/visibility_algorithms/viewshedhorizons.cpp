#include <cmath>
#include <limits>

#include "viewshedhorizons.h"

using viewshed::ViewshedHorizons;

ViewshedHorizons::ViewshedHorizons( double horizon, double notHorizon ) : mHorizon( horizon ), mNotHorizon( notHorizon )
{
}

double ViewshedHorizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    if ( losValues->isTargetHorizon() )
    {
        return mHorizon;
    }

    return mNotHorizon;
}

const QString ViewshedHorizons::name() { return QString( "Horizons" ); }