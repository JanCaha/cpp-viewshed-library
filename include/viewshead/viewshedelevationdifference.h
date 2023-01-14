#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFF_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFF_H

#include "iviewshedalgorithm.h"

class ViewshedElevationDifference : public IViewshedAlgorithm
{

  public:
    double result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                   std::shared_ptr<ViewPoint> vp ) override;
    const double viewpointValue() override;
    const double invisible() override;
    const double completlyVisible() override;
    const QString name() override;
    void extractValues( StatusNode &sn, StatusNode &poi, int &position ) override;
};

#endif