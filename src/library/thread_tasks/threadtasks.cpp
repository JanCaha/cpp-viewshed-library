#include "threadtasks.h"
#include "abstractlos.h"
#include "abstractviewshed.h"
#include "losevaluator.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::AbstractLoS;
using viewshed::AbstractViewshedAlgorithm;
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
        results->at( j )->writeValue( losEval.results().mRow, losEval.results().mCol, losEval.resultAt( j ) );
    }
}