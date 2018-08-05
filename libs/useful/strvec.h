/* The Cook project
 * A sortable vector of strings.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __STRVEC_H__
#define __STRVEC_H__

#include "vector.h"
#include "str.h"

/**
 * Initialize a empty vector object, given an estimate of vector size.
 * The vector must contain only str_t* elements, freeable with str_free().
 * @param vec The vector to initialize.
 * @param size Estimated number of elements in the vector.
 * Can be 0 in which case vector will grow as needed.
 */
extern void vector_str_init (vector_t *vec, int size);

/**
 * Create a new vector object, and ensure it may hold at least size
 * elements without memory reallocations.
 * @param size Estimated number of elements in the vector.
 * Can be 0 in which case vector will grow as needed.
 * @return The allocated vector object.
 */
extern vector_t *vector_str_new (int size);

#endif /* __STRVEC_H__ */
