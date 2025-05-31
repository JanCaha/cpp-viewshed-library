#pragma once
#include "viewshed_export.h"

namespace viewshed
{
    /**
     * @brief Enum that specifies types of data, that can be extracted from LoSNode.
     *
     */
    enum class DLL_API ValueType
    {
        Angle,
        Elevation,
        Distance
    };

    /**
     * @brief Enum that specifies cell event position.
     *
     */
    enum DLL_API CellEventPositionType
    {
        ENTER = 0,
        CENTER = 1,
        EXIT = 2
    };
} // namespace viewshed
