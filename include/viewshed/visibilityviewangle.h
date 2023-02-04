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
            ViewAngle( double pointValue = 180 );

            double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

            double pointValue() override { return mPointValue; };

            const QString name() override;

          private:
            double mPointValue;
        };
    } // namespace visibilityalgorithm
} // namespace viewshed

#endif