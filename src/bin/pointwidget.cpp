#include "QHBoxLayout"
#include "QVariant"

#include "pointwidget.h"

PointWidget::PointWidget( bool addCrsLabel, QWidget *parent ) : QWidget( parent ), mAddCrsLabel( addCrsLabel )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );

    mDoubleValidator = new QgsDoubleValidator( this );

    mPointX = new QLineEdit( this );
    mPointX->setValidator( mDoubleValidator );
    mPointY = new QLineEdit( this );
    mPointY->setValidator( mDoubleValidator );

    connect( mPointY, &QLineEdit::textChanged, this, &PointWidget::updatePoint );
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

    if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
    {
        mPoint.setX( QgsDoubleValidator::toDouble( text ) );
    }
    else
        return;

    text = mPointY->text();

    if ( mDoubleValidator->validate( text ) == QgsDoubleValidator::Acceptable )
    {
        mPoint.setY( QgsDoubleValidator::toDouble( text ) );
    }
    else
        return;

    mPointValid = true;

    emit pointChanged( point() );
    emit pointXYChanged( pointXY() );
}

bool PointWidget::isPointValid() { return mPointValid; }

QgsPoint PointWidget::point() { return mPoint; };

QgsPointXY PointWidget::pointXY() { return QgsPointXY( mPoint.x(), mPoint.y() ); };

void PointWidget::setXY( double x, double y )
{
    mPointX->setText( QVariant( x ).toString() );
    mPointY->setText( QVariant( y ).toString() );
    mPointValid = true;
}

void PointWidget::setPoint( QgsPoint p )
{
    setXY( p.x(), p.y() );
    mPointValid = true;
}

void PointWidget::setCrs( QString crs ) { mCrsLabel->setText( QString( "[%1]" ).arg( crs ) ); }