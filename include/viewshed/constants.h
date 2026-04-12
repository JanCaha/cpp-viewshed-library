#pragma once
#include <numbers>

namespace viewshed
{
    const double REFRACTION_COEFFICIENT{ 0.142860 };
    const double EARTH_DIAMETER{ 12740000 };
    const double OBSERVER_OFFSET{ 1.6 };
    const double DEG_TO_RAD{ std::numbers::pi / 180.0 };
    const double RAD_TO_DEG{ 180.0 / std::numbers::pi };
} // namespace viewshed