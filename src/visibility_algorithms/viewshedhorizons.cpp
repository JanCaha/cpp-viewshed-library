#include <limits>

#include "viewshedhorizons.h"

using viewshed::ViewshedHorizons;

double ViewshedHorizons::result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los )
{

    bool isHorizon = false;

    if ( losValues->mIndexPoi + 1 < los->numberOfNodes() && losValues->mMaxGradientBefore < los->targetGradient() )
    {
        double gradientBehind =
            los->nodeAt( losValues->mIndexPoi + 1 ).valueAtAngle( los->horizontalAngle(), ValueType::Gradient );

        if ( gradientBehind < los->targetGradient() )
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