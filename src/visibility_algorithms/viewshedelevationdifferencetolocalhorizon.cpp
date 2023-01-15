#include "math.h"

#include "losevaluator.h"
#include "viewshedelevationdifferencetolocalhorizon.h"

using viewshed::ViewshedElevationDifferenceToLocalHorizon;

ViewshedElevationDifferenceToLocalHorizon::ViewshedElevationDifferenceToLocalHorizon( bool all, double invisibleValue,
                                                                                      double differenceWithoutHorizon )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon )
{
}

double ViewshedElevationDifferenceToLocalHorizon::result( LoSEvaluator *losevaluator,
                                                          std::vector<StatusNode> &statusNodes, StatusNode &poi,
                                                          std::shared_ptr<IPoint> vp )
{
    double change;
    double difference;

    StatusNode horizon = statusNodes.at( losevaluator->mIndexHorizonBefore );

    if ( losevaluator->mIndexHorizonBefore != 0 )
    {
        change = std::tan( ( M_PI / 180 ) * losevaluator->mMaxGradientBefore ) *
                 ( poi.centreDistance() - horizon.valueAtAngle( poi.centreAngle(), ValueType::Distance ) );
        difference =
            poi.centreElevation() - ( change + horizon.valueAtAngle( poi.centreAngle(), ValueType::Elevation ) );
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

void ViewshedElevationDifferenceToLocalHorizon::extractValues( StatusNode &sn, StatusNode &poi, int &position ) {}

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