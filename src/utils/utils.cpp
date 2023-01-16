#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "utils.h"

using viewshed::Utils;

void Utils::saveToCsv( std::vector<std::pair<double, double>> distanceElevation, std::shared_ptr<IPoint> vp,
                       std::string fileName )
{
    std::ofstream resultCsvFile;

    resultCsvFile.open( fileName );

    double distance;
    double elevation;

    resultCsvFile << "distance,elevation\n";
    resultCsvFile << "0"
                  << "," << vp->totalElevation() << "\n";

    for ( int i = 0; i < distanceElevation.size(); i++ )
    {
        std::ostringstream dist, elev;

        dist << distanceElevation.at( i ).first;
        elev << distanceElevation.at( i ).second;

        std::string a = dist.str();
        std::string b = elev.str();

        resultCsvFile << dist.str();
        resultCsvFile << ",";
        resultCsvFile << elev.str();

        resultCsvFile << "\n";
    }

    resultCsvFile.close();
}

std::vector<std::pair<double, double>> Utils::distanceElevation( std::shared_ptr<std::vector<LoSNode>> los,
                                                                 LoSNode poi )
{
    std::vector<std::pair<double, double>> data;

    for ( int i = 0; i < los->size(); i++ )
    {
        LoSNode ln = los->at( i );
        data.push_back( std::make_pair<double, double>( ln.valueAtAngle( poi.centreAngle(), ValueType::Distance ),
                                                        ln.valueAtAngle( poi.centreAngle(), ValueType::Elevation ) ) );
    }

    return data;
}

std::vector<std::pair<double, double>> Utils::rasterCoordinates( std::shared_ptr<std::vector<LoSNode>> los,
                                                                 LoSNode poi )
{
    std::vector<std::pair<double, double>> data;

    for ( int i = 0; i < los->size(); i++ )
    {
        LoSNode ln = los->at( i );
        data.push_back( std::make_pair<double, double>( ln.row, ln.col ) );
    }

    return data;
}