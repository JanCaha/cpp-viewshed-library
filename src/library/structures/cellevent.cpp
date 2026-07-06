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

bool CellEvent::operator==( const CellEvent &other ) const
{
    return mRow == other.mRow && mCol == other.mCol && mEventType == other.mEventType &&
           mBehindTargetForInverseLoS == other.mBehindTargetForInverseLoS;
}

// order of events at equal angle: EXIT first, then CENTER, then ENTER
static int eventTypeSortRank( CellEventPositionType eventType )
{
    switch ( eventType )
    {
        case CellEventPositionType::EXIT:
            return 0;
        case CellEventPositionType::CENTER:
            return 1;
        default:
            return 2;
    }
}

// must be a strict weak ordering, otherwise std::sort is undefined behaviour
// (the previous version returned true for `a<a` on EXIT events with equal angles,
// which made std::sort in libc++ on macOS read out of bounds and segfault)
bool CellEvent::operator<( const CellEvent &other ) const
{
    if ( mAngle < other.mAngle )
    {
        return true;
    }

    if ( other.mAngle < mAngle )
    {
        return false;
    }

    /*a.angle == b.angle */
    return eventTypeSortRank( mEventType ) < eventTypeSortRank( other.mEventType );
}