#ifndef VIEWSHEDLIB_ENUMS_H
#define VIEWSHEDLIB_ENUMS_H

namespace viewshed
{
    /**
     * @brief Enum that specifies types of data, that can be extracted from LoSNode.
     *
     */
    enum class ValueType
    {
        Angle,
        Elevation,
        Distance,
        Gradient
    };

    /**
     * @brief Enum that specifies cell event position.
     *
     */
    enum CellEventPositionType
    {
        ENTER = 0,
        CENTER = 1,
        EXIT = 2
    };
} // namespace viewshed

#endif