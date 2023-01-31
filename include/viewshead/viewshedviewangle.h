#ifndef VIEWSHEDLIB_VIEWSHEDANGLE_H
#define VIEWSHEDLIB_VIEWSHEDANGLE_H

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedViewAngle : public AbstractViewshedAlgorithm
    {

      public:
        ViewshedViewAngle( double pointValue = 180 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

        double pointValue() override { return mPointValue; };

        const QString name() override;

      private:
        double mPointValue;
    };
} // namespace viewshed

#endif