#ifndef VIEWSHEDLIB_VIEWSHEDANGLE_H
#define VIEWSHEDLIB_VIEWSHEDANGLE_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    namespace visibilityalgorithm
    {
        class ViewAngle : public AbstractViewshedAlgorithm
        {

          public:
            ViewAngle( bool onlyVisible = false, double invisibleValue = -91, double pointValue = -90 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mPointValue; };

            const QString name() override;

          private:
            double mPointValue;
            double mInvisibleValue;
            bool mOnlyVisible;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif