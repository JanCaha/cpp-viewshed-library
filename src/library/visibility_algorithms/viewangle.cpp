#include <limits>

#include "visibilityviewangle.h"

using viewshed::visibilityalgorithm::ViewAngle;

ViewAngle::ViewAngle( bool onlyVisible, double invisibleValue, double pointValue )
    : mPointValue( pointValue ), mOnlyVisible( onlyVisible ), mInvisibleValue( invisibleValue )
{
}

double ViewAngle::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( mOnlyVisible )
    {
        if ( los->targetGradient() <= losValues->maxGradientBefore )
            return mInvisibleValue;
    }

    return los->targetGradient();
}

const std::string ViewAngle::name() { return "Visibility_Angle"; }