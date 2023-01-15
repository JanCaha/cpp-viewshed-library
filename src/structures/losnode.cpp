#include <iterator>
#include <limits>

#include "losnode.h"
#include "visibility.h"

using viewshed::LoSNode;

LoSNode::LoSNode()
{
    row = 0;
    col = 0;
}

LoSNode::LoSNode( int row_, int col_ )
{
    row = row_;
    col = col_;
}

LoSNode::LoSNode( std::shared_ptr<IPoint> point, CellEvent *e, double &cellSize )
{
    row = e->row;
    col = e->col;
    elevs[CellPosition::CENTER] = e->elevation[CellPosition::CENTER];
    elevs[CellPosition::ENTER] = e->elevation[CellPosition::ENTER];
    elevs[CellPosition::EXIT] = e->elevation[CellPosition::EXIT];

    angle[CellPosition::CENTER] = Visibility::calculateAngle( row, col, point );

    Position posEnter = Visibility::calculateEventPosition( CellPosition::ENTER, e->row, e->col, point );
    double angleEnter = Visibility::calculateAngle( &posEnter, point );

    if ( angleEnter > angle[CellPosition::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    angle[CellPosition::ENTER] = angleEnter;

    Position posExit = Visibility::calculateEventPosition( CellPosition::EXIT, e->row, e->col, point );
    angle[CellPosition::EXIT] = Visibility::calculateAngle( &posExit, point );

    distances[CellPosition::CENTER] = Visibility::calculateDistance( row, col, point, cellSize );
    distances[CellPosition::ENTER] = Visibility::calculateDistance( &posEnter, point, cellSize );
    distances[CellPosition::EXIT] = Visibility::calculateDistance( &posExit, point, cellSize );

    double dRow = (double)row;
    double dCol = (double)col;

    gradient[CellPosition::CENTER] = Visibility::calculateGradient( point, dRow, dCol, elevs[CellPosition::CENTER],
                                                                    distances[CellPosition::CENTER] );
    gradient[CellPosition::ENTER] =
        Visibility::calculateGradient( point, &posEnter, elevs[CellPosition::ENTER], distances[CellPosition::ENTER] );
    gradient[CellPosition::EXIT] =
        Visibility::calculateGradient( point, &posExit, elevs[CellPosition::EXIT], distances[CellPosition::EXIT] );
}

double LoSNode::value( int position, ValueType valueType )
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

double LoSNode::valueAtAngle( const double &specificAngle, ValueType valueType )
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

double LoSNode::centreAngle() { return angle[CellPosition::CENTER]; }

double LoSNode::centreGradient() { return gradient[CellPosition::CENTER]; }

double LoSNode::centreElevation() { return elevs[CellPosition::CENTER]; }

double LoSNode::centreDistance() { return distances[CellPosition::CENTER]; }

bool LoSNode::operator==( const LoSNode &other )
{
    if ( row == other.row && col == other.col )
        return true;
    else
        return false;
}

bool LoSNode::operator!=( const LoSNode &other )
{
    if ( row == other.row && col == other.col )
        return false;
    else
        return true;
}

bool LoSNode::operator<( const LoSNode other )
{
    return distances[CellPosition::CENTER] < other.distances[CellPosition::CENTER];
}