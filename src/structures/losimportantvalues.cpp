#include "losimportantvalues.h"

using viewshed::LoSImportantValues;

LoSImportantValues::LoSImportantValues() { reset(); }

void LoSImportantValues::reset()
{
    mMaxGradientBefore = -180;
    mMaxGradient = -180;
    mIndexPoi = -1;
    mIndexMaxGradientBefore = -1;
    mIndexMaxGradient = -1;
    mIndexHorizonBefore = -1;
    mIndexHorizon = -1;
    mCountHorizonBefore = 0;
    mCountHorizon = 0;
}

bool LoSImportantValues::horizonBeforeExist() { return mIndexHorizonBefore != -1; }

bool LoSImportantValues::horizonExist() { return mIndexHorizon != -1; }
