#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityhorizons.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

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
        algs->push_back( std::make_shared<Boolean>() );
        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, noData, noData, 0 ) );
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

        los = v.getLoS( poiPoint, true );
        QVERIFY( los->size() == 98 );

        std::vector<DataTriplet> data = ViewshedUtils::distanceElevation( los );
        QVERIFY( data.size() == 100 );

        ViewshedUtils::saveToCsv( data, "distance,elevation,target_point\n", TEST_DATA_LOS );
    }

    void testInitCells()
    {
        InverseViewshed v( tp, 2, dem, algs );
        QVERIFY( v.numberOfValidCells() == 0 );
        QVERIFY( v.numberOfCellEvents() == 0 );
        v.initEventList();
        QVERIFY( v.numberOfValidCells() == 37990 );
        QVERIFY( v.numberOfCellEvents() == ( v.numberOfValidCells() - 1 ) * 5 );
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
