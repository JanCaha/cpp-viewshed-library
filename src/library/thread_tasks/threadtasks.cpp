#include "threadtasks.h"
#include "abstractviewshed.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

void viewshed::evaluateLoS( std::shared_ptr<AbstractLoS> los,
                            std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                            std::shared_ptr<std::vector<std::shared_ptr<SingleBandRaster>>> results )
{
    LoSEvaluator losEval( los, algs );

    losEval.calculate();

    for ( int j = 0; j < algs->size(); j++ )
    {
        results->at( j )->writeValue( losEval.results().row, losEval.results().col, losEval.resultAt( j ) );
    }
}