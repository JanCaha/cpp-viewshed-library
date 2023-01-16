#include <limits>

#include "viewshedangledifferencetolocalhorizon.h"

using viewshed::ViewshedAngleDifferenceToLocalHorizon;

ViewshedAngleDifferenceToLocalHorizon::ViewshedAngleDifferenceToLocalHorizon( bool all, double invisibleValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue )
{
}

double ViewshedAngleDifferenceToLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                      std::shared_ptr<std::vector<LoSNode>> los,
                                                      std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp )
{
    double difference;
    if ( losValues->mIndexHorizonBefore != 0 )
    {
        difference = poi->centreGradient() -
                     los->at( losValues->mIndexHorizonBefore ).valueAtAngle( poi->centreAngle(), ValueType::Gradient );
    }
    else
    {
        difference = 90 + poi->centreGradient();
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