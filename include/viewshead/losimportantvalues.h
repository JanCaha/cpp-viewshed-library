#ifndef VIEWSHEDLIB_LOSVALUES_H
#define VIEWSHEDLIB_LOSVALUES_H

namespace viewshed
{
    struct LoSImportantValues
    {
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

        bool horizonBeforeExist();
        bool horizonExist();
        bool isTargetHorizon();
    };
} // namespace viewshed

#endif