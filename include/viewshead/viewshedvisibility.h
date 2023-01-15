#ifndef VIEWSHEDLIB_VIEWSHEDVISIBILITY_H
#define VIEWSHEDLIB_VIEWSHEDVISIBILITY_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedVisibility : public IViewshedAlgorithm
    {

      public:
        double result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                       std::shared_ptr<IPoint> vp ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;
        void extractValues( StatusNode &sn, StatusNode &poi, int &position ) override;
    };
} // namespace viewshed

#endif