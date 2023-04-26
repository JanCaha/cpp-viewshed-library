#ifndef VIEWSHEDLIB_EVENT_H
#define VIEWSHEDLIB_EVENT_H

#include "defaultdatatypes.h"
#include "enums.h"
#include "point.h"
#include "rasterposition.h"

namespace viewshed
{
    /**
     * @brief Class representing cell events for Van Kreveld's plane sweep algorithm. Stores cell position (row and
     * column), type of event (Enter, Center, Exit), elevation of all three points associated with given cell, distance
     * from important point (viewpoint or target point), horizontal angle from important point.
     *
     */
    class CellEvent : public RasterPosition
    {
      public:
        CellEventPositionType eventType;
        CELL_EVENT_DATA_TYPE dist2point;
        CELL_EVENT_DATA_TYPE angle;
        CELL_EVENT_DATA_TYPE elevation[3];
        bool behindTargetForInverseLoS = false;

        CellEvent();

        CellEvent( CellEventPositionType eventType_, int row_, int col_, double dist_, double angle_,
                   double elevation_[3] );

        bool operator<( const CellEvent other ) const;
        bool operator==( const CellEvent other ) const;
    };
} // namespace viewshed

#endif