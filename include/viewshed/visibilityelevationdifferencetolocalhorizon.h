#pragma once

#include "abstractviewshedalgorithm.h"
#include "losimportantvalues.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class ElevationDifferenceToLocalHorizon : public AbstractViewshedAlgorithm
        {

          public:
            ElevationDifferenceToLocalHorizon( bool all = false, double invisibleValue = -9999,
                                               double differenceWithoutHorizon = 0, double pointValue = 0 );
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
