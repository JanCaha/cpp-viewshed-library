#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "enums.h"
#include "points.h"
#include "rasterposition.h"
#include "losnode.h"
#include "viewshed.h"

namespace viewshed
{
    ViewshedValues evaluateLoSForPoI( ViewshedAlgorithms algs, std::vector<LoSNode> statusList,
                                      std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> point );
}

#endif