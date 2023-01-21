#include "threadtasks.h"
#include "iviewshed.h"
#include "losnode.h"
#include "points.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues viewshed::evaluateLoSForPoI( std::shared_ptr<LoS> los,
                                            std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs )
{
    LoSEvaluator losEval( los, algs );

    losEval.calculate();

    return losEval.mResultValues;
}