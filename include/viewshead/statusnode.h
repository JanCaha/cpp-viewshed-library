#ifndef VIEWSHEDLIB_STATUSNODE_H
#define VIEWSHEDLIB_STATUSNODE_H

#include "enums.h"
#include "event.h"
#include "points.h"

struct StatusNode
{
    int row, col;
    double gradient[3];
    double angle[3];
    double elevs[3];
    double distances[3];

    StatusNode();
    StatusNode( int row_, int col_ );
    StatusNode( std::shared_ptr<ViewPoint> vp, Event *e, double &cellSize );

    bool operator==( const StatusNode &other );
    bool operator!=( const StatusNode &other );
    bool operator<( const StatusNode other );

    double valueAtAngle( const double &angle, ValueType valueType = ValueType::Gradient );
    double value( int position, ValueType valueType = ValueType::Gradient );
    double centreAngle();
    double centreGradient();
    double centreElevation();
    double centreDistance();
};

#endif