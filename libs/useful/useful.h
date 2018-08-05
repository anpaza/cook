/* The Cook project
 * Useful macros and definitions
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __USEFUL_H__
#define __USEFUL_H__

#include <stdint.h>

/// Get the number of elements in a static array
#define ARRAY_LEN(x)		(sizeof (x) / sizeof (x [0]))

//#ifndef __cplusplus
/// Define the bool type for C
typedef enum { false = 0, true = !false } bool;
//#endif

/**
 * Get the index of highest '1' bit
 * @param bits
 *      The number to look at.
 * @return
 *      Highest set bit index (0..31).
 */
extern unsigned fls32 (uint32_t bits);

/**
 * Return the minimum of two integers
 * @param x first number
 * @param y second number
 * @return the smallest of the two
 */
static inline int imin (int x, int y)
{ return (x <= y) ? x : y; }

/**
 * Return the maximum of two integers
 * @param x first number
 * @param y second number
 * @return the smallest of the two
 */
static inline int imax (int x, int y)
{ return (x >= y) ? x : y; }

/**
 * Return the minimum of two unsigned integers
 * @param x first number
 * @param y second number
 * @return the smallest of the two
 */
static inline unsigned umin (unsigned x, unsigned y)
{ return (x <= y) ? x : y; }

/**
 * Return the maximum of two unsigned integers
 * @param x first number
 * @param y second number
 * @return the smallest of the two
 */
static inline unsigned umax (unsigned x, unsigned y)
{ return (x >= y) ? x : y; }

#endif /* __USEFUL_H__ */
