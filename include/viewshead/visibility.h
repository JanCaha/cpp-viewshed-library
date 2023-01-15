#ifndef VIEWSHEDLIB_VISIBILITY_H
#define VIEWSHEDLIB_VISIBILITY_H

#include "enums.h"
#include "points.h"
#include "position.h"
#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{
    class Visibility
    {
      public:
        static Position calculateEventPosition( CellPosition eventType, int row, int col,
                                                std::shared_ptr<IPoint> point );
        static double calculateAngle( double row, double column, std::shared_ptr<IPoint> point );
        static double calculateAngle( Position *pos, std::shared_ptr<IPoint> point );
        static double calculateDistance( int &row, int &column, std::shared_ptr<IPoint> point, double &cellSize );
        static double calculateDistance( double &row, double &column, std::shared_ptr<IPoint> point, double &cellSize );
        static double calculateDistance( Position *pos, std::shared_ptr<IPoint> point, double &cellSize );
        static double calculateGradient( std::shared_ptr<IPoint> point, Position *pos, double elevation,
                                         double &distance );
        static double calculateGradient( std::shared_ptr<IPoint> point, double &row, double &column, double elevation,
                                         double &distance );
        static std::vector<std::pair<double, double>> distanceElevation( SharedStatusList los, LoSNode poi );
    };
} // namespace viewshed

#endif