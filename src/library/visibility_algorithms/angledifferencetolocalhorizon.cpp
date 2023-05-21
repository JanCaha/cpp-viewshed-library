#include <limits>

#include "visibilityangledifferencetolocalhorizon.h"

using viewshed::visibilityalgorithm::AngleDifferenceToLocalHorizon;

AngleDifferenceToLocalHorizon::AngleDifferenceToLocalHorizon( bool all, double invisibleValue,
                                                              double differenceWithoutHorizon, double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( pointValue )
{
}

double AngleDifferenceToLocalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                              std::shared_ptr<AbstractLoS> los )
{
    double difference;
    if ( losValues->horizonBeforeExist() )
    {
        difference = los->targetGradient() - los->gradient( losValues->indexHorizonBefore );
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

const std::string AngleDifferenceToLocalHorizon::name()
{
    std::string allPoints = "False";

    if ( mAllPoints )
        allPoints = "True";

    return "Angle_Difference_To_Local_Horizon_All_Points_-_" + allPoints;
}