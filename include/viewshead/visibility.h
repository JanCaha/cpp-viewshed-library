#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "enums.h"
#include "points.h"
#include "position.h"

class Visibility
{
  public:
    static Position calculateEventPosition( CellPosition eventType, int row, int col, ViewPoint point );
    static double calculateAngle( double row, double column, std::shared_ptr<ViewPoint> vp );
    static double calculateAngle( Position *pos, std::shared_ptr<ViewPoint> vp );
    static double calculateDistance( int &row, int &column, std::shared_ptr<ViewPoint> vp, double &cellSize );
    static double calculateDistance( double &row, double &column, std::shared_ptr<ViewPoint> vp, double &cellSize );
    static double calculateDistance( Position *pos, std::shared_ptr<ViewPoint> vp, double &cellSize );
    static double calculateGradient( std::shared_ptr<ViewPoint> vp, Position *pos, double elevation, double &distance );
    static double calculateGradient( std::shared_ptr<ViewPoint> vp, double &row, double &column, double elevation,
                                     double &distance );
};

#endif