#ifndef VIEWSHEDLIB_VIEWSHEDVISIBILITY_H
#define VIEWSHEDLIB_VIEWSHEDVISIBILITY_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedVisibility : public IViewshedAlgorithm
    {

      public:
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<ILoS> los ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
    };
} // namespace viewshed

#endif