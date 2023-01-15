#include <limits>

#include "losevaluator.h"
#include "viewshedviewangle.h"

using viewshed::ViewshedViewAngle;

double ViewshedViewAngle::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                                  std::shared_ptr<IPoint> vp )
{
    return poi.gradient[CellPosition::CENTER];
}

void ViewshedViewAngle::extractValues( LoSNode &ln, LoSNode &poi, int &position ) {}

const double ViewshedViewAngle::invisible() { return -91.0; }

const double ViewshedViewAngle::completlyVisible() { return 90.0; }

const double ViewshedViewAngle::viewpointValue() { return 180.0; }

const QString ViewshedViewAngle::name() { return QString( "Visibility Angle" ); }