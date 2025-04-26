#include <QString>

#include <QtTest/QtTest>

#include "pointwidget.h"

#include "gdal_version.h"

#include "testsettings.h"

using ViewshedBinaries::PointWidget;

namespace ViewshedBinaries
{
    class TestPointWidget : public QObject
    {
        Q_OBJECT
      private slots:

        void initTestCase() { widget = new PointWidget(); }

        void sendKeysXYWrong()
        {
            QCOMPARE( widget->mPoint.getX(), 0 );
            QCOMPARE( widget->mPoint.getY(), 0 );

            QTest::keyClicks( widget->mPointX, "a" );
            Q_ASSERT( widget->mPointValid == false );

            QTest::keyClicks( widget->mPointY, "-1189108.615" );
        }

        void sendKeysXY()
        {
            QCOMPARE( widget->mPoint.getX(), 0 );
            QCOMPARE( widget->mPoint.getY(), 0 );

            QTest::keyClicks( widget->mPointX, "-336364.021" );
            QTest::keyClicks( widget->mPointY, "-1189108.615" );

            Q_ASSERT( qFuzzyCompare( widget->mPoint.getX(), -336364.021 ) );
            Q_ASSERT( qFuzzyCompare( widget->mPoint.getY(), -1189108.615 ) );
            Q_ASSERT( widget->mPointValid );

            OGRWktOptions wktOpts = OGRWktOptions();
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION( 3, 9, 0 )
            wktOpts.xyPrecision = 3;
#else
            wktOpts.precision = 3;
#endif

            QCOMPARE( widget->point().exportToWkt( wktOpts ),
                      OGRPoint( -336364.021, -1189108.615 ).exportToWkt( wktOpts ) );
            Q_ASSERT( widget->crs().IsEmpty() );
        }

        void changeCrs()
        {
            Q_ASSERT( widget->crs().IsEmpty() );

            OGRSpatialReference crs;
            crs.SetFromUserInput( "EPSG:4326" );

            widget->setCrs( crs );

            Q_ASSERT( widget->crs().IsEmpty() == false );
            Q_ASSERT( widget->crs().IsSame( &crs ) );
            QCOMPARE( widget->mCrsLabel->text(), "[EPSG:4326]" );
        }

      private:
        PointWidget *widget;
    };
} // namespace ViewshedBinaries

QTEST_MAIN( ViewshedBinaries::TestPointWidget )

#include "testpointwidget.moc"