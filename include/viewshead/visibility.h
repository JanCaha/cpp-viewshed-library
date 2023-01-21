#ifndef VIEWSHEDLIB_VISIBILITY_H
#define VIEWSHEDLIB_VISIBILITY_H

#include "celleventposition.h"
#include "enums.h"
#include "points.h"

namespace viewshed
{
    class Visibility
    {
      public:
        static CellEventPosition calculateEventPosition( CellEventPositionType eventType, int row, int col,
                                                         std::shared_ptr<Point> point );
        static double calculateAngle( double row, double column, std::shared_ptr<Point> point );
        static double calculateAngle( CellEventPosition *pos, std::shared_ptr<Point> point );
        static double calculateAngle( RasterPosition *pos, std::shared_ptr<Point> point );
        static double calculateDistance( const double &x1, const double &y1, const double &x2, const double &y2,
                                         double &cellSize );
        static double calculateDistance( int &row, int &column, std::shared_ptr<Point> point, double &cellSize );
        static double calculateDistance( std::shared_ptr<Point> point1, std::shared_ptr<Point> point2,
                                         double &cellSize );
        static double calculateDistance( double &row, double &column, std::shared_ptr<Point> point, double &cellSize );
        static double calculateDistance( CellEventPosition *pos, std::shared_ptr<Point> point, double &cellSize );
        static double calculateGradient( std::shared_ptr<Point> point, double elevation, double &distance );
    };
} // namespace viewshed

#endif