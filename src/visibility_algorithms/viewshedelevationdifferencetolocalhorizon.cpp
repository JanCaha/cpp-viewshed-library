#include "math.h"

#include "viewshedelevationdifferencetolocalhorizon.h"

using viewshed::ViewshedElevationDifferenceToLocalHorizon;

ViewshedElevationDifferenceToLocalHorizon::ViewshedElevationDifferenceToLocalHorizon( bool all, double invisibleValue,
                                                                                      double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedElevationDifferenceToLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                          std::shared_ptr<ILoS> los )
{
    double change;
    double difference;

    LoSNode horizon = los->nodeAt( losValues->mIndexHorizonBefore );

    if ( losValues->mIndexHorizonBefore != 0 )
    {
        change = std::tan( ( M_PI / 180 ) * losValues->mMaxGradientBefore ) *
                 ( los->targetDistance() - horizon.valueAtAngle( los->horizontalAngle(), ValueType::Distance ) );
        difference =
            los->targetElevation() - ( change + horizon.valueAtAngle( los->horizontalAngle(), ValueType::Elevation ) );
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

const double ViewshedElevationDifferenceToLocalHorizon::invisible() { return mInvisibleValue; }

const double ViewshedElevationDifferenceToLocalHorizon::completlyVisible() { return 0; }

const double ViewshedElevationDifferenceToLocalHorizon::viewpointValue() { return 0.0; }

const QString ViewshedElevationDifferenceToLocalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Elevation Difference To Local Horizon All Points - %1" ).arg( allPoints );
}