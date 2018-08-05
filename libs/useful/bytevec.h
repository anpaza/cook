/* The Cook project
 * Byte vector
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __bytevec_H__
#define __bytevec_H__

#include "str.h"

/**
 * A dynamically-sized array of bytes.
 * This is based on the str class, but can contain zero bytes.
 */
typedef str_t byte_vector_t;

#define byte_vector_init str_init
#define byte_vector_new  str_new
#define byte_vector_done str_done
#define byte_vector_free str_free

#endif /* __bytevec_H__ */
