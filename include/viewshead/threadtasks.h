#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "enums.h"
#include "los.h"
#include "losnode.h"
#include "point.h"
#include "viewshed.h"

namespace viewshed
{
    ViewshedValues evaluateLoSForPoI( std::shared_ptr<AbstractLoS> los,
                                      std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs );

} // namespace viewshed

#endif