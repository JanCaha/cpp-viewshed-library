#include "math.h"

#include "viewshedelevationdifferencetoglobalhorizon.h"

using viewshed::ViewshedElevationDifferenceToGlobalHorizon;

ViewshedElevationDifferenceToGlobalHorizon::ViewshedElevationDifferenceToGlobalHorizon( bool all, double invisibleValue,
                                                                                        double differenceWithoutHorizon,
                                                                                        double pointValue )
    : mAllPoints( all ), mInvisibleValue( invisibleValue ), mDifferenceWithoutHorizon( differenceWithoutHorizon ),
      mPointValue( mPointValue )
{
}

double ViewshedElevationDifferenceToGlobalHorizon::result( std::shared_ptr<LoSImportantValues> losValues,
                                                           std::shared_ptr<AbstractLoS> los )
{
    double change;
    double difference;
    double distance;

    if ( losValues->horizonExist() )
    {
        distance = los->targetDistance() - los->distance( losValues->mIndexHorizon );
        change = std::tan( ( M_PI / 180 ) * losValues->mMaxGradient ) * distance;
        difference = los->targetElevation() - ( los->elevation( losValues->mIndexHorizon ) + change );
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

const QString ViewshedElevationDifferenceToGlobalHorizon::name()
{
    QString allPoints = QString::fromStdString( "False" );
    if ( mAllPoints )
        allPoints = QString::fromStdString( "True" );

    return QString( "Elevation_Difference_To_Global_Horizon_All_Points_-_%1" ).arg( allPoints );
}