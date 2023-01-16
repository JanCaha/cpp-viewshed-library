#include "losimportantvalues.h"

using viewshed::LoSImportantValues;

LoSImportantValues::LoSImportantValues() { reset(); }

void LoSImportantValues::reset()
{
    mMaxGradientBefore = -180;
    mMaxGradient = -180;
    mIndexPoi = 0;
    mIndexMaxGradientBefore = 0;
    mIndexMaxGradient = 0;
    mIndexHorizonBefore = 0;
    mIndexHorizon = 0;
    mCountHorizonBefore = 0;
    mCountHorizon = 0;
}