#include "QHBoxLayout"
#include "QVariant"

#include "pointwidget.h"

using ViewshedBinaries::PointWidget;

PointWidget::PointWidget( bool addCrsLabel, QWidget *parent ) : QWidget( parent ), mAddCrsLabel( addCrsLabel )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );

    mDoubleValidator = new DoubleValidator( this );

    mPointX = new QLineEdit( this );
    mPointX->setValidator( mDoubleValidator );
    mPointY = new QLineEdit( this );
    mPointY->setValidator( mDoubleValidator );

    connect( mPointX, &QLineEdit::textChanged, this, &PointWidget::updatePoint );
    connect( mPointY, &QLineEdit::textChanged, this, &PointWidget::updatePoint );

    layout->addWidget( mPointX );
    layout->addWidget( mPointY );

    mCrsLabel = new QLabel( this );

    if ( mAddCrsLabel )
    {
        mCrsLabel->setText( "" );
        mCrsLabel->setMinimumWidth( 20 );
        layout->addWidget( mCrsLabel );
    }
    else
    {
        mCrsLabel->setHidden( true );
    }
}

void PointWidget::updatePoint()
{
    mPointValid = false;

    QString text = mPointX->text();

    if ( mDoubleValidator->validate( text ) == DoubleValidator::Acceptable )
    {
        mPoint.setX( DoubleValidator::toDouble( text ) );
    }
    else
        return;

    text = mPointY->text();

    if ( mDoubleValidator->validate( text ) == DoubleValidator::Acceptable )
    {
        mPoint.setY( DoubleValidator::toDouble( text ) );
    }
    else
        return;

    mPointValid = true;

    emit pointChanged( point() );
}

bool PointWidget::isPointValid() { return mPointValid; }

OGRPoint PointWidget::point() { return mPoint; };

void PointWidget::setXY( double x, double y )
{
    mPointX->setText( QVariant( x ).toString() );
    mPointY->setText( QVariant( y ).toString() );
    mPointValid = true;
}

void PointWidget::setPoint( OGRPoint p )
{
    setXY( p.getX(), p.getY() );
    mPointValid = true;
}

void PointWidget::setCrs( OGRSpatialReference crs )
{
    mCrs = crs;

    QString code = QString::fromStdString( crs.GetAuthorityCode( nullptr ) );
    QString name = QString::fromStdString( crs.GetAuthorityName( nullptr ) );

    mCrsLabel->setText( QString( "[%1:%2]" ).arg( name ).arg( code ) );
}

OGRSpatialReference PointWidget::crs() { return mCrs; }