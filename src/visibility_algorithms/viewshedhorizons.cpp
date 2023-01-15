#include <limits>

#include "losevaluator.h"
#include "viewshedhorizons.h"

using viewshed::ViewshedHorizons;

double ViewshedHorizons::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                                 std::shared_ptr<IPoint> vp )
{

    bool isHorizon = false;

    if ( losevaluator->mIndexPoi + 1 < statusNodes.size() && losevaluator->mMaxGradientBefore < poi.centreGradient() )
    {
        double gradientBehind =
            statusNodes.at( losevaluator->mIndexPoi + 1 ).valueAtAngle( poi.centreAngle(), ValueType::Gradient );

        if ( gradientBehind < poi.centreGradient() )
        {
            return completlyVisible();
        }
    }

    return invisible();
}

void ViewshedHorizons::extractValues( LoSNode &sn, LoSNode &poi, int &position ) {}

const double ViewshedHorizons::invisible() { return 0.0; }

const double ViewshedHorizons::completlyVisible() { return 1.0; }

const double ViewshedHorizons::viewpointValue() { return 1.0; }

const QString ViewshedHorizons::name() { return QString( "Horizons" ); }