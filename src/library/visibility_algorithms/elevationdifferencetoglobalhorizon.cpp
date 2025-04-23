#include <cmath>

#include "visibilityelevationdifferencetoglobalhorizon.h"

using viewshed::visibilityalgorithm::ElevationDifferenceToGlobalHorizon;

ElevationDifferenceToGlobalHorizon::ElevationDifferenceToGlobalHorizon( bool all, double invisibleValue,
                                                                        double differenceWithoutHorizon,
                                                                        double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( pointValue )
{
}

double ElevationDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                   std::shared_ptr<AbstractLoS> los )
{
    double change;
    double difference;
    double distance;

    if ( losValues->horizonExist() )
    {
        change = std::tan( ( M_PI / 180 ) * los->gradient( losValues->indexHorizon ) ) * los->targetDistance();
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

const std::string ElevationDifferenceToGlobalHorizon::name()
{
    std::string allPoints = "False";
    if ( mAllPoints )
        allPoints = "True";

    return "Elevation_Difference_To_Global_Horizon_All_Points_-_" + allPoints;
}