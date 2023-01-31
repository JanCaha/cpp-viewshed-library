#ifndef VIEWSHEDLIB_ABSTRACTVIEWSHEDALGORITHM_H
#define VIEWSHEDLIB_ABSTRACTVIEWSHEDALGORITHM_H

#include <limits>

#include "QString"

#include "abstractlos.h"
#include "losimportantvalues.h"
#include "losnode.h"
#include "point.h"

namespace viewshed
{
    class AbstractViewshedAlgorithm
    {
      public:
        virtual double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) = 0;
        virtual double pointValue() = 0;
        virtual const QString name() = 0;
    };

} // namespace viewshed

#endif