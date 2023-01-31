#include <limits>

#include "viewshedviewangle.h"

using viewshed::ViewshedViewAngle;

ViewshedViewAngle::ViewshedViewAngle( double pointValue ) : mPointValue( pointValue ) {}

double ViewshedViewAngle::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetGradient();
}

const QString ViewshedViewAngle::name() { return QString( "Visibility_Angle" ); }