//
// Created by Dorus on 12-12-2015.
//

#include "SimpleRandomSample.h"

unsigned long sample(const unsigned long currentSeed) {
    return (unsigned long) (((currentSeed * RUZZIE_RANDOM_PONE_POW_THREE) - (RUZZIE_RANDOM_PTWO_POW_TWO)) % RUZZIE_RANDOM_MODULO_VALUE);
}