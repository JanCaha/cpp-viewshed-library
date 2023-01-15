#include <limits>

#include "losevaluator.h"
#include "viewshedangledifferencetoglobalhorizon.h"

using viewshed::ViewshedAngleDifferenceToGlobalHorizon;

ViewshedAngleDifferenceToGlobalHorizon::ViewshedAngleDifferenceToGlobalHorizon( bool all, double invisibleValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue )
{
}

double ViewshedAngleDifferenceToGlobalHorizon::result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes,
                                                       LoSNode &poi, std::shared_ptr<IPoint> vp )
{
    double difference;
    if ( losevaluator->mIndexHorizon != 0 )
    {
        difference =
            poi.centreGradient() -
            statusNodes.at( losevaluator->mIndexHorizon ).valueAtAngle( poi.centreAngle(), ValueType::Gradient );
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

void ViewshedAngleDifferenceToGlobalHorizon::extractValues( LoSNode &ln, LoSNode &poi, int &position ) {}

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