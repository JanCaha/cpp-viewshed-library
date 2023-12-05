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

        void initTestCase()
        {
            mWindow = new MainCalculatorWindow();
            readRasters();
            removeRasters();
            readRasters();
        }

        void runViewshed()
        {
            QTest::keyClicks( mWindow->mPointWidget->mPointX, "-336311.5" );
            QTest::keyClicks( mWindow->mPointWidget->mPointY, "-1189034.5" );

            QTest::keyClicks( mWindow->mFileWidget->mText, QString( TEST_DATA_DSM_SMALL ) );
            QTest::keyClicks( mWindow->mFolderWidget->mText, QString( TEST_DATA_RESULTS_DIR ) );

            QTest::keyClicks( mWindow->mObserverOffset, "1.6" );
            QTest::keyClicks( mWindow->mTargetOffset, "0.0" );

            QCOMPARE( mWindow->mViewshedType->currentData( Qt::UserRole ),
                      MainCalculatorWindow::ViewshedType::TypeClassicViewshed );

            removeRasters();
            readRasters();

            QCOMPARE( mResultRasters.count(), 0 );

            QCOMPARE( mWindow->mProgressBar->value(), -1 );

            QTest::mouseClick( mWindow->mCalculateButton, Qt::LeftButton );

            QCOMPARE( mWindow->mProgressBar->value(), 100 );

            readRasters();

            QCOMPARE( mResultRasters.count(), 17 );
        }

        void runInverseViewshed()
        {
            QTest::keyClicks( mWindow->mPointWidget->mPointX, "-336311.5" );
            QTest::keyClicks( mWindow->mPointWidget->mPointY, "-1189034.5" );

            QTest::keyClicks( mWindow->mFileWidget->mText, QString( TEST_DATA_DSM_SMALL ) );
            QTest::keyClicks( mWindow->mFolderWidget->mText, QString( TEST_DATA_RESULTS_DIR ) );

            QTest::keyClicks( mWindow->mObserverOffset, "1.6" );
            QTest::keyClicks( mWindow->mTargetOffset, "0.0" );

            mWindow->mViewshedType->setCurrentIndex( 1 );
            QCOMPARE( mWindow->mViewshedType->currentData( Qt::UserRole ),
                      MainCalculatorWindow::ViewshedType::TypeInverseViewshed );

            removeRasters();
            readRasters();

            QCOMPARE( mResultRasters.count(), 0 );

            mWindow->mProgressBar->reset();

            QCOMPARE( mWindow->mProgressBar->value(), -1 );

            QTest::mouseClick( mWindow->mCalculateButton, Qt::LeftButton );

            QCOMPARE( mWindow->mProgressBar->value(), 100 );

            readRasters();

            QCOMPARE( mResultRasters.count(), 17 );
        }

      private:
        MainCalculatorWindow *mWindow;
        QDir mResultDir = QDir( QString( TEST_DATA_RESULTS_DIR ) );
        QStringList mResultRasters;

        void removeRasters()
        {
            for ( QString raster : mResultRasters )
            {
                mResultDir.remove( raster );
            }
        }

        void readRasters()
        {
            mResultRasters = mResultDir.entryList( QStringList() << "*.tif"
                                                                 << "*.TIF",
                                                   QDir::Files );
        }
    };
} // namespace ViewshedBinaries

QTEST_MAIN( ViewshedBinaries::TestViewshedCalculatorWindow )

#include "testviewshedcalculator.moc"