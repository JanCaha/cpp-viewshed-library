#include <cmath>
#include <limits>

#include "visibilityslopetoviewangle.h"

using viewshed::visibilityalgorithm::LoSSlopeToViewAngle;

LoSSlopeToViewAngle::LoSSlopeToViewAngle( double invisible, double pointValue )
    : mInvisibile( invisible ), mPointValue( pointValue )
{
}

double LoSSlopeToViewAngle::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    double gradientDiff;
    if ( los->targetIndex() == 0 )
    {
        gradientDiff =
            ( 180 / M_PI ) * atan( ( los->targetElevation() - los->viewPointElevation() ) / ( los->targetDistance() ) );
    }
    else
    {
        gradientDiff = ( 180 / M_PI ) * atan( ( los->targetElevation() - los->elevation( los->targetIndex() - 1 ) ) /
                                              ( los->targetDistance() - los->distance( los->targetIndex() - 1 ) ) );
    }

    if ( losValues->maxGradientBefore < los->targetGradient() )
        return gradientDiff - los->targetGradient();
    else
        return mInvisibile;
}

const std::string LoSSlopeToViewAngle::name() { return "LoS Slope to View Angle"; }