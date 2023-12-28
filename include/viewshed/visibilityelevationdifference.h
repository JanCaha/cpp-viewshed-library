#pragma once

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class ElevationDifference : public AbstractViewshedAlgorithm
        {

          public:
            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override;

            const std::string name() override;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
