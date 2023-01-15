#include <iterator>
#include <limits>

#include "statusnode.h"
#include "visibility.h"

using viewshed::StatusNode;

StatusNode::StatusNode()
{
    row = 0;
    col = 0;
}

StatusNode::StatusNode( int row_, int col_ )
{
    row = row_;
    col = col_;
}

StatusNode::StatusNode( std::shared_ptr<ViewPoint> vp, Event *e, double &cellSize )
{
    row = e->row;
    col = e->col;
    elevs[CellPosition::CENTER] = e->elevation[CellPosition::CENTER];
    elevs[CellPosition::ENTER] = e->elevation[CellPosition::ENTER];
    elevs[CellPosition::EXIT] = e->elevation[CellPosition::EXIT];

    angle[CellPosition::CENTER] = Visibility::calculateAngle( row, col, vp );

    Position posEnter = Visibility::calculateEventPosition( CellPosition::ENTER, e->row, e->col, *vp );
    double angleEnter = Visibility::calculateAngle( &posEnter, vp );

    if ( angleEnter > angle[CellPosition::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    angle[CellPosition::ENTER] = angleEnter;

    Position posExit = Visibility::calculateEventPosition( CellPosition::EXIT, e->row, e->col, *vp );
    angle[CellPosition::EXIT] = Visibility::calculateAngle( &posExit, vp );

    distances[CellPosition::CENTER] = Visibility::calculateDistance( row, col, vp, cellSize );
    distances[CellPosition::ENTER] = Visibility::calculateDistance( &posEnter, vp, cellSize );
    distances[CellPosition::EXIT] = Visibility::calculateDistance( &posExit, vp, cellSize );

    double dRow = (double)row;
    double dCol = (double)col;

    gradient[CellPosition::CENTER] =
        Visibility::calculateGradient( vp, dRow, dCol, elevs[CellPosition::CENTER], distances[CellPosition::CENTER] );
    gradient[CellPosition::ENTER] =
        Visibility::calculateGradient( vp, &posEnter, elevs[CellPosition::ENTER], distances[CellPosition::ENTER] );
    gradient[CellPosition::EXIT] =
        Visibility::calculateGradient( vp, &posExit, elevs[CellPosition::EXIT], distances[CellPosition::EXIT] );
}

double StatusNode::value( int position, ValueType valueType )
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
        case ValueType::Gradient:
        {
            return gradient[position];
        }
        default:
        {
            return 0;
        }
    }
}

double StatusNode::valueAtAngle( const double &specificAngle, ValueType valueType )
{
    if ( specificAngle == angle[CellPosition::CENTER] )
        return value( CellPosition::CENTER, valueType );
    else if ( specificAngle == angle[CellPosition::ENTER] )
        return value( CellPosition::ENTER, valueType );
    else if ( specificAngle == angle[CellPosition::EXIT] )
        return value( CellPosition::EXIT, valueType );
    else if ( angle[CellPosition::ENTER] < specificAngle && specificAngle < angle[CellPosition::CENTER] )
    {
        double difference = angle[CellPosition::CENTER] - angle[CellPosition::ENTER];
        double ratio = ( specificAngle - angle[CellPosition::ENTER] ) / difference;
        return ( ratio * value( CellPosition::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellPosition::ENTER, valueType ) );
    }
    else if ( angle[CellPosition::CENTER] < specificAngle && specificAngle < angle[CellPosition::EXIT] )
    {
        double difference = angle[CellPosition::EXIT] - angle[CellPosition::CENTER];
        double ratio = ( angle[CellPosition::EXIT] - specificAngle ) / difference;
        return ( ratio * value( CellPosition::CENTER, valueType ) +
                 ( 1 - ratio ) * value( CellPosition::EXIT, valueType ) );
    }
    else
    {
        return std::numeric_limits<double>::max() * ( -1 );
    }
}

double StatusNode::centreAngle() { return angle[CellPosition::CENTER]; }

double StatusNode::centreGradient() { return gradient[CellPosition::CENTER]; }

double StatusNode::centreElevation() { return elevs[CellPosition::CENTER]; }

double StatusNode::centreDistance() { return distances[CellPosition::CENTER]; }

bool StatusNode::operator==( const StatusNode &other )
{
    if ( row == other.row && col == other.col )
        return true;
    else
        return false;
}

bool StatusNode::operator!=( const StatusNode &other )
{
    if ( row == other.row && col == other.col )
        return false;
    else
        return true;
}

bool StatusNode::operator<( const StatusNode other )
{
    return distances[CellPosition::CENTER] < other.distances[CellPosition::CENTER];
}