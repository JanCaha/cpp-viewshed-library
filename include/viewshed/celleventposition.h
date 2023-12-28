#pragma once

namespace viewshed
{
    /**
     * @brief Simple tuple representing cell event position in term of row and col (as double).
     *
     */
    struct CellEventPosition
    {
        double mRow, mCol;

        CellEventPosition( double row, double col );
    };
} // namespace viewshed
