#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "iviewshedalgorithm.h"
#include "points.h"
#include "utils.h"
#include "viewshed.h"
#include "viewshedangledifferencetolocalhorizon.h"
#include "viewshedhorizons.h"
#include "viewshedvisibility.h"
#include "visibility.h"

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

    void testLoS()
    {

        QgsPoint poiPoint = QgsPoint( -336428.767, -1189102.785 );

        Viewshed v( vp, dem, algs );
        v.initEventList();
        v.sortEventList();
        v.prefillStatusList();

        StatusNode poi = v.statusNodeFromPoint( poiPoint );

        std::shared_ptr<std::vector<StatusNode>> los = v.LoSToPoint( poiPoint );
        QVERIFY( los->size() == 71 );

        los = v.LoSToPoint( poiPoint, true );
        QVERIFY( los->size() == 71 );

        std::vector<std::pair<double, double>> data = Utils::distanceElevation( los, poi );
        QVERIFY( data.size() == 71 );

        Utils::saveToCsv( data, TEST_DATA_LOS );
    }

    void test()
    {
        Viewshed v( vp, dem, algs );
        v.initEventList();
        v.sortEventList();
        v.prefillStatusList();
        v.parseEventList();
        // v.calculate();
        // v.saveResults( TEST_DATA_RESULTS_DIR );
    }
};

QTEST_MAIN( TestViewshed )
#include "testviewshed.moc"
