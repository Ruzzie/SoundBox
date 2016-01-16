//
// Created by Dorus on 10-12-2015.
//
#include "SimpleRandomSample.h"

#ifndef RANDOM_SIMPLE_RANDOM_H
#define RANDOM_SIMPLE_RANDOM_H
namespace Ruzzie {

    class SimpleRandom {
    public:
        SimpleRandom(void);

        SimpleRandom(const long seed);

        unsigned long Next(const unsigned long exclusiveMaximum);

    private:
        volatile unsigned long currentSample;

        unsigned long NextSample(void);
    };
}
#endif //RANDOM_SIMPLE_RANDOM_H
