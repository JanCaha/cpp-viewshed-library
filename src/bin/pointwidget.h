#ifndef POINTWIDGET_H
#define POINTWIDGET_H

#include <QLineEdit>
#include <QWidget>

#include "qgsdoublevalidator.h"
#include "qgspoint.h"
#include "qgspointxy.h"

class PointWidget : public QWidget
{
    Q_OBJECT

  public:
    PointWidget( QWidget *parent = nullptr );

    bool isPointValid();
    QgsPoint point();
    QgsPointXY pointXY();
    void setXY( double x, double y );
    void setPoint( QgsPoint p );

  signals:
    void pointChanged( QgsPoint point );
    void pointXYChanged( QgsPointXY point );

  private:
    void updatePoint();

    QLineEdit *mPointX = nullptr;
    QLineEdit *mPointY = nullptr;
    QgsDoubleValidator *mDoubleValidator = nullptr;
    QgsPoint mPoint;
    bool mPointValid = false;
};

#endif