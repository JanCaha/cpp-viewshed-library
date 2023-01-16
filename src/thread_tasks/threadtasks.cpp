#include "threadtasks.h"
#include "iviewshed.h"
#include "losevaluator.h"
#include "losnode.h"
#include "points.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues viewshed::evaluateLoSForPoI( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                                            std::vector<LoSNode> statusList, std::shared_ptr<LoSNode> poi,
                                            std::shared_ptr<IPoint> point )
{
    std::sort( statusList.begin(), statusList.end() );

    LoSEvaluator losEval;

    losEval.calculate( algs, statusList, poi, point );

    return losEval.mResultValues;
}