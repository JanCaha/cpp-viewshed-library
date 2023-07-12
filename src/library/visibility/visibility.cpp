#include "math.h"

#include "celleventposition.h"
#include "point.h"
#include "rasterposition.h"
#include "visibility.h"

using viewshed::CellEventPosition;
using viewshed::Point;
using viewshed::RasterPosition;
using viewshed::Visibility;

double Visibility::angle( RasterPosition *pos, std::shared_ptr<Point> point )
{
    return angle( (double)pos->mRow, (double)pos->mCol, point );
}

double Visibility::angle( CellEventPosition *pos, std::shared_ptr<Point> point )
{
    return angle( pos->mRow, pos->mCol, point );
}

double Visibility::angle( double row, double column, std::shared_ptr<Point> point )
{
    double angle = atan( fabs( row - point->mRow ) / fabs( column - point->mCol ) );

    if ( point->mRow == row && column > point->mCol )
    {
        return 0; /*between 1st and 4th quadrant */
    }
    else if ( column > point->mCol && row < point->mRow )
    {
        /*first quadrant */
        return angle;
    }
    else if ( point->mCol == column && point->mRow > row )
    {
        /*between 1st and 2nd quadrant */
        return M_PI / 2;
    }
    else if ( column < point->mCol && row < point->mRow )
    {
        /*second quadrant */
        return ( M_PI - angle );
    }
    else if ( point->mRow == row && column < point->mCol )
    {
        /*between 1st and 3rd quadrant */
        return M_PI;
    }
    else if ( row > point->mRow && column < point->mCol )
    {
        /*3rd quadrant */
        return ( M_PI + angle );
    }
    else if ( point->mCol == column && point->mRow < row )
    {
        /*between 3rd and 4th quadrant */
        return ( ( M_PI * 3.0 ) / 2.0 );
    }
    else if ( column > point->mCol && row > point->mRow )
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
    return distance( pos->mCol, pos->mRow, point->mCol, point->mRow, cellSize );
}

double Visibility::distance( double &row, double &column, std::shared_ptr<Point> point, double &cellSize )
{
    return distance( column, row, point->mCol, point->mRow, cellSize );
}

double Visibility::distance( std::shared_ptr<Point> point1, std::shared_ptr<Point> point2, double &cellSize )
{
    return distance( point1->mCol, point1->mRow, point2->mCol, point2->mRow, cellSize );
}

double Visibility::distance( int &row, int &column, std::shared_ptr<Point> point, double &cellSize )
{
    double r = (double)row;
    double c = (double)column;
    return distance( r, c, point, cellSize );
}

double Visibility::gradient( std::shared_ptr<Point> point, double elevation, double distance )
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

    if ( row < point->mRow && col < point->mCol )
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
    else if ( col == point->mCol && row < point->mRow )
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
    else if ( col > point->mCol && row < point->mRow )
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
    else if ( row == point->mRow && col > point->mCol )
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
    else if ( col > point->mCol && row > point->mRow )
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
    else if ( col == point->mCol && row > point->mRow )
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
    else if ( col < point->mCol && row > point->mRow )
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
    else if ( row == point->mRow && col < point->mCol )
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

double Visibility::curvatureCorrections( double distance, double refractionCoeff, double earthDiameter )
{
    return -( pow( distance, 2 ) / earthDiameter ) + refractionCoeff * ( pow( distance, 2 ) / earthDiameter );
}
