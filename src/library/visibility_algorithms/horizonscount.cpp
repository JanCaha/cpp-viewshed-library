#include <cmath>
#include <limits>

#include "visibilityhorizonscount.h"

using viewshed::visibilityalgorithm::HorizonsCount;

HorizonsCount::HorizonsCount( bool beforeTarget ) : mBeforeTarget( beforeTarget ) {}

double HorizonsCount::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( mBeforeTarget )
    {
        return losValues->mCountHorizonBefore;
    }

    return losValues->mCountHorizon - losValues->mCountHorizonBefore;
}

const QString HorizonsCount::name()
{
    if ( mBeforeTarget )
    {
        return QString( "Horizons Before Target Count" );
    }

    return QString( "Horizons After Target Count" );
}