#ifndef VIEWSHEDLIB_LOSEVALUATOR_H
#define VIEWSHEDLIB_LOSEVALUATOR_H

#include <limits>

#include "iviewshedalgorithm.h"
#include "los.h"
#include "losimportantvalues.h"
#include "losnode.h"
#include "points.h"
#include "viewshedvalues.h"

using viewshed::ILoS;

namespace viewshed
{
    class LoSEvaluator
    {
      public:
        LoSEvaluator( std::shared_ptr<ILoS> los,
                      std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs );

        void calculate();

        void reset();

        int size();
        std::shared_ptr<IViewshedAlgorithm> algorithmAt( int i );
        double resultAt( int i );

        bool mAlreadyParsed = false;

        ViewshedValues mResultValues;
        std::shared_ptr<LoSImportantValues> mLosValues = std::make_shared<LoSImportantValues>();

      private:
        std::shared_ptr<ILoS> mLos;
        std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> mAlgs;

        void parseNodes();
    };
} // namespace viewshed

#endif