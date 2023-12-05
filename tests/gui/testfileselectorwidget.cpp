#include <QString>

#include <QtTest/QtTest>

#include "fileselectorwidget.h"

#include "testsettings.h"

using ViewshedBinaries::FileSelectorWidget;

namespace ViewshedBinaries
{
    class TestFileSelectorWidget : public QObject
    {
        Q_OBJECT
      private slots:

        void initTestCase() { widget = new FileSelectorWidget(); }

        void sendKeysNotPath()
        {
            widget->clearValue();
            widget->setStorageMode( FileSelectorWidget::StorageMode::GetFile );

            QSignalSpy spyFileChanged( widget, &FileSelectorWidget::fileChanged );

            QTest::keyClicks( widget->mText, "a/b/c" );

            QCOMPARE( spyFileChanged.count(), 0 );
            QCOMPARE( widget->mFilePath, QString() );
        }

        void sendKeysPath()
        {
            widget->clearValue();
            widget->setStorageMode( FileSelectorWidget::StorageMode::GetFile );

            QSignalSpy spyFileChanged( widget, &FileSelectorWidget::fileChanged );

            QTest::keyClicks( widget->mText, QString( TEST_DATA_DSM ) );

            QCOMPARE( spyFileChanged.count(), 1 );
            QCOMPARE( widget->mFilePath, TEST_DATA_DSM );
        }

        void sendKeysFolder()
        {
            widget->clearValue();
            widget->setStorageMode( FileSelectorWidget::StorageMode::GetDirectory );

            QSignalSpy spyFileChanged( widget, &FileSelectorWidget::fileChanged );

            QTest::keyClicks( widget->mText, QString( TEST_DATA_RESULTS_DIR ) );

            QCOMPARE( spyFileChanged.count(), 16 );
            QCOMPARE( widget->mFilePath, TEST_DATA_RESULTS_DIR );
        }

      private:
        FileSelectorWidget *widget;
    };
} // namespace ViewshedBinaries

QTEST_MAIN( ViewshedBinaries::TestFileSelectorWidget )

#include "testfileselectorwidget.moc"