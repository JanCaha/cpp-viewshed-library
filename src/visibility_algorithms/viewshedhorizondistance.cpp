#include <limits>

#include "viewshedhorizondistance.h"

using viewshed::ViewshedHorizonDistance;

double ViewshedHorizonDistance::result( std::shared_ptr<LoSImportantValues> losValues,
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

const QString ViewshedHorizonDistance::name() { return QString( "Horizon_Distance" ); }