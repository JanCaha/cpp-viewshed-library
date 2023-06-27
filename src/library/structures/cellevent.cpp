#include "cellevent.h"

using viewshed::CellEvent;
using viewshed::CellEventPositionType;

CellEvent::CellEvent()
{
    mEventType = CellEventPositionType::CENTER;
    mRow = -1;
    mCol = -1;
    mDist2point = -1;
    mAngle = -1;
    mElevation[CellEventPositionType::ENTER] = -1;
    mElevation[CellEventPositionType::CENTER] = -1;
    mElevation[CellEventPositionType::EXIT] = -1;
}

CellEvent::CellEvent( CellEventPositionType eventType, int row, int col, double dist, double angle,
                      double elevation[3] )
{
    mEventType = eventType;
    mRow = row;
    mCol = col;
    mAngle = angle;
    mDist2point = dist;
    mElevation[CellEventPositionType::ENTER] = elevation[CellEventPositionType::ENTER];
    mElevation[CellEventPositionType::CENTER] = elevation[CellEventPositionType::CENTER];
    mElevation[CellEventPositionType::EXIT] = elevation[CellEventPositionType::EXIT];
}

bool CellEvent::operator==( const CellEvent other ) const
{
    return mRow == other.mRow && mCol == other.mCol && mEventType == other.mEventType &&
           mBehindTargetForInverseLoS == other.mBehindTargetForInverseLoS;
}

bool CellEvent::operator<( const CellEvent other ) const
{

    if ( mRow == other.mRow && mCol == other.mCol && mEventType == other.mEventType && mBehindTargetForInverseLoS &&
         other.mBehindTargetForInverseLoS )
        return false;

    if ( mAngle > other.mAngle )
    {
        return false;
    }
    else if ( mAngle < other.mAngle )
    {
        return true;
    }
    else
    {
        /*a.angle == b.angle */
        if ( mEventType == CellEventPositionType::EXIT )
            return true;
        if ( other.mEventType == CellEventPositionType::EXIT )
            return false;
        if ( mEventType == CellEventPositionType::ENTER )
            return false;
        if ( other.mEventType == CellEventPositionType::ENTER )
            return true;
        return false;
    }
}