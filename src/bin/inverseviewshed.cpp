#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshedutils.h"
#include "visibilityalgorithms.h"

#include "commandlinehelper.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );
    QCoreApplication::setApplicationName( "Inverse Viewshed" );
    QCoreApplication::setApplicationVersion( "0.5" );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Calculate Inverse Viewshed with defined parameters." );

    parser.addHelpOption();
    parser.addVersionOption();

    addDem( parser );

    addResultFolder( parser );

    QCommandLineOption targetPosition( QStringLiteral( "targetPosition" ), "Target position in for XXX.XX;YY.YYY .",
                                       " " );
    parser.addOption( targetPosition );

    addHeightObserver( parser );

    addHeightTarget( parser );

    addCurvatureCorrections( parser );

    addRefractionCoefficient( parser );

    addEarthDiameter( parser );

    addVisibilityMask( parser );

    addInvisibleNoData( parser );

    parser.process( app );

    const QStringList args = parser.positionalArguments();

    QString demFilePath = getDemFilePath( parser );

    std::shared_ptr<ProjectedSquareCellRaster> rl =
        std::make_shared<ProjectedSquareCellRaster>( demFilePath.toStdString() );

    std::string rasterError;
    if ( !ViewshedUtils::validateRaster( rl, rasterError ) )
    {
        exitWithError( rasterError, parser );
    }

    QString maskFilePath = getVisibilityMask( parser );

    std::shared_ptr<ProjectedSquareCellRaster> mask = nullptr;

    if ( !maskFilePath.isEmpty() )
    {
        mask = std::make_shared<ProjectedSquareCellRaster>( maskFilePath.toStdString() );

        if ( !ViewshedUtils::validateRaster( mask, rasterError ) )
        {
            exitWithError( rasterError, parser );
        }

        if ( !ViewshedUtils::compareRasters( rl, mask, rasterError ) )
        {
            exitWithError( "Dem and VisibilityMask raster comparison. " + rasterError, parser );
        }
    }

    QString resultFolder = resultFolderAbsolute( parser );

    Coord coord = getCoords( parser, QStringLiteral( "targetPosition" ) );

    double observerOffset = getHeightObserver( parser );

    double targetOffset = getHeightTarget( parser );

    bool curvatureCorrections = getCurvatureCorrections( parser );

    double earthDiam = getEarthDiameter( parser );

    double refCoeff = getRefractionCoefficient( parser );

    bool invisibleNoData = getInvisibleNoData( parser );

    std::shared_ptr<viewshed::Point> tp =
        std::make_shared<viewshed::Point>( OGRPoint( coord.x, coord.y ), rl, targetOffset );

    if ( !rl->isInside( OGRPoint( coord.x, coord.y ) ) )
    {
        exitWithError( "Error: Target point does not lie on the Dem raster.", parser );
    }

    std::shared_ptr<ViewshedAlgorithms> algs;

    if ( invisibleNoData )
    {
        double noData = rl->noData();
        algs = ViewshedUtils::allAlgorithms( noData );
    }
    else
    {
        algs = ViewshedUtils::allAlgorithms();
    }

    InverseViewshed iv( tp, observerOffset, rl, algs, curvatureCorrections, earthDiam, refCoeff );

    if ( mask )
    {
        iv.setVisibilityMask( mask );
    }

    iv.calculate( printTimeInfo, printProgressInfo );

    iv.saveResults( resultFolder.toStdString(), "Inverse" );

    return 0;
}
