#include <limits>

#include "losevaluator.h"
#include "viewshedangledifferencetolocalhorizon.h"

using viewshed::ViewshedAngleDifferenceToLocalHorizon;

ViewshedAngleDifferenceToLocalHorizon::ViewshedAngleDifferenceToLocalHorizon( bool all, double invisibleValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue )
{
}

double ViewshedAngleDifferenceToLocalHorizon::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes,
                                                      LoSNode &poi, std::shared_ptr<IPoint> vp )
{
    double difference;
    if ( losevaluator->mIndexHorizonBefore != 0 )
    {
        difference =
            poi.centreGradient() -
            statusNodes.at( losevaluator->mIndexHorizonBefore ).valueAtAngle( poi.centreAngle(), ValueType::Gradient );
    }
    else
    {
        difference = 90 + poi.centreGradient();
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