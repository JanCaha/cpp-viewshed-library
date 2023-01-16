#include "math.h"

#include "viewshedelevationdifferencetoglobalhorizon.h"

using viewshed::ViewshedElevationDifferenceToGlobalHorizon;

ViewshedElevationDifferenceToGlobalHorizon::ViewshedElevationDifferenceToGlobalHorizon(
    bool all, double invisibleValue, double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedElevationDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                           std::shared_ptr<std::vector<LoSNode>> los,
                                                           std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp )
{
    double change;
    double difference;
    double distance;

    LoSNode horizon = los->at( losValues->mIndexHorizon );

    if ( losValues->mIndexHorizonBefore != 0 )
    {
        distance = poi->centreDistance() - horizon.valueAtAngle( poi->centreAngle(), ValueType::Distance );
        change = std::tan( ( M_PI / 180 ) * losValues->mMaxGradient ) * distance;
        difference =
            poi->centreElevation() - ( horizon.valueAtAngle( poi->centreAngle(), ValueType::Elevation ) + change );
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
        if ( poi->centreGradient() < losValues->mMaxGradientBefore )
            return invisible();
        else
            return difference;
    }
}

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