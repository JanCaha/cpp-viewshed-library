#include <chrono>

#include "abstractlos.h"
#include "db.h"

using viewshed::AbstractLoS;

void handle_inverse_viewshed_los_timing( std::shared_ptr<AbstractLoS> los )
{
    pg.add_los_timing_data_to( "inverseviewshed_los_timing", los );
}

void handle_viewshed_los_timing( std::shared_ptr<AbstractLoS> los )
{
    pg.add_los_timing_data_to( "viewshed_los_timing", los );
}