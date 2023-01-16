#include <limits>

#include "viewshedangledifferencetoglobalhorizon.h"

using viewshed::ViewshedAngleDifferenceToGlobalHorizon;

ViewshedAngleDifferenceToGlobalHorizon::ViewshedAngleDifferenceToGlobalHorizon( bool all, double invisibleValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue )
{
}

double ViewshedAngleDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                       std::shared_ptr<std::vector<LoSNode>> los,
                                                       std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp )
{
    double difference;
    if ( losValues->mIndexHorizon != 0 )
    {
        difference = poi->centreGradient() -
                     los->at( losValues->mIndexHorizon ).valueAtAngle( poi->centreAngle(), ValueType::Gradient );
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