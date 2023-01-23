#ifndef VIEWSHEDLIB_VIEWSHEDANGLEDIFFLOCAL_H
#define VIEWSHEDLIB_VIEWSHEDANGLEDIFFLOCAL_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedAngleDifferenceToLocalHorizon : public IViewshedAlgorithm
    {

      public:
        ViewshedAngleDifferenceToLocalHorizon( bool all = false, double invisibleValue = -91 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<ILoS> los ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;

      private:
        bool mAllPoints = false;
        double mInvisibleValue;
    };
} // namespace viewshed

#endif