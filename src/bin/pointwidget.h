#ifndef POINTWIDGET_H
#define POINTWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "qgsdoublevalidator.h"
#include "qgspoint.h"
#include "qgspointxy.h"

class PointWidget : public QWidget
{
    Q_OBJECT

  public:
    PointWidget( bool addCrsLabel = true, QWidget *parent = nullptr );

    bool isPointValid();
    QgsPoint point();
    QgsPointXY pointXY();
    void setXY( double x, double y );
    void setPoint( QgsPoint p );
    void setCrs( QString crs );

  signals:
    void pointChanged( QgsPoint point );
    void pointXYChanged( QgsPointXY point );

  private:
    void updatePoint();

    QLineEdit *mPointX = nullptr;
    QLineEdit *mPointY = nullptr;
    QLabel *mCrsLabel = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QgsPoint mPoint;
    bool mPointValid = false;
    bool mAddCrsLabel = true;
};

#endif