#include "math.h"

#include "visibilityelevationdifferencetolocalhorizon.h"

using viewshed::visibilityalgorithm::ElevationDifferenceToLocalHorizon;

ElevationDifferenceToLocalHorizon::ElevationDifferenceToLocalHorizon( bool all, double invisibleValue,
                                                                      double differenceWithoutHorizon,
                                                                      double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( pointValue )
{
}

double ElevationDifferenceToLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                  std::shared_ptr<AbstractLoS> los )
{
    double change;
    double difference;

    if ( losValues->horizonBeforeExist() )
    {
        change = std::tan( ( M_PI / 180 ) * losValues->mMaxGradientBefore ) * los->targetDistance();
        difference = los->targetElevation() - change;
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
            return mInvisibleValue;
        else
            return difference;
    }
}

const QString ElevationDifferenceToLocalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Elevation_Difference_To_Local_Horizon_All_Points_-_%1" ).arg( allPoints );
}