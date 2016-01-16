//
// Created by Dorus on 10-12-2015.
//

#include "SimpleRandom.h"

namespace Ruzzie {

    SimpleRandom::SimpleRandom(void) : SimpleRandom(1) {

    }

    SimpleRandom::SimpleRandom(const long seed) {
        currentSample = Sample((unsigned long) seed);
    }

    unsigned long SimpleRandom::NextSample(void) {
        unsigned long retVal = currentSample;
        currentSample = Sample(currentSample);
        return retVal;
    }

    unsigned long SimpleRandom::Next(const unsigned long exclusiveMaximum) {
        return NextSample() % exclusiveMaximum;
    }
}