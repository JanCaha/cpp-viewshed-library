#pragma once

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DistanceLocalHorizon : public AbstractViewshedAlgorithm
        {

          public:
            DistanceLocalHorizon( double noHorizon = -1 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 0; };

            const std::string name() override;

          private:
            double mNoHorizon = -1;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
