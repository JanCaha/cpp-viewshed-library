#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFF_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFF_H

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedElevationDifference : public IViewshedAlgorithm
    {

      public:
        double result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                       std::shared_ptr<IPoint> vp ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
        void extractValues( LoSNode &sn, LoSNode &poi, int &position ) override;
    };
} // namespace viewshed

#endif