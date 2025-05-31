#pragma once

#include <vector>

#include "rasterposition.h"

namespace viewshed
{
    class Point;

    /**
     * @brief A simple class that stores results of visibility indices calculation (in form of vector of doubles) for
     * specific cell.
     *
     */
    class DLL_API ViewshedValues : public RasterPosition
    {
      public:
        std::vector<double> values;

        ViewshedValues() {};

        ViewshedValues( int row_, int col_ )
        {
            mRow = row_;
            mCol = col_;
        };
    };
} // namespace viewshed
