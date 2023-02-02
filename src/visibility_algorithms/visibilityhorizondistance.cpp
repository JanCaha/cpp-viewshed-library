#include <limits>

#include "visibilityhorizondistance.h"

using viewshed::VisibilityHorizonDistance;

double VisibilityHorizonDistance::result( std::shared_ptr<LoSImportantValues> losValues,
                                          std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->mIndexHorizonBefore != -1 )
    {
        return los->targetDistance() - los->distance( losValues->mIndexHorizonBefore );
    }
    else
    {
        return 0;
    }
}

const QString VisibilityHorizonDistance::name() { return QString( "Horizon_Distance" ); }