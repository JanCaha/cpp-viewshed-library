#include "chrono"
#include <fstream>

#include <QApplication>
#include <QCheckBox>
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
#include <QStatusBar>
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
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityalgorithms.h"

#include "pointwidget.h"

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

    MainWindow() : mSettings( QSettings( this ) ), mDemValid( false )
    {
        initMenu();
        initGui();
        resize( mSettings.value( QStringLiteral( "UI/mainwindowsize" ), QSize( 800, 400 ) ).toSize() );
    }

    void resizeEvent( QResizeEvent *event )
    {
        mSettings.setValue( QStringLiteral( "UI/mainwindowsize" ), event->size() );
    }

    void prepareAlgorithms()
    {
        double noData = mDem->dataProvider()->sourceNoDataValue( 1 );

        mAlgs = std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

        if ( mNoDataForInvisible->isChecked() )
        {
            mAlgs = ViewshedUtils::allAlgorithms( noData );
        }
        else
        {
            mAlgs = ViewshedUtils::allAlgorithms();
        }
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

                     saveCurrentValuesToSettings( settings );

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

                     readValuesFromSettings( settings );
                 } );

        menu->addSeparator();

        QAction *resetSettings = new QAction( "Reset settings", this );
        menu->addAction( resetSettings );

        connect( resetSettings, &QAction::triggered, this,
                 [=]
                 {
                     mFileWidget->setFilePath( "" );
                     mFolderWidget->setFilePath( "" );

                     mViewshedType->setCurrentIndex( 0 );

                     mPointWidget->setXY( 0, 0 );
                     mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );
                     mTargetOffset->setText( "0.0" );

                     mRefractionCoefficient->setText( QString::number( REFRACTION_COEFFICIENT, 'f' ) );
                     mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );
                 } );
    }

    void initGui()
    {

        mWidget = new QWidget( this );
        setCentralWidget( mWidget );

        mLayout = new QFormLayout();
        mWidget->setLayout( mLayout );

        statusBar()->show();

        mViewshedType = new QComboBox( this );
        mViewshedType->addItem( "Viewshed", ViewshedType::TypeClassicViewshed );
        mViewshedType->addItem( "InverseViewshed", ViewshedType::TypeInverseViewshed );

        mCalculateButton = new QPushButton( this );
        mCalculateButton->setText( QStringLiteral( "Calculate!" ) );

        mPointWidget = new PointWidget( true, this );

        mCurvatureCorrections = new QCheckBox( this );
        mRefractionCoefficient = new QLineEdit( this );
        mRefractionCoefficient->setText( QString::number( REFRACTION_COEFFICIENT, 'f' ) );
        mRefractionCoefficient->setValidator( mDoubleValidator );
        mEarthDiameter = new QLineEdit( this );
        mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );
        mEarthDiameter->setValidator( mDoubleValidator );

        mFileWidget = new QgsFileWidget( this );
        mFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
        mFileWidget->setStorageMode( QgsFileWidget::GetFile );
        mFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

        mMaskFileWidget = new QgsFileWidget( this );
        mMaskFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
        mMaskFileWidget->setStorageMode( QgsFileWidget::GetFile );
        mMaskFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

        mPointLabel = new QLabel();
        mPointLabel->setText( mPoint.asWkt( 5 ) );

        mDoubleValidator = new QgsDoubleValidator( this );

        mObserverOffset = new QLineEdit( this );
        mObserverOffset->setValidator( mDoubleValidator );

        mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

        mTargetOffset = new QLineEdit( this );
        mTargetOffset->setValidator( mDoubleValidator );
        mTargetOffset->setText( QStringLiteral( "0.0" ) );

        mNoDataForInvisible = new QCheckBox( this );
        mNoDataForInvisible->setChecked( true );

        mFolderWidget = new QgsFileWidget( this );
        mFolderWidget->setStorageMode( QgsFileWidget::StorageMode::GetDirectory );

        mProgressBar = new QProgressBar( this );

        mLayout->addRow( QStringLiteral( "Type of viewshed:" ), mViewshedType );
        mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
        mLayout->addRow( QStringLiteral( "Select Visibility Mask file:" ), mMaskFileWidget );
        mLayout->addRow( QStringLiteral( "Important Point Coordinates (x,y):" ), mPointWidget );
        mLayout->addRow( QStringLiteral( "Point:" ), mPointLabel );
        mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
        mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
        mLayout->addRow( QStringLiteral( "Use curvature corrections:" ), mCurvatureCorrections );
        mLayout->addRow( QStringLiteral( "Reffraction coefficient:" ), mRefractionCoefficient );
        mLayout->addRow( QStringLiteral( "Earth diamether:" ), mEarthDiameter );
        mLayout->addRow( QStringLiteral( "Set invisible areas to No Data for some visibility indices:" ),
                         mNoDataForInvisible );
        mLayout->addRow( QStringLiteral( "Folder for results:" ), mFolderWidget );
        mLayout->addRow( mCalculateButton );
        mLayout->addRow( mProgressBar );

        readSettings();

        connect( mViewshedType, qOverload<int>( &QComboBox::currentIndexChanged ), this, &MainWindow::saveSettings );
        connect( mNoDataForInvisible, &QCheckBox::stateChanged, this, &MainWindow::saveSettings );
        connect( mCurvatureCorrections, &QCheckBox::stateChanged, this, &MainWindow::saveSettings );
        connect( mRefractionCoefficient, &QLineEdit::textChanged, this, &MainWindow::saveSettings );
        connect( mEarthDiameter, &QLineEdit::textChanged, this, &MainWindow::saveSettings );
        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateDem );
        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateMask );
        connect( mMaskFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateMask );
        connect( mPointWidget, &PointWidget::pointChanged, this, &MainWindow::updatePoint );
        connect( mPointWidget, &PointWidget::pointXYChanged, this, &MainWindow::updatePointLabel );
        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::updatePointRaster );
        connect( mFolderWidget, &QgsFileWidget::fileChanged, this, &MainWindow::saveSettings );
        connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );
        connect( mObserverOffset, &QLineEdit::textChanged, this, &MainWindow::saveSettings );
        connect( mTargetOffset, &QLineEdit::textChanged, this, &MainWindow::saveSettings );

        enableCalculation();
    }

    void readSettings() { readValuesFromSettings( mSettings ); }

    void readValuesFromSettings( QSettings &settings )
    {
        int index = mViewshedType->findData( settings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
        mViewshedType->setCurrentIndex( index );

        mFileWidget->setFilePath( settings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

        mMaskFileWidget->setFilePath( settings.value( QStringLiteral( "mask" ), QStringLiteral( "" ) ).toString() );

        mPoint.fromWkt( settings.value( QStringLiteral( "point" ), QStringLiteral( "POINT(0 0)" ) ).toString() );
        mPointWidget->setPoint( mPoint );

        mObserverOffset->setText( settings.value( QStringLiteral( "observerOffset" ), "1.6" ).toString() );
        mTargetOffset->setText( settings.value( QStringLiteral( "targetOffset" ), "0.0" ).toString() );

        mFolderWidget->setFilePath(
            settings.value( QStringLiteral( "resultFolder" ), QStringLiteral( "" ) ).toString() );

        mCurvatureCorrections->setChecked(
            settings.value( QStringLiteral( "useCurvatureCorrections" ), false ).toBool() );

        mRefractionCoefficient->setText(
            settings.value( QStringLiteral( "reffractionCoefficient" ), QString::number( REFRACTION_COEFFICIENT, 'f' ) )
                .toString() );

        mEarthDiameter->setText(
            settings.value( QStringLiteral( "earthDiameter" ), QString::number( (double)EARTH_DIAMETER, 'f', 1 ) )
                .toString() );

        mNoDataForInvisible->setChecked( settings.value( QStringLiteral( "noDataForInvisible" ), true ).toBool() );

        validateDem();
        validateMask();
        updatePointLabel( mPointWidget->pointXY() );
    }

    void saveSettings() { saveCurrentValuesToSettings( mSettings ); }

    void saveCurrentValuesToSettings( QSettings &settings )
    {
        settings.setValue( QStringLiteral( "viewshedType" ), mViewshedType->currentData( Qt::UserRole ) );
        settings.setValue( QStringLiteral( "dem" ), mFileWidget->filePath() );
        settings.setValue( QStringLiteral( "mask" ), mMaskFileWidget->filePath() );
        settings.setValue( QStringLiteral( "point" ), mPointWidget->point().asWkt() );
        settings.setValue( QStringLiteral( "observerOffset" ), mObserverOffset->text() );
        settings.setValue( QStringLiteral( "targetOffset" ), mTargetOffset->text() );

        settings.setValue( QStringLiteral( "useCurvatureCorrections" ), mCurvatureCorrections->isChecked() );
        settings.setValue( QStringLiteral( "reffractionCoefficient" ), mRefractionCoefficient->text() );
        settings.setValue( QStringLiteral( "earthDiameter" ), mEarthDiameter->text() );

        settings.setValue( QStringLiteral( "noDataForInvisible" ), mNoDataForInvisible->isChecked() );

        settings.setValue( QStringLiteral( "resultFolder" ), mFolderWidget->filePath() );
    }

    void validateDem()
    {
        mPointWidget->setCrs( "Unkown" );
        mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );

        mCalculateButton->setEnabled( false );
        mDemValid = false;
        mDem = nullptr;

        if ( mFileWidget->filePath() == QStringLiteral( "" ) )
        {
            return;
        }

        std::shared_ptr<QgsRasterLayer> rl = std::make_shared<QgsRasterLayer>(
            mFileWidget->filePath(), QStringLiteral( "dem" ), QStringLiteral( "gdal" ) );

        std::string rasterError;
        if ( !ViewshedUtils::validateRaster( rl, rasterError ) )
        {
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ), QString::fromStdString( rasterError ) );
        }

        mDemValid = true;

        mDem = std::make_shared<QgsRasterLayer>( mFileWidget->filePath(), QStringLiteral( "dem" ),
                                                 QStringLiteral( "gdal" ) );

        mPointWidget->setCrs( mDem->crs().authid() );
        mEarthDiameter->setText( QString::number( ViewshedUtils::earthDiameter( mDem->crs() ), 'f' ) );

        enableCalculation();

        saveSettings();
    }

    void validateMask()
    {
        if ( !mDem || mMaskFileWidget->filePath() == QStringLiteral( "" ) )
        {
            return;
        }

        mMask = nullptr;

        std::shared_ptr<QgsRasterLayer> mask = std::make_shared<QgsRasterLayer>(
            mMaskFileWidget->filePath(), QStringLiteral( "mask" ), QStringLiteral( "gdal" ) );

        std::string rasterError;
        if ( !ViewshedUtils::validateRaster( mask, rasterError ) )
        {
            mMaskFileWidget->setFilePath( "" );
            mErrorMessageBox.critical( this, QStringLiteral( "Error" ), QString::fromStdString( rasterError ) );
        }

        if ( !ViewshedUtils::compareRasters( mDem, mask, rasterError ) )
        {
            mMaskFileWidget->setFilePath( "" );
            mErrorMessageBox.critical(
                this, QStringLiteral( "Error" ),
                QString::fromStdString( "Dem and Visibility Mask raster comparison. " + rasterError ) );
        }

        mMask = std::make_shared<QgsRasterLayer>( mMaskFileWidget->filePath(), QStringLiteral( "mask" ),
                                                  QStringLiteral( "gdal" ) );

        saveSettings();
    }

    void enableCalculation() { mCalculateButton->setEnabled( mDemValid && mPointWidget->isPointValid() ); }

    void updatePoint( QgsPoint point )
    {
        mPoint = point;
        saveSettings();
    }

    void updatePointRaster() { updatePoint( mPoint ); }

    void updatePointLabel( QgsPointXY point )
    {
        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( point ) )
            {
                mPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( point, 1, &sampledOk );

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

        prepareAlgorithms();

        bool useCurvartureCorrections = mCurvatureCorrections->checkState() == Qt::CheckState::Checked;
        double refractionCoefficient = QgsDoubleValidator::toDouble( mRefractionCoefficient->text() );
        double earthDimeter = QgsDoubleValidator::toDouble( mEarthDiameter->text() );

        mTimingMessages = "";

        std::function addTimingMessage = [=]( std::string text, double time )
        { mTimingMessages = mTimingMessages + text + std::to_string( time / 1000000000 ) + " seconds.\n"; };

        std::function setProgress = [=]( int size, int i )
        {
            if ( i % 1000 == 0 )
            {
                mProgressBar->setValue( (int)( ( (double)i / (double)size ) * 100 ) );
            }
        };

        using namespace std::chrono::_V2;
        using namespace std::chrono;

        system_clock::time_point startTime = high_resolution_clock::now();

        mProgressBar->setRange( 0, 100 );
        mProgressBar->setValue( 0 );

        if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
        {
            std::shared_ptr<Point> vp =
                std::make_shared<Point>( mPoint, mDem, QgsDoubleValidator::toDouble( mObserverOffset->text() ) );
            Viewshed v = Viewshed( vp, mDem, mAlgs, useCurvartureCorrections, earthDimeter, refractionCoefficient );

            if ( mMask )
            {
                v.setVisibilityMask( mMask );
            }

            v.calculate( addTimingMessage, setProgress );

            mProgressBar->setValue( 100 );

            v.saveResults( mFolderWidget->filePath() );
        }
        else if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeInverseViewshed )
        {
            std::shared_ptr<Point> tp =
                std::make_shared<Point>( mPoint, mDem, QgsDoubleValidator::toDouble( mTargetOffset->text() ) );
            InverseViewshed iv =
                InverseViewshed( tp, QgsDoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs,
                                 useCurvartureCorrections, earthDimeter, refractionCoefficient );

            if ( mMask )
            {
                iv.setVisibilityMask( mMask );
            }

            iv.calculate( addTimingMessage, setProgress );

            mProgressBar->setValue( 100 );

            iv.saveResults( mFolderWidget->filePath(), "Inverse" );
        }

        mCalculateButton->setEnabled( true );

        milliseconds elapsed = duration_cast<milliseconds>( high_resolution_clock::now() - startTime );

        statusBar()->showMessage( QString( "Calculation lasted: %1 seconds." ).arg( elapsed.count() / (double)1000 ),
                                  5 * 60 * 1000 );

        std::ofstream timingTextFile;
        QString filename = mFolderWidget->filePath() + QString( "/timing.txt" );
        timingTextFile.open( filename.toStdString() );
        timingTextFile << mTimingMessages;
        timingTextFile.close();
    }

  private:
    QFormLayout *mLayout = nullptr;
    QWidget *mWidget = nullptr;
    QgsFileWidget *mFileWidget = nullptr;
    QgsFileWidget *mMaskFileWidget = nullptr;
    QgsFileWidget *mFolderWidget = nullptr;
    QLineEdit *mObserverOffset = nullptr;
    QLineEdit *mTargetOffset = nullptr;
    QLabel *mPointLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QProgressBar *mProgressBar;
    QPushButton *mCalculateButton;
    QComboBox *mViewshedType;
    QCheckBox *mCurvatureCorrections;
    QLineEdit *mRefractionCoefficient;
    QLineEdit *mEarthDiameter;
    std::shared_ptr<QgsRasterLayer> mDem;
    std::shared_ptr<QgsRasterLayer> mMask;
    QSettings mSettings;
    QMessageBox mErrorMessageBox;
    PointWidget *mPointWidget = nullptr;
    QgsPoint mPoint;
    std::string mTimingMessages;

    bool mDemValid;

    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> mAlgs;
};

int main( int argc, char *argv[] )

{
    QApplication app( argc, argv );
    QApplication::setApplicationName( QStringLiteral( "Viewshed Calculator" ) );
    QApplication::setApplicationVersion( QStringLiteral( "0.2" ) );
    QCoreApplication::setOrganizationName( QStringLiteral( "JanCaha" ) );
    QCoreApplication::setOrganizationDomain( QStringLiteral( "cahik.cz" ) );
    QSettings::setDefaultFormat( QSettings::Format::IniFormat );

    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "viewshedcalculator.moc"