#include "math.h"

#include "rasterposition.h"
#include "visibility.h"

using viewshed::CellEventPosition;
using viewshed::RasterPosition;
using viewshed::Visibility;

double Visibility::angle( RasterPosition *pos, std::shared_ptr<Point> point )
{
    return angle( (double)pos->row, (double)pos->col, point );
}

double Visibility::angle( CellEventPosition *pos, std::shared_ptr<Point> point )
{
    return angle( pos->row, pos->col, point );
}

double Visibility::angle( double row, double column, std::shared_ptr<Point> point )
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

double Visibility::distance( const double &x1, const double &y1, const double &x2, const double &y2, double &cellSize )
{
    return sqrt( pow( x1 - x2, 2 ) + pow( y1 - y2, 2 ) ) * cellSize;
}

double Visibility::distance( CellEventPosition *pos, std::shared_ptr<Point> point, double &cellSize )
{
    return distance( pos->col, pos->row, point->col, point->row, cellSize );
}

double Visibility::distance( double &row, double &column, std::shared_ptr<Point> point, double &cellSize )
{
    return distance( column, row, point->col, point->row, cellSize );
}

double Visibility::distance( std::shared_ptr<Point> point1, std::shared_ptr<Point> point2, double &cellSize )
{
    return distance( point1->col, point1->row, point2->col, point2->row, cellSize );
}

double Visibility::distance( int &row, int &column, std::shared_ptr<Point> point, double &cellSize )
{
    double r = (double)row;
    double c = (double)column;
    return distance( r, c, point, cellSize );
}

double Visibility::gradient( std::shared_ptr<Point> point, double elevation, double &distance )
{
    double elevationDifference = elevation - point->totalElevation();

    if ( elevationDifference == 0 )
        return 0;

    return atan( elevationDifference / distance ) * ( 180 / M_PI );
}

CellEventPosition Visibility::eventPosition( CellEventPositionType eventType, int row, int col,
                                             std::shared_ptr<Point> point )
{
    double rRow, rCol;

    if ( eventType == CellEventPositionType::CENTER )
    {
        /*FOR CENTER_EVENTS */
        rRow = row;
        rCol = col;
    }

    if ( row < point->row && col < point->col )
    {
        /*first quadrant */
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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
        if ( eventType == CellEventPositionType::ENTER )
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

    return CellEventPosition( rRow, rCol );
}
