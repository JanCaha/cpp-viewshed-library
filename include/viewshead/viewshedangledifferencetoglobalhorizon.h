#include "limits"

#include "iviewshedalgorithm.h"

class ViewshedAngleDifferenceToGlobalHorizon : public IViewshedAlgorithm
{

  public:
    ViewshedAngleDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -91 );
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
};