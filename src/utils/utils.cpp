#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "inverselos.h"
#include "utils.h"

using viewshed::InverseLoS;
using viewshed::Utils;

void Utils::saveToCsv( std::vector<std::pair<double, double>> data, std::string header, std::string fileName )
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

        resultCsvFile << "\n";
    }

    resultCsvFile.close();
}

std::vector<std::pair<double, double>> Utils::distanceElevation( std::shared_ptr<AbstractLoS> los )
{
    std::vector<std::pair<double, double>> data;

    auto inverseLoS = std::dynamic_pointer_cast<InverseLoS>( los );
    bool inverseLoSEndingAtTarget = los->distance( los->numberOfNodes() - 1 ) <= los->targetDistance();

    data.push_back( std::make_pair<double, double>( 0, los->vp()->totalElevation() ) );

    for ( int i = 0; i < los->numberOfNodes(); i++ )
    {
        if ( inverseLoS && ( i == 0 && los->targetDistance() < los->distance( i ) ) )
        {
            data.push_back( std::make_pair<double, double>( los->targetDistance(), los->targetElevation() ) );
        }

        data.push_back( std::make_pair<double, double>( los->distance( i ), los->elevation( i ) ) );

        if ( inverseLoS && i + 1 < los->numberOfNodes() )
        {
            if ( los->distance( i ) < los->targetDistance() && los->targetDistance() < los->distance( i + 1 ) )
            {
                data.push_back( std::make_pair<double, double>( los->targetDistance(), los->targetElevation() ) );
            }
        }
    }

    if ( !inverseLoS || inverseLoSEndingAtTarget )
    {
        data.push_back( std::make_pair<double, double>( los->targetDistance(), los->targetElevation() ) );
    }

    return data;
}
