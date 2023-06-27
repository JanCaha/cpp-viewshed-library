#include <cmath>
#include <iterator>
#include <limits>

#include "cellevent.h"
#include "celleventposition.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::CellEventPosition;
using viewshed::LoSNode;

LoSNode::LoSNode()
{
    mRow = 0;
    mCol = 0;
}

LoSNode::LoSNode( int row, int col )
{
    mRow = row;
    mCol = col;
}

LoSNode::LoSNode( std::shared_ptr<Point> point, CellEvent *e, double &cellSize )
{
    mRow = e->mRow;
    mCol = e->mCol;

    mElevs[CellEventPositionType::CENTER] = e->mElevation[CellEventPositionType::CENTER];
    mElevs[CellEventPositionType::ENTER] = e->mElevation[CellEventPositionType::ENTER];
    mElevs[CellEventPositionType::EXIT] = e->mElevation[CellEventPositionType::EXIT];

    mAngle[CellEventPositionType::CENTER] = Visibility::angle( mRow, mCol, point );

    CellEventPosition posEnter = Visibility::eventPosition( CellEventPositionType::ENTER, e->mRow, e->mCol, point );
    double angleEnter = Visibility::angle( &posEnter, point );

    if ( angleEnter > mAngle[CellEventPositionType::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    mAngle[CellEventPositionType::ENTER] = angleEnter;

    CellEventPosition posExit = Visibility::eventPosition( CellEventPositionType::EXIT, e->mRow, e->mCol, point );
    mAngle[CellEventPositionType::EXIT] = Visibility::angle( &posExit, point );

    if ( e->mBehindTargetForInverseLoS )
    {
        mInverseLoSBehindTarget = true;
    }

    mDistances[CellEventPositionType::CENTER] = Visibility::distance( mRow, mCol, point, cellSize );
    mDistances[CellEventPositionType::ENTER] = Visibility::distance( &posEnter, point, cellSize );
    mDistances[CellEventPositionType::EXIT] = Visibility::distance( &posExit, point, cellSize );
}

double LoSNode::value( CellEventPositionType position, ValueType valueType )
{
    switch ( valueType )
    {
        case ValueType::Angle:
        {
            return mAngle[position];
        }
        case ValueType::Distance:
        {
            return mDistances[position];
        }
        case ValueType::Elevation:
        {
            return mElevs[position];
        }
        default:
        {
            return 0;
        }
    }
}

double LoSNode::valueAtAngle( const double &specificAngle, ValueType valueType )
{
    if ( specificAngle == mAngle[CellEventPositionType::CENTER] )
        return value( CellEventPositionType::CENTER, valueType );
    else if ( specificAngle == mAngle[CellEventPositionType::ENTER] )
        return value( CellEventPositionType::ENTER, valueType );
    else if ( specificAngle == mAngle[CellEventPositionType::EXIT] )
        return value( CellEventPositionType::EXIT, valueType );
    else if ( mAngle[CellEventPositionType::ENTER] < specificAngle &&
              specificAngle < mAngle[CellEventPositionType::CENTER] )
    {
        double difference = mAngle[CellEventPositionType::CENTER] - mAngle[CellEventPositionType::ENTER];
        double ratio = ( specificAngle - mAngle[CellEventPositionType::ENTER] ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::ENTER, valueType ) );
    }
    else if ( mAngle[CellEventPositionType::CENTER] < specificAngle &&
              specificAngle < mAngle[CellEventPositionType::EXIT] )
    {
        double difference = mAngle[CellEventPositionType::EXIT] - mAngle[CellEventPositionType::CENTER];
        double ratio = ( mAngle[CellEventPositionType::EXIT] - specificAngle ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::EXIT, valueType ) );
    }
    // special case for rotation ending LoS
    else if ( specificAngle > ( M_PI * 1.5 ) && mAngle[CellEventPositionType::ENTER] < specificAngle - ( 2 * M_PI ) )
    {
        double difference = mAngle[CellEventPositionType::CENTER] - mAngle[CellEventPositionType::ENTER];
        double ratio = ( ( specificAngle - ( 2 * M_PI ) ) - mAngle[CellEventPositionType::ENTER] ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::ENTER, valueType ) );
    }
    else
    {
        return std::numeric_limits<double>::max() * ( -1 );
    }
}

double LoSNode::centreAngle() { return mAngle[CellEventPositionType::CENTER]; }

double LoSNode::centreElevation() { return mElevs[CellEventPositionType::CENTER]; }

double LoSNode::centreDistance() { return mDistances[CellEventPositionType::CENTER]; }

bool LoSNode::operator==( const LoSNode &other )
{
    if ( mRow == other.mRow && mCol == other.mCol )
        return true;
    else
        return false;
}

bool LoSNode::operator!=( const LoSNode &other )
{
    if ( mRow == other.mRow && mCol == other.mCol )
        return false;
    else
        return true;
}

bool LoSNode::operator<( const LoSNode other )
{
    return mDistances[CellEventPositionType::CENTER] < other.mDistances[CellEventPositionType::CENTER];
}