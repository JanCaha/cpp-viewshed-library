#ifndef VIEWSHEDLIB_LOSVALUES_H
#define VIEWSHEDLIB_LOSVALUES_H

namespace viewshed
{
    /**
     * @brief Representation of important values and LoSNode indexes that can be extracted from LoS and are important
     * for Visibility Algorithms.
     *
     */
    struct LoSImportantValues
    {
        /**
         * @brief Construct a new Lo S Important Values object
         *
         */
        LoSImportantValues();

        void reset();

        double mMaxGradientBefore = -180;
        double mMaxGradient = -180;
        int mIndexMaxGradientBefore = -1;
        int mIndexMaxGradient = -1;
        int mIndexHorizonBefore = -1;
        int mIndexHorizon = -1;
        int mCountHorizonBefore = 0;
        int mCountHorizon = 0;
        bool mHorizon = false;

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
    };
} // namespace viewshed

#endif