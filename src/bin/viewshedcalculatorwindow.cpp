
#include <chrono>
#include <fstream>

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshed.h"
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityalgorithms.h"

#include "doublevalidator.h"
#include "fileselectorwidget.h"
#include "pointwidget.h"

#include "viewshedcalculatorwindow.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;
using ViewshedBinaries::MainCalculatorWindow;

MainCalculatorWindow::MainCalculatorWindow() : mSettings( QSettings( this ) ), mDemValid( false )
{
    initMenu();
    initGui();
    resize( mSettings.value( QStringLiteral( "UI/MainCalculatorWindowsize" ), QSize( 800, 400 ) ).toSize() );
}

void MainCalculatorWindow::resizeEvent( QResizeEvent *event )
{
    mSettings.setValue( QStringLiteral( "UI/MainCalculatorWindowsize" ), event->size() );
}

void MainCalculatorWindow::prepareAlgorithms()
{
    double noData = mDem->noData();

    mAlgs = std::make_shared<ViewshedAlgorithms>();

    if ( mNoDataForInvisible->isChecked() )
    {
        mAlgs = ViewshedUtils::allAlgorithms( noData );
    }
    else
    {
        mAlgs = ViewshedUtils::allAlgorithms();
    }
};

void MainCalculatorWindow::initMenu()
{
    QMenu *menu = new QMenu( QStringLiteral( "File" ), this );
    menuBar()->addMenu( menu );

    QAction *saveFile = new QAction( "Save settings", this );
    menu->addAction( saveFile );

    connect( saveFile, &QAction::triggered, this,
             [this]
             {
                 QString lastUsedDir =
                     mSettings.value( QStringLiteral( "lastUsedDirForSettings" ), QDir::homePath() ).toString();

                 QString outputFileName = QFileDialog::getSaveFileName(
                     this, QStringLiteral( "Save settings" ), lastUsedDir, QStringLiteral( "Settings file (*.ini)" ) );

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
             [this]
             {
                 QString lastUsedDir =
                     mSettings.value( QStringLiteral( "lastUsedDirForSettings" ), QDir::homePath() ).toString();

                 QString inputFileName = QFileDialog::getOpenFileName(
                     this, QStringLiteral( "Load settings" ), lastUsedDir, QStringLiteral( "Settings file (*.ini)" ) );

                 if ( inputFileName.isEmpty() )
                     return;

                 QSettings settings = QSettings( inputFileName, QSettings::IniFormat, this );

                 readValuesFromSettings( settings );
             } );

    menu->addSeparator();

    QAction *resetSettings = new QAction( "Reset settings", this );
    menu->addAction( resetSettings );

    connect( resetSettings, &QAction::triggered, this,
             [this]
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

void MainCalculatorWindow::initGui()
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

    mFileWidget = new FileSelectorWidget( this );
    // mFileWidget->setFilter( simplerasters::rasterFormatsFileFilters() );
    mFileWidget->setStorageMode( FileSelectorWidget::GetFile );
    // mFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

    mMaskFileWidget = new FileSelectorWidget( this );
    // mMaskFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
    mMaskFileWidget->setStorageMode( FileSelectorWidget::GetFile );

    mPointLabel = new QLabel();
    mPointLabel->setText( QString::fromStdString( mPoint.exportToWkt() ) );

    mDoubleValidator = new DoubleValidator( this );

    mObserverOffset = new QLineEdit( this );
    mObserverOffset->setValidator( mDoubleValidator );

    mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

    mTargetOffset = new QLineEdit( this );
    mTargetOffset->setValidator( mDoubleValidator );
    mTargetOffset->setText( QStringLiteral( "0.0" ) );

    mNoDataForInvisible = new QCheckBox( this );
    mNoDataForInvisible->setChecked( true );

    mFolderWidget = new FileSelectorWidget( this );
    mFolderWidget->setStorageMode( FileSelectorWidget::StorageMode::GetDirectory );

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

    // readSettings();

    connect( mViewshedType, qOverload<int>( &QComboBox::currentIndexChanged ), this,
             &MainCalculatorWindow::saveSettings );
    connect( mNoDataForInvisible, &QCheckBox::stateChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mCurvatureCorrections, &QCheckBox::stateChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mRefractionCoefficient, &QLineEdit::textChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mEarthDiameter, &QLineEdit::textChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mFileWidget, &FileSelectorWidget::fileChanged, this, &MainCalculatorWindow::validateDem );
    connect( mFileWidget, &FileSelectorWidget::fileChanged, this, &MainCalculatorWindow::validateMask );
    connect( mFileWidget, &FileSelectorWidget::fileChanged, this, &MainCalculatorWindow::updatePointRaster );
    connect( mMaskFileWidget, &FileSelectorWidget::fileChanged, this, &MainCalculatorWindow::validateMask );
    connect( mPointWidget, &PointWidget::pointChanged, this, &MainCalculatorWindow::updatePoint );
    connect( mPointWidget, &PointWidget::pointChanged, this, &MainCalculatorWindow::updatePointLabel );
    connect( mFolderWidget, &FileSelectorWidget::fileChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mCalculateButton, &QPushButton::clicked, this, &MainCalculatorWindow::calculateViewshed );
    connect( mObserverOffset, &QLineEdit::textChanged, this, &MainCalculatorWindow::saveSettings );
    connect( mTargetOffset, &QLineEdit::textChanged, this, &MainCalculatorWindow::saveSettings );

    enableCalculation();
}

void MainCalculatorWindow::readSettings() { readValuesFromSettings( mSettings ); }

void MainCalculatorWindow::readValuesFromSettings( QSettings &settings )
{
    int index = mViewshedType->findData( settings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
    mViewshedType->setCurrentIndex( index );

    mFileWidget->setFilePath( settings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

    mMaskFileWidget->setFilePath( settings.value( QStringLiteral( "mask" ), QStringLiteral( "" ) ).toString() );

    std::string p =
        settings.value( QStringLiteral( "point" ), QStringLiteral( "POINT(0 0)" ) ).toString().toStdString();
    const char *poP = p.c_str();

    mPoint.importFromWkt( const_cast<const char **>( &poP ) );
    mPointWidget->setPoint( mPoint );

    mObserverOffset->setText( settings.value( QStringLiteral( "observerOffset" ), "1.6" ).toString() );
    mTargetOffset->setText( settings.value( QStringLiteral( "targetOffset" ), "0.0" ).toString() );

    mFolderWidget->setFilePath( settings.value( QStringLiteral( "resultFolder" ), QStringLiteral( "" ) ).toString() );

    mCurvatureCorrections->setChecked( settings.value( QStringLiteral( "useCurvatureCorrections" ), false ).toBool() );

    mRefractionCoefficient->setText(
        settings.value( QStringLiteral( "reffractionCoefficient" ), QString::number( REFRACTION_COEFFICIENT, 'f' ) )
            .toString() );

    mEarthDiameter->setText(
        settings.value( QStringLiteral( "earthDiameter" ), QString::number( (double)EARTH_DIAMETER, 'f', 1 ) )
            .toString() );

    mNoDataForInvisible->setChecked( settings.value( QStringLiteral( "noDataForInvisible" ), true ).toBool() );

    validateDem();
    validateMask();
    updatePointLabel( mPointWidget->point() );
}

void MainCalculatorWindow::saveSettings() { saveCurrentValuesToSettings( mSettings ); }

void MainCalculatorWindow::saveCurrentValuesToSettings( QSettings &settings )
{
    settings.setValue( QStringLiteral( "viewshedType" ), mViewshedType->currentData( Qt::UserRole ) );
    settings.setValue( QStringLiteral( "dem" ), mFileWidget->filePath() );
    settings.setValue( QStringLiteral( "mask" ), mMaskFileWidget->filePath() );
    settings.setValue( QStringLiteral( "point" ), QString::fromStdString( mPointWidget->point().exportToWkt() ) );
    settings.setValue( QStringLiteral( "observerOffset" ), mObserverOffset->text() );
    settings.setValue( QStringLiteral( "targetOffset" ), mTargetOffset->text() );

    settings.setValue( QStringLiteral( "useCurvatureCorrections" ), mCurvatureCorrections->isChecked() );
    settings.setValue( QStringLiteral( "reffractionCoefficient" ), mRefractionCoefficient->text() );
    settings.setValue( QStringLiteral( "earthDiameter" ), mEarthDiameter->text() );

    settings.setValue( QStringLiteral( "noDataForInvisible" ), mNoDataForInvisible->isChecked() );

    settings.setValue( QStringLiteral( "resultFolder" ), mFolderWidget->filePath() );
}

void MainCalculatorWindow::validateDem()
{

    std::string defaultCrs = "EPSG:4326";
    OGRSpatialReference srs = OGRSpatialReference( nullptr );
    srs.SetFromUserInput( defaultCrs.c_str() );

    mPointWidget->setCrs( srs );
    mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );

    mCalculateButton->setEnabled( false );
    mDemValid = false;
    mDem = nullptr;

    if ( mFileWidget->filePath() == QStringLiteral( "" ) )
    {
        return;
    }

    std::shared_ptr<ProjectedSquareCellRaster> rl =
        std::make_shared<ProjectedSquareCellRaster>( mFileWidget->filePath().toStdString() );

    std::string rasterError;
    if ( !ViewshedUtils::validateRaster( rl, rasterError ) )
    {
        mErrorMessageBox.critical( this, QStringLiteral( "Error" ),
                                   mFileWidget->filePath() + QString( ": \n" ) +
                                       QString::fromStdString( rasterError ) );
        mFileWidget->setFilePath( "" );
        return;
    }

    mDemValid = true;

    mDem = std::make_shared<ProjectedSquareCellRaster>( mFileWidget->filePath().toStdString() );

    mPointWidget->setCrs( mDem->crs() );
    mEarthDiameter->setText( QString::number( ViewshedUtils::earthDiameter( mDem->crs() ), 'f' ) );

    enableCalculation();

    saveSettings();
}

void MainCalculatorWindow::validateMask()
{
    if ( !mDem || mMaskFileWidget->filePath() == QStringLiteral( "" ) )
    {
        return;
    }

    mMask = nullptr;

    std::shared_ptr<ProjectedSquareCellRaster> mask =
        std::make_shared<ProjectedSquareCellRaster>( mMaskFileWidget->filePath().toStdString() );

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

    mMask = std::make_shared<ProjectedSquareCellRaster>( mMaskFileWidget->filePath().toStdString() );

    saveSettings();
}

void MainCalculatorWindow::enableCalculation()
{
    mCalculateButton->setEnabled( mDemValid && mPointWidget->isPointValid() );
}

void MainCalculatorWindow::updatePoint( OGRPoint point )
{
    mPoint = point;
    saveSettings();
}

void MainCalculatorWindow::updatePointRaster() { updatePoint( mPoint ); }

void MainCalculatorWindow::updatePointLabel( OGRPoint point )
{
    double elevation;
    bool sampledNoData = false;

    if ( mDemValid )
    {

        if ( !mDem->isInside( point ) )
        {
            mPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
            return;
        }

        double row, colum;

        mDem->transformCoordinatesToRaster( std::make_shared<OGRPoint>( point.getX(), point.getY() ), row, colum );

        sampledNoData = mDem->isNoData( row, colum );

        elevation = mDem->value( row, colum );

        if ( sampledNoData )
        {
            mPointLabel->setText( QString::fromStdString( mPoint.exportToWkt() ) +
                                  QString( " without valid elevation." ) );
            return;
        }
    }

    if ( !sampledNoData )
    {
        mPointLabel->setText( QString::fromStdString( mPoint.exportToWkt() ) +
                              QString( " with elevation: %1" ).arg( elevation ) );
    }
    else
    {
        mPointLabel->setText( QString::fromStdString( mPoint.exportToWkt() ) + QString( " without valid elevation." ) );
    }

    enableCalculation();
}

void MainCalculatorWindow::calculateViewshed()
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
    double refractionCoefficient = DoubleValidator::toDouble( mRefractionCoefficient->text() );
    double earthDimeter = DoubleValidator::toDouble( mEarthDiameter->text() );

    mTimingMessages = "";

    std::function addTimingMessage = [this]( std::string text, double time )
    { mTimingMessages = mTimingMessages + text + std::to_string( time / 1000000000 ) + " seconds.\n"; };

    std::function setProgress = [this]( int size, int i )
    {
        if ( i % 1000 == 0 )
        {
            mProgressBar->setValue( (int)( ( (double)i / (double)size ) * 100 ) );
        }
    };

    using namespace std::chrono;

    steady_clock::time_point startTime = steady_clock::now();

    mProgressBar->setRange( 0, 100 );
    mProgressBar->setValue( 0 );

    if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
    {
        std::shared_ptr<Point> vp =
            std::make_shared<Point>( mPoint, mDem, DoubleValidator::toDouble( mObserverOffset->text() ) );
        Viewshed v = Viewshed( vp, mDem, mAlgs, useCurvartureCorrections, earthDimeter, refractionCoefficient );

        if ( mMask )
        {
            v.setVisibilityMask( mMask );
        }

        v.calculate( addTimingMessage, setProgress );

        mProgressBar->setValue( 100 );

        v.saveResults( mFolderWidget->filePath().toStdString() );
    }
    else if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeInverseViewshed )
    {
        std::shared_ptr<Point> tp =
            std::make_shared<Point>( mPoint, mDem, DoubleValidator::toDouble( mTargetOffset->text() ) );
        InverseViewshed iv = InverseViewshed( tp, DoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs,
                                              useCurvartureCorrections, earthDimeter, refractionCoefficient );

        if ( mMask )
        {
            iv.setVisibilityMask( mMask );
        }

        iv.calculate( addTimingMessage, setProgress );

        mProgressBar->setValue( 100 );

        iv.saveResults( mFolderWidget->filePath().toStdString(), "Inverse" );
    }

    mCalculateButton->setEnabled( true );

    milliseconds elapsed = duration_cast<milliseconds>( steady_clock::now() - startTime );

    statusBar()->showMessage( QString( "Calculation lasted: %1 seconds." ).arg( elapsed.count() / (double)1000 ),
                              5 * 60 * 1000 );

    std::ofstream timingTextFile;
    QString filename = mFolderWidget->filePath() + QString( "/timing.txt" );
    timingTextFile.open( filename.toStdString() );
    timingTextFile << mTimingMessages;
    timingTextFile.close();
}
