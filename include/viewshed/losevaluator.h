#ifndef VIEWSHEDLIB_LOSEVALUATOR_H
#define VIEWSHEDLIB_LOSEVALUATOR_H

#include <limits>

#include "abstractlos.h"
#include "abstractviewshedalgorithm.h"
#include "losimportantvalues.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"

using viewshed::AbstractLoS;

namespace viewshed
{
    class LoSEvaluator
    {
      public:
        LoSEvaluator( std::shared_ptr<AbstractLoS> los,
                      std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs );

        void calculate();

        void reset();

        int size();
        std::shared_ptr<AbstractViewshedAlgorithm> algorithmAt( int i );
        double resultAt( int i );

        bool mAlreadyParsed = false;

        ViewshedValues mResultValues;
        std::shared_ptr<LoSImportantValues> mLosValues = std::make_shared<LoSImportantValues>();

        ViewshedValues results();

      private:
        std::shared_ptr<AbstractLoS> mLos;
        std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> mAlgs;
        ViewshedValues mResultValues;

        void parseNodes();
    };
} // namespace viewshed

#endif