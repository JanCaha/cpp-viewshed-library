#pragma once
#include "viewshed_export.h"

#include <limits>

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DLL_API Boolean : public AbstractViewshedAlgorithm
        {

          public:
            Boolean( double visible = 1, double invisible = 0, double pointValue = 1 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mPointValue; };

            const std::string name() override;

          private:
            double mVisibile;
            double mInvisibile;
            double mPointValue;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
