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
        int mIndexPoi = 0;
        int mIndexMaxGradientBefore = 0;
        int mIndexMaxGradient = 0;
        int mIndexHorizonBefore = 0;
        int mIndexHorizon = 0;
        int mCountHorizonBefore = 0;
        int mCountHorizon = 0;
    };
} // namespace viewshed

#endif