#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "inverseviewshed.h"
#include "iviewshedalgorithm.h"
#include "points.h"
#include "utils.h"
#include "viewshedangledifferencetolocalhorizon.h"
#include "viewshedhorizons.h"
#include "viewshedvisibility.h"
#include "visibility.h"

using namespace viewshed;

class TestInverseViewshed : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<QgsRasterLayer> dem;
    std::shared_ptr<Point> tp;
    std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<IViewshedAlgorithm>>>();

  private slots:

    void initTestCase()
    {
        dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" );
        tp = std::make_shared<Point>( QgsPoint( -336364.021, -1189108.615 ), dem, 0 );
        algs->push_back( std::make_shared<ViewshedVisibility>() );
    }

    void testLoS()
    {

        // QgsPoint poiPoint = QgsPoint( -336428.767, -1189102.785 );
        QgsPoint poiPoint = QgsPoint( -336409.028, -1189172.056 );

        InverseViewshed v( tp, 3, dem, algs );
        v.initEventList();
        v.sortEventList();

        LoSNode poi = v.statusNodeFromPoint( poiPoint );

        std::shared_ptr<InverseLoS> los = v.getLoS( poiPoint );

        LoSEvaluator loseval = LoSEvaluator( los, algs );
        loseval.calculate();

        // QVERIFY( los->size() == 182 );

        //     std::shared_ptr<LoS> los = v.getLoS( poiPoint, true );
        //     QVERIFY( los->size() == 70 );

        std::vector<std::pair<double, double>> data = Utils::distanceElevation( los );
        //     QVERIFY( data.size() == 72 );

        Utils::saveToCsv( data, "distance,elevation\n", TEST_DATA_LOS );
    }

    void TestRaster()
    {
        InverseViewshed v( tp, 3, dem, algs );
        v.setMaxThreads( 1 );
        v.calculate();
        v.saveResults( TEST_DATA_RESULTS_DIR, "Inverse" );
    }
};

QTEST_MAIN( TestInverseViewshed )
#include "testinverseviewshed.moc"
