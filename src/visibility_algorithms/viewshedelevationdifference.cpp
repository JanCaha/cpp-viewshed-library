#include "viewshedelevationdifference.h"
#include "losevaluator.h"

double ViewshedElevationDifference::result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes,
                                            StatusNode &poi, std::shared_ptr<ViewPoint> vp )
{
    return poi.elevs[CellPosition::CENTER] - vp->totalElevation();
}

void ViewshedElevationDifference::extractValues( StatusNode &sn, StatusNode &poi, int &position ) {}

const double ViewshedElevationDifference::invisible() { return 0.0; }

const double ViewshedElevationDifference::completlyVisible() { return 0.0; }

const double ViewshedElevationDifference::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifference::name() { return QString( "Elevation Difference" ); }