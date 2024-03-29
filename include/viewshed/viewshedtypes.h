#pragma once

#include <memory>
#include <vector>

#include "simplerasters.h"

namespace viewshed
{
    class AbstractViewshedAlgorithm;
    class AbstractLoS;
    class LoSNode;

    using ViewshedAlgorithms = std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>;
    using ResultRasters = std::vector<std::shared_ptr<SingleBandRaster>>;
} // namespace viewshed
