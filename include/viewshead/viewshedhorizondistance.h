#ifndef VIEWSHEDLIB_VIEWSHEDHORIZONDISTANCE_H
#define VIEWSHEDLIB_VIEWSHEDHORIZONDISTANCE_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedHorizonDistance : public AbstractViewshedAlgorithm
    {

      public:
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
    };
} // namespace viewshed

#endif