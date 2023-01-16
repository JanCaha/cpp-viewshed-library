#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "enums.h"
#include "losnode.h"
#include "points.h"
#include "rasterposition.h"
#include "viewshed.h"

namespace viewshed
{
    ViewshedValues evaluateLoSForPoI( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                                      std::shared_ptr<std::vector<LoSNode>> los, std::shared_ptr<LoSNode> poi,
                                      std::shared_ptr<IPoint> point );
}

#endif