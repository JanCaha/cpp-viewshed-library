#ifndef VIEWSHEDLIB_VIEWSHEDHORIZONCOUNT_H
#define VIEWSHEDLIB_VIEWSHEDHORIZONCOUNT_H

#include "limits"

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class HorizonsCount : public AbstractViewshedAlgorithm
        {

          public:
            HorizonsCount( bool beforeTarget = true );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 0; };

            const QString name() override;

          private:
            bool mBeforeTarget = true;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif