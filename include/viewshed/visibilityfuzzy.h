#pragma once
#include "viewshed_export.h"

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DLL_API FuzzyVisibility : public AbstractViewshedAlgorithm
        {

          public:
            FuzzyVisibility( double clearVisibility = 500, double halfDropout = 1500,
                             bool includeVerticalDistance = false, double notVisible = -1 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 1; };

            const std::string name() override;

          private:
            double mB1;
            double mB2;
            double mNotVisible = -1;
            bool mVerticalDistance = false;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
