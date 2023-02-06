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
#include <QVariant>

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
                     settings.setValue( QStringLiteral( "resultCsv" ), mCsvFileWidget->filePath() );
                     settings.setValue( QStringLiteral( "viewPoint" ), mViewPoint.asWkt() );
                     settings.setValue( QStringLiteral( "targetPoint" ), mTargetPoint.asWkt() );
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
                     mCsvFileWidget->setFilePath( settings.value( QStringLiteral( "resultCsv" ), "" ).toString() );

                     mViewPoint.fromWkt(
                         settings.value( QStringLiteral( "viewPoint" ), QStringLiteral( "POINT(0 0)" ) ).toString() );

                     mViewPointWidget->setPoint( mViewPoint );

                     mTargetPoint.fromWkt(
                         settings.value( QStringLiteral( "targetPoint" ), QStringLiteral( "POINT(0 0)" ) ).toString() );
                     mTargetPointWidget->setPoint( mTargetPoint );

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
        mCalculateButton->setText( QStringLiteral( "Extract!" ) );

        mViewPointWidget = new PointWidget( this );

        mTargetPointWidget = new PointWidget( this );

        mCsvFileWidget = new QgsFileWidget( this );
        mCsvFileWidget->setStorageMode( QgsFileWidget::StorageMode::SaveFile );
        mCsvFileWidget->setFilter( "*.csv" );

        mFileWidget = new QgsFileWidget( this );
        mFileWidget->setFilter( QgsProviderRegistry::instance()->fileRasterFilters() );
        mFileWidget->setStorageMode( QgsFileWidget::GetFile );
        mFileWidget->setOptions( QFileDialog::HideNameFilterDetails );

        connect( mFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::validateDem );

        mFileWidget->setFilePath( mSettings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

        mViewPoint.fromWkt(
            mSettings.value( QStringLiteral( "viewpoint" ), QStringLiteral( "POINT(0 0)" ) ).toString() );

        mViewPointLabel = new QLabel();
        mViewPointLabel->setText( mViewPoint.asWkt( 5 ) );

        mTargetPoint.fromWkt(
            mSettings.value( QStringLiteral( "targetpoint" ), QStringLiteral( "POINT(0 0)" ) ).toString() );

        mTargetPointLabel = new QLabel();
        mTargetPointLabel->setText( mTargetPoint.asWkt( 5 ) );

        mDoubleValidator = new QgsDoubleValidator( this );

        mObserverOffset = new QLineEdit( this );
        mObserverOffset->setValidator( mDoubleValidator );

        mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

        mTargetOffset = new QLineEdit( this );
        mTargetOffset->setValidator( mDoubleValidator );
        mTargetOffset->setText( QStringLiteral( "0.0" ) );

        mCsvFileWidget->setFilePath(
            mSettings.value( QStringLiteral( "resultCsv" ), QStringLiteral( "" ) ).toString() );

        connect( mCsvFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::updateResultCsv );

        connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );

        mViewPointWidget->setPoint( mViewPoint );

        connect( mViewPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateViewPoint );
        connect( mViewPointWidget, &PointWidget::pointXYChanged, this, &MainWindow::updateViewPointLabel );

        mTargetPointWidget->setPoint( mTargetPoint );

        connect( mTargetPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateTargetPoint );
        connect( mTargetPointWidget, &PointWidget::pointXYChanged, this, &MainWindow::updateTargetPointLabel );

        mLayout->addRow( QStringLiteral( "Type of viewshed:" ), mViewshedType );
        mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
        mLayout->addRow( QStringLiteral( "ViewPoint Coordinates (x,y):" ), mViewPointWidget );
        mLayout->addRow( QStringLiteral( "ViewPoint:" ), mViewPointLabel );
        mLayout->addRow( QStringLiteral( "TargetPoint Coordinates (x,y):" ), mTargetPointWidget );
        mLayout->addRow( QStringLiteral( "TargetPoint:" ), mTargetPointLabel );
        mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
        mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
        mLayout->addRow( QStringLiteral( "Folder for results:" ), mCsvFileWidget );
        mLayout->addRow( mCalculateButton );

        enableCalculation();
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

    void enableCalculation()
    {
        bool csvEmpty = mCsvFileWidget->filePath().isEmpty();
        mCalculateButton->setEnabled( mDemValid && mViewPointWidget->isPointValid() &&
                                      mTargetPointWidget->isPointValid() && !csvEmpty );
    }

    void updateResultCsv()
    {
        mSettings.setValue( QStringLiteral( "resultCsv" ), mCsvFileWidget->filePath() );
        enableCalculation();
    }

    void updateViewPoint( QgsPoint point ) { mViewPoint = point; }

    void updateViewPointLabel( QgsPointXY point )
    {
        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( point ) )
            {
                mViewPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( point, 1, &sampledOk );

            if ( !sampledOk )
            {
                mViewPointLabel->setText( mViewPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
                return;
            }
        }

        if ( sampledOk )
        {
            mViewPointLabel->setText( mViewPoint.asWkt( 5 ) + QString( " with elevation: %1" ).arg( elevation ) );
        }
        else
        {
            mViewPointLabel->setText( mViewPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
        }

        mSettings.setValue( QStringLiteral( "viewpoint" ), mViewPoint.asWkt() );

        enableCalculation();
    }

    void updateTargetPoint( QgsPoint point ) { mTargetPoint = point; }

    void updateTargetPointLabel( QgsPointXY point )
    {
        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( point ) )
            {
                mTargetPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( point, 1, &sampledOk );

            if ( !sampledOk )
            {
                mTargetPointLabel->setText( mTargetPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
                return;
            }
        }

        if ( sampledOk )
        {
            mTargetPointLabel->setText( mTargetPoint.asWkt( 5 ) + QString( " with elevation: %1" ).arg( elevation ) );
        }
        else
        {
            mTargetPointLabel->setText( mTargetPoint.asWkt( 5 ) + QString( " without valid elevation." ) );
        }

        mSettings.setValue( QStringLiteral( "targetpoint" ), mTargetPoint.asWkt() );

        enableCalculation();
    }

    void calculateViewshed()
    {
        mCalculateButton->setEnabled( false );

        QString csvSuffix = QStringLiteral( ".csv" );

        QString resultFile = mCsvFileWidget->filePath();

        if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
        {
            std::shared_ptr<Point> vp =
                std::make_shared<Point>( mViewPoint, mDem, QgsDoubleValidator::toDouble( mObserverOffset->text() ) );

            Viewshed v = Viewshed( vp, mDem, mAlgs );
            v.initEventList();
            v.sortEventList();

            std::shared_ptr<LoS> los = v.getLoS( mTargetPoint );

            std::vector<DataTriplet> data = Utils::distanceElevation( los );

            Utils::saveToCsv( data, "distance,elevation,target_point\n", resultFile.toStdString() );

            mCalculateButton->setEnabled( true );
        }
        else if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeInverseViewshed )
        {
            std::shared_ptr<Point> tp =
                std::make_shared<Point>( mTargetPoint, mDem, QgsDoubleValidator::toDouble( mTargetOffset->text() ) );

            InverseViewshed iv =
                InverseViewshed( tp, QgsDoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs );
            iv.initEventList();
            iv.sortEventList();

            std::shared_ptr<InverseLoS> inverselos = iv.getLoS( mViewPoint );

            std::vector<DataTriplet> data = Utils::distanceElevation( inverselos );

            Utils::saveToCsv( data, "distance,elevation,target_point\n", resultFile.toStdString() );

            mCalculateButton->setEnabled( true );
        }
    }

  private:
    QFormLayout *mLayout = nullptr;

    QWidget *mWidget = nullptr;
    QgsFileWidget *mFileWidget = nullptr;
    QgsFileWidget *mCsvFileWidget = nullptr;
    QLineEdit *mObserverOffset = nullptr;
    QLineEdit *mTargetOffset = nullptr;
    QLabel *mViewPointLabel = nullptr;
    QLabel *mTargetPointLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QgsPoint mViewPoint;
    QgsPoint mTargetPoint;
    QPushButton *mCalculateButton;
    QComboBox *mViewshedType;
    std::shared_ptr<QgsRasterLayer> mDem;
    QSettings mSettings;
    QMessageBox mErrorMessageBox;
    PointWidget *mViewPointWidget = nullptr;
    PointWidget *mTargetPointWidget = nullptr;

    bool mDemValid;

    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> mAlgs;
};

int main( int argc, char *argv[] )

{
    QApplication app( argc, argv );
    QApplication::setApplicationName( QStringLiteral( "LoS Extractor" ) );
    QApplication::setApplicationVersion( QStringLiteral( "0.1" ) );
    QCoreApplication::setOrganizationName( QStringLiteral( "JanCaha" ) );
    QCoreApplication::setOrganizationDomain( QStringLiteral( "cahik.cz" ) );

    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "losextractor.moc"