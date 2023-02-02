#include <limits>

#include "visibilityviewangle.h"

using viewshed::VisibilityViewAngle;

VisibilityViewAngle::VisibilityViewAngle( double pointValue ) : mPointValue( pointValue ) {}

double VisibilityViewAngle::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetGradient();
}

const QString VisibilityViewAngle::name() { return QString( "Visibility_Angle" ); }