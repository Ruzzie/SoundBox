//
// Created by Dorus on 12-12-2015.
//

#ifndef RANDOM_SIMPLE_RANDOM_SAMPLE_H
#define RANDOM_SIMPLE_RANDOM_SAMPLE_H

#define RUZZIE_RANDOM_MODULO_VALUE 3121041941
#define RUZZIE_RANDOM_PONE_POW_THREE 4495139485377102241
#define RUZZIE_RANDOM_PTWO_POW_TWO 908196443315401744

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long sample(const unsigned long currentSeed);

#ifdef __cplusplus
}
#endif
#endif //RANDOM_SIMPLE_RANDOM_SAMPLE_H
