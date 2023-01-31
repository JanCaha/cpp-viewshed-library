#include "threadtasks.h"
#include "abstractviewshed.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues
viewshed::evaluateLoSForPoI( std::shared_ptr<AbstractLoS> los,
                             std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs )
{
    LoSEvaluator losEval( los, algs );

    losEval.calculate();

    return losEval.mResultValues;
}