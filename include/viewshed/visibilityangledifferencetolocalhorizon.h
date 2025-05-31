#pragma once
#include "viewshed_export.h"

#include <limits>

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DLL_API AngleDifferenceToLocalHorizon : public AbstractViewshedAlgorithm
        {

          public:
            AngleDifferenceToLocalHorizon( bool all = false, double invisibleValue = -181,
                                           double differenceWithoutHorizon = -180, double pointValue = 0 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mPointValue; };

            const std::string name() override;

          private:
            bool mAllPoints = false;
            double mInvisibleValue;
            double mDifferenceWithoutHorizon;
            double mPointValue;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed
