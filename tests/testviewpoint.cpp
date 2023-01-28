#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "point.h"

using viewshed::Point;

class TestViewpoint : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<QgsRasterLayer> dem;

  private slots:

    void initTestCase() { dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" ); }

    void constructOk()
    {
        Point vp( QgsPoint( -336404.98, -1189162.66 ), dem, 1.6 );
        QVERIFY( vp.isValid() );
        QVERIFY( vp.x == -336404.98 );
        QVERIFY( vp.y == -1189162.66 );
        QVERIFY( vp.row == 130 );
        QVERIFY( vp.col == 126 );
        QVERIFY( qgsDoubleNear( vp.elevation, 1017.5416, 0.0001 ) );
        QVERIFY( qgsDoubleNear( vp.offset, 1.6 ) );
        QVERIFY( qgsDoubleNear( vp.totalElevation(), 1017.5416 + 1.6, 0.0001 ) );
    }

    void constructOutside()
    {
        Point vp( QgsPoint( -336699.62, -1189319.20 ), dem, 1.6 );
        QVERIFY( vp.isValid() == false );
    }
};

QTEST_MAIN( TestViewpoint )
#include "testviewpoint.moc"
