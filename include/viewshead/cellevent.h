#ifndef VIEWSHEDLIB_EVENT_H
#define VIEWSHEDLIB_EVENT_H

#include "enums.h"
#include "point.h"
#include "rasterposition.h"

namespace viewshed
{
    class CellEvent : public RasterPosition
    {
      public:
        CellEventPositionType eventType;
        double dist2vp;
        double angle;
        double elevation[3];
        bool behindTargetForInverseLoS = false;

        CellEvent( CellEventPositionType eventType_, int row_, int col_, double dist_, double angle_,
                   double elevation_[3] );

        bool operator<( const CellEvent other ) const;
    };
} // namespace viewshed

#endif