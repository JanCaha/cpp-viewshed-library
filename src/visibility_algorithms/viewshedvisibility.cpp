#include <limits>

#include "losevaluator.h"
#include "viewshedvisibility.h"

double ViewshedVisibility::result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                                   std::shared_ptr<ViewPoint> vp )
{
    if ( poi.centreGradient() < losevaluator->mMaxGradientBefore )
        return invisible();
    else
        return completlyVisible();
}

void ViewshedVisibility::extractValues( StatusNode &sn, StatusNode &poi, int &position ) {}

const double ViewshedVisibility::invisible() { return 0.0; }

const double ViewshedVisibility::completlyVisible() { return 1.0; }

const double ViewshedVisibility::viewpointValue() { return 1.0; }

const QString ViewshedVisibility::name() { return QString( "Visibility" ); }