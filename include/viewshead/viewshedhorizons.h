#ifndef VIEWSHEDLIB_VIEWSHEDHORIZON_H
#define VIEWSHEDLIB_VIEWSHEDHORIZON_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedHorizons : public IViewshedAlgorithm
    {

      public:
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<LoS> los ) override;

        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
    };
} // namespace viewshed

#endif