#ifndef VIEWSHEDLIB_LOSNODE_H
#define VIEWSHEDLIB_LOSNODE_H

#include "cellevent.h"
#include "enums.h"
#include "points.h"

namespace viewshed
{
    struct LoSNode
    {
        int row, col;
        double gradient[3];
        double angle[3];
        double elevs[3];
        double distances[3];

        LoSNode();
        LoSNode( int row_, int col_ );
        LoSNode( std::shared_ptr<IPoint> point, CellEvent *e, double &cellSize );

        bool operator==( const LoSNode &other );
        bool operator!=( const LoSNode &other );
        bool operator<( const LoSNode other );

        double valueAtAngle( const double &angle, ValueType valueType = ValueType::Gradient );
        double value( int position, ValueType valueType = ValueType::Gradient );
        double centreAngle();
        double centreGradient();
        double centreElevation();
        double centreDistance();
    };
} // namespace viewshed

#endif