#include <limits>

#include "viewshedangledifferencetoglobalhorizon.h"

using viewshed::ViewshedAngleDifferenceToGlobalHorizon;

ViewshedAngleDifferenceToGlobalHorizon::ViewshedAngleDifferenceToGlobalHorizon( bool all, double invisibleValue,
                                                                                double differenceWithoutHorizon,
                                                                                double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( pointValue )
{
}

double ViewshedAngleDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                       std::shared_ptr<AbstractLoS> los )
{
    double difference;
    if ( losValues->horizonExist() )
    {
        difference = los->targetGradient() - los->gradient( losValues->mIndexHorizon );
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

const QString ViewshedAngleDifferenceToGlobalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Angle_Difference_To_Global_Horizon_All_Points_-_%1" ).arg( allPoints );
}