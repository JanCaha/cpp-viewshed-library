#ifndef VIEWSHEDLIB_CELLEVENTPOSITION_H
#define VIEWSHEDLIB_CELLEVENTPOSITION_H

namespace viewshed
{
    /**
     * @brief Simple tuple representing cell event position in term of row and col (as double).
     *
     */
    struct CellEventPosition
    {
        double row, col;

        CellEventPosition( double row_, double col_ );
    };
} // namespace viewshed

#endif