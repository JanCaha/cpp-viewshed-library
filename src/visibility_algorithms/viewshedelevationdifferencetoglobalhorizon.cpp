#include "math.h"

#include "losevaluator.h"
#include "viewshedelevationdifferencetoglobalhorizon.h"

using viewshed::ViewshedElevationDifferenceToGlobalHorizon;

ViewshedElevationDifferenceToGlobalHorizon::ViewshedElevationDifferenceToGlobalHorizon(
    bool all, double invisibleValue, double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedElevationDifferenceToGlobalHorizon::result( LoSEvaluator *losevaluator,
                                                           std::vector<StatusNode> &statusNodes, StatusNode &poi,
                                                           std::shared_ptr<ViewPoint> vp )
{
    double change;
    double difference;
    double distance;

    StatusNode horizon = statusNodes.at( losevaluator->mIndexHorizon );

    if ( losevaluator->mIndexHorizonBefore != 0 )
    {
        distance = poi.centreDistance() - horizon.valueAtAngle( poi.centreAngle(), ValueType::Distance );
        change = std::tan( ( M_PI / 180 ) * losevaluator->mMaxGradient ) * distance;
        difference =
            poi.centreElevation() - ( horizon.valueAtAngle( poi.centreAngle(), ValueType::Elevation ) + change );
    }
    else
    {
        difference = mDifferenceWithoutHorizon;
    }

    if ( mAllPoints )
    {
        return difference;
    }
    else
    {
        if ( poi.centreGradient() < losevaluator->mMaxGradientBefore )
            return invisible();
        else
            return difference;
    }
}

void ViewshedElevationDifferenceToGlobalHorizon::extractValues( StatusNode &sn, StatusNode &poi, int &position ) {}

const double ViewshedElevationDifferenceToGlobalHorizon::invisible() { return mInvisibleValue; }

const double ViewshedElevationDifferenceToGlobalHorizon::completlyVisible() { return 0; }

const double ViewshedElevationDifferenceToGlobalHorizon::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifferenceToGlobalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Elevation Difference To Global Horizon All Points - %1" ).arg( allPoints );
}