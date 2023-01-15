#ifndef VIEWSHEDLIB_THREADTASK_H
#define VIEWSHEDLIB_THREADTASK_H

#include "enums.h"
#include "points.h"
#include "position.h"
#include "statusnode.h"
#include "viewshed.h"

namespace viewshed
{
    ViewshedValues evaluateLoSForPoI( ViewshedAlgorithms algs, std::vector<StatusNode> statusList,
                                      std::shared_ptr<StatusNode> poi, std::shared_ptr<IPoint> point );
}

#endif