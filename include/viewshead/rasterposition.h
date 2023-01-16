#ifndef VIEWSHEDLIB_RASTERPOSITION_H
#define VIEWSHEDLIB_RASTERPOSITION_H

namespace viewshed
{
    struct RasterPosition
    {
        double row, col;

        RasterPosition( double row_, double col_ );
    };
} // namespace viewshed

#endif