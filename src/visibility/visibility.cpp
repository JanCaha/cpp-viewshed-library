#include "math.h"

#include "visibility.h"

using viewshed::Position;
using viewshed::Visibility;

double Visibility::calculateAngle( Position *pos, std::shared_ptr<IPoint> point )
{
    return calculateAngle( pos->row, pos->col, point );
}

double Visibility::calculateAngle( double row, double column, std::shared_ptr<IPoint> point )
{
    double angle = atan( fabs( row - point->row ) / fabs( column - point->col ) );

    if ( point->row == row && column > point->col )
    {
        return 0; /*between 1st and 4th quadrant */
    }
    else if ( column > point->col && row < point->row )
    {
        /*first quadrant */
        return angle;
    }
    else if ( point->col == column && point->row > row )
    {
        /*between 1st and 2nd quadrant */
        return M_PI / 2;
    }
    else if ( column < point->col && row < point->row )
    {
        /*second quadrant */
        return ( M_PI - angle );
    }
    else if ( point->row == row && column < point->col )
    {
        /*between 1st and 3rd quadrant */
        return M_PI;
    }
    else if ( row > point->row && column < point->col )
    {
        /*3rd quadrant */
        return ( M_PI + angle );
    }
    else if ( point->col == column && point->row < row )
    {
        /*between 3rd and 4th quadrant */
        return ( ( M_PI * 3.0 ) / 2.0 );
    }
    else if ( column > point->col && row > point->row )
    {
        /*4th quadrant */
        return ( M_PI * 2.0 - angle );
    }

    return 0;
}

double Visibility::calculateDistance( Position *pos, std::shared_ptr<IPoint> point, double &cellSize )
{
    return calculateDistance( pos->row, pos->col, point, cellSize );
}

double Visibility::calculateDistance( double &row, double &column, std::shared_ptr<IPoint> point, double &cellSize )
{
    return sqrt( pow( row - point->row, 2 ) + pow( column - point->col, 2 ) ) * cellSize;
}

double Visibility::calculateDistance( int &row, int &column, std::shared_ptr<IPoint> point, double &cellSize )
{
    double r = (double)row;
    double c = (double)column;
    return calculateDistance( r, c, point, cellSize );
}

double Visibility::calculateGradient( std::shared_ptr<IPoint> point, Position *pos, double elevation, double &distance )
{
    return calculateGradient( point, pos->row, pos->col, elevation, distance );
}

double Visibility::calculateGradient( std::shared_ptr<IPoint> point, double &row, double &column, double elevation,
                                      double &distance )
{
    double dx = point->col - column;
    double dy = point->row - row;

    double elevationDifference = elevation - point->totalElevation();

    if ( elevationDifference == 0 )
        return 0;

    return atan( elevationDifference / distance ) * ( 180 / M_PI );
}

Position Visibility::calculateEventPosition( CellPosition eventType, int row, int col, std::shared_ptr<IPoint> point )
{

    double rRow, rCol;

    if ( eventType == CellPosition::CENTER )
    {
        /*FOR CENTER_EVENTS */
        rRow = row;
        rCol = col;
    }

    if ( row < point->row && col < point->col )
    {
        /*first quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row - 0.5;
            rCol = col + 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row + 0.5;
            rCol = col - 0.5;
        }
    }
    else if ( col == point->col && row < point->row )
    {
        /*between the first and second quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row + 0.5;
            rCol = col + 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row + 0.5;
            rCol = col - 0.5;
        }
    }
    else if ( col > point->col && row < point->row )
    {
        /*second quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row + 0.5;
            rCol = col + 0.5;
        }
        else
        { /*otherwise it is EXITING_EVENT */
            rRow = row - 0.5;
            rCol = col - 0.5;
        }
    }
    else if ( row == point->row && col > point->col )
    {
        /*between the second and the fourth quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row + 0.5;
            rCol = col - 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row - 0.5;
            rCol = col - 0.5;
        }
    }
    else if ( col > point->col && row > point->row )
    {
        /*fourth quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row + 0.5;
            rCol = col - 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row - 0.5;
            rCol = col + 0.5;
        }
    }
    else if ( col == point->col && row > point->row )
    {
        /*between the third and fourth quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row - 0.5;
            rCol = col - 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row - 0.5;
            rCol = col + 0.5;
        }
    }
    else if ( col < point->col && row > point->row )
    {
        /*third quadrant */
        if ( eventType == CellPosition::ENTER )
        {
            /*if it is ENTERING_EVENT */
            rRow = row - 0.5;
            rCol = col - 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row + 0.5;
            rCol = col + 0.5;
        }
    }
    else if ( row == point->row && col < point->col )
    {
        /*between first and third quadrant */
        if ( eventType == CellPosition::ENTER )
        { /*if it is ENTERING_EVENT */
            rRow = row - 0.5;
            rCol = col + 0.5;
        }
        else
        {
            /*otherwise it is EXITING_EVENT */
            rRow = row + 0.5;
            rCol = col + 0.5;
        }
    }
    else
    {
        /*must be the viewpoint cell itself */
        rCol = col;
        rRow = row;
    }

    return Position( rRow, rCol );
}
