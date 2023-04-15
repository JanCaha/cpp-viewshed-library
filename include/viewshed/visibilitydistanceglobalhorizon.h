#ifndef VIEWSHEDLIB_VIEWSHEDGLOBALHORIZONDISTANCE_H
#define VIEWSHEDLIB_VIEWSHEDGLOBALHORIZONDISTANCE_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class DistanceGlobalHorizon : public AbstractViewshedAlgorithm
        {

          public:
            DistanceGlobalHorizon( double noHorizon = -99999 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return 0; };

            const QString name() override;

          private:
            double mNoHorizon = -99999;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif