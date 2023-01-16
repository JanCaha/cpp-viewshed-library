#ifndef VIEWSHEDLIB_VIEWSHEDVALUES_H
#define VIEWSHEDLIB_VIEWSHEDVALUES_H

#include <vector>

namespace viewshed{
struct ViewshedValues
{
    double row, col;
    std::vector<double> values;

    ViewshedValues(){};

    ViewshedValues( double row_, double col_ )
    {
        row = row_;
        col = col_;
    };
};
}

#endif