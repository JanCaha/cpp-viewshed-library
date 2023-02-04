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
        difference = los->targetGradient() - los->gradient( losValues->mIndexHorizonBefore );
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

const QString AngleDifferenceToLocalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Angle_Difference_To_Local_Horizon_All_Points_-_%1" ).arg( allPoints );
}