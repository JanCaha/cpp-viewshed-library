#ifndef VIEWSHEDLIB_VISIBILITY_H
#define VIEWSHEDLIB_VISIBILITY_H

#include "celleventposition.h"
#include "enums.h"
#include "point.h"
#include "rasterposition.h"

namespace viewshed
{
    class Visibility
    {
      public:
        static CellEventPosition eventPosition( CellEventPositionType eventType, int row, int col,
                                                std::shared_ptr<Point> point );

        static double angle( double row, double column, std::shared_ptr<Point> point );

        static double angle( CellEventPosition *pos, std::shared_ptr<Point> point );

        static double angle( RasterPosition *pos, std::shared_ptr<Point> point );

        static double distance( const double &x1, const double &y1, const double &x2, const double &y2,
                                double &cellSize );

        static double distance( int &row, int &column, std::shared_ptr<Point> point, double &cellSize );

        static double distance( std::shared_ptr<Point> point1, std::shared_ptr<Point> point2, double &cellSize );

        static double distance( double &row, double &column, std::shared_ptr<Point> point, double &cellSize );

        static double distance( CellEventPosition *pos, std::shared_ptr<Point> point, double &cellSize );

        static double gradient( std::shared_ptr<Point> point, double elevation, double &distance );
    };
} // namespace viewshed

#endif