#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
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
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

  private slots:

    void initTestCase()
    {
        // dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM_SMALL, "dsm", "gdal" );
        // tp = std::make_shared<Point>( QgsPoint( -336312.978, -1189034.372 ), dem, 0 );
        dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" );
        tp = std::make_shared<Point>( QgsPoint( -336364.021, -1189108.615 ), dem, 0 );
        double noData = dem->dataProvider()->sourceNoDataValue( 1 );
        algs->push_back( std::make_shared<ViewshedVisibility>() );
        algs->push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( true ) );
        algs->push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( false, noData, noData, 0 ) );
    }

    void testLoS()
    {

        QgsPoint poiPoint = QgsPoint( -336409.028, -1189172.056 );

        InverseViewshed v( tp, 3, dem, algs );
        v.initEventList();
        v.sortEventList();

        LoSNode poi = v.statusNodeFromPoint( poiPoint );

        std::shared_ptr<InverseLoS> los = v.getLoS( poiPoint );

        QVERIFY( los->size() == 218 );

        // // TODO !!! not yet implemented
        // // los = v.getLoS( poiPoint, true );
        // // QVERIFY( los->size() == 70 );

        std::vector<std::pair<double, double>> data = Utils::distanceElevation( los );
        // // QVERIFY( data.size() == 72 );

        Utils::saveToCsv( data, "distance,elevation\n", TEST_DATA_LOS );
    }

    void testInverseViewshedCalculation()
    {
        InverseViewshed v( tp, 2, dem, algs );
        v.setMaxThreads( 1 );
        v.calculate();
        v.saveResults( TEST_DATA_RESULTS_DIR, "Inverse" );
    }
};

QTEST_MAIN( TestInverseViewshed )
#include "testinverseviewshed.moc"
