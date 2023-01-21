#ifndef VIEWSHEDLIB_ENUMS_H
#define VIEWSHEDLIB_ENUMS_H

namespace viewshed
{
    enum class ValueType
    {
        Angle,
        Elevation,
        Distance,
        Gradient
    };

    enum CellEventPositionType
    {
        ENTER = 0,
        CENTER = 1,
        EXIT = 2
    };
} // namespace viewshed

#endif