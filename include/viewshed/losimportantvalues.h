#pragma once
#include "viewshed_export.h"

namespace viewshed
{
    /**
     * @brief Representation of important values and LoSNode indexes that can be extracted from LoS and are important
     * for Visibility Algorithms.
     *
     */
    struct DLL_API LoSImportantValues
    {
        /**
         * @brief Construct a new Lo S Important Values object
         *
         */
        LoSImportantValues();

        void reset();

        double maxGradientBefore = -180;
        double maxGradient = -180;
        int indexMaxGradientBefore = -1;
        int indexMaxGradient = -1;
        int indexHorizonBefore = -1;
        int indexHorizon = -1;
        int countHorizonBefore = 0;
        int countHorizon = 0;
        bool targetHorizon = false;
        int targetIndex = -1;

        /**
         * @brief Is there a horizon between view point and target point?
         *
         * @return true
         * @return false
         */
        bool horizonBeforeExist();

        /**
         * @brief Is there a horizon on the LoS?
         *
         * @return true
         * @return false
         */
        bool horizonExist();

        /**
         * @brief Is the target point a horizon?
         *
         * @return true
         * @return false
         */
        bool isTargetHorizon();

        /**
         * @brief Is Target Point global horizon?
         *
         * @return true
         * @return false
         */
        bool isTargetGlobalHorizon();
    };
} // namespace viewshed
