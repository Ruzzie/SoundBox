//
// Created by Dorus on 12-12-2015.
//

#include <stdlib.h>
#include "SimpleRandomSample.h"

unsigned long Sample(const unsigned long currentSeed) {
    return ((currentSeed * RUZZIE_RANDOM_PONE_POW_THREE) - (RUZZIE_RANDOM_PTWO_POW_TWO)) % RUZZIE_RANDOM_MODULO_VALUE;
}

unsigned long NextSample(struct RndData *data) {
    unsigned long retVal = data->CurrentSample;
    data->CurrentSample = Sample(retVal);
    return retVal;
}

int NextRandomInt(const int exclusiveMaximum, struct RndData *data) {
    return (int) (NextSample(data) % (unsigned long) exclusiveMaximum);
}

struct RndData *InitRandom() {
    struct RndData *data = malloc(sizeof(struct RndData));
    data->CurrentSample = 1;
    NextSample(data);
    return data;
}


