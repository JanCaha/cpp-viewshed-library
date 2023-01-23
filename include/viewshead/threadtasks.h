#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "enums.h"
#include "los.h"
#include "losnode.h"
#include "points.h"
#include "viewshed.h"

namespace viewshed
{
    ViewshedValues evaluateLoSForPoI( std::shared_ptr<ILoS> los,
                                      std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs );

} // namespace viewshed

#endif