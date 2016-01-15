//
// Created by Dorus on 10-12-2015.
//

#include "SimpleRandom.h"

namespace Ruzzie {

    SimpleRandom::SimpleRandom() : SimpleRandom(1) {

    }

    SimpleRandom::SimpleRandom(const int seed) {
        currentSample = Sample((unsigned long) seed);
    }

    SimpleRandom::~SimpleRandom() {

    }

    unsigned long SimpleRandom::NextSample() {
        unsigned long retVal = currentSample;
        currentSample = Sample(currentSample);
        return retVal;
    }

    int SimpleRandom::Next() {
        return Next(RUZZIE_MAX_VALUE_INT32);
    }

    int SimpleRandom::Next(const int exclusiveMaximum) {
        return (int) (NextSample() % (unsigned long) exclusiveMaximum);
    }
}