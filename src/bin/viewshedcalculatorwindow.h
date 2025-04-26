#pragma once

#include <chrono>
#include <fstream>

#include <QDebug>

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

#include "viewshedtypes.h"

#include "doublevalidator.h"
#include "fileselectorwidget.h"
#include "pointwidget.h"

using namespace viewshed;

#define DEFAULT_OBSERVER_OFFSET "1.6"

class OGRPoint;
class ProjectedSquareCellRaster;

namespace ViewshedBinaries
{
    class MainCalculatorWindow : public QMainWindow
    {
        Q_OBJECT

      public:
        enum ViewshedType
        {
            TypeClassicViewshed,
            TypeInverseViewshed,
        };

        MainCalculatorWindow();

        virtual ~MainCalculatorWindow() {};

        void resizeEvent( QResizeEvent *event );
        void prepareAlgorithms();

        void initMenu();
        void initGui();

        void readSettings();

        void readValuesFromSettings( QSettings &settings );

        void saveSettings();

        void saveCurrentValuesToSettings( QSettings &settings );

        void validateDem();

        void validateMask();

        void enableCalculation();

        void updatePoint( OGRPoint point );

        void updatePointRaster();

        void updatePointLabel( OGRPoint point );

        void calculateViewshed();

      private:
        QFormLayout *mLayout = nullptr;
        QWidget *mWidget = nullptr;
        FileSelectorWidget *mFileWidget = nullptr;
        FileSelectorWidget *mMaskFileWidget = nullptr;
        FileSelectorWidget *mFolderWidget = nullptr;
        QLineEdit *mObserverOffset = nullptr;
        QLineEdit *mTargetOffset = nullptr;
        QLabel *mPointLabel = nullptr;
        DoubleValidator *mDoubleValidator = nullptr;
        QProgressBar *mProgressBar;
        QPushButton *mCalculateButton;
        QComboBox *mViewshedType;
        QCheckBox *mCurvatureCorrections;
        QLineEdit *mRefractionCoefficient;
        QLineEdit *mEarthDiameter;
        QCheckBox *mNoDataForInvisible;

        std::shared_ptr<ProjectedSquareCellRaster> mDem;
        std::shared_ptr<ProjectedSquareCellRaster> mMask;
        QSettings mSettings;
        QMessageBox mErrorMessageBox;
        PointWidget *mPointWidget = nullptr;
        OGRPoint mPoint;
        std::string mTimingMessages;

        bool mDemValid;

        std::shared_ptr<ViewshedAlgorithms> mAlgs;

        friend class TestViewshedCalculatorWindow;
    };
} // namespace ViewshedBinaries