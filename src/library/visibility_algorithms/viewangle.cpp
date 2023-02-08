#include <limits>

#include "visibilityviewangle.h"

using viewshed::visibilityalgorithm::ViewAngle;

ViewAngle::ViewAngle( double pointValue ) : mPointValue( pointValue ) {}

double ViewAngle::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{
    return los->targetGradient();
}

const QString ViewAngle::name() { return QString( "Visibility_Angle" ); }