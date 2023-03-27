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

    addDem( parser );

    addResultFolder( parser );

    QCommandLineOption observerPosition( QStringLiteral( "observerPosition" ),
                                         "Observer position in for XXX.XX;YY.YYY .", " " );
    parser.addOption( observerPosition );

    addHeightObserver( parser );

    addCurvatureCorrections( parser );

    addRefractionCoefficient( parser );

    addEarthDiameter( parser );

    addVisibilityMask( parser );

    parser.process( app );

    const QStringList args = parser.positionalArguments();

    QString demFilePath = getDemFilePath( parser );

    std::shared_ptr<QgsRasterLayer> rl = std::make_shared<QgsRasterLayer>( demFilePath, "dem", "gdal" );

    std::string rasterError;
    if ( !Utils::validateRaster( rl, rasterError ) )
    {
        exitWithError( rasterError, parser );
    }

    QString maskFilePath = getVisibilityMask( parser );

    std::shared_ptr<QgsRasterLayer> mask = nullptr;

    if ( !maskFilePath.isEmpty() )
    {
        mask = std::make_shared<QgsRasterLayer>( maskFilePath, "dem", "gdal" );
    }

    QString resultFolder = resultFolderAbsolute( parser );

    Coord coord = getCoords( parser, QStringLiteral( "observerPosition" ) );

    double observerOffset = getHeightObserver( parser );

    bool curvatureCorrections = getCurvatureCorrections( parser );

    double earthDiam = getEarthDiameter( parser );

    double refCoeff = getRefractionCoefficient( parser );

    std::shared_ptr<viewshed::Point> vp =
        std::make_shared<viewshed::Point>( QgsPoint( coord.x, coord.y ), rl, observerOffset );

    if ( !rl->extent().contains( coord.x, coord.y ) )
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

    v.saveResults( resultFolder );

    return 0;
}
