#pragma once

#include <limits>

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class LoSSlopeToViewAngle : public AbstractViewshedAlgorithm
        {

          public:
            LoSSlopeToViewAngle( double invisible = -1, double pointValue = 0 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mPointValue; };

            const std::string name() override;

          private:
            double mInvisibile;
            double mPointValue;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
