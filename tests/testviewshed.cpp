#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "iviewshedalgorithm.h"
#include "points.h"
#include "viewshed.h"
#include "viewshedangledifferencetolocalhorizon.h"
#include "viewshedhorizons.h"
#include "viewshedvisibility.h"

class TestViewshed : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<QgsRasterLayer> dem;
    std::shared_ptr<ViewPoint> vp;
    std::vector<std::shared_ptr<IViewshedAlgorithm>> algs;

  private slots:

    void initTestCase()
    {
        dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" );
        vp = std::make_shared<ViewPoint>( QgsPoint( -336364.021, -1189108.615 ), dem );
        algs.push_back( std::make_shared<ViewshedVisibility>() );
        algs.push_back( std::make_shared<ViewshedHorizons>() );
        algs.push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( true ) );
        algs.push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( false ) );
    }

    void test()
    {
        Viewshed v( vp, dem, algs );
        v.calculate();
        v.saveResults( TEST_DATA_RESULTS_DIR );
    }
};

QTEST_MAIN( TestViewshed )
#include "testviewshed.moc"
