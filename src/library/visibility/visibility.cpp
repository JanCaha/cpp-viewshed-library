#include <cmath>
#include <numbers>

#include "celleventposition.h"
#include "point.h"
#include "rasterposition.h"
#include "visibility.h"

using viewshed::CellEventPosition;
using viewshed::Point;
using viewshed::RasterPosition;
using viewshed::Visibility;

double Visibility::distance( const double &x1, const double &y1, const double &x2, const double &y2,
                             const double &cellSize )
{
    return sqrt( pow( x1 - x2, 2 ) + pow( y1 - y2, 2 ) ) * cellSize;
}

double Visibility::gradient( double elevationDiff, double distance )
{
    if ( elevationDiff == 0 )
        return 0;

    return atan( elevationDiff / distance ) * ( 180 / std::numbers::pi );
}

double Visibility::curvatureCorrections( const double &distance, const double &refractionCoeff,
                                         const double &earthDiameter )
{
    return -( pow( distance, 2 ) / earthDiameter ) + refractionCoeff * ( pow( distance, 2 ) / earthDiameter );
}

CellEventPosition Visibility::eventPosition( const CellEventPositionType &eventType, const int &row, const int &col,
                                             const int &pointRow, const int &pointCol )
{
    double rRow, rCol;

    if ( eventType == CellEventPositionType::CENTER )
    {
        /*FOR CENTER_EVENTS */
        rRow = row;
        rCol = col;
    }

    if ( row < pointRow && col < pointCol )
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
    else if ( col == pointCol && row < pointRow )
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
    else if ( col > pointCol && row < pointRow )
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
    else if ( row == pointRow && col > pointCol )
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
    else if ( col > pointCol && row > pointRow )
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
    else if ( col == pointCol && row > pointRow )
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
    else if ( col < pointCol && row > pointRow )
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
    else if ( row == pointRow && col < pointCol )
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

double Visibility::angle( const double &row, const double &column, const int &pointRow, const int &pointCol )
{
    double angle = atan( fabs( row - pointRow ) / fabs( column - pointCol ) );

    if ( pointRow == row && column > pointCol )
    {
        return 0; /*between 1st and 4th quadrant */
    }
    else if ( column > pointCol && row < pointRow )
    {
        /*first quadrant */
        return angle;
    }
    else if ( pointCol == column && pointRow > row )
    {
        /*between 1st and 2nd quadrant */
        return std::numbers::pi / 2;
    }
    else if ( column < pointCol && row < pointRow )
    {
        /*second quadrant */
        return ( std::numbers::pi - angle );
    }
    else if ( pointRow == row && column < pointCol )
    {
        /*between 1st and 3rd quadrant */
        return std::numbers::pi;
    }
    else if ( row > pointRow && column < pointCol )
    {
        /*3rd quadrant */
        return ( std::numbers::pi + angle );
    }
    else if ( pointCol == column && pointRow < row )
    {
        /*between 3rd and 4th quadrant */
        return ( ( std::numbers::pi * 3.0 ) / 2.0 );
    }
    else if ( column > pointCol && row > pointRow )
    {
        /*4th quadrant */
        return ( std::numbers::pi * 2.0 - angle );
    }

    return 0;
}
