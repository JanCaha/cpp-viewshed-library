#ifndef VIEWSHEDLIB_VIEWSHEDANGLE_H
#define VIEWSHEDLIB_VIEWSHEDANGLE_H

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedViewAngle : public IViewshedAlgorithm
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