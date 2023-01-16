#include <limits>

#include "viewshedhorizons.h"

using viewshed::ViewshedHorizons;

double ViewshedHorizons::result( std::shared_ptr<LoSImportantValues> losValues, std::vector<LoSNode> &statusNodes,
                                 LoSNode &poi, std::shared_ptr<IPoint> vp )
{

    bool isHorizon = false;

    if ( losValues->mIndexPoi + 1 < statusNodes.size() && losValues->mMaxGradientBefore < poi.centreGradient() )
    {
        double gradientBehind =
            statusNodes.at( losValues->mIndexPoi + 1 ).valueAtAngle( poi.centreAngle(), ValueType::Gradient );

        if ( gradientBehind < poi.centreGradient() )
        {
            return completlyVisible();
        }
    }

    return invisible();
}

const double ViewshedHorizons::invisible() { return 0.0; }

const double ViewshedHorizons::completlyVisible() { return 1.0; }

const double ViewshedHorizons::viewpointValue() { return 1.0; }

const QString ViewshedHorizons::name() { return QString( "Horizons" ); }