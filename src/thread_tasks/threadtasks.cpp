#include "threadtasks.h"
#include "iviewshed.h"
#include "losevaluator.h"
#include "points.h"
#include "statusnode.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues viewshed::evaluateLoSForPoI( ViewshedAlgorithms algs, std::vector<StatusNode> statusList,
                                            std::shared_ptr<StatusNode> poi, std::shared_ptr<IPoint> point )
{
    std::sort( statusList.begin(), statusList.end() );

    LoSEvaluator losEval;

    losEval.calculate( algs, statusList, poi, point );

    return losEval.mResultValues;
}