#ifndef VIEWSHEDLIB_VIEWSHEDHORIZON_H
#define VIEWSHEDLIB_VIEWSHEDHORIZON_H

#include "limits"

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class Horizons : public AbstractViewshedAlgorithm
        {

          public:
            Horizons( double horizon = 1, double notHorizon = 0 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mNotHorizon; };

            const QString name() override;

          private:
            double mNotHorizon;
            double mHorizon;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif