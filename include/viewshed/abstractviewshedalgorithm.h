#pragma once
#include "viewshed_export.h"

#include <limits>
#include <string>

#include "abstractlos.h"
#include "losimportantvalues.h"
#include "losnode.h"
#include "point.h"

namespace viewshed
{
    /**
     * @brief Base class for all Viewshed Algorithms.
     *
     */
    class DLL_API AbstractViewshedAlgorithm
    {
      public:
        /**
         * @brief Extract resulting value based on LoSImportantValues and specific los.
         *
         * @param losValues
         * @param los
         * @return double
         */
        virtual double result( std::shared_ptr<LoSImportantValues> losValues, std::shared_ptr<AbstractLoS> los ) = 0;

        /**
         * @brief Value at important point, either view point for viewshed or target point for inverse viewshed.
         *
         * @return double
         */
        virtual double pointValue() = 0;

        /**
         * @brief Name of the algorithm.
         *
         * @return const std::string
         */
        virtual const std::string name() = 0;
    };

} // namespace viewshed
