#include <string>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
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

#include "simplerasters.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshed.h"
#include "viewshedutils.h"
#include "visibilityalgorithms.h"

#include "doublevalidator.h"
#include "fileselectorwidget.h"
#include "pointwidget.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

#define DEFAULT_OBSERVER_OFFSET "1.6"

namespace ViewshedBinaries
{
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
            if ( mDem )
            {
                double noData = mDem->noData();
                mAlgs = ViewshedUtils::allAlgorithms( noData );
            }
        };

        void initMenu()
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
                     [this]
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

            mCalculateButton = new QPushButton( this );
            mCalculateButton->setText( QStringLiteral( "Extract!" ) );
            mCalculateButton->setEnabled( false );

            mViewPointWidget = new PointWidget( true, this );

            mTargetPointWidget = new PointWidget( true, this );

            mCsvFileWidget = new FileSelectorWidget( this );
            mCsvFileWidget->setStorageMode( FileSelectorWidget::StorageMode::SaveFile );
            mCsvFileWidget->setFilter( "*.csv" );

            mFileWidget = new FileSelectorWidget( this );
            mFileWidget->setStorageMode( FileSelectorWidget::GetFile );

            mFileWidget->setFilePath( mSettings.value( QStringLiteral( "dem" ), QStringLiteral( "" ) ).toString() );

            std::string p = mSettings.value( QStringLiteral( "viewpoint" ), QStringLiteral( "POINT(0 0)" ) )
                                .toString()
                                .toStdString();
            const char *poP = p.c_str();

            mViewPoint.importFromWkt( const_cast<const char **>( &poP ) );

            mViewPointLabel = new QLabel();
            mViewPointLabel->setText( QString::fromStdString( mViewPoint.exportToWkt() ) );

            p = mSettings.value( QStringLiteral( "targetpoint" ), QStringLiteral( "POINT(0 0)" ) )
                    .toString()
                    .toStdString();
            poP = p.c_str();

            mTargetPoint.importFromWkt( const_cast<const char **>( &poP ) );

            mTargetPointLabel = new QLabel();
            mTargetPointLabel->setText( QString::fromStdString( mViewPoint.exportToWkt() ) );

            mDoubleValidator = new DoubleValidator( this );

            mObserverOffset = new QLineEdit( this );
            mObserverOffset->setValidator( mDoubleValidator );

            mObserverOffset->setText( DEFAULT_OBSERVER_OFFSET );

            mTargetOffset = new QLineEdit( this );
            mTargetOffset->setValidator( mDoubleValidator );
            mTargetOffset->setText( QStringLiteral( "0.0" ) );

            mCurvatureCorrections = new QCheckBox( this );
            mRefractionCoefficient = new QLineEdit( this );
            mRefractionCoefficient->setText( QString::number( REFRACTION_COEFFICIENT, 'f' ) );
            mRefractionCoefficient->setValidator( mDoubleValidator );
            mEarthDiameter = new QLineEdit( this );
            mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );
            mEarthDiameter->setValidator( mDoubleValidator );

            mCsvFileWidget->setFilePath(
                mSettings.value( QStringLiteral( "resultCsv" ), QStringLiteral( "" ) ).toString() );

            mViewPointWidget->setPoint( mViewPoint );

            mTargetPointWidget->setPoint( mTargetPoint );

            mLayout->addRow( QStringLiteral( "Type of viewshed (LoS):" ), mViewshedType );
            mLayout->addRow( QStringLiteral( "Select DEM file:" ), mFileWidget );
            mLayout->addRow( QStringLiteral( "ViewPoint Coordinates (x,y):" ), mViewPointWidget );
            mLayout->addRow( QStringLiteral( "ViewPoint:" ), mViewPointLabel );
            mLayout->addRow( QStringLiteral( "TargetPoint Coordinates (x,y):" ), mTargetPointWidget );
            mLayout->addRow( QStringLiteral( "TargetPoint:" ), mTargetPointLabel );
            mLayout->addRow( QStringLiteral( "Observer offset:" ), mObserverOffset );
            mLayout->addRow( QStringLiteral( "Target offset:" ), mTargetOffset );
            mLayout->addRow( QStringLiteral( "Use curvature corrections:" ), mCurvatureCorrections );
            mLayout->addRow( QStringLiteral( "Refraction coefficient:" ), mRefractionCoefficient );
            mLayout->addRow( QStringLiteral( "Earth diameter:" ), mEarthDiameter );

            mLayout->addRow( QStringLiteral( "CSV output file:" ), mCsvFileWidget );
            mLayout->addRow( mCalculateButton );

            readSettings();

            connect( mViewshedType, qOverload<int>( &QComboBox::currentIndexChanged ), this,
                     &MainWindow::saveSettings );
            connect( mViewPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateViewPoint );
            connect( mViewPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateViewPointLabel );
            connect( mTargetPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateTargetPoint );
            connect( mTargetPointWidget, &PointWidget::pointChanged, this, &MainWindow::updateTargetPointLabel );
            connect( mFileWidget, &FileSelectorWidget::fileChanged, this, &MainWindow::demUpdated );
            connect( mCsvFileWidget, &FileSelectorWidget::fileChanged, this, &MainWindow::updateResultCsv );
            connect( mCalculateButton, &QPushButton::clicked, this, &MainWindow::calculateViewshed );

            // Qt 6
            // connect( mCurvatureCorrections, &QCheckBox::checkStateChanged, this, &MainWindow::saveSettings );
            connect( mCurvatureCorrections, &QCheckBox::stateChanged, this, &MainWindow::saveSettings );

            connect( mRefractionCoefficient, &QLineEdit::textChanged, this, &MainWindow::saveSettings );
            connect( mEarthDiameter, &QLineEdit::textChanged, this, &MainWindow::saveSettings );

            enableCalculation();
        }

        void readSettings() { readValuesFromSettings( mSettings ); }

        void readValuesFromSettings( QSettings &settings )
        {
            mFileWidget->setFilePath( settings.value( QStringLiteral( "dem" ), "" ).toString() );
            mCsvFileWidget->setFilePath( settings.value( QStringLiteral( "resultCsv" ), "" ).toString() );

            std::string p = mSettings.value( QStringLiteral( "viewpoint" ), QStringLiteral( "POINT(0 0)" ) )
                                .toString()
                                .toStdString();
            const char *poP = p.c_str();

            mViewPoint.importFromWkt( const_cast<const char **>( &poP ) );

            mViewPointWidget->setPoint( mViewPoint );

            p = mSettings.value( QStringLiteral( "targetPoint" ), QStringLiteral( "POINT(0 0)" ) )
                    .toString()
                    .toStdString();
            poP = p.c_str();

            mTargetPoint.importFromWkt( const_cast<const char **>( &poP ) );

            mTargetPointWidget->setPoint( mTargetPoint );

            mObserverOffset->setText( settings.value( QStringLiteral( "observerOffset" ), "" ).toString() );
            mTargetOffset->setText( settings.value( QStringLiteral( "targetOffset" ), "" ).toString() );

            int index = mViewshedType->findData( settings.value( QStringLiteral( "viewshedType" ), 0 ).toInt() );
            mViewshedType->setCurrentIndex( index );

            mCurvatureCorrections->setChecked(
                settings.value( QStringLiteral( "useCurvatureCorrections" ), false ).toBool() );

            mRefractionCoefficient->setText(
                settings
                    .value( QStringLiteral( "reffractionCoefficient" ), QString::number( REFRACTION_COEFFICIENT, 'f' ) )
                    .toString() );

            mEarthDiameter->setText(
                settings.value( QStringLiteral( "earthDiameter" ), QString::number( (double)EARTH_DIAMETER, 'f', 1 ) )
                    .toString() );

            demUpdated();
        }

        void saveSettings() { saveCurrentValuesToSettings( mSettings ); }

        void saveCurrentValuesToSettings( QSettings &settings )
        {
            settings.setValue( QStringLiteral( "dem" ), mFileWidget->filePath() );
            settings.setValue( QStringLiteral( "resultCsv" ), mCsvFileWidget->filePath() );
            settings.setValue( QStringLiteral( "viewPoint" ), QString::fromStdString( mViewPoint.exportToWkt() ) );
            settings.setValue( QStringLiteral( "targetPoint" ), QString::fromStdString( mTargetPoint.exportToWkt() ) );
            settings.setValue( QStringLiteral( "observerOffset" ), mObserverOffset->text() );
            settings.setValue( QStringLiteral( "targetOffset" ), mTargetOffset->text() );
            settings.setValue( QStringLiteral( "viewshedType" ), mViewshedType->currentData( Qt::UserRole ) );
            settings.setValue( QStringLiteral( "useCurvatureCorrections" ), mCurvatureCorrections->isChecked() );
            settings.setValue( QStringLiteral( "reffractionCoefficient" ), mRefractionCoefficient->text() );
            settings.setValue( QStringLiteral( "earthDiameter" ), mEarthDiameter->text() );
        }

        void demUpdated()
        {
            validateDem();
            prepareAlgorithms();
            enableCalculation();
            updateViewPointLabel( OGRPoint( mViewPoint.getX(), mViewPoint.getY() ) );
            updateTargetPointLabel( OGRPoint( mTargetPoint.getX(), mTargetPoint.getY() ) );

            saveSettings();
        }

        void validateDem()
        {
            std::string defaultCrs = "EPSG:4326";
            OGRSpatialReference srs = OGRSpatialReference( nullptr );
            srs.SetFromUserInput( defaultCrs.c_str() );

            mViewPointWidget->setCrs( srs );
            mTargetPointWidget->setCrs( srs );
            mEarthDiameter->setText( QString::number( (double)EARTH_DIAMETER, 'f', 1 ) );

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

            mViewPointWidget->setCrs( mDem->crs() );
            mTargetPointWidget->setCrs( mDem->crs() );

            mEarthDiameter->setText( QString::number( ViewshedUtils::earthDiameter( mDem->crs() ), 'f' ) );
        }

        void enableCalculation()
        {
            bool csvEmpty = mCsvFileWidget->filePath().isEmpty();
            mCalculateButton->setEnabled( mDemValid && mViewPointWidget->isPointValid() &&
                                          mTargetPointWidget->isPointValid() && !csvEmpty );
        }

        void updateResultCsv()
        {
            saveSettings();
            enableCalculation();
        }

        void updateViewPoint( OGRPoint point )
        {
            mViewPoint = point;
            saveSettings();
        }

        void updateViewPointLabel( OGRPoint point ) { updatePointLabel( point, mViewPointLabel ); }

        void updatePointLabel( OGRPoint point, QLabel *label )
        {
            double elevation;
            bool sampledNoData = false;

            if ( mDemValid )
            {

                if ( !mDem->isInside( point ) )
                {
                    mViewPointLabel->setText( QStringLiteral( "Point is located outside of the raster." ) );
                    return;
                }

                double row, colum;

                mDem->transformCoordinatesToRaster( std::make_shared<OGRPoint>( point.getX(), point.getY() ), row,
                                                    colum );

                sampledNoData = mDem->isNoData( row, colum );

                elevation = mDem->value( row, colum );

                if ( sampledNoData )
                {
                    label->setText( QString::fromStdString( mViewPoint.exportToWkt() ) +
                                    QString( " without valid elevation." ) );
                    return;
                }
            }

            if ( sampledNoData )
            {
                label->setText( QString::fromStdString( mViewPoint.exportToWkt() ) +
                                QString( " with elevation: %1" ).arg( elevation ) );
            }
            else
            {
                label->setText( QString::fromStdString( mViewPoint.exportToWkt() ) +
                                QString( " without valid elevation." ) );
            }

            enableCalculation();
        }

        void updateTargetPoint( OGRPoint point )
        {
            mTargetPoint = point;
            saveSettings();
        }

        void updateTargetPointLabel( OGRPoint point ) { updatePointLabel( point, mTargetPointLabel ); }

        void calculateViewshed()
        {
            mCalculateButton->setEnabled( false );

            QString csvSuffix = QStringLiteral( ".csv" );
            QString resultFile = mCsvFileWidget->filePath();

            if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeClassicViewshed )
            {
                std::shared_ptr<Point> vp =
                    std::make_shared<Point>( mViewPoint, mDem, DoubleValidator::toDouble( mObserverOffset->text() ) );

                Viewshed v = Viewshed( vp, mDem, mAlgs );
                v.initEventList();
                v.sortEventList();

                std::shared_ptr<LoS> los = v.getLoS( mTargetPoint );

                std::vector<DataTriplet> data = ViewshedUtils::distanceElevation( los );

                ViewshedUtils::saveToCsv( data, "distance,elevation,target_point\n", resultFile.toStdString() );

                QFileInfo fileInfo = QFileInfo( resultFile );
                QString visibilityIndicesFile = QString( "%1/%2_visibility_idices.csv" )
                                                    .arg( fileInfo.absoluteDir().path() )
                                                    .arg( fileInfo.baseName() );

                ViewshedUtils::saveToCsv( evalToRows( los ), "\"visibility index\", value\n",
                                          visibilityIndicesFile.toStdString() );

                mCalculateButton->setEnabled( true );
            }
            else if ( mViewshedType->currentData( Qt::UserRole ) == ViewshedType::TypeInverseViewshed )
            {
                std::shared_ptr<Point> tp =
                    std::make_shared<Point>( mTargetPoint, mDem, DoubleValidator::toDouble( mTargetOffset->text() ) );

                InverseViewshed iv =
                    InverseViewshed( tp, DoubleValidator::toDouble( mObserverOffset->text() ), mDem, mAlgs );
                iv.initEventList();
                iv.sortEventList();

                std::shared_ptr<InverseLoS> inverselos = iv.getLoS( mViewPoint );

                std::vector<DataTriplet> data = ViewshedUtils::distanceElevation( inverselos );

                ViewshedUtils::saveToCsv( data, "distance,elevation,target_point\n", resultFile.toStdString() );

                mCalculateButton->setEnabled( true );
            }
        }

        std::vector<std::string> evalToRows( std::shared_ptr<AbstractLoS> los )
        {
            std::vector<std::string> rows;
            LoSEvaluator losEval = LoSEvaluator( los, mAlgs );

            losEval.calculate();

            for ( std::size_t i = 0; i < mAlgs->size(); i++ )
            {
                std::string row = "\"" + mAlgs->at( i )->name() + "\";";
                row += std::to_string( losEval.resultAt( i ) );
                rows.push_back( row );
            }

            return rows;
        }

      private:
        QFormLayout *mLayout = nullptr;

        QWidget *mWidget = nullptr;
        FileSelectorWidget *mFileWidget = nullptr;
        FileSelectorWidget *mCsvFileWidget = nullptr;
        QLineEdit *mObserverOffset = nullptr;
        QLineEdit *mTargetOffset = nullptr;
        QLabel *mViewPointLabel = nullptr;
        QLabel *mTargetPointLabel = nullptr;
        DoubleValidator *mDoubleValidator = nullptr;
        OGRPoint mViewPoint;
        OGRPoint mTargetPoint;
        QPushButton *mCalculateButton;
        QComboBox *mViewshedType;
        QCheckBox *mCurvatureCorrections;
        QLineEdit *mRefractionCoefficient;
        QLineEdit *mEarthDiameter;
        std::shared_ptr<ProjectedSquareCellRaster> mDem;
        QSettings mSettings;
        QMessageBox mErrorMessageBox;
        PointWidget *mViewPointWidget = nullptr;
        PointWidget *mTargetPointWidget = nullptr;

        bool mDemValid;

        std::shared_ptr<ViewshedAlgorithms> mAlgs;
    };
} // namespace ViewshedBinaries

int main( int argc, char *argv[] )
{
    using ViewshedBinaries::MainWindow;

    QApplication app( argc, argv );
    QApplication::setApplicationName( QStringLiteral( "LoS Extractor" ) );
    QApplication::setApplicationVersion( QStringLiteral( "0.5" ) );
    QCoreApplication::setOrganizationName( QStringLiteral( "JanCaha" ) );
    QCoreApplication::setOrganizationDomain( QStringLiteral( "cahik.cz" ) );

    MainWindow mw;
    mw.show();
    return app.exec();
}

#include "losextractor.moc"