build/src/bin/viewshed \
    --dem tests/data/dsm.tif \
    --resultsFolder "tests/data/results" --observerPosition "-336428.767;-1189102.785" \
    --heightObserver "1.6" \
    --refractionCoefficient "0.142860" \
    --useCurvatureCorrections "true" \
    --earthDiameter "12754794.310000" \
    --visibilityMask tests/data/visiblity_mask.tif

build/src/bin/inverseviewshed \
    --dem "tests/data/dsm.tif" \
    --resultsFolder "tests/data/results" --targetPosition "-336428.767;-1189102.785" \
    --heightObserver "1.6" \
    --heightTarget "0.0" \
    --refractionCoefficient "0.142860" \
    --useCurvatureCorrections "true" \
    --earthDiameter "12754794.310000" \
    --visibilityMask tests/data/visiblity_mask.tif