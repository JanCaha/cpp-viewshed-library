#ifndef VIEWSHEDLIB_IVIEWSHEDALGORITHM_H
#define VIEWSHEDLIB_IVIEWSHEDALGORITHM_H

#include <limits>

#include "QString"

#include "losnode.h"
#include "points.h"

namespace viewshed
{
    class LoSEvaluator;

    class IViewshedAlgorithm
    {
      public:
        virtual double result( LoSEvaluator *losevaluator, std::vector<LoSNode> &statusNodes, LoSNode &poi,
                               std::shared_ptr<IPoint> vp ) = 0;
        virtual void extractValues( LoSNode &ln, LoSNode &poi, int &position ) = 0;
        virtual const double viewpointValue() = 0;
        virtual const double invisible() = 0;
        virtual const double completlyVisible() = 0;
        virtual const QString name() = 0;
    };
} // namespace viewshed

#endif