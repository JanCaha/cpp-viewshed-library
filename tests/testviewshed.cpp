#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "point.h"
#include "utils.h"
#include "viewshed.h"
#include "visibility.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityhorizons.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

class TestViewshed : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<QgsRasterLayer> dem;
    std::shared_ptr<Point> vp;
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

  private slots:

    void initTestCase()
    {
        dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" );
        vp = std::make_shared<Point>( QgsPoint( -336364.021, -1189108.615 ), dem );
        algs->push_back( std::make_shared<Boolean>() );
        algs->push_back( std::make_shared<Horizons>() );
        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false ) );
    }

    void testLoS()
    {

        QgsPoint poiPoint = QgsPoint( -336428.767, -1189102.785 );

        Viewshed v( vp, dem, algs );
        v.initEventList();
        v.sortEventList();

        LoSNode poi = v.statusNodeFromPoint( poiPoint );

        std::shared_ptr<LoS> los = v.getLoS( poiPoint );
        QVERIFY( los->size() == 182 );

        los = v.getLoS( poiPoint, true );
        QVERIFY( los->size() == 70 );

        std::vector<DataTriplet> data = Utils::distanceElevation( los );
        QVERIFY( data.size() == 72 );

        Utils::saveToCsv( data, "distance,elevation\n", TEST_DATA_LOS );
    }

    void testInitCells()
    {
        Viewshed v( vp, dem, algs );
        QVERIFY( v.numberOfValidCells() == 0 );
        QVERIFY( v.numberOfCellEvents() == 0 );
        v.initEventList();
        QVERIFY( v.numberOfValidCells() == 37990 );
        QVERIFY( v.numberOfCellEvents() == ( v.numberOfValidCells() - 1 ) * 3 );
    }

    void testViewshedCalculation()
    {
        Viewshed v( vp, dem, algs );
        v.calculate();
        v.saveResults( TEST_DATA_RESULTS_DIR );
    }
};

QTEST_MAIN( TestViewshed )
#include "testviewshed.moc"
