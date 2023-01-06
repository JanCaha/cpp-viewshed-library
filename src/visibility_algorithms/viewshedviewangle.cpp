#include <limits>

#include "losevaluator.h"
#include "viewshedviewangle.h"

double ViewshedViewAngle::result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                                  std::shared_ptr<ViewPoint> vp )
{
    return poi.gradient[CellPosition::CENTER];
}

void ViewshedViewAngle::extractValues( StatusNode &sn, StatusNode &poi, int &position ) {}

const double ViewshedViewAngle::invisible() { return -91.0; }

const double ViewshedViewAngle::completlyVisible() { return 90.0; }

const double ViewshedViewAngle::viewpointValue() { return 180.0; }

const QString ViewshedViewAngle::name() { return QString( "Visibility Angle" ); }