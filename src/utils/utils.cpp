#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "inverselos.h"
#include "utils.h"

using viewshed::DataTriplet;
using viewshed::InverseLoS;
using viewshed::Utils;

std::string toStr( bool val ) { return val ? "true" : "false"; }

void Utils::saveToCsv( std::vector<DataTriplet> data, std::string header, std::string fileName )
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

std::vector<DataTriplet> Utils::distanceElevation( std::shared_ptr<AbstractLoS> los )
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
