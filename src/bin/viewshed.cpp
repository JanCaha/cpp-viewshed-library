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

#include "commandlinehelper.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );
    QCoreApplication::setApplicationName( "Viewshed" );
    QCoreApplication::setApplicationVersion( "0.2" );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Viewshed." );

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption demOption( QStringList() << "dem", "Raster file representing DEM for viewshed calculation.",
                                  " " );
    parser.addOption( demOption );

    QCommandLineOption resultsFolder( QStringList() << "resultsFolder", "Output folder to store the resuls in.", " " );
    parser.addOption( resultsFolder );

    QCommandLineOption observerPosition( QStringList() << "observerPosition",
                                         "Observer position in for XXX.XX;YY.YYY .", " " );
    parser.addOption( observerPosition );

    QCommandLineOption heightObserver( QStringList() << "heightObserver", "Height of the observer.", " " );
    parser.addOption( heightObserver );

    // QCommandLineOption heightTarget( QStringList() << "targetOffset", "Height of the target.", "0.0" );
    // parser.addOption( heightTarget );

    QCommandLineOption useCurvatureCorrections( QStringList() << "useCurvatureCorrections",
                                                "Use curvature corrections?", "true" );
    parser.addOption( useCurvatureCorrections );

    QCommandLineOption refractionCoefficient( QStringList() << "refractionCoefficient", "Refraction coefficient.",
                                              "0.142860" );
    parser.addOption( refractionCoefficient );

    QCommandLineOption earthDiameter( QStringList() << "earthDiameter", "Earth diameter.", "12740000" );
    parser.addOption( earthDiameter );

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

    // double targetOffset = QVariant( parser.value( heightTarget ) ).toDouble();

    bool curvatureCorrections = QVariant( parser.value( useCurvatureCorrections ) ).toBool();

    double earthDiam = QVariant( parser.value( earthDiameter ) ).toDouble();

    double refCoeff = QVariant( parser.value( refractionCoefficient ) ).toDouble();

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

    Viewshed v( vp, rl, algs, curvatureCorrections, earthDiam, refCoeff );

    v.calculate( printTimeInfo, printProgressInfo );

    v.saveResults( output.absolutePath() );

    return 0;
}
