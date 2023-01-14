#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H

#include "iviewshedalgorithm.h"

class ViewshedElevationDifferenceToLocalHorizon : public IViewshedAlgorithm
{

  public:
    ViewshedElevationDifferenceToLocalHorizon( bool all = false, double invisibleValue = -9999,
                                               double differenceWithoutHorizon = 0 );
    double result( LoSEvaluator *losevaluator, std::vector<StatusNode> &statusNodes, StatusNode &poi,
                   std::shared_ptr<ViewPoint> vp ) override;
    const double viewpointValue() override;
    const double invisible() override;
    const double completlyVisible() override;
    const QString name() override;
    void extractValues( StatusNode &sn, StatusNode &poi, int &position ) override;

  private:
    bool mAllPoints = false;
    double mInvisibleValue;
    double mDifferenceWithoutHorizon;
};

#endif