#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H

#include "iviewshedalgorithm.h"
#include "losimportantvalues.h"

namespace viewshed
{
    class ViewshedElevationDifferenceToLocalHorizon : public IViewshedAlgorithm
    {

      public:
        ViewshedElevationDifferenceToLocalHorizon( bool all = false, double invisibleValue = -9999,
                                                   double differenceWithoutHorizon = 0 );
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<std::vector<LoSNode>> los,
                       std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp ) override;
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