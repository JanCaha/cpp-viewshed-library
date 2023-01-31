#include <limits>

#include "viewshedvisibility.h"

using viewshed::ViewshedVisibility;

ViewshedVisibility::ViewshedVisibility( double visible, double invisible, double pointValue )
    : mVisibile( visible ), mInvisibile( invisible ), mPointValue( pointValue )
{
}

double ViewshedVisibility::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    if ( losValues->mMaxGradientBefore < los->targetGradient() )
        return mVisibile;
    else
        return mInvisibile;
}

const QString ViewshedVisibility::name() { return QString( "Visibility" ); }