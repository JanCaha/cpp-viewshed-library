#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "simplerasters.h"
#include "viewshedtypes.h"

using viewshed::ResultRasters;
using viewshed::ViewshedAlgorithms;

namespace viewshed
{
    class AbstractLoS;
    class AbstractViewshedAlgorithm;

    /**
     * @brief Function that can be run in separate thread and evaluates given \a AbstractLoS and calculates all
     * visibility indices for the vector of provided \a AbstractViewshedAlgorithm.
     *
     * @param los
     * @param algs
     * @return ViewshedValues
     */
    void evaluateLoS( std::shared_ptr<AbstractLoS> los, std::shared_ptr<ViewshedAlgorithms> algs,
                      std::shared_ptr<ResultRasters> results );

} // namespace viewshed

#endif