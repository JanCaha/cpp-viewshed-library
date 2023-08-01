#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ogr_spatialref.h"

#include "simplerasters.h"

#include "abstractlos.h"
#include "abstractviewshedalgorithm.h"
#include "inverselos.h"
#include "losnode.h"
#include "viewshed.h"
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityalgorithms.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::DataTriplet;
using viewshed::InverseLoS;
using viewshed::ViewshedUtils;
using namespace viewshed::visibilityalgorithm;

std::string toStr( bool val ) { return val ? "true" : "false"; }

void ViewshedUtils::saveToCsv( std::vector<std::string> rows, std::string header, std::string fileName )
{
    std::ofstream resultCsvFile;

    resultCsvFile.open( fileName );

    for ( std::size_t i = 0; i < rows.size(); i++ )
    {
        resultCsvFile << rows[i];

        resultCsvFile << "\n";
    }

    resultCsvFile.close();
}

void ViewshedUtils::saveToCsv( std::vector<DataTriplet> data, std::string header, std::string fileName )
{
    std::ofstream resultCsvFile;

    resultCsvFile.open( fileName );

    double distance;
    double elevation;

    resultCsvFile << header;

    for ( std::size_t i = 0; i < data.size(); i++ )
    {
        std::ostringstream dist, elev;

        dist << data.at( i ).first;
        elev << data.at( i ).second;

        resultCsvFile << dist.str();
        resultCsvFile << ",";
        resultCsvFile << elev.str();
        resultCsvFile << ",";
        resultCsvFile << toStr( data.at( i ).third );

        resultCsvFile << "\n";
    }

    resultCsvFile.close();
}

std::vector<DataTriplet> ViewshedUtils::distanceElevation( std::shared_ptr<AbstractLoS> los )
{
    std::vector<DataTriplet> data;

    auto inverseLoS = std::dynamic_pointer_cast<InverseLoS>( los );
    bool inverseLoSEndingAtTarget = los->distance( los->numberOfNodes() - 1 ) <= los->targetDistance();

    data.push_back( DataTriplet( 0, los->vp()->totalElevation(), false ) );

    for ( std::size_t i = 0; i < los->numberOfNodes(); i++ )
    {
        if ( inverseLoS && ( i == 0 && los->targetDistance() < los->distance( i ) ) )
        {
            data.push_back( DataTriplet( los->targetDistance(), los->targetElevation(), true ) );
        }

        data.push_back( DataTriplet( los->distance( i ), los->elevation( i ), false ) );

        if ( inverseLoS && i + 1 < los->numberOfNodes() )
        {
            if ( los->distance( i ) < los->targetDistance() && los->targetDistance() < los->distance( i + 1 ) )
            {
                data.push_back( DataTriplet( los->targetDistance(), los->targetElevation(), true ) );
            }
        }
    }

    if ( !inverseLoS || inverseLoSEndingAtTarget )
    {
        data.push_back( DataTriplet( los->targetDistance(), los->targetElevation(), true ) );
    }

    return data;
}

double ViewshedUtils::earthDiameter( OGRSpatialReference crs )
{
    if ( !crs.IsEmpty() )
    {
        OGRErr *error;
        double semiMajor = crs.GetSemiMajor( error );

        if ( error == OGRERR_NONE )
        {
            return semiMajor * 2;
        }
    }
    return EARTH_DIAMETER;
};

bool ViewshedUtils::compareRasters( std::shared_ptr<SingleBandRaster> r1, std::shared_ptr<SingleBandRaster> r2,
                                    std::string &error )
{
    OGRSpatialReference crs2 = r2->crs();

    if ( !r1->crs().IsSame( &crs2 ) )
    {
        error = "Crs of rasters are not the same.";
        return false;
    }

    if ( r1->extent() != r2->extent() )
    {
        error = "Extents of rasters are not the same.";
        return false;
    }

    if ( r1->width() != r2->width() )
    {
        error = "Widths of rasters are not the same.";
        return false;
    }

    if ( r1->height() != r2->height() )
    {
        error = "Heights of rasters are not the same.";
        return false;
    }

    return true;
}

bool ViewshedUtils::validateRaster( std::shared_ptr<SingleBandRaster> rl, std::string &error )
{
    if ( !rl )
    {
        error = "Could not load the raster.";
        return false;
    }

    if ( !rl->isValid() )
    {
        error = "Raster is not valid. " + rl->error();
        return false;
    }

    // if ( rl->bandCount() != 1 )
    // {
    //     error = "Raster layer needs to have only one band.";
    //     return false;
    // }

    if ( !rl->isProjected() )
    {
        error = "Raster needs to be projected.";
        return false;
    }

    return true;
}

std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> ViewshedUtils::allAlgorithms()
{
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

    algs->push_back( std::make_shared<Boolean>() );
    algs->push_back( std::make_shared<Horizons>() );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false ) );
    algs->push_back( std::make_shared<ElevationDifference>() );
    algs->push_back( std::make_shared<DistanceLocalHorizon>() );
    algs->push_back( std::make_shared<DistanceGlobalHorizon>() );
    algs->push_back( std::make_shared<LoSSlopeToViewAngle>() );
    algs->push_back( std::make_shared<ViewAngle>() );
    algs->push_back( std::make_shared<HorizonsCount>( true ) );
    algs->push_back( std::make_shared<HorizonsCount>( false ) );

    return algs;
}

std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>
ViewshedUtils::allAlgorithms( double invisibleValue )
{
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

    algs->push_back( std::make_shared<Boolean>() );
    algs->push_back( std::make_shared<Horizons>( 1, invisibleValue, 2 ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifference>() );
    algs->push_back( std::make_shared<DistanceLocalHorizon>( invisibleValue ) );
    algs->push_back( std::make_shared<DistanceGlobalHorizon>( invisibleValue ) );
    algs->push_back( std::make_shared<LoSSlopeToViewAngle>( invisibleValue ) );
    algs->push_back( std::make_shared<ViewAngle>() );
    algs->push_back( std::make_shared<HorizonsCount>( true ) );
    algs->push_back( std::make_shared<HorizonsCount>( false ) );

    return algs;
}
