#include "math.h"

#include "viewshedelevationdifferencetoglobalhorizon.h"

using viewshed::ViewshedElevationDifferenceToGlobalHorizon;

ViewshedElevationDifferenceToGlobalHorizon::ViewshedElevationDifferenceToGlobalHorizon(
    bool all, double invisibleValue, double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedElevationDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                           std::shared_ptr<ILoS> los )
{
    double change;
    double difference;
    double distance;

    if ( losValues->horizonExist() != 0 )
    {
        LoSNode horizon = los->nodeAt( losValues->mIndexHorizon );

        distance = los->targetDistance() - horizon.valueAtAngle( los->horizontalAngle(), ValueType::Distance );
        change = std::tan( ( M_PI / 180 ) * losValues->mMaxGradient ) * distance;
        difference =
            los->targetElevation() - ( horizon.valueAtAngle( los->horizontalAngle(), ValueType::Elevation ) + change );
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
        if ( los->targetGradient() < losValues->mMaxGradientBefore )
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