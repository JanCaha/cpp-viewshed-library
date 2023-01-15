#ifndef VIEWSHEDLIB_POSITION_H
#define VIEWSHEDLIB_POSITION_H

namespace viewshed
{
    struct Position
    {
        double row, col;

        Position( double row_, double col_ );
    };
} // namespace viewshed

#endif