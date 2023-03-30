#include "threadtasks.h"
#include "../utils/debug_info_handling_functions.h"
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

#if CALCULATE_INDIVIDUAL_LOS_TIMING
    auto inverseLoS = std::dynamic_pointer_cast<InverseLoS>( los );

    if ( inverseLoS )
    {
        handle_inverse_viewshed_los_timing( los->timeToCopy, los->timeToEval );
    }
    else
    {
        handle_viewshed_los_timing( los->timeToCopy, los->timeToEval );
    }
#endif

    return losEval.results();
}