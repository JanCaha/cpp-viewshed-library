#pragma once
#include "viewshed_export.h"

namespace viewshed
{
    /**
     * @brief Simple tuple representing cell event position in term of row and col (as double).
     *
     */
    struct DLL_API CellEventPosition
    {
        double mRow, mCol;

        CellEventPosition( double row, double col );
    };
} // namespace viewshed
