#include <cmath>
#include <limits>

#include "visibilityfuzzy.h"

using viewshed::visibilityalgorithm::FuzzyVisibility;

FuzzyVisibility::FuzzyVisibility( double clearVisibility, double halfDropout, bool includeVerticalDistance,
                                  double notVisible )
    : mB1( clearVisibility ), mB2( halfDropout ), mNotVisible( notVisible ),
      mVerticalDistance( includeVerticalDistance )
{
}

double FuzzyVisibility::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->maxGradientBefore > los->targetGradient() )
    {
        return mNotVisible;
    }

    double distance = los->distance( losValues->targetIndex );

    if ( mVerticalDistance )
    {
        distance = sqrt( pow( los->distance( losValues->targetIndex ), 2 ) +
                         pow( los->vp()->totalElevation() - los->targetElevation(), 2 ) );
    }

    double fuzzyVisibility = 1;

    if ( distance > mB1 )
    {
        fuzzyVisibility = 1 / ( 1 + pow( ( distance - mB1 ) / ( mB2 ), 2 ) );
    }

    return fuzzyVisibility;
}

const QString FuzzyVisibility::name() { return QString( "Fuzzy Visibility" ); }