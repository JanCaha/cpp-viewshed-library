#pragma once
#include "viewshed_export.h"

namespace viewshed
{

    /**
     * @brief Class to represent position on raster in term of row and column.
     *
     */
    class DLL_API RasterPosition
    {
      public:
        RasterPosition(){};

        int mRow, mCol;
    };
} // namespace viewshed
