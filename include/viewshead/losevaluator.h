#ifndef VIEWSHEDLIB_LOSEVALUATOR_H
#define VIEWSHEDLIB_LOSEVALUATOR_H

#include <limits>

#include "iviewshedalgorithm.h"
#include "points.h"
#include "losnode.h"
#include "viewshedvalues.h"

namespace viewshed
{
    class LoSEvaluator
    {
      public:
        void calculate( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs, std::vector<LoSNode> &statusNodes,
                        std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> point );
        void reset();

        int size();
        std::shared_ptr<IViewshedAlgorithm> algorithmAt( int i );
        double resultAt( int i );

        double mMaxGradientBefore = -180;
        double mMaxGradient = -180;
        int mIndexPoi = 0;
        int mIndexMaxGradientBefore = 0;
        int mIndexMaxGradient = 0;
        int mIndexHorizonBefore = 0;
        int mIndexHorizon = 0;
        int mCountHorizonBefore = 0;
        int mCountHorizon = 0;

        bool mAlreadyParsed = false;

        ViewshedValues mResultValues;

      private:
        std::vector<std::shared_ptr<IViewshedAlgorithm>> mAlgs;

        void parseNodes( std::vector<LoSNode> &statusNodes, std::shared_ptr<LoSNode> poi );
    };
} // namespace viewshed

#endif