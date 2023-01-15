#include "viewshedelevationdifference.h"
#include "losevaluator.h"

using viewshed::ViewshedElevationDifference;

double ViewshedElevationDifference::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                                            std::shared_ptr<IPoint> vp )
{
    return poi.elevs[CellPosition::CENTER] - vp->totalElevation();
}

const double ViewshedElevationDifference::invisible() { return 0.0; }

const double ViewshedElevationDifference::completlyVisible() { return 0.0; }

const double ViewshedElevationDifference::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifference::name() { return QString( "Elevation Difference" ); }