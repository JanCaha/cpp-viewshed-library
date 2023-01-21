#include <limits>

#include "viewshedangledifferencetolocalhorizon.h"

using viewshed::ViewshedAngleDifferenceToLocalHorizon;

ViewshedAngleDifferenceToLocalHorizon::ViewshedAngleDifferenceToLocalHorizon( bool all, double invisibleValue )
    : mInvisibleValue( invisibleValue )
{
    mAllPoints = all;
}

double ViewshedAngleDifferenceToLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                      std::shared_ptr<LoS> los )
{
    double difference;
    if ( losValues->mIndexHorizonBefore != 0 )
    {
        difference =
            los->targetGradient() -
            los->at( losValues->mIndexHorizonBefore ).valueAtAngle( los->horizontalAngle(), ValueType::Gradient );
    }
    else
    {
        difference = 90 + los->targetGradient();
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

const double ViewshedAngleDifferenceToLocalHorizon::invisible() { return mInvisibleValue; }

const double ViewshedAngleDifferenceToLocalHorizon::completlyVisible() { return 0; }

const double ViewshedAngleDifferenceToLocalHorizon::viewpointValue() { return 90.0; }

const QString ViewshedAngleDifferenceToLocalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Angle Difference To Local Horizon All Points - %1" ).arg( allPoints );
}