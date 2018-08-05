/* The Cook project
 * Direct drop-in replacement for standard C library functions,
 * a simple portable repeatable random number generator implementation.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "useful.h"

/*
 * Xorshift RNG by George Marsaglia.
 * https://en.wikipedia.org/wiki/Xorshift
 */

static unsigned state [5];

void srand (unsigned seed)
{
    for (int i = 0; i < ARRAY_LEN (state); i++)
        state [i] = seed + i;
}

int rand ()
{
    uint32_t s, t;

    /* Algorithm "xorwow" from p. 5 of Marsaglia, "Xorshift RNGs" */

    t = state [3];
    t ^= t >> 2;
    t ^= t << 1;
    state [3] = state [2];
    state [2] = state [1];
    s = state [1] = state [0];
    t ^= s;
    t ^= s << 4;
    state [0] = t;

    return t + (state [4] += 362437);
}
