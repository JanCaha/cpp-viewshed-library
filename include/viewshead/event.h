#ifndef VIEWSHEDLIB_EVENT_H
#define VIEWSHEDLIB_EVENT_H

#include "enums.h"

// using viewshed::CellPosition;

namespace viewshed
{
    struct Event
    {
        CellPosition eventType;
        int row, col;
        double dist2vp;
        double angle;
        double elevation[3];

        Event( CellPosition eventType_, int row_, int col_, double dist_, double angle_, double elevation_[3] );

        bool operator<( const Event other ) const;
    };
} // namespace viewshed

#endif