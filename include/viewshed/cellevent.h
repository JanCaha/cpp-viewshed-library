#ifndef VIEWSHEDLIB_EVENT_H
#define VIEWSHEDLIB_EVENT_H

#include "defaultdatatypes.h"
#include "enums.h"
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
        CellEventPositionType mEventType;
        CELL_EVENT_DATA_TYPE mDist2point;
        CELL_EVENT_DATA_TYPE mAngle;
        CELL_EVENT_DATA_TYPE mElevation[3];
        bool mBehindTargetForInverseLoS = false;

        CellEvent();

        CellEvent( CellEventPositionType eventType, int row, int col, double dist, double angle, double elevation[3] );

        bool operator<( const CellEvent other ) const;
        bool operator==( const CellEvent other ) const;
    };
} // namespace viewshed

#endif