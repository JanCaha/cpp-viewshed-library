#ifndef VIEWSHEDLIB_VIEWSHEDHORIZON_H
#define VIEWSHEDLIB_VIEWSHEDHORIZON_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedHorizons : public IViewshedAlgorithm
    {

      public:
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<std::vector<LoSNode>> los,
                       std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp ) override;

        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
    };
} // namespace viewshed

#endif