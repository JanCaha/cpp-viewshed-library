#include <cmath>
#include <limits>

#include "viewshedhorizons.h"

using viewshed::ViewshedHorizons;

double ViewshedHorizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    if ( losValues->isTargetHorizon() )
    {
        return completlyVisible();
    }

    return invisible();
}

const double ViewshedHorizons::invisible() { return 0.0; }

const double ViewshedHorizons::completlyVisible() { return 1.0; }

const double ViewshedHorizons::viewpointValue() { return 1.0; }

const QString ViewshedHorizons::name() { return QString( "Horizons" ); }