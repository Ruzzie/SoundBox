//
// Created by Dorus on 12-12-2015.
//

#ifndef RANDOM_SIMPLE_RANDOM_SAMPLE_H
#define RANDOM_SIMPLE_RANDOM_SAMPLE_H

#define RUZZIE_RANDOM_MODULO_VALUE 2716594357
#define RUZZIE_RANDOM_PONE_POW_THREE 2529729720263398577
#define RUZZIE_RANDOM_PTWO_POW_TWO 743585891622490225

#ifdef __cplusplus
extern "C" {
#endif
struct RndData{
    unsigned long CurrentSample;
};
extern unsigned long Sample(const unsigned long currentSeed);
extern int NextRandomInt(const int exclusiveMaximum, struct RndData *data);
extern struct RndData *InitRandom();
#ifdef __cplusplus
}
#endif
#endif //RANDOM_SIMPLE_RANDOM_SAMPLE_H
