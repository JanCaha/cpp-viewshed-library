#ifndef VIEWSHEDLIB_VIEWSHEDVISIBILITY_H
#define VIEWSHEDLIB_VIEWSHEDVISIBILITY_H

#include "limits"

#include "abstractviewshedalgorithm.h"

namespace viewshed
{
    class ViewshedVisibility : public AbstractViewshedAlgorithm
    {

      public:
        ViewshedVisibility( double visible = 1, double invisible = 0, double pointValue = 1 );

        double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) override;

        double pointValue() override { return mPointValue; };

        const QString name() override;

      private:
        double mVisibile;
        double mInvisibile;
        double mPointValue;
    };
} // namespace viewshed

#endif