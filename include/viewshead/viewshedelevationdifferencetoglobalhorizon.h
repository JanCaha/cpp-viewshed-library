#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFFGLOBAL_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFFGLOBAL_H

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedElevationDifferenceToGlobalHorizon : public IViewshedAlgorithm
    {

      public:
        ViewshedElevationDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -9999,
                                                    double differenceWithoutHorizon = 0 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<ILoS> los ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;

      private:
        bool mAllPoints = false;
        double mInvisibleValue;
        double mDifferenceWithoutHorizon;
    };
} // namespace viewshed

#endif