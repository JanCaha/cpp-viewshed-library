#include "losimportantvalues.h"

using viewshed::LoSImportantValues;

LoSImportantValues::LoSImportantValues() { reset(); }

void LoSImportantValues::reset()
{
    maxGradientBefore = -180;
    maxGradient = -180;
    indexMaxGradientBefore = -1;
    indexMaxGradient = -1;
    indexHorizonBefore = -1;
    indexHorizon = -1;
    countHorizonBefore = 0;
    countHorizon = 0;
    targetHorizon = false;
}

bool LoSImportantValues::horizonBeforeExist() { return indexHorizonBefore != -1; }

bool LoSImportantValues::horizonExist() { return indexHorizon != -1; }

bool LoSImportantValues::isTargetHorizon() { return targetHorizon; }

bool LoSImportantValues::isTargetGlobalHorizon()
{
    if ( horizonExist() )
    {
        return indexHorizon == targetIndex;
    }
    return false;
}
