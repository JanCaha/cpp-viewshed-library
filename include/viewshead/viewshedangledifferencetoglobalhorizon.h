#ifndef VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H
#define VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedAngleDifferenceToGlobalHorizon : public IViewshedAlgorithm
    {

      public:
        ViewshedAngleDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -91 );
        double result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                       std::shared_ptr<IPoint> vp ) override;
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