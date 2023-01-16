#include <limits>

#include "viewshedviewangle.h"

using viewshed::ViewshedViewAngle;

double ViewshedViewAngle::result( std::shared_ptr<LoSImportantValues> losValues,
                                  std::shared_ptr<std::vector<LoSNode>> los, std::shared_ptr<LoSNode> poi,
                                  std::shared_ptr<IPoint> vp )
{
    return poi->gradient[CellPosition::CENTER];
}

const double ViewshedViewAngle::invisible() { return -91.0; }

const double ViewshedViewAngle::completlyVisible() { return 90.0; }

const double ViewshedViewAngle::viewpointValue() { return 180.0; }

const QString ViewshedViewAngle::name() { return QString( "Visibility Angle" ); }