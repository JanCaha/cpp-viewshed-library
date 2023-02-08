#include <limits>

#include "visibilityhorizondistance.h"

using viewshed::visibilityalgorithm::HorizonDistance;

double HorizonDistance::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
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

const QString HorizonDistance::name() { return QString( "Horizon_Distance" ); }