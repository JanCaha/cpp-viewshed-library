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

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget( mTabWidget );
    mainLayout->addWidget( mButtonBox );
    setLayout( mainLayout );
}

void AlgorithmsDialog::setUpPage1()
{
    mPage1 = new QWidget( this );
    QFormLayout *layout = new QFormLayout( this );
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
    QFormLayout *layout = new QFormLayout( this );
    mPage2->setLayout( layout );
    mHorizons = new QCheckBox( this );
    mHorizons->setChecked( true );
    mLocalHorizon = new IntegerSpinBox( 1, false );

    mGlobalHorizon = new IntegerSpinBox( 2, false );

    mNotHorizon = new IntegerSpinBox( 0, false );
    layout->addRow( "Calculate: ", mHorizons );
    layout->addRow( "Local Horizon: ", mLocalHorizon );
    layout->addRow( "Global Horizon: ", mGlobalHorizon );
    layout->addRow( "Not Horizon: ", mNotHorizon );

    mTabWidget->addTab( mPage2, "Horizons" );
}

void AlgorithmsDialog::setUpPage3()
{
    mPage3 = new QWidget( this );
    QFormLayout *layout = new QFormLayout( this );
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
    QFormLayout *layout = new QFormLayout( this );
    mPage3->setLayout( layout );
    mViewAngle = new QCheckBox( this );
    mViewAngle->setChecked( true );

    layout->addRow( "Calculate:", mViewAngle );

    mTabWidget->addTab( mPage3, "View Angle" );
}