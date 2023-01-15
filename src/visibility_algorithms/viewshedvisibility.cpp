#include <limits>

#include "losevaluator.h"
#include "viewshedvisibility.h"

using viewshed::ViewshedVisibility;

double ViewshedVisibility::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                                   std::shared_ptr<IPoint> vp )
{
    if ( poi.centreGradient() < losevaluator->mMaxGradientBefore )
        return invisible();
    else
        return completlyVisible();
}

const double ViewshedVisibility::invisible() { return 0.0; }

const double ViewshedVisibility::completlyVisible() { return 1.0; }

const double ViewshedVisibility::viewpointValue() { return 1.0; }

const QString ViewshedVisibility::name() { return QString( "Visibility" ); }