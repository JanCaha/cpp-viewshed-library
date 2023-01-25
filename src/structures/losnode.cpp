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

LoSNode::LoSNode( std::shared_ptr<Point> point, CellEvent *e, double &cellSize )
{
    row = e->row;
    col = e->col;

    elevs[CellEventPositionType::CENTER] = e->elevation[CellEventPositionType::CENTER];
    elevs[CellEventPositionType::ENTER] = e->elevation[CellEventPositionType::ENTER];
    elevs[CellEventPositionType::EXIT] = e->elevation[CellEventPositionType::EXIT];

    angle[CellEventPositionType::CENTER] = Visibility::calculateAngle( row, col, point );

    CellEventPosition posEnter =
        Visibility::calculateEventPosition( CellEventPositionType::ENTER, e->row, e->col, point );
    double angleEnter = Visibility::calculateAngle( &posEnter, point );

    if ( angleEnter > angle[CellEventPositionType::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    angle[CellEventPositionType::ENTER] = angleEnter;

    CellEventPosition posExit =
        Visibility::calculateEventPosition( CellEventPositionType::EXIT, e->row, e->col, point );
    angle[CellEventPositionType::EXIT] = Visibility::calculateAngle( &posExit, point );

    if ( 0 > e->dist2vp )
    {
        inverseLoSBehindTarget = true;
    }

    distances[CellEventPositionType::CENTER] = Visibility::calculateDistance( row, col, point, cellSize );
    distances[CellEventPositionType::ENTER] = Visibility::calculateDistance( &posEnter, point, cellSize );
    distances[CellEventPositionType::EXIT] = Visibility::calculateDistance( &posExit, point, cellSize );

    gradient[CellEventPositionType::CENTER] = Visibility::calculateGradient(
        point, elevs[CellEventPositionType::CENTER], distances[CellEventPositionType::CENTER] );
    gradient[CellEventPositionType::ENTER] = Visibility::calculateGradient( point, elevs[CellEventPositionType::ENTER],
                                                                            distances[CellEventPositionType::ENTER] );
    gradient[CellEventPositionType::EXIT] = Visibility::calculateGradient( point, elevs[CellEventPositionType::EXIT],
                                                                           distances[CellEventPositionType::EXIT] );
}

void LoSNode::setInverse( std::shared_ptr<Point> vp, double &cellSize )
{
    double temp = elevs[CellEventPositionType::ENTER];
    elevs[CellEventPositionType::ENTER] = elevs[CellEventPositionType::EXIT];
    elevs[CellEventPositionType::EXIT] = temp;

    angle[CellEventPositionType::CENTER] = Visibility::calculateAngle( row, col, vp );

    CellEventPosition posEnter = Visibility::calculateEventPosition( CellEventPositionType::ENTER, row, col, vp );
    double angleEnter = Visibility::calculateAngle( &posEnter, vp );

    if ( angleEnter > angle[CellEventPositionType::CENTER] )
    {
        angleEnter = angleEnter - ( 2 * M_PI );
    }

    angle[CellEventPositionType::ENTER] = angleEnter;

    CellEventPosition posExit = Visibility::calculateEventPosition( CellEventPositionType::EXIT, row, col, vp );
    angle[CellEventPositionType::EXIT] = Visibility::calculateAngle( &posExit, vp );

    distances[CellEventPositionType::CENTER] = Visibility::calculateDistance( row, col, vp, cellSize );
    distances[CellEventPositionType::ENTER] = Visibility::calculateDistance( &posEnter, vp, cellSize );
    distances[CellEventPositionType::EXIT] = Visibility::calculateDistance( &posExit, vp, cellSize );

    gradient[CellEventPositionType::CENTER] = Visibility::calculateGradient( vp, elevs[CellEventPositionType::CENTER],
                                                                             distances[CellEventPositionType::CENTER] );
    gradient[CellEventPositionType::ENTER] = Visibility::calculateGradient( vp, elevs[CellEventPositionType::ENTER],
                                                                            distances[CellEventPositionType::ENTER] );
    gradient[CellEventPositionType::EXIT] =
        Visibility::calculateGradient( vp, elevs[CellEventPositionType::EXIT], distances[CellEventPositionType::EXIT] );
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
    else
    {
        return std::numeric_limits<double>::max() * ( -1 );
    }
}

double LoSNode::centreAngle() { return angle[CellEventPositionType::CENTER]; }

double LoSNode::centreGradient() { return gradient[CellEventPositionType::CENTER]; }

double LoSNode::centreElevation() { return elevs[CellEventPositionType::CENTER]; }

double LoSNode::centreDistance() { return distances[CellEventPositionType::CENTER]; }

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
    return distances[CellEventPositionType::CENTER] < other.distances[CellEventPositionType::CENTER];
}