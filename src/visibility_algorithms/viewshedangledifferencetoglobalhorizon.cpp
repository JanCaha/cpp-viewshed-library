#include <limits>

#include "viewshedangledifferencetoglobalhorizon.h"

using viewshed::ViewshedAngleDifferenceToGlobalHorizon;

ViewshedAngleDifferenceToGlobalHorizon::ViewshedAngleDifferenceToGlobalHorizon( bool all, double invisibleValue,
                                                                                double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedAngleDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                       std::shared_ptr<AbstractLoS> los )
{
    double difference;
    if ( losValues->horizonExist() )
    {
        difference = los->targetGradient() - los->gradient( losValues->mIndexHorizon );
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

const double ViewshedAngleDifferenceToGlobalHorizon::invisible() { return mInvisibleValue; }

const double ViewshedAngleDifferenceToGlobalHorizon::completlyVisible() { return 0; }

const double ViewshedAngleDifferenceToGlobalHorizon::viewpointValue() { return 90.0; }

const QString ViewshedAngleDifferenceToGlobalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Angle Difference To Global Horizon All Points - %1" ).arg( allPoints );
}