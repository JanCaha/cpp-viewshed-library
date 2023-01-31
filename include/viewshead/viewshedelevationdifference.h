#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFF_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFF_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedElevationDifference : public AbstractViewshedAlgorithm
    {

      public:
        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

        double pointValue() override;

        const QString name() override;
    };
} // namespace viewshed

#endif