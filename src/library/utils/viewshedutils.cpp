#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "qgscoordinatereferencesystem.h"
#include "qgsellipsoidutils.h"

#include "abstractviewshedalgorithm.h"
#include "inverselos.h"
#include "viewshedutils.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::DataTriplet;
using viewshed::InverseLoS;
using viewshed::ViewshedUtils;
using namespace viewshed::visibilityalgorithm;

std::string toStr( bool val ) { return val ? "true" : "false"; }

void ViewshedUtils::saveToCsv( std::vector<DataTriplet> data, std::string header, std::string fileName )
{
    std::ofstream resultCsvFile;

    resultCsvFile.open( fileName );

    double distance;
    double elevation;

    resultCsvFile << header;

    for ( int i = 0; i < data.size(); i++ )
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

    for ( int i = 0; i < los->numberOfNodes(); i++ )
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

double ViewshedUtils::earthDiameter( QgsCoordinateReferenceSystem crs )
{
    QString ellipsoid = crs.ellipsoidAcronym();
    if ( !ellipsoid.isEmpty() )
    {
        QgsEllipsoidUtils::EllipsoidParameters params = QgsEllipsoidUtils::ellipsoidParameters( ellipsoid );
        return params.semiMajor * 2;
    }
    return EARTH_DIAMETER;
};

bool ViewshedUtils::compareRasters( std::shared_ptr<QgsRasterLayer> r1, std::shared_ptr<QgsRasterLayer> r2,
                                    std::string &error )
{

    if ( r1->crs() != r2->crs() )
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

    if ( !qgsDoubleNear( r1->rasterUnitsPerPixelX(), r2->rasterUnitsPerPixelX(), 0.0001 ) )
    {
        error = "Pixel size of rasters are not the same.";
        return false;
    }

    return true;
}

bool ViewshedUtils::validateRaster( std::shared_ptr<QgsRasterLayer> rl, std::string &error )
{
    if ( !rl )
    {
        error = "Could not load the raster.";
        return false;
    }

    if ( !rl->isValid() )
    {
        error = "Raster is not valid. " + rl->error().message().toStdString();
        return false;
    }

    if ( rl->bandCount() != 1 )
    {
        error = "Raster layer needs to have only one band.";
        return false;
    }

    if ( rl->crs().isGeographic() )
    {
        error = "Raster needs to be projected.";
        return false;
    }

    if ( !qgsDoubleNear( rl->rasterUnitsPerPixelX(), rl->rasterUnitsPerPixelY(), 0.0001 ) )
    {
        error = "Raster needs to have rectangular cells.";
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
    algs->push_back( std::make_shared<HorizonDistance>() );
    algs->push_back( std::make_shared<LoSSlopeToViewAngle>() );
    algs->push_back( std::make_shared<ViewAngle>() );

    return algs;
}

std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>
ViewshedUtils::allAlgorithms( double invisibleValue )
{
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

    algs->push_back( std::make_shared<Boolean>() );
    algs->push_back( std::make_shared<Horizons>( 1, invisibleValue ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false, invisibleValue ) );
    algs->push_back( std::make_shared<ElevationDifference>() );
    algs->push_back( std::make_shared<HorizonDistance>() );
    algs->push_back( std::make_shared<LoSSlopeToViewAngle>( invisibleValue ) );
    algs->push_back( std::make_shared<ViewAngle>() );

    return algs;
}
