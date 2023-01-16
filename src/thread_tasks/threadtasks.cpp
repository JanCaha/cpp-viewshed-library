#include "threadtasks.h"
#include "iviewshed.h"
#include "losevaluator.h"
#include "losnode.h"
#include "points.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues viewshed::evaluateLoSForPoI( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                                            std::shared_ptr<std::vector<LoSNode>> los, std::shared_ptr<LoSNode> poi,
                                            std::shared_ptr<IPoint> point )
{
    std::sort( los->begin(), los->end() );

    LoSEvaluator losEval( los, algs );

    losEval.calculate( poi, point );

    return losEval.mResultValues;
}