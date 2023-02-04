#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

#include "qgspoint.h"
#include "qgsrasterlayer.h"

#include "abstractviewshedalgorithm.h"
#include "point.h"
#include "utils.h"
#include "viewshed.h"
#include "visibilityalgorithms.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

int exitWithError( const char *error, QCommandLineParser &parser, const char *contex = "main" )
{
    fprintf( stderr, "%s\n", qPrintable( QCoreApplication::translate( contex, error ) ) );
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

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );
    QCoreApplication::setApplicationName( "Viewshed" );
    QCoreApplication::setApplicationVersion( "0.1" );

    QCommandLineParser parser;
    parser.setApplicationDescription( QCoreApplication::translate( "main", "Viewshed ." ) );

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption demOption(
        QStringList() << "dem",
        QCoreApplication::translate( "dem", "Raster file representing DEM for viewshed calculation." ), " " );
    parser.addOption( demOption );

    QCommandLineOption resultsFolder(
        QStringList() << "resultsFolder",
        QCoreApplication::translate( "resultsFolder", "Output folder to store the resuls in." ), " " );
    parser.addOption( resultsFolder );

    QCommandLineOption observerPosition(
        QStringList() << "observerPosition",
        QCoreApplication::translate( "observerPosition", "Observer position in for XXX.XX;YY.YYY ." ), " " );
    parser.addOption( observerPosition );

    QCommandLineOption heightObserver( QStringList() << "heightObserver",
                                       QCoreApplication::translate( "heightObserver", "Height of the observer." ),
                                       " " );
    parser.addOption( heightObserver );

    parser.process( app );

    const QStringList args = parser.positionalArguments();

    QFile demFile( parser.value( demOption ) );

    if ( !demFile.exists() )
    {
        exitWithError( "Error: Dem file does not exist.", parser );
    }

    std::shared_ptr<QgsRasterLayer> rl = std::make_shared<QgsRasterLayer>( demFile.fileName(), "dem", "gdal" );

    if ( !rl->isValid() )
    {
        exitWithError( rl->error().message().toLocal8Bit().constData(), parser );
    }

    QDir output( parser.value( resultsFolder ) );

    if ( !output.exists() )
    {
        exitWithError( "Error: Output folder does not exist.", parser );
    }

    QString coords( parser.value( observerPosition ) );

    double x, y;
    if ( coords.contains( ";" ) )
    {
        x = coords.split( ";" )[0].toDouble();
        y = coords.split( ";" )[1].toDouble();
    }

    double observerOffset = QVariant( parser.value( heightObserver ) ).toDouble();

    std::shared_ptr<viewshed::Point> vp = std::make_shared<viewshed::Point>( QgsPoint( x, y ), rl, observerOffset );

    if ( !rl->extent().contains( x, y ) )
    {
        exitWithError( "Error: Viewpoint does not lie on the Dem raster.", parser );
    }

    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

    algs->push_back( std::make_shared<Boolean>() );
    algs->push_back( std::make_shared<Horizons>() );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false ) );

    Viewshed v( vp, rl, algs );

    v.calculate( printTimeInfo, printProgressInfo );

    v.saveResults( output.absolutePath() );

    // v.initEventList();
    // v.sortEventList();

    // QgsPoint poiPoint = QgsPoint( 459939.45, 303229.18 );
    // LoSNode poi = v.statusNodeFromPoint( poiPoint );

    // std::shared_ptr<LoS> los = v.getLoS( poiPoint );

    // std::vector<std::pair<double, double>> data = Utils::distanceElevation( los );

    // Utils::saveToCsv( data, "distance,elevation\n", "/home/cahik/los.csv" );

    return 0;
}
