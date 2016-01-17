//
// Created by Dorus on 10-12-2015.
//

#include "SimpleRandom.h"

namespace Ruzzie {

    SimpleRandom::SimpleRandom(void) : SimpleRandom(1) {

    }

    SimpleRandom::SimpleRandom(const long seed) {
        currentSample_ = sample((unsigned long) seed);
    }

    unsigned long SimpleRandom::nextSample(void) {
        unsigned long retVal = currentSample_;
        currentSample_ = sample(currentSample_);
        return retVal;
    }

    unsigned long SimpleRandom::next(const unsigned long exclusiveMaximum) {
        return nextSample() % exclusiveMaximum;
    }
}