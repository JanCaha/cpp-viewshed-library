#include <chrono>

#include "db.h"

void handle_inverse_viewshed_los_timing( std::chrono::nanoseconds timeToCopy, std::chrono::nanoseconds timeToEval )
{
    pg.add_los_timing_data_to( "inverseviewshed_los_timing", timeToCopy, timeToEval );
}

void handle_viewshed_los_timing( std::chrono::nanoseconds timeToCopy, std::chrono::nanoseconds timeToEval )
{
    pg.add_los_timing_data_to( "viewshed_los_timing", timeToCopy, timeToEval );
}