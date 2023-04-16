#ifndef VIEWSHEDLIB_FUZZY_H
#define VIEWSHEDLIB_FUZZY_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class FuzzyVisibility : public AbstractViewshedAlgorithm
        {

          public:
            FuzzyVisibility( double clearVisibility = 500, double halfDropout = 1500,
                             bool includeVerticalDistance = false, double notVisible = -1 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 1; };

            const QString name() override;

          private:
            double mB1;
            double mB2;
            double mNotVisibile = -1;
            bool mVerticalDistane = false;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif