#include <cmath>
#include <limits>

#include "visibilityfuzzy.h"

using viewshed::visibilityalgorithm::FuzzyVisibility;

FuzzyVisibility::FuzzyVisibility( double clearVisibility, double halfDropout, bool includeVerticalDistance,
                                  double notVisible )
    : mB1( clearVisibility ), mB2( halfDropout ), mNotVisibile( notVisible ),
      mVerticalDistane( includeVerticalDistance )
{
}

double FuzzyVisibility::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    double distance = los->distance( losValues->mTargetIndex );

    if ( mVerticalDistane )
    {
        distance = sqrt( pow( los->distance( losValues->mTargetIndex ), 2 ) +
                         pow( los->vp()->totalElevation() - los->targetElevation(), 2 ) );
    }

    double fuzzyVisibility = 1;

    if ( distance > mB1 )
    {
        fuzzyVisibility = 1 / ( 1 - ( ( distance - mB1 ) / ( mB2 ) ) );
    }

    return fuzzyVisibility;
}

const QString FuzzyVisibility::name() { return QString( "Fuzzy Visibility" ); }