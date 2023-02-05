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
#include "utils.h"
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

    MainWindow() : mSettings( QSettings( this ) ), mDemValid( false ), mViewPointValid( false )
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
                     settings.setValue( QStringLiteral( "resultsFolder" ), mCsvFileWidget->filePath() );
                     settings.setValue( QStringLiteral( "observerX" ), mViewPointX->text() );
                     settings.setValue( QStringLiteral( "observerY" ), mViewPointY->text() );
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
                     mCsvFileWidget->setFilePath( settings.value( QStringLiteral( "resultsFolder" ), "" ).toString() );
                     mViewPointX->setText( settings.value( QStringLiteral( "observerX" ), "" ).toString() );
                     mViewPointY->setText( settings.value( QStringLiteral( "observerY" ), "" ).toString() );
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

        mViewPointX = new QLineEdit( this );
        mViewPointX->setValidator( mDoubleValidator );
        mViewPointY = new QLineEdit( this );
        mViewPointY->setValidator( mDoubleValidator );

        connect( mViewPointX, &QLineEdit::textChanged, this, &MainWindow::updateViewPoint );
        connect( mViewPointY, &QLineEdit::textChanged, this, &MainWindow::updateViewPoint );

        mViewPointX->setText( QVariant( mViewPoint.x() ).toString() );
        mViewPointY->setText( QVariant( mViewPoint.y() ).toString() );

        //
        mTargetPointX = new QLineEdit( this );
        mTargetPointX->setValidator( mDoubleValidator );
        mTargetPointY = new QLineEdit( this );
        mTargetPointY->setValidator( mDoubleValidator );

        connect( mTargetPointX, &QLineEdit::textChanged, this, &MainWindow::updateTargetPoint );
        connect( mTargetPointY, &QLineEdit::textChanged, this, &MainWindow::updateTargetPoint );

        mTargetPointX->setText( QVariant( mTargetPoint.x() ).toString() );
        mTargetPointY->setText( QVariant( mTargetPoint.y() ).toString() );
        //

        mObserverOffset = new QLineEdit( this );
        mObserverOffset->setValidator( mDoubleValidator );

        mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

        mTargetOffset = new QLineEdit( this );
        mTargetOffset->setValidator( mDoubleValidator );
        mTargetOffset->setText( QStringLiteral( "0.0" ) );

        mCsvFileWidget = new QgsFileWidget( this );
        mCsvFileWidget->setStorageMode( QgsFileWidget::StorageMode::SaveFile );
        mCsvFileWidget->setFilter( "*.csv" );
        mCsvFileWidget->setFilePath(
            mSettings.value( QStringLiteral( "resultFolder" ), QStringLiteral( "" ) ).toString() );

        connect( mCsvFileWidget, &QgsFileWidget::fileChanged, this, &MainWindow::updateResultFolder );

        connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );

        mProgressBar = new QProgressBar( this );

        mLayout->addRow( QStringLiteral( "Type of viewshed:" ), mViewshedType );
        mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
        mLayout->addRow( QStringLiteral( "ViewPoint coord X:" ), mViewPointX );
        mLayout->addRow( QStringLiteral( "ViewPoint coord Y:" ), mViewPointY );
        mLayout->addRow( QStringLiteral( "ViewPoint:" ), mViewPointLabel );
        mLayout->addRow( QStringLiteral( "TargetPoint coord X:" ), mTargetPointX );
        mLayout->addRow( QStringLiteral( "TargetPoint coord Y:" ), mTargetPointY );
        mLayout->addRow( QStringLiteral( "TargetPoint:" ), mTargetPointLabel );
        mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
        mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
        mLayout->addRow( QStringLiteral( "Folder for results:" ), mCsvFileWidget );
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

    void enableCalculation() { mCalculateButton->setEnabled( mDemValid && mViewPointValid && mTargetPointValid ); }

    void updateResultFolder() { mSettings.setValue( QStringLiteral( "resultFolder" ), mCsvFileWidget->filePath() ); }

    void updateViewPoint()
    {
        mViewPointValid = false;

        QString text = mViewPointX->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mViewPoint.setX( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        text = mViewPointY->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mViewPoint.setY( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( mViewPoint.x(), mViewPoint.y() ) )
            {
                mViewPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( QgsPointXY( mViewPoint.x(), mViewPoint.y() ), 1, &sampledOk );

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

        mViewPointValid = true;
        enableCalculation();
    }

    void updateTargetPoint()
    {
        mTargetPointValid = false;

        QString text = mTargetPointX->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mTargetPoint.setX( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        text = mTargetPointY->text();

        if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
        {
            mTargetPoint.setY( QgsDoubleValidator::toDouble( text ) );
        }
        else
            return;

        double elevation;
        bool sampledOk = false;

        if ( mDemValid )
        {

            if ( !mDem->extent().contains( mTargetPoint.x(), mTargetPoint.y() ) )
            {
                mTargetPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                return;
            }

            elevation = mDem->dataProvider()->sample( QgsPointXY( mTargetPoint.x(), mTargetPoint.y() ), 1, &sampledOk );

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

        mTargetPointValid = true;
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
    QLineEdit *mViewPointX = nullptr;
    QLineEdit *mViewPointY = nullptr;
    QLineEdit *mTargetPointX = nullptr;
    QLineEdit *mTargetPointY = nullptr;
    QLineEdit *mObserverOffset = nullptr;
    QLineEdit *mTargetOffset = nullptr;
    QLabel *mViewPointLabel = nullptr;
    QLabel *mTargetPointLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QgsPoint mViewPoint;
    QgsPoint mTargetPoint;
    QProgressBar *mProgressBar;
    QPushButton *mCalculateButton;
    QComboBox *mViewshedType;
    std::shared_ptr<QgsRasterLayer> mDem;
    QSettings mSettings;
    QMessageBox mErrorMessageBox;

    bool mDemValid;
    bool mViewPointValid;
    bool mTargetPointValid;

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