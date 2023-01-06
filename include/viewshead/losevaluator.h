#ifndef LOSEVALUATOR_H
#define LOSEVALUATOR_H

#include <limits>

#include "iviewshedalgorithm.h"
#include "points.h"
#include "statusnode.h"

class LoSEvaluator
{
  public:
    LoSEvaluator( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs );

    void calculate( std::vector<StatusNode> &statusNodes, StatusNode &poi, std::shared_ptr<ViewPoint> vp );
    void reset();

    int size();
    std::shared_ptr<IViewshedAlgorithm> algorithmAt( int i );

    double mMaxGradientBefore = std::numeric_limits<double>::max() * ( -1 );
    double mMaxGradient = std::numeric_limits<double>::max() * ( -1 );
    int mIndexPoi = 0;
    int mIndexMaxGradientBefore = 0;
    int mIndexMaxGradient = 0;
    int mIndexHorizonBefore = 0;
    int mIndexHorizon = 0;
    int mCountHorizonBefore = 0;
    int mCountHorizon = 0;

    std::vector<double> mResults;
    double resultAt( int i );

  private:
    std::vector<std::shared_ptr<IViewshedAlgorithm>> mAlgs;

    void parseNodes( std::vector<StatusNode> &statusNodes, StatusNode &poi );
};

#endif