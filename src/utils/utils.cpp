#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "utils.h"

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

std::vector<std::pair<double, double>> Utils::distanceElevation( std::shared_ptr<LoS> los )
{
    std::vector<std::pair<double, double>> data;

    data.push_back( std::make_pair<double, double>( 0, los->vp()->totalElevation() ) );

    for ( int i = 0; i < los->size(); i++ )
    {
        LoSNode ln = los->at( i );
        data.push_back( std::make_pair<double, double>( los->distance( i ), los->elevation( i ) ) );
    }

    data.push_back( std::make_pair<double, double>( los->targetDistance(), los->targetElevation() ) );

    return data;
}
