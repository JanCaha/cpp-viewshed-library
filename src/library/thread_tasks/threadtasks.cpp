#include "threadtasks.h"
#include "../utils/db.h"
#include "abstractviewshed.h"
#include "inverselos.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::InverseLoS;
using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

ViewshedValues
viewshed::evaluateLoSForPoI( std::shared_ptr<AbstractLoS> los,
                             std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs )
{
    LoSEvaluator losEval( los, algs );

    losEval.calculate();

    auto inverseLoS = std::dynamic_pointer_cast<InverseLoS>( los );

    if ( inverseLoS )
    {
        db.add_los_timing_data_to( "inverseviewshed_los_timing", los->timeToCopy, los->timeToEval );
    }
    else
    {
        db.add_los_timing_data_to( "viewshed_los_timing", los->timeToCopy, los->timeToEval );
    }

    return losEval.results();
}