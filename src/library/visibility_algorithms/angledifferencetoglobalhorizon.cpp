#include <limits>

#include "visibilityangledifferencetoglobalhorizon.h"

using viewshed::visibilityalgorithm::AngleDifferenceToGlobalHorizon;

AngleDifferenceToGlobalHorizon::AngleDifferenceToGlobalHorizon( bool all, double invisibleValue,
                                                                double differenceWithoutHorizon, double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( pointValue )
{
}

double AngleDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                               std::shared_ptr<AbstractLoS> los )
{
    double difference;
    if ( losValues->horizonExist() )
    {
        difference = los->targetGradient() - los->gradient( losValues->indexHorizon );
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

const std::string AngleDifferenceToGlobalHorizon::name()
{
    std::string allPoints = "False";
    if ( mAllPoints )
        allPoints = "True";

    return "Angle_Difference_To_Global_Horizon_All_Points_-_" + allPoints;
}