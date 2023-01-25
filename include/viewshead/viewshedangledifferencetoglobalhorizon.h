#ifndef VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H
#define VIEWSHEDLIB_VIEWSHEDANGLEDIFFGLOBAL_H

#include "limits"

#include "iviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedAngleDifferenceToGlobalHorizon : public IViewshedAlgorithm
    {

      public:
        ViewshedAngleDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -181,
                                                double differenceWithoutHorizon = -180 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<ILoS> los ) override;
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