#include "QObject"
#include "QTest"

#include "simplerasters.h"

#include "testsettings.h"

#include "point.h"
#include "viewshedutils.h"

using viewshed::Point;
using namespace viewshed;

class TestViewpoint : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<ProjectedSquareCellRaster> dem;

  private slots:

    void initTestCase() { dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM ); }

    void constructOk()
    {
        Point vp( OGRPoint( -336404.98, -1189162.66 ), dem, 1.6 );
        QVERIFY( vp.isValid() );
        QVERIFY( vp.x == -336404.98 );
        QVERIFY( vp.y == -1189162.66 );
        QVERIFY( vp.row == 130 );
        QVERIFY( vp.col == 126 );
        QVERIFY( ViewshedUtils::doubleEqual( vp.elevation, 1017.5416, 0.0001 ) );
        QVERIFY( ViewshedUtils::doubleEqual( vp.offset, 1.6 ) );
        QVERIFY( ViewshedUtils::doubleEqual( vp.totalElevation(), 1017.5416 + 1.6, 0.0001 ) );
    }

    void constructOutside()
    {
        Point vp( OGRPoint( -336699.62, -1189319.20 ), dem, 1.6 );
        QVERIFY( vp.isValid() == false );
    }
};

QTEST_MAIN( TestViewpoint )
#include "testviewpoint.moc"
