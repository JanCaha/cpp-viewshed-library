#include <limits>

#include "visibilityboolean.h"

using viewshed::visibilityalgorithm::Boolean;

Boolean::Boolean( double visible, double invisible, double pointValue )
    : mVisibile( visible ), mInvisibile( invisible ), mPointValue( pointValue )
{
}

double Boolean::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->maxGradientBefore < los->targetGradient() )
        return mVisibile;
    else
        return mInvisibile;
}

const std::string Boolean::name() { return "Visibility"; }