#include "algorithmsdialog.h"

AlgorithmsDialog::AlgorithmsDialog( QWidget *parent ) : QDialog( parent ) { initGui(); }

void AlgorithmsDialog::initGui()
{
    mButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    connect( mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );

    mTabWidget = new QTabWidget( this );

    setUpPage1();
    setUpPage2();
    setUpPage3();
    setUpPage4();
    setUpPage5();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget( mTabWidget );
    mainLayout->addWidget( mButtonBox );
    setLayout( mainLayout );
}

void AlgorithmsDialog::setUpPage1()
{
    mPage1 = new QWidget( this );
    QFormLayout *layout = new QFormLayout();
    mPage1->setLayout( layout );
    mBinaryViewshed = new QCheckBox( this );
    mBinaryViewshed->setChecked( true );
    mBinaryViewshedVisible = new IntegerSpinBox( 1, this );
    mBinaryViewshedInvisible = new IntegerSpinBox( 0, this );
    layout->addRow( "Calculate: ", mBinaryViewshed );
    layout->addRow( "Visible value: ", mBinaryViewshedVisible );
    layout->addRow( "Invisible value: ", mBinaryViewshedInvisible );

    mTabWidget->addTab( mPage1, "Binary viewshed" );
}

void AlgorithmsDialog::setUpPage2()
{
    mPage2 = new QWidget( this );
    QFormLayout *layout = new QFormLayout();
    mPage2->setLayout( layout );
    mHorizons = new QCheckBox( this );
    mHorizons->setChecked( true );
    mLocalHorizon = new IntegerSpinBox( 1, false );

    mGlobalHorizon = new IntegerSpinBox( 2, false );

    mNotHorizon = new IntegerSpinBox( 0, true );
    layout->addRow( "Calculate: ", mHorizons );
    layout->addRow( "Local Horizon: ", mLocalHorizon );
    layout->addRow( "Global Horizon: ", mGlobalHorizon );
    layout->addRow( "Not Horizon: ", mNotHorizon );

    mTabWidget->addTab( mPage2, "Horizons" );
}

void AlgorithmsDialog::setUpPage3()
{
    mPage3 = new QWidget( this );
    QFormLayout *layout = new QFormLayout();
    mPage3->setLayout( layout );
    mHorizonsCount = new QCheckBox( this );
    mHorizonsCount->setChecked( true );
    mHorizonsCountBefore = new QCheckBox( this );
    mHorizonsCountBefore->setChecked( true );
    mHorizonsCountAfter = new QCheckBox( this );
    mHorizonsCountAfter->setChecked( true );

    layout->addRow( "Calculate:", mHorizonsCount );
    layout->addRow( "Horizons before target:", mHorizonsCountBefore );
    layout->addRow( "Horizons after target:", mHorizonsCountAfter );

    mTabWidget->addTab( mPage3, "Horizons Count" );
}

void AlgorithmsDialog::setUpPage4()
{
    mPage4 = new QWidget( this );
    QFormLayout *layout = new QFormLayout();
    mPage4->setLayout( layout );
    mViewAngle = new QCheckBox( this );
    mViewAngle->setChecked( true );

    layout->addRow( "Calculate:", mViewAngle );

    mTabWidget->addTab( mPage4, "View Angle" );
}

void AlgorithmsDialog::setUpPage5()
{
    mPage5 = new QWidget( this );
    QFormLayout *layout = new QFormLayout();
    mPage5->setLayout( layout );

    mFuzzy = new QCheckBox( this );
    mFuzzy->setChecked( true );

    mClearVisibility = new QgsDoubleSpinBox( this );
    mClearVisibility->setMinimum( 0 );
    mClearVisibility->setMaximum( MAX );
    mClearVisibility->setValue( 1000 );

    mHalfDropout = new QgsDoubleSpinBox( this );
    mHalfDropout->setMinimum( 0 );
    mHalfDropout->setMaximum( MAX );
    mHalfDropout->setValue( 1500 );

    mVerticalDistance = new QCheckBox( this );
    mVerticalDistance->setChecked( false );

    mFuzzyNotVisible = new IntegerSpinBox( true, -1, 0, this );

    layout->addRow( "Calculate:", mFuzzy );
    layout->addRow( "Clear visibility limit:", mClearVisibility );
    layout->addRow( "Half dropout limit:", mHalfDropout );
    layout->addRow( "Not visible value:", mFuzzyNotVisible );
    layout->addRow( "Include vertical distance:", mVerticalDistance );

    mTabWidget->addTab( mPage5, "Fuzzy visibility" );
}