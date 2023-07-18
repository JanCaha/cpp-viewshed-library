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
        change = std::tan( ( M_PI / 180 ) * los->gradient( losValues->indexHorizonBefore ) ) * los->targetDistance();
        difference = los->targetElevation() - ( los->vp()->totalElevation() + change );
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
        if ( los->targetGradient() < losValues->maxGradientBefore )
            return mInvisibleValue;
        else
            return difference;
    }
}

const std::string ElevationDifferenceToLocalHorizon::name()
{
    std::string allPoints = "False";
    if ( mAllPoints )
        allPoints = "True";

    return "Elevation_Difference_To_Local_Horizon_All_Points_-_" + allPoints;
}