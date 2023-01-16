#include <limits>

#include "viewshedvisibility.h"

using viewshed::ViewshedVisibility;

double ViewshedVisibility::result( std::shared_ptr<LoSImportantValues> losValues,
                                   std::shared_ptr<std::vector<LoSNode>> los, std::shared_ptr<LoSNode> poi,
                                   std::shared_ptr<IPoint> vp )
{
    if ( poi->centreGradient() < losValues->mMaxGradientBefore )
        return invisible();
    else
        return completlyVisible();
}

const double ViewshedVisibility::invisible() { return 0.0; }

const double ViewshedVisibility::completlyVisible() { return 1.0; }

const double ViewshedVisibility::viewpointValue() { return 1.0; }

const QString ViewshedVisibility::name() { return QString( "Visibility" ); }