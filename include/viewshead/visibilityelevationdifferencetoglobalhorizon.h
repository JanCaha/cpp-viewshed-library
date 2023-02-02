#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFFGLOBAL_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFFGLOBAL_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class VisibilityElevationDifferenceToGlobalHorizon : public AbstractViewshedAlgorithm
    {

      public:
        VisibilityElevationDifferenceToGlobalHorizon( bool all = false, double invisibleValue = -9999,
                                                      double differenceWithoutHorizon = 0, double pointValue = 0 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

        double pointValue() override { return mPointValue; };

        const QString name() override;

      private:
        bool mAllPoints = false;
        double mInvisibleValue;
        double mDifferenceWithoutHorizon;
        double mPointValue;
    };
} // namespace viewshed

#endif