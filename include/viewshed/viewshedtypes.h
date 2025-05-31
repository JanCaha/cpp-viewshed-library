#pragma once
#include "viewshed_export.h"

#include <memory>
#include <vector>

#include "simplerasters.h"

namespace viewshed
{
    class DLL_API AbstractViewshedAlgorithm;
    class DLL_API AbstractLoS;
    class DLL_API LoSNode;

    using ViewshedAlgorithms = std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>;
    using ResultRasters = std::vector<std::shared_ptr<SingleBandRaster>>;
} // namespace viewshed
