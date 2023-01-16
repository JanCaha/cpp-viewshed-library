#ifndef VIEWSHEDLIB_IVIEWSHEDALGORITHM_H
#define VIEWSHEDLIB_IVIEWSHEDALGORITHM_H

#include <limits>

#include "QString"

#include "losimportantvalues.h"
#include "losnode.h"
#include "points.h"

namespace viewshed
{
    class IViewshedAlgorithm
    {
      public:
        virtual double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<std::vector<LoSNode>> los,
                               std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> vp ) = 0;
        virtual const double viewpointValue() = 0;
        virtual const double invisible() = 0;
        virtual const double completlyVisible() = 0;
        virtual const QString name() = 0;
    };

} // namespace viewshed

#endif