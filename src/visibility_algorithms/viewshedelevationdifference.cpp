#include "viewshedelevationdifference.h"

using viewshed::ViewshedElevationDifference;

double ViewshedElevationDifference::result( std::shared_ptr<LoSImportantValues> losValues,
                                            std::vector<LoSNode> &statusNodes, LoSNode &poi,
                                            std::shared_ptr<IPoint> vp )
{
    return poi.elevs[CellPosition::CENTER] - vp->totalElevation();
}

const double ViewshedElevationDifference::invisible() { return 0.0; }

const double ViewshedElevationDifference::completlyVisible() { return 0.0; }

const double ViewshedElevationDifference::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifference::name() { return QString( "Elevation Difference" ); }