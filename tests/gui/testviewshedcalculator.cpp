#include <QDir>
#include <QString>

#include <QtTest/QtTest>

#include "viewshedcalculatorwindow.h"

#include "testsettings.h"

namespace ViewshedBinaries
{
    class TestViewshedCalculatorWindow : public QObject
    {
        Q_OBJECT
      private slots:

        void initTestCase() { mWindow = new MainCalculatorWindow(); }

        void setup()
        {
            QTest::keyClicks( mWindow->mPointWidget->mPointX, "-336364.021" );
            QTest::keyClicks( mWindow->mPointWidget->mPointY, "-1189108.615" );

            QTest::keyClicks( mWindow->mFileWidget->mText, QString( TEST_DATA_DSM ) );
            QTest::keyClicks( mWindow->mFolderWidget->mText, QString( TEST_DATA_RESULTS_DIR ) );

            // QTest::keyClicks( mWindow->mObserverOffset, "1.6" );
            // QTest::keyClicks( mWindow->mTargetOffset, "0.0" );
        }

        // void runViewshed()
        // {
        //     QTest::mouseClick( mWindow->mCalculateButton, Qt::LeftButton );

        //     // QTest::qWait( 0 );
        //     QTest::qWait( 15000 );

        //     QDir directory( TEST_DATA_RESULTS_DIR );
        //     QStringList rasters = directory.entryList( QStringList() << "*.tif"
        //                                                              << "*.TIF",
        //                                                QDir::Files );
        //     Q_ASSERT( rasters.count() == 20 );
        // }

      private:
        MainCalculatorWindow *mWindow;
    };
} // namespace ViewshedBinaries

QTEST_MAIN( ViewshedBinaries::TestViewshedCalculatorWindow )

#include "testviewshedcalculator.moc"