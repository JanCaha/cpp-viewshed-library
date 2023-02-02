#include <limits>

#include "visibilityboolean.h"

using viewshed::VisibilityBoolean;

VisibilityBoolean::VisibilityBoolean( double visible, double invisible, double pointValue )
    : mVisibile( visible ), mInvisibile( invisible ), mPointValue( pointValue )
{
}

double VisibilityBoolean::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->mMaxGradientBefore < los->targetGradient() )
        return mVisibile;
    else
        return mInvisibile;
}

const QString VisibilityBoolean::name() { return QString( "Visibility" ); }