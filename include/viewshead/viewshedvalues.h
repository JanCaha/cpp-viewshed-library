#ifndef VIEWSHEDLIB_VIEWSHEDVALUES_H
#define VIEWSHEDLIB_VIEWSHEDVALUES_H

#include <vector>

#include "points.h"

namespace viewshed
{

    class ViewshedValues : public RasterPosition
    {
      public:
        std::vector<double> values;

        ViewshedValues(){};

        ViewshedValues( int row_, int col_ )
        {
            row = row_;
            col = col_;
        };
    };
} // namespace viewshed

#endif