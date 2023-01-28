#ifndef VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H
#define VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H

#include "limits"

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedAngleDifferenceToGlobalHorizon : public AbstractViewshedAlgorithm
    {

      public:
        ViewshedAngleDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -181,
                                                double differenceWithoutHorizon = -180 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;
        const double viewpointValue() override;
        const double invisible() override;
        const double completlyVisible() override;
        const QString name() override;

      private:
        bool mAllPoints = false;
        double mInvisibleValue;
        double mDifferenceWithoutHorizon;
    };
} // namespace viewshed

#endif