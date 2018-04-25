/**
 * @file    common.h
 * @author  James Sturgis
 * @date    April 10, 2018
 * @brief   Some common and useful definitions
 */

#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <cstdlib>
#include <cmath>

#define simple_min(a,b)        (a<b)?(a):(b)
#define simple_max(a,b)        (a>b)?(a):(b)

#define M_2PI           (M_PI+M_PI)
#define rnd_lin(range)  ((range*(double)rand())/(double)RAND_MAX)

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

#endif /* COMMON_H */

