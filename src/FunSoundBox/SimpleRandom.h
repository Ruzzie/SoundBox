//
// Created by Dorus on 10-12-2015.
//
//#include <Arduino.h>
#include "SimpleRandomSample.h"

#ifndef RANDOM_SIMPLE_RANDOM_H
#define RANDOM_SIMPLE_RANDOM_H
namespace Ruzzie {

#define RUZZIE_MAX_VALUE_INT32 2147483647

    class SimpleRandom {
    public:
        SimpleRandom();

        SimpleRandom(const int seed);

        int Next(const int exclusiveMaximum);

        int Next();

        ~SimpleRandom();

    private:
        volatile unsigned long currentSample;

        unsigned long NextSample();
    };
}
#endif //RANDOM_SIMPLE_RANDOM_H
