#ifndef VIEWSHEDLIB_CELLEVENTPOSITION_H
#define VIEWSHEDLIB_CELLEVENTPOSITION_H

namespace viewshed
{
    struct CellEventPosition
    {
        double row, col;

        CellEventPosition( double row_, double col_ );
    };
} // namespace viewshed

#endif