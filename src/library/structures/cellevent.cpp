#include "cellevent.h"

using viewshed::CellEvent;
using viewshed::CellEventPositionType;

CellEvent::CellEvent()
{
    eventType = CellEventPositionType::CENTER;
    mRow = -1;
    mCol = -1;
    dist2point = -1;
    angle = -1;
    elevation[CellEventPositionType::ENTER] = -1;
    elevation[CellEventPositionType::CENTER] = -1;
    elevation[CellEventPositionType::EXIT] = -1;
}

CellEvent::CellEvent( CellEventPositionType eventType_, int row_, int col_, double dist_, double angle_,
                      double elevation_[3] )
{
    eventType = eventType_;
    mRow = row_;
    mCol = col_;
    angle = angle_;
    dist2point = dist_;
    elevation[CellEventPositionType::ENTER] = elevation_[CellEventPositionType::ENTER];
    elevation[CellEventPositionType::CENTER] = elevation_[CellEventPositionType::CENTER];
    elevation[CellEventPositionType::EXIT] = elevation_[CellEventPositionType::EXIT];
}

bool CellEvent::operator==( const CellEvent other ) const
{
    return mRow == other.mRow && mCol == other.mCol && eventType == other.eventType &&
           behindTargetForInverseLoS == other.behindTargetForInverseLoS;
}

bool CellEvent::operator<( const CellEvent other ) const
{

    if ( mRow == other.mRow && mCol == other.mCol && eventType == other.eventType && behindTargetForInverseLoS &&
         other.behindTargetForInverseLoS )
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
        if ( eventType == CellEventPositionType::EXIT )
            return true;
        if ( other.eventType == CellEventPositionType::EXIT )
            return false;
        if ( eventType == CellEventPositionType::ENTER )
            return false;
        if ( other.eventType == CellEventPositionType::ENTER )
            return true;
        return false;
    }
}