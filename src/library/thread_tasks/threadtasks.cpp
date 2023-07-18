#include <random>

#include "../utils/debug_info_handling_functions.h"
#include "abstractlos.h"
#include "abstractviewshed.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "losnode.h"
#include "point.h"
#include "threadtasks.h"
#include "viewshedvalues.h"

typedef std::mt19937 MyRNG; // the Mersenne Twister with a popular choice of parameters
uint32_t seed_val;          // populate somehow

MyRNG rng;
std::uniform_int_distribution<uint32_t> uint_dist10( 0, 200 );

using viewshed::AbstractLoS;
using viewshed::AbstractViewshedAlgorithm;
using viewshed::InverseLoS;
using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;

void viewshed::evaluateLoS( std::shared_ptr<AbstractLoS> los,
                            std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                            std::shared_ptr<std::vector<std::shared_ptr<SingleBandRaster>>> results )
{
    LoSEvaluator losEval( los, algs );

#if CALCULATE_INDIVIDUAL_LOS_TIMING
    auto inverseLoS = std::dynamic_pointer_cast<InverseLoS>( los );

    if ( uint_dist10( rng ) < 5 )
    {
        losEval.calculate();

        if ( inverseLoS )
        {
            handle_inverse_viewshed_los_timing( los->timeToCopy, los->timeToEval, los->numberOfNodes() );
        }
        else
        {
            handle_viewshed_los_timing( los->timeToCopy, los->timeToEval, los->numberOfNodes() );
        }
    }
#else
    losEval.calculate();
#endif

    for ( int j = 0; j < algs->size(); j++ )
    {
        results->at( j )->writeValue( losEval.results().mRow, losEval.results().mCol, losEval.resultAt( j ) );
    }
}