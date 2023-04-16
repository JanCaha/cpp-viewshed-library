#include "QCheckBox"
#include "QDialog"
#include "QDialogButtonBox"
#include "QFormLayout"
#include "QLineEdit"
#include "QTabWidget"
#include "QVBoxLayout"

#include "qgis.h"
#include "qgsdoublespinbox.h"
#include "qgsdoublevalidator.h"

#include "integerspinbox.h"

class AlgorithmsDialog : public QDialog
{
    Q_OBJECT

  public:
    AlgorithmsDialog( QWidget *parent = nullptr );

    void initGui();

  private:
    QDialogButtonBox *mButtonBox = nullptr;
    QTabWidget *mTabWidget = nullptr;

    QgsDoubleValidator *mDoubleValidator = new QgsDoubleValidator( this );

    QWidget *mPage1 = nullptr;
    QCheckBox *mBinaryViewshed = nullptr;
    IntegerSpinBox *mBinaryViewshedVisible = nullptr;
    IntegerSpinBox *mBinaryViewshedInvisible = nullptr;

    QWidget *mPage2 = nullptr;
    QCheckBox *mHorizons = nullptr;
    IntegerSpinBox *mLocalHorizon = nullptr;
    IntegerSpinBox *mGlobalHorizon = nullptr;
    IntegerSpinBox *mNotHorizon = nullptr;

    QWidget *mPage3 = nullptr;
    QCheckBox *mHorizonsCount = nullptr;
    QCheckBox *mHorizonsCountBefore = nullptr;
    QCheckBox *mHorizonsCountAfter = nullptr;

    QWidget *mPage4 = nullptr;
    QCheckBox *mViewAngle = nullptr;

    void setUpPage1();
    void setUpPage2();
    void setUpPage3();
    void setUpPage4();
};
