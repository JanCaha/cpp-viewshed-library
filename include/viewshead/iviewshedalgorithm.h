#ifndef VIEWSHEDLIB_IVIEWSHEDALGORITHM_H
#define VIEWSHEDLIB_IVIEWSHEDALGORITHM_H

#include <limits>

#include "QString"

#include "points.h"
#include "statusnode.h"

class LoSEvaluator;

class IViewshedAlgorithm
{
  public:
    virtual double result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                           std::shared_ptr<ViewPoint> vp ) = 0;
    virtual void extractValues( StatusNode &sn, StatusNode &poi, int &position ) = 0;
    virtual const double viewpointValue() = 0;
    virtual const double invisible() = 0;
    virtual const double completlyVisible() = 0;
    virtual const QString name() = 0;
};

#endif