#include <cmath>
#include <iterator>
#include <limits>

#include "losnode.h"
#include "visibility.h"

using viewshed::LoSNode;

LoSNode::LoSNode()
{
    mRow = 0;
    mCol = 0;
}

LoSNode::LoSNode( int row_, int col_ )
{
    mRow = row_;
    mCol = col_;
}

LoSNode::LoSNode( std::shared_ptr<Point> point, CellEvent *e, double &cellSize )
{
    mRow = e->mRow;
    mCol = e->mCol;

    elevs[CellEventPositionType::CENTER] = e->elevation[CellEventPositionType::CENTER];
    elevs[CellEventPositionType::ENTER] = e->elevation[CellEventPositionType::ENTER];
    elevs[CellEventPositionType::EXIT] = e->elevation[CellEventPositionType::EXIT];

    angle[CellEventPositionType::CENTER] = Visibility::angle( mRow, mCol, point );

    CellEventPosition posEnter = Visibility::eventPosition( CellEventPositionType::ENTER, e->mRow, e->mCol, point );
    double angleEnter = Visibility::angle( &posEnter, point );

    if ( angleEnter > angle[CellEventPositionType::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    angle[CellEventPositionType::ENTER] = angleEnter;

    CellEventPosition posExit = Visibility::eventPosition( CellEventPositionType::EXIT, e->mRow, e->mCol, point );
    angle[CellEventPositionType::EXIT] = Visibility::angle( &posExit, point );

    if ( e->behindTargetForInverseLoS )
    {
        inverseLoSBehindTarget = true;
    }

    distances[CellEventPositionType::CENTER] = Visibility::distance( mRow, mCol, point, cellSize );
    distances[CellEventPositionType::ENTER] = Visibility::distance( &posEnter, point, cellSize );
    distances[CellEventPositionType::EXIT] = Visibility::distance( &posExit, point, cellSize );
}

double LoSNode::value( CellEventPositionType position, ValueType valueType )
{
    switch ( valueType )
    {
        case ValueType::Angle:
        {
            return angle[position];
        }
        case ValueType::Distance:
        {
            return distances[position];
        }
        case ValueType::Elevation:
        {
            return elevs[position];
        }
        default:
        {
            return 0;
        }
    }
}

double LoSNode::valueAtAngle( const double &specificAngle, ValueType valueType )
{
    if ( specificAngle == angle[CellEventPositionType::CENTER] )
        return value( CellEventPositionType::CENTER, valueType );
    else if ( specificAngle == angle[CellEventPositionType::ENTER] )
        return value( CellEventPositionType::ENTER, valueType );
    else if ( specificAngle == angle[CellEventPositionType::EXIT] )
        return value( CellEventPositionType::EXIT, valueType );
    else if ( angle[CellEventPositionType::ENTER] < specificAngle &&
              specificAngle < angle[CellEventPositionType::CENTER] )
    {
        double difference = angle[CellEventPositionType::CENTER] - angle[CellEventPositionType::ENTER];
        double ratio = ( specificAngle - angle[CellEventPositionType::ENTER] ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::ENTER, valueType ) );
    }
    else if ( angle[CellEventPositionType::CENTER] < specificAngle &&
              specificAngle < angle[CellEventPositionType::EXIT] )
    {
        double difference = angle[CellEventPositionType::EXIT] - angle[CellEventPositionType::CENTER];
        double ratio = ( angle[CellEventPositionType::EXIT] - specificAngle ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::EXIT, valueType ) );
    }
    // special case for rotation ending LoS
    else if ( specificAngle > ( M_PI * 1.5 ) && angle[CellEventPositionType::ENTER] < specificAngle - ( 2 * M_PI ) )
    {
        double difference = angle[CellEventPositionType::CENTER] - angle[CellEventPositionType::ENTER];
        double ratio = ( ( specificAngle - ( 2 * M_PI ) ) - angle[CellEventPositionType::ENTER] ) / difference;
        return ( ratio * value( CellEventPositionType::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellEventPositionType::ENTER, valueType ) );
    }
    else
    {
        return std::numeric_limits<double>::max() * ( -1 );
    }
}

double LoSNode::centreAngle() { return angle[CellEventPositionType::CENTER]; }

double LoSNode::centreElevation() { return elevs[CellEventPositionType::CENTER]; }

double LoSNode::centreDistance() { return distances[CellEventPositionType::CENTER]; }

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
    return distances[CellEventPositionType::CENTER] < other.distances[CellEventPositionType::CENTER];
}