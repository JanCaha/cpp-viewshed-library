#ifndef VIEWSHEDLIB_RASTERPOSITION_H
#define VIEWSHEDLIB_RASTERPOSITION_H

namespace viewshed
{

    /**
     * @brief Class to represent position on raster in term of row and column.
     *
     */
    class RasterPosition
    {
      public:
        RasterPosition(){};

        int mRow, mCol;
    };
} // namespace viewshed

#endif