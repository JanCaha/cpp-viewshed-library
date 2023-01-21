#ifndef VIEWSHEDLIB_LOSNODE_H
#define VIEWSHEDLIB_LOSNODE_H

#include "cellevent.h"
#include "enums.h"
#include "points.h"

namespace viewshed
{
    class LoSNode : public RasterPosition
    {
      public:
        double gradient[3];
        double angle[3];
        double elevs[3];
        double distances[3];

        LoSNode();
        LoSNode( int row_, int col_ );
        LoSNode( std::shared_ptr<Point> point, CellEvent *e, double &cellSize );

        bool operator==( const LoSNode &other );
        bool operator!=( const LoSNode &other );
        bool operator<( const LoSNode other );

        double valueAtAngle( const double &angle, ValueType valueType = ValueType::Gradient );
        double value( CellEventPositionType position, ValueType valueType = ValueType::Gradient );
        double centreAngle();
        double centreGradient();
        double centreElevation();
        double centreDistance();

        void setInverse( std::shared_ptr<Point> vp, double &cellSize );
    };
} // namespace viewshed

#endif