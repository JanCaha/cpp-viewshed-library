#include "math.h"

#include "visibility.h"

double Visibility::calculateAngle( Position *pos, std::shared_ptr<ViewPoint> vp )
{
    return calculateAngle( pos->row, pos->col, vp );
}

double Visibility::calculateAngle( double row, double column, std::shared_ptr<ViewPoint> vp )
{
    double angle = atan( fabs( row - vp->row ) / fabs( column - vp->col ) );

    if ( vp->row == row && column > vp->col )
    {
        return 0; /*between 1st and 4th quadrant */
    }
    else if ( column > vp->col && row < vp->row )
    {
        /*first quadrant */
        return angle;
    }
    else if ( vp->col == column && vp->row > row )
    {
        /*between 1st and 2nd quadrant */
        return M_PI / 2;
    }
    else if ( column < vp->col && row < vp->row )
    {
        /*second quadrant */
        return ( M_PI - angle );
    }
    else if ( vp->row == row && column < vp->col )
    {
        /*between 1st and 3rd quadrant */
        return M_PI;
    }
    else if ( row > vp->row && column < vp->col )
    {
        /*3rd quadrant */
        return ( M_PI + angle );
    }
    else if ( vp->col == column && vp->row < row )
    {
        /*between 3rd and 4th quadrant */
        return ( ( M_PI * 3.0 ) / 2.0 );
    }
    else if ( column > vp->col && row > vp->row )
    {
        /*4th quadrant */
        return ( M_PI * 2.0 - angle );
    }

    return 0;
}

double Visibility::calculateDistance( Position *pos, std::shared_ptr<ViewPoint> vp, double &cellSize )
{
    return calculateDistance( pos->row, pos->col, vp, cellSize );
}

double Visibility::calculateDistance( double &row, double &column, std::shared_ptr<ViewPoint> vp, double &cellSize )
{
    return sqrt( pow( row - vp->row, 2 ) + pow( column - vp->col, 2 ) ) * cellSize;
}

double Visibility::calculateDistance( int &row, int &column, std::shared_ptr<ViewPoint> vp, double &cellSize )
{
    double r = (double)row;
    double c = (double)column;
    return calculateDistance( r, c, vp, cellSize );
}

double Visibility::calculateGradient( std::shared_ptr<ViewPoint> vp, Position *pos, double elevation, double &distance )
{
    return calculateGradient( vp, pos->row, pos->col, elevation, distance );
}

double Visibility::calculateGradient( std::shared_ptr<ViewPoint> vp, double &row, double &column, double elevation,
                                      double &distance )
{
    double dx = vp->col - column;
    double dy = vp->row - row;

    double elevationDifference = elevation - vp->totalElevation();

    if ( elevationDifference == 0 )
        return 0;

    return atan( elevationDifference / distance ) * ( 180 / M_PI );
}

Position Visibility::calculateEventPosition( CellPosition eventType, int row, int col, ViewPoint point )
{

    double rRow, rCol;

    if ( eventType == CellPosition::CENTER )
    {
        /*FOR CENTER_EVENTS */
        rRow = row;
        rCol = col;
    }

    if ( row < point.row && col < point.col )
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
    else if ( col == point.col && row < point.row )
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
    else if ( col > point.col && row < point.row )
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
    else if ( row == point.row && col > point.col )
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
    else if ( col > point.col && row > point.row )
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
    else if ( col == point.col && row > point.row )
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
    else if ( col < point.col && row > point.row )
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
    else if ( row == point.row && col < point.col )
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
