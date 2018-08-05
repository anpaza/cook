/* The Cook project
 * Useful macros and definitions
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "useful.h"

unsigned fls32 (uint32_t bits)
{
    unsigned r = 0;
    if (bits & 0xffff0000) { r += 16; bits >>= 16; }
    if (bits & 0x0000ff00) { r +=  8; bits >>=  8; }
    if (bits & 0x000000f0) { r +=  4; bits >>=  4; }
    if (bits & 0x0000000c) { r +=  2; bits >>=  2; }
    if (bits & 0x00000002) { r +=  1;              }
    return r;
}
