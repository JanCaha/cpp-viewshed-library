#ifndef VIEWSHEDLIB_VIEWSHEDANGLE_H
#define VIEWSHEDLIB_VIEWSHEDANGLE_H

#include "iviewshedalgorithm.h"

namespace viewshed{
class ViewshedViewAngle : public IViewshedAlgorithm
{

  public:
    double result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                   std::shared_ptr<IPoint> vp ) override;
    void extractValues( StatusNode &sn, StatusNode &poi, int &position ) override;
    const double viewpointValue() override;
    const double invisible() override;
    const double completlyVisible() override;
    const QString name() override;
};
}

#endif