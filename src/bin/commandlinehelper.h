#include "iostream"
#include "ostream"
#include <cmath>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>

[[noreturn]] void exitWithError( std::string error, QCommandLineParser &parser )
{
    fprintf( stderr, "%s\n", error.c_str() );
    parser.showHelp( 1 );
}

void printTimeInfo( std::string text, double time ) { std::cout << text << time << " seconds." << std::endl; }

void printProgressInfo( int size, int i )
{
    if ( i % 1000 == 0 )
    {
        double percent = std::round( ( ( i / (double)size ) * 100 ) * 100 ) / 100;

        std::cout << "Evaluated: " << percent << "%"
                  << "     "
                  << "\r" << std::flush;
    }
}

void addEarthDiameter( QCommandLineParser &parser )
{
    QCommandLineOption earthDiameter( QStringLiteral( "earthDiameter" ), "Earth diameter.", "12740000" );
    parser.addOption( earthDiameter );
}

double getEarthDiameter( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "earthDiameter" ) ) ).toDouble();
}

void addCurvatureCorrections( QCommandLineParser &parser )
{
    QCommandLineOption useCurvatureCorrections( QStringLiteral( "useCurvatureCorrections" ),
                                                "Use curvature corrections?", "true" );
    parser.addOption( useCurvatureCorrections );
}

bool getCurvatureCorrections( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "useCurvatureCorrections" ) ) ).toBool();
}

void addRefractionCoefficient( QCommandLineParser &parser )
{
    QCommandLineOption refractionCoefficient( QStringLiteral( "refractionCoefficient" ), "Refraction coefficient.",
                                              "0.142860" );
    parser.addOption( refractionCoefficient );
}

double getRefractionCoefficient( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "refractionCoefficient" ) ) ).toDouble();
}

void addDem( QCommandLineParser &parser )
{
    QCommandLineOption dem( QStringLiteral( "dem" ), "Raster file representing DEM for viewshed calculation.", " " );
    parser.addOption( dem );
}

void addResultFolder( QCommandLineParser &parser )
{
    QCommandLineOption resultsFolder( QStringLiteral( "resultsFolder" ), "Output folder to store the results in.",
                                      " " );
    parser.addOption( resultsFolder );
}

void addHeightObserver( QCommandLineParser &parser )
{
    QCommandLineOption heightObserver( QStringLiteral( "heightObserver" ), "Height of the observer.", " " );
    parser.addOption( heightObserver );
}

double getHeightObserver( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "heightObserver" ) ) ).toDouble();
}

void addHeightTarget( QCommandLineParser &parser )
{
    QCommandLineOption heightTarget( QStringLiteral( "heightTarget" ), "Height of the target.", "0" );
    parser.addOption( heightTarget );
}

double getHeightTarget( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "heightTarget" ) ) ).toDouble();
}

struct Coord
{
    double x = 0;
    double y = 0;
};

Coord getCoords( QCommandLineParser &parser, QString paramName )
{
    QString coords( parser.value( paramName ) );

    Coord coord;
    if ( coords.contains( ";" ) )
    {
        coord.x = coords.split( ";" )[0].toDouble();
        coord.y = coords.split( ";" )[1].toDouble();
    }

    return coord;
}

QString resultFolderAbsolute( QCommandLineParser &parser )
{
    QDir output( parser.value( QStringLiteral( "resultsFolder" ) ) );

    if ( !output.exists() )
    {
        exitWithError( "Error: Output folder does not exist.", parser );
    }

    return output.absolutePath();
}

QString getDemFilePath( QCommandLineParser &parser )
{
    QFile demFile( parser.value( QStringLiteral( "dem" ) ) );

    if ( !demFile.exists() )
    {
        exitWithError( "Error: Dem file does not exist.", parser );
    }

    return demFile.fileName();
}

void addVisibilityMask( QCommandLineParser &parser )
{
    QCommandLineOption visibilityMask( QStringLiteral( "visibilityMask" ),
                                       "Raster file representing visibility mask, specifying areas to calculate "
                                       "visibility in, for viewshed calculation.",
                                       " " );
    parser.addOption( visibilityMask );
}

QString getVisibilityMask( QCommandLineParser &parser )
{
    QString path = parser.value( QStringLiteral( "visibilityMask" ) );

    if ( path.trimmed().isEmpty() )
    {
        return QString();
    }

    QFile file( path );

    if ( !file.exists() )
    {
        exitWithError( "Error: Dem file does not exist.", parser );
    }

    return file.fileName();
}

void addInvisibleNoData( QCommandLineParser &parser )
{
    QCommandLineOption useNoDataForInvisble(
        QStringLiteral( "invisibleAsNoData" ),
        "Set invisible areas to No Data for some visibility indices. Otherwise default values are set.", "true" );
    parser.addOption( useNoDataForInvisble );
}

bool getInvisibleNoData( QCommandLineParser &parser )
{
    return QVariant( parser.value( QStringLiteral( "invisibleAsNoData" ) ) ).toBool();
}