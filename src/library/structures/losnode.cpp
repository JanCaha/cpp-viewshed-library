#include <cmath>
#include <iterator>
#include <limits>
#include <numbers>

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

LoSNode::LoSNode( const int &pointRow, const int &pointCol, const CellEvent *e, const double &cellSize )
{
    mRow = e->mRow;
    mCol = e->mCol;

    mElevs[CellEventPositionType::CENTER] = e->mElevation[CellEventPositionType::CENTER];
    mElevs[CellEventPositionType::ENTER] = e->mElevation[CellEventPositionType::ENTER];
    mElevs[CellEventPositionType::EXIT] = e->mElevation[CellEventPositionType::EXIT];

    mAngle[CellEventPositionType::CENTER] = Visibility::angle( mRow, mCol, pointRow, pointCol );

    CellEventPosition posEnter =
        Visibility::eventPosition( CellEventPositionType::ENTER, e->mRow, e->mCol, pointRow, pointCol );
    double angleEnter = Visibility::angle( posEnter.mRow, posEnter.mCol, pointRow, pointCol );

    if ( angleEnter > mAngle[CellEventPositionType::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * std::numbers::pi );
    }

    mAngle[CellEventPositionType::ENTER] = angleEnter;

    CellEventPosition posExit =
        Visibility::eventPosition( CellEventPositionType::EXIT, e->mRow, e->mCol, pointRow, pointCol );
    mAngle[CellEventPositionType::EXIT] = Visibility::angle( posExit.mRow, posExit.mCol, pointRow, pointCol );

    if ( e->mBehindTargetForInverseLoS )
    {
        mInverseLoSBehindTarget = true;
    }

    mDistances[CellEventPositionType::CENTER] =
        Visibility::distance( static_cast<double>( mRow ), static_cast<double>( mCol ), static_cast<double>( pointRow ),
                              static_cast<double>( pointCol ), cellSize );

    mDistances[CellEventPositionType::ENTER] =
        Visibility::distance( static_cast<double>( posEnter.mRow ), static_cast<double>( posEnter.mCol ),
                              static_cast<double>( pointRow ), static_cast<double>( pointCol ), cellSize );

    mDistances[CellEventPositionType::EXIT] =
        Visibility::distance( static_cast<double>( posExit.mRow ), static_cast<double>( posExit.mCol ),
                              static_cast<double>( pointRow ), static_cast<double>( pointCol ), cellSize );
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
    else if ( specificAngle > ( std::numbers::pi * 1.5 ) &&
              mAngle[CellEventPositionType::ENTER] < specificAngle - ( 2 * std::numbers::pi ) )
    {
        double difference = mAngle[CellEventPositionType::CENTER] - mAngle[CellEventPositionType::ENTER];
        double ratio =
            ( ( specificAngle - ( 2 * std::numbers::pi ) ) - mAngle[CellEventPositionType::ENTER] ) / difference;
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

double LoSNode::elevationAtAngle( const double &angle ) { return valueAtAngle( angle, ValueType::Elevation ); }

double LoSNode::distanceAtAngle( const double &angle ) { return valueAtAngle( angle, ValueType::Distance ); }
