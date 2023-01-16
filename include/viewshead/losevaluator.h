#ifndef VIEWSHEDLIB_LOSEVALUATOR_H
#define VIEWSHEDLIB_LOSEVALUATOR_H

#include <limits>

#include "iviewshedalgorithm.h"
#include "losimportantvalues.h"
#include "losnode.h"
#include "points.h"
#include "viewshedvalues.h"

namespace viewshed
{
    class LoSEvaluator
    {
      public:
        LoSEvaluator( std::shared_ptr<std::vector<LoSNode>> los,
                      std::vector<std::shared_ptr<IViewshedAlgorithm>> algs );

        void calculate( std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> point );

        void reset();

        int size();
        std::shared_ptr<IViewshedAlgorithm> algorithmAt( int i );
        double resultAt( int i );

        bool mAlreadyParsed = false;

        ViewshedValues mResultValues;
        std::shared_ptr<LoSImportantValues> mLosValues = std::make_shared<LoSImportantValues>();

      private:
        std::shared_ptr<std::vector<LoSNode>> mLos;
        std::vector<std::shared_ptr<IViewshedAlgorithm>> mAlgs;

        void parseNodes( std::shared_ptr<LoSNode> poi );
    };
} // namespace viewshed

#endif