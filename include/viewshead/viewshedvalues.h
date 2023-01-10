#ifndef VIEWSHEDVALUES_H
#define VIEWSHEDVALUES_H

#include <vector>

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

#endif