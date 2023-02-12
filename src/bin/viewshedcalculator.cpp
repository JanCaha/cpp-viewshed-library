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
#include <QTextEdit>

#include "qgsdoublevalidator.h"
#include "qgsfilewidget.h"
#include "qgspoint.h"
#include "qgsproviderregistry.h"
#include "qgsrasterlayer.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "utils.h"
#include "viewshed.h"
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
        resize( mSettings.value( QStringLiteral( "UI/mainwindowsize" ), QSize( 600, 400 ) ).toSize() );
    }

    void resizeEvent( QResizeEvent *event )
    {
        mSettings.setValue( QStringLiteral( "UI/mainwindowsize" ), event->size() );
    }

    void prepareAlgorithms()
    {
        double noData = mDem->dataProvider()->sourceNoDataValue( 1 );

        mAlgs = std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();

        mAlgs->push_back( std::make_shared<Boolean>() );
        mAlgs->push_back( std::make_shared<Horizons>() );
        mAlgs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, noData ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, noData ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false, noData ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true ) );
        mAlgs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false, noData ) );
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
                     settings.setValue( QStringLiteral( "point" ), mPoint.asWkt() );
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

                     mPoint.fromWkt(
                         settings.value( QStringLiteral( "point" ), QStringLiteral( "POINT(0 0)" ) ).toString() );
                     mPointWidget->setPoint( mPoint );

                     mObserverOffset->setText( settings.value( QStringLiteral( "observerOffset" ), "" ).toString() );
                     mTargetOffset->setText( settings.value( QStringLiteral( "targetOffset" ), "" ).toString() );
                     int index =
                         mViewshedType->findData( settings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
                     mViewshedType->setCurrentIndex( index );
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

                     mReffractionCoefficient->setText( QString::number( REFRACTION_COEFFICIENT, 'f' ) );
                     mEarthDiamether->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );
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

        mCalculateButton = new QPushButton( this );
        mCalculateButton->setText( QStringLiteral( "Calculate!" ) );

        mPointWidget = new PointWidget( true, this );

        mCurvatureCorrections = new QCheckBox( this );
        mReffractionCoefficient = new QLineEdit( this );
        mReffractionCoefficient->setText( QString::number( REFRACTION_COEFFICIENT, 'f' ) );
        mReffractionCoefficient->setValidator( mDoubleValidator );
        mEarthDiamether = new QLineEdit( this );
        mEarthDiamether->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );
        mEarthDiamether->setValidator( mDoubleValidator );

        mFileWidget = new QgsFileWidget( this );
        mFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
        mFileWidget->setStorageMode( QgsFileWidget::GetFile );
        mFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

        mPointLabel = new QLabel();
        mPointLabel->setText( mPoint.asWkt( 5 ) );

        mDoubleValidator = new QgsDoubleValidator( this );

        mObserverOffset = new QLineEdit( this );
        mObserverOffset->setValidator( mDoubleValidator );

        mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

        mTargetOffset = new QLineEdit( this );
        mTargetOffset->setValidator( mDoubleValidator );
        mTargetOffset->setText( QStringLiteral( "0.0" ) );

        mFolderWidget = new QgsFileWidget( this );
        mFolderWidget->setStorageMode( QgsFileWidget::StorageMode::GetDirectory );

        mProgressBar = new QProgressBar( this );

        mLayout->addRow( QStringLiteral( "Type of viewshed:" ), mViewshedType );
        mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
        mLayout->addRow( QStringLiteral( "Important Point Coordinates (x,y):" ), mPointWidget );
        mLayout->addRow( QStringLiteral( "Point:" ), mPointLabel );
        mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
        mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
        mLayout->addRow( QStringLiteral( "Use curvature corrections:" ), mCurvatureCorrections );
        mLayout->addRow( QStringLiteral( "Reffraction coefficient:" ), mReffractionCoefficient );
        mLayout->addRow( QStringLiteral( "Earth diamether:" ), mEarthDiamether );
        mLayout->addRow( QStringLiteral( "Folder for results:" ), mFolderWidget );
        mLayout->addRow( mCalculateButton );
        mLayout->addRow( mProgressBar );

        readFromSettings();

        connect( mViewshedType, qOverload<int>( &QComboBox::currentIndexChanged ), this, &MainWindow::saveSettings );
        connect( mCurvatureCorrections, &QCheckBox::stateChanged, this, &MainWindow::saveSettings );
        connect( mReffractionCoefficient, &QLineEdit::textChanged, this, &MainWindow::saveSettings );
        connect( mEarthDiamether, &QLineEdit::textChanged, this, &MainWindow::saveSettings );

        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateDem );

        connect( mPointWidget, &PointWidget::pointChanged, this, &MainWindow::updatePoint );
        connect( mPointWidget, &PointWidget::pointXYChanged, this, &MainWindow::updatePointLabel );
        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::updatePointRaster );

        connect( mFolderWidget, &QgsFileWidget::fileChanged, this, &MainWindow::saveSettings );
        connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );

        enableCalculation();
    }

    void readFromSettings()
    {
        int index = mViewshedType->findData( mSettings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
        mViewshedType->setCurrentIndex( index );

        mFileWidget->setFilePath( mSettings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

        std::string wkt =
            mSettings.value( QStringLiteral( "point" ), QStringLiteral( "POINT(0 0)" ) ).toString().toStdString();

        mPoint.fromWkt( mSettings.value( QStringLiteral( "point" ), QStringLiteral( "POINT(0 0)" ) ).toString() );
        mPointWidget->setPoint( mPoint );

        mFolderWidget->setFilePath(
            mSettings.value( QStringLiteral( "resultFolder" ), QStringLiteral( "" ) ).toString() );

        mCurvatureCorrections->setChecked(
            mSettings.value( QStringLiteral( "useCurvatureCorrections" ), false ).toBool() );

        mReffractionCoefficient->setText(
            mSettings
                .value( QStringLiteral( "reffractionCoefficient" ), QString::number( REFRACTION_COEFFICIENT, 'g' ) )
                .toString() );

        mEarthDiamether->setText(
            mSettings.value( QStringLiteral( "earthDiameter" ), QString::number( (double)EARTH_DIAMETER, 'g', 1 ) )
                .toString() );
    }

    void saveSettings()
    {
        mSettings.setValue( QStringLiteral( "viewshedType" ), mViewshedType->currentData( Qt::UserRole ) );
        mSettings.setValue( QStringLiteral( "dem" ), mFileWidget->filePath() );
        mSettings.setValue( QStringLiteral( "point" ), mPointWidget->point().asWkt() );
        mSettings.setValue( QStringLiteral( "observerOffset" ), mObserverOffset->text() );
        mSettings.setValue( QStringLiteral( "targetOffset" ), mTargetOffset->text() );

        mSettings.setValue( QStringLiteral( "useCurvatureCorrections" ), mCurvatureCorrections->isChecked() );
        mSettings.setValue( QStringLiteral( "reffractionCoefficient" ), mReffractionCoefficient->text() );
        mSettings.setValue( QStringLiteral( "earthDiameter" ), mEarthDiamether->text() );

        mSettings.setValue( QStringLiteral( "resultFolder" ), mFolderWidget->filePath() );
    }

    void validateDem()
    {
        mPointWidget->setCrs( "Unkown" );
        mEarthDiamether->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );

        mCalculateButton->setEnabled( false );
        mDemValid = false;
        mDem = nullptr;

        if ( mFileWidget->filePath() == QStringLiteral( "" ) )
        {
            return;
        }

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

        mDemValid = true;

        mDem = std::make_shared<QgsRasterLayer>( mFileWidget->filePath(), QStringLiteral( "dem" ),
                                                 QStringLiteral( "gdal" ) );

        mPointWidget->setCrs( mDem->crs().geographicCrsAuthId() );
        mEarthDiamether->setText( QString::number( Utils::earthDiameter( mDem->crs() ), 'f' ) );

        enableCalculation();

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
        double refractionCoefficient = QgsDoubleValidator::toDouble( mReffractionCoefficient->text() );
        double earthDimeter = QgsDoubleValidator::toDouble( mEarthDiamether->text() );

        if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
        {
            std::shared_ptr<Point> vp =
                std::make_shared<Point>( mPoint, mDem, QgsDoubleValidator::toDouble( mObserverOffset->text() ) );
            Viewshed v = Viewshed( vp, mDem, mAlgs, useCurvartureCorrections, earthDimeter, refractionCoefficient );

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
                InverseViewshed( tp, QgsDoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs,
                                 useCurvartureCorrections, earthDimeter, refractionCoefficient );

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
    QLineEdit *mObserverOffset = nullptr;
    QLineEdit *mTargetOffset = nullptr;
    QLabel *mPointLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QProgressBar *mProgressBar;
    QPushButton *mCalculateButton;
    QComboBox *mViewshedType;
    QCheckBox *mCurvatureCorrections;
    QLineEdit *mReffractionCoefficient;
    QLineEdit *mEarthDiamether;
    std::shared_ptr<QgsRasterLayer> mDem;
    QSettings mSettings;
    QMessageBox mErrorMessageBox;
    PointWidget *mPointWidget = nullptr;
    QgsPoint mPoint;

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