#ifndef POINTWIDGET_H
#define POINTWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "ogr_geometry.h"
#include "ogr_spatialref.h"

#include "doublevalidator.h"

class PointWidget : public QWidget
{
    Q_OBJECT

  public:
    PointWidget( bool addCrsLabel = true, QWidget *parent = nullptr );

    bool isPointValid();
    OGRPoint point();
    void setXY( double x, double y );
    void setPoint( OGRPoint p );
    void setCrs( OGRSpatialReference crs );

  signals:
    void pointChanged( OGRPoint point );

  private:
    void updatePoint();

    QLineEdit *mPointX = nullptr;
    QLineEdit *mPointY = nullptr;
    QLabel *mCrsLabel = nullptr;
    DoubleValidator *mDoubleValidator = nullptr;
    OGRPoint mPoint;
    bool mPointValid = false;
    bool mAddCrsLabel = true;
};

#endif