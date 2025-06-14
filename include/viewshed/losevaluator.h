#pragma once
#include "viewshed_export.h"

#include <limits>
#include <memory>
#include <vector>

#include "losimportantvalues.h"
#include "viewshedtypes.h"
#include "viewshedvalues.h"

using viewshed::ViewshedAlgorithms;

namespace viewshed
{
    class AbstractLoS;
    class AbstractViewshedAlgorithm;

    /**
     * @brief Class that takes care of analysing AbstractLoS with respect to given AbstractViewshedAlgorithms, the
     * result can be obtained from
     *
     */
    class DLL_API LoSEvaluator
    {
      public:
        LoSEvaluator( std::shared_ptr<AbstractLoS> los, std::shared_ptr<ViewshedAlgorithms> visibilityIndices );

        /**
         * @brief Calculate results of LoS evaluation for every algorithms.
         *
         */
        void calculate();

        /**
         * @brief Resets outcomes.
         *
         */
        void reset();

        /**
         * @brief Number of algorithms.
         *
         * @return int
         */
        int size();

        /**
         * @brief Algorithm at specific index.
         *
         * @param i
         * @return std::shared_ptr<AbstractViewshedAlgorithm>
         */
        std::shared_ptr<AbstractViewshedAlgorithm> algorithmAt( int i );

        /**
         * @brief Algorithm result at specific index.
         *
         * @param i
         * @return double
         */
        double resultAt( int i );

        bool mAlreadyParsed = false;

        std::shared_ptr<LoSImportantValues> mLosValues = std::make_shared<LoSImportantValues>();

        /**
         * @brief Results of the algorithms.
         *
         * @return ViewshedValues
         */
        ViewshedValues results();

      private:
        std::shared_ptr<AbstractLoS> mLos;
        std::shared_ptr<ViewshedAlgorithms> mVisibilityIndices;
        ViewshedValues mResultValues;

        void parseNodes();
    };
} // namespace viewshed
