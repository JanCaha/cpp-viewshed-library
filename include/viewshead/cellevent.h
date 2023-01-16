#ifndef VIEWSHEDLIB_EVENT_H
#define VIEWSHEDLIB_EVENT_H

#include "enums.h"

namespace viewshed
{
    struct CellEvent
    {
        CellPosition eventType;
        int row, col;
        double dist2vp;
        double angle;
        double elevation[3];

        CellEvent( CellPosition eventType_, int row_, int col_, double dist_, double angle_, double elevation_[3] );

        bool operator<( const CellEvent other ) const;
    };
} // namespace viewshed

#endif