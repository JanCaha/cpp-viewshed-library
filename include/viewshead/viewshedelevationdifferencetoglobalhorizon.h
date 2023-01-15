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
        double result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                       std::shared_ptr<IPoint> vp ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
        void extractValues( LoSNode &sn, LoSNode &poi, int &position ) override;

      private:
        bool mAllPoints = false;
        double mInvisibleValue;
        double mDifferenceWithoutHorizon;
    };
} // namespace viewshed

#endif