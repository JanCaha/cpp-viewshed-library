#pragma once
#include "viewshed_export.h"

#include <limits>

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DLL_API HorizonsCount : public AbstractViewshedAlgorithm
        {

          public:
            HorizonsCount( bool beforeTarget = true, bool onlyVisible = false, double invisibleValue = -1 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 0; };

            const std::string name() override;

          private:
            bool mBeforeTarget = true;
            bool mOnlyVisible = false;
            int mInvisibleValue = -1;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
