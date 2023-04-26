#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "abstractlos.h"
#include "enums.h"
#include "losnode.h"
#include "point.h"
#include "viewshed.h"

namespace viewshed
{
    /**
     * @brief Function that can be run in separate thread and evaluates given \a AbstractLoS and calculates all
     * visibility indices for the vector of provided \a AbstractViewshedAlgorithm.
     *
     * @param los
     * @param algs
     * @return ViewshedValues
     */
    void evaluateLoSForPoI( std::shared_ptr<AbstractLoS> los,
                                      std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs,
                                      std::shared_ptr<std::vector<std::shared_ptr<MemoryRaster>>> results );

} // namespace viewshed

#endif