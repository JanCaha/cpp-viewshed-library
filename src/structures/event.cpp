#include "event.h"

Event::Event( CellPosition eventType_, int row_, int col_, double dist_, double angle_, double elevation_[3] )
{
    eventType = eventType_;
    row = row_;
    col = col_;
    angle = angle_;
    dist2vp = dist_;
    elevation[CellPosition::ENTER] = elevation_[CellPosition::ENTER];
    elevation[CellPosition::CENTER] = elevation_[CellPosition::CENTER];
    elevation[CellPosition::EXIT] = elevation_[CellPosition::EXIT];
}

bool Event::operator<( const Event other ) const
{

    if ( row == other.row && col == other.col && eventType == other.eventType )
        return false;

    if ( angle > other.angle )
    {
        return false;
    }
    else if ( angle < other.angle )
    {
        return true;
    }
    else
    {
        /*a.angle == b.angle */
        if ( eventType == CellPosition::EXIT )
            return true;
        if ( other.eventType == CellPosition::EXIT )
            return false;
        if ( eventType == CellPosition::ENTER )
            return false;
        if ( other.eventType == CellPosition::ENTER )
            return true;
        return false;
    }
}