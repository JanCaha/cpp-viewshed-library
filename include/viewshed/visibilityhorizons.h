#pragma once
#include "viewshed_export.h"

#include <limits>

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DLL_API Horizons : public AbstractViewshedAlgorithm
        {

          public:
            Horizons( double horizon = 1, double notHorizon = 0,
                      double globalHorizon = std::numeric_limits<double>::quiet_NaN() );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mNotHorizon; };

            const std::string name() override;

          private:
            double mNotHorizon;
            double mHorizon;
            double mGlobalHorizon;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
