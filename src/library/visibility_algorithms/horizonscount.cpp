#include <cmath>
#include <limits>

#include "visibilityhorizonscount.h"

using viewshed::visibilityalgorithm::HorizonsCount;

HorizonsCount::HorizonsCount( bool beforeTarget, bool onlyVisible, double invisibleValue )
    : mBeforeTarget( beforeTarget ), mOnlyVisible( onlyVisible ), mInvisibleValue( invisibleValue )
{
}

double HorizonsCount::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( mOnlyVisible && losValues->mMaxGradientBefore > los->targetGradient() )
    {
        return mInvisibleValue;
    }

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