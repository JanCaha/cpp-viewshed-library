#ifndef VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H
#define VIEWSHEDLIB_VIEWSHEDELEVDIFFLOCAL_H

#include "abstractviewshedalgorithm.h"
#include "losimportantvalues.h"

namespace viewshed
{
    class ViewshedElevationDifferenceToLocalHorizon : public AbstractViewshedAlgorithm
    {

      public:
        ViewshedElevationDifferenceToLocalHorizon( bool all = false, double invisibleValue = -9999,
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