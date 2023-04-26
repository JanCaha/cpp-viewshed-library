#include "threadtasks.h"
#include "abstractviewshed.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

void viewshed::evaluateLoSForPoI( std::shared_ptr<AbstractLoS> los,
                                  std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                                  std::shared_ptr<std::vector<std::shared_ptr<MemoryRaster>>> results )
{
    LoSEvaluator losEval( los, algs );

    losEval.calculate();

    for ( int j = 0; j < algs->size(); j++ )
    {
        results->at( j )->setValue( losEval.resultAt( j ), losEval.results().col, losEval.results().row );
    }
}