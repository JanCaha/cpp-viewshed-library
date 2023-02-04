#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QTextEdit>

#include "qgsdoublevalidator.h"
#include "qgsfilewidget.h"
#include "qgspoint.h"
#include "qgsproviderregistry.h"
#include "qgsrasterlayer.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshed.h"
#include "visibilityalgorithms.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

#define DEFAULT_OBSERVER_OFFSET "1.6"

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    enum ViewshedType
    {
        TypeClassicViewshed,
        TypeInverseViewshed,
    };

    MainWindow() : mSettings( QSettings( this ) ), mDemValid( false ), mPointValid( false )
    {
        initMenu();
        initGui();
        resize( mSettings.value( QStringLiteral( "UI/mainwindowsize" ), QSize( 600, 400 ) ).toSize() );
        prepareAlgorithms();
    }

    void resizeEvent( QResizeEvent *event )
    {
        mSettings.setValue( QStringLiteral( "UI/mainwindowsize" ), event->size() );
    }

    void prepareAlgorithms()
    {
        mAlgs = std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

        mAlgs->push_back( std::make_shared<Boolean>() );
        mAlgs->push_back( std::make_shared<Horizons>() );
        mAlgs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false ) );
    };

    void initMenu()
    {
        QMenu *menu = new QMenu( QStringLiteral( "File" ), this );
        menuBar()->addMenu( menu );

        QAction *saveFile = new QAction( "Save settings", this );
        menu->addAction( saveFile );

        connect( saveFile, &QAction::triggered, this,
                 [=]
                 {
                     QString lastUsedDir =
                         mSettings.value( QStringLiteral( "lastUsedDirForSettings" ), QDir::homePath() ).toString();

                     QString outputFileName =
                         QFileDialog::getSaveFileName( this, QStringLiteral( "Save settings" ), lastUsedDir,
                                                       QStringLiteral( "Settings file (*.ini)" ) );

                     if ( outputFileName.isEmpty() )
                         return;

                     QString extension = QString( ".ini" );

                     if ( !outputFileName.endsWith( extension ) )
                         outputFileName = outputFileName + extension;

                     QSettings settings = QSettings( outputFileName, QSettings::IniFormat, this );

                     settings.setValue( QStringLiteral( "demFile" ), mFileWidget->filePath() );
                     settings.setValue( QStringLiteral( "resultsFolder" ), mFolderWidget->filePath() );
                     settings.setValue( QStringLiteral( "observerX" ), mPointX->text() );
                     settings.setValue( QStringLiteral( "observerY" ), mPointY->text() );
                     settings.setValue( QStringLiteral( "observerOffset" ), mObserverOffset->text() );
                     settings.setValue( QStringLiteral( "targetOffset" ), mTargetOffset->text() );
                     settings.setValue( QStringLiteral( "viewshedType" ), mViewshedType->currentData( Qt::UserRole ) );

                     QDir file( outputFileName );
                     mSettings.setValue( QStringLiteral( "lastUsedDirForSettings" ), file.dirName() );
                 } );

        QAction *loadFile = new QAction( "Load settings", this );
        menu->addAction( loadFile );

        connect( loadFile, &QAction::triggered, this,
                 [=]
                 {
                     QString lastUsedDir =
                         mSettings.value( QStringLiteral( "lastUsedDirForSettings" ), QDir::homePath() ).toString();

                     QString inputFileName =
                         QFileDialog::getOpenFileName( this, QStringLiteral( "Load settings" ), lastUsedDir,
                                                       QStringLiteral( "Settings file (*.ini)" ) );

                     if ( inputFileName.isEmpty() )
                         return;

                     QSettings settings = QSettings( inputFileName, QSettings::IniFormat, this );

                     mFileWidget->setFilePath( settings.value( QStringLiteral( "demFile" ), "" ).toString() );
                     mFolderWidget->setFilePath( settings.value( QStringLiteral( "resultsFolder" ), "" ).toString() );
                     mPointX->setText( settings.value( QStringLiteral( "observerX" ), "" ).toString() );
                     mPointY->setText( settings.value( QStringLiteral( "observerY" ), "" ).toString() );
                     mObserverOffset->setText( settings.value( QStringLiteral( "observerOffset" ), "" ).toString() );
                     mTargetOffset->setText( settings.value( QStringLiteral( "targetOffset" ), "" ).toString() );
                     int index =
                         mViewshedType->findData( settings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
                     mViewshedType->setCurrentIndex( index );
                 } );
    }

    void initGui()
    {

        mWidget = new QWidget( this );
        setCentralWidget( mWidget );

        mLayout = new QFormLayout();
        mWidget->setLayout( mLayout );

        mViewshedType = new QComboBox( this );
        mViewshedType->addItem( "Viewshed", ViewshedType::TypeClassicViewshed );
        mViewshedType->addItem( "InverseViewshed", ViewshedType::TypeInverseViewshed );

        int index = mViewshedType->findData( mSettings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
        mViewshedType->setCurrentIndex( index );

        connect( mViewshedType, qOverload<int>( &QComboBox::currentIndexChanged ), this,
                 [=]( int ) { mSettings.setValue( "viewshedType", mViewshedType->currentData( Qt::UserRole ) ); } );

        mCalculateButton = new QPushButton( this );
        mCalculateButton->setText( QStringLiteral( "Calculate!" ) );

        mFileWidget = new QgsFileWidget( this );
        mFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
        mFileWidget->setStorageMode( QgsFileWidget::GetFile );
        mFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateDem );

        mFileWidget->setFilePath( mSettings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

        mPoint.fromWkt( mSettings.value( QStringLiteral( "viewpoint" ), QStringLiteral( "POINT(0 0)" ) ).toString() );

        mPointLabel = new QLabel();
        mPointLabel->setText( mPoint.asWkt( 5 ) );

        mDoubleValidator = new QgsDoubleValidator( this );

        mPointX = new QLineEdit( this );
        mPointX->setValidator( mDoubleValidator );
        mPointY = new QLineEdit( this );
        mPointY->setValidator( mDoubleValidator );

        connect( mPointX, &QLineEdit::textChanged, this, &MainWindow::updatePoint );
        connect( mPointY, &QLineEdit::textChanged, this, &MainWindow::updatePoint );

        mPointX->setText( QVariant( mPoint.x() ).toString() );
        mPointY->setText( QVariant( mPoint.y() ).toString() );

        mObserverOffset = new QLineEdit( this );
        mObserverOffset->setValidator( mDoubleValidator );

        mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

        mTargetOffset = new QLineEdit( this );
        mTargetOffset->setValidator( mDoubleValidator );
        mTargetOffset->setText( QStringLiteral( "0.0" ) );

        mFolderWidget = new QgsFileWidget( this );
        mFolderWidget->setStorageMode( QgsFileWidget::StorageMode::GetDirectory );
        mFolderWidget->setFilePath(
            mSettings.value( QStringLiteral( "resultFolder" ), QStringLiteral( "" ) ).toString() );

        connect( mFolderWidget, &QgsFileWidget::fileChanged, this, &MainWindow::updateResultFolder );

        connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );

        mProgressBar = new QProgressBar( this );

        mLayout->addRow( QStringLiteral( "Type of viewshed:" ), mViewshedType );
        mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
        mLayout->addRow( QStringLiteral( "Point coord X:" ), mPointX );
        mLayout->addRow( QStringLiteral( "Point coord Y:" ), mPointY );
        mLayout->addRow( QStringLiteral( "Point:" ), mPointLabel );
        mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
        mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
        mLayout->addRow( QStringLiteral( "Folder for results:" ), mFolderWidget );
        mLayout->addRow( mCalculateButton );
        mLayout->addRow( mProgressBar );
    }

    void validateDem()
    {
        mDemValid = false;
        mDem = nullptr;

        QgsRasterLayer *rl =
            new QgsRasterLayer( mFileWidget->filePath(), QStringLiteral( "dem" ), QStringLiteral( "gdal" ) );

        if ( !rl )
        {
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ),
                                       QStringLiteral( "Could not load the raster." ) );
            return;
        }

        if ( !rl->isValid() )
        {
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ), QStringLiteral( "Raster is not valid." ) );
            return;
        }

        if ( rl->bandCount() != 1 )
        {
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ),
                                       QStringLiteral( "Raster layer needs to have only one band." ) );
            return;
        }

        if ( rl->crs().isGeographic() )
        {
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ),
                                       QStringLiteral( "Raster needs to be projected." ) );
            return;
        }

        mSettings.setValue( QStringLiteral( "dem" ), mFileWidget->filePath() );

        mDemValid = true;

        mDem = std::make_shared<QgsRasterLayer>( mFileWidget->filePath(), QStringLiteral( "dem" ),
                                                 QStringLiteral( "gdal" ) );

        enableCalculation();
    }

    void enableCalculation() { mCalculateButton->setEnabled( mDemValid && mPointValid ); }

    void updateResultFolder() { mSettings.setValue( QStringLiteral( "resultFolder" ), mFolderWidget->filePath() ); }

    void updatePoint()
    {
        mPointValid = false;

        QString text = mPointX->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mPoint.setX( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        text = mPointY->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mPoint.setY( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( mPoint.x(), mPoint.y() ) )
            {
                mPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( QgsPointXY( mPoint.x(), mPoint.y() ), 1, &sampledOk );

            if ( !sampledOk )
            {
                mPointLabel->setText( mPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
                return;
            }
        }

        if ( sampledOk )
        {
            mPointLabel->setText( mPoint.asWkt( 5 ) + QString( " with elevation: %1" ).arg( elevation ) );
        }
        else
        {
            mPointLabel->setText( mPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
        }

        mSettings.setValue( QStringLiteral( "viewpoint" ), mPoint.asWkt() );

        mPointValid = true;
        enableCalculation();
    }

    void calculateViewshed()
    {
        mCalculateButton->setEnabled( false );

        QDir dirToSave = QDir( mFolderWidget->filePath() );
        if ( !dirToSave.exists() )
        {
            QDir tmpDir;
            tmpDir.mkdir( mFolderWidget->filePath() );
        }

        if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
        {
            std::shared_ptr<Point> vp =
                std::make_shared<Point>( mPoint, mDem, QgsDoubleValidator::toDouble( mObserverOffset->text() ) );
            Viewshed v = Viewshed( vp, mDem, mAlgs );

            mProgressBar->setRange( 0, 100 );

            v.calculate( []( std::string text, double time ) {},
                         [=]( int size, int i )
                         {
                             if ( i % 1000 == 0 )
                             {
                                 mProgressBar->setValue( (int)( ( (double)i / (double)size ) * 100 ) );
                             }
                         } );

            mProgressBar->setValue( 100 );

            v.saveResults( mFolderWidget->filePath() );

            mCalculateButton->setEnabled( true );
        }
        else if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeInverseViewshed )
        {
            std::shared_ptr<Point> tp =
                std::make_shared<Point>( mPoint, mDem, QgsDoubleValidator::toDouble( mTargetOffset->text() ) );
            InverseViewshed iv =
                InverseViewshed( tp, QgsDoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs );

            mProgressBar->setRange( 0, 100 );

            iv.calculate( []( std::string text, double time ) {},
                          [=]( int size, int i )
                          {
                              if ( i % 1000 == 0 )
                              {
                                  mProgressBar->setValue( (int)( ( (double)i / (double)size ) * 100 ) );
                              }
                          } );

            mProgressBar->setValue( 100 );

            iv.saveResults( mFolderWidget->filePath(), "Inverse" );

            mCalculateButton->setEnabled( true );
        }
    }

  private:
    QFormLayout *mLayout = nullptr;
    QWidget *mWidget = nullptr;
    QgsFileWidget *mFileWidget = nullptr;
    QgsFileWidget *mFolderWidget = nullptr;
    QLineEdit *mPointX = nullptr;
    QLineEdit *mPointY = nullptr;
    QLineEdit *mObserverOffset = nullptr;
    QLineEdit *mTargetOffset = nullptr;
    QLabel *mPointLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QgsPoint mPoint;
    QProgressBar *mProgressBar;
    QPushButton *mCalculateButton;
    QComboBox *mViewshedType;
    std::shared_ptr<QgsRasterLayer> mDem;
    QSettings mSettings;
    QMessageBox mErrorMessageBox;

    bool mDemValid;
    bool mPointValid;

    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> mAlgs;
};

int main( int argc, char *argv[] )

{
    QApplication app( argc, argv );
    QApplication::setApplicationName( QStringLiteral( "Viewshed Calculator" ) );
    QApplication::setApplicationVersion( QStringLiteral( "0.1" ) );
    QCoreApplication::setOrganizationName( QStringLiteral( "JanCaha" ) );
    QCoreApplication::setOrganizationDomain( QStringLiteral( "cahik.cz" ) );

    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "viewshedcalculator.moc"