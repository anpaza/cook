/* The Cook project
 * Universal C vector implementation
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "vector.h"
#include "useful.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/* Decide how many elements to allocate.
 * Currently we round allocation size up to nearest larger power of two.
 */
static int vector_alloc_size (int size)
{
    if (size <= 0)
        return 0;

    unsigned fls = fls32 ((uint32_t)size);

    // don't allocate less than 16 elements as it makes little sense
    if (fls < 3)
        fls = 3;

    return 1 << (fls + 1);
}

void vector_init (vector_t *vec, int size)
{
    assert (vec);

    memset (vec, 0, sizeof (*vec));
    vector_allocate (vec, size);
}

vector_t *vector_new (int size)
{
    vector_t *vec = malloc (sizeof (vector_t));
    if (vec)
        vector_init (vec, size);

    return vec;
}

void vector_clear (vector_t *vec)
{
    assert (vec);

    if (vec->vmt && vec->vmt->free)
        for (int i = 0; i < vec->size; i++)
            vec->vmt->free (vec->data [i]);
    vec->size = 0;
}

void vector_done (vector_t *vec)
{
    assert (vec);

    vector_clear (vec);

    if (vec->allocated)
    {
        free (vec->data);
        vec->allocated = 0;
    }
}

void vector_free (vector_t *vec)
{
    if (vec)
    {
        vector_done (vec);
        free (vec);
    }
}

bool vector_allocate (vector_t *vec, int size)
{
    assert (vec);

    int allocated = vector_alloc_size (size);
    if (allocated > vec->allocated)
    {
        vec->data = realloc (vec->data,
                             (size_t)allocated * sizeof (vec->data [0]));
        if (!vec->data)
        {
            // whoops, memory allocation failed
            vec->size = 0;
            vec->allocated = 0;
            return false;
        }

        vec->allocated = allocated;
    }

    return true;
}

static inline bool vector_expand (vector_t *vec, int xsize)
{
    // we're not going to shrink it
    if (xsize < 0)
        return true;

    return vector_allocate (vec, vec->size + xsize);
}

void *vector_get (vector_t *vec, int pos)
{
    assert (vec);

    if ((pos < 0) || (pos >= vec->size))
        return NULL;

    return vec->data [pos];
}

bool vector_set (vector_t *vec, int pos, void *item)
{
    assert (vec);

    if ((pos < 0) || (pos > vec->size))
        return false;

    if (pos == vec->size)
    {
        if (!vector_expand (vec, 1))
            return false;

        vec->size = pos + 1;
    }
    else
    {
        if (vec->data [pos] && vec->vmt && vec->vmt->free)
            vec->vmt->free (vec->data [pos]);
    }

    vec->data [pos] = item;

    return true;
}

bool vector_insert_one (vector_t *vec, int pos, void *item)
{
    assert (vec);

    if ((pos < 0) || (pos > vec->size))
        return false;

    // ensure we have enough space in vector
    if (!vector_expand (vec, 1))
        return false;

    // push aside old elements
    memmove (vec->data + pos + 1, vec->data + pos,
             (size_t)(vec->size - pos) * sizeof (vec->data [0]));

    vec->data [pos] = item;
    vec->size++;

    return true;
}

bool vector_append (vector_t *vec, void *item)
{
    return vector_insert_one (vec, vec->size, item);
}

void *vector_pop (vector_t *vec)
{
    assert (vec->size);

    void *ret = vec->data [vec->size - 1];
    vec->size--;

    return ret;
}

bool vector_join (vector_t *vec_to, vector_t *vec_from)
{
    assert (vec_to);
    assert (vec_from);
    assert (vec_to != vec_from);

    // ensure we have enough space in vector
    if (!vector_expand (vec_to, vec_from->size))
        return false;

    memcpy (vec_to->data, vec_from->data,
            (size_t)vec_from->size * sizeof (vec_to->data [0]));
    vec_from->size = 0;

    return true;
}

bool vector_insert (vector_t *vec, int pos, ...)
{
    assert (vec);

    int i, count = 0;
    va_list args;

    if ((pos < 0) || (pos > vec->size))
        return false;

    // count number of arguments
    va_start (args, pos);
    while (va_arg (args, void *))
        count++;
    va_end (args);

    // ensure we have enough space in vector
    if (!vector_expand (vec, count))
        return false;

    // push aside old elements
    memmove (vec->data + pos + count, vec->data + pos,
             (size_t)(vec->size - pos) * sizeof (vec->data [0]));
    vec->size += count;

    // put new elements into vector
    va_start (args, pos);
    for (i = 0; i < count; i++)
        vec->data [pos + i] = va_arg (args, void *);
    va_end (args);

    return true;
}

bool vector_delete (vector_t *vec, int pos, int count)
{
    assert (vec);

    int i;

    if ((pos < 0) || (pos >= vec->size) || (pos + count > vec->size))
        return false;

    if (vec->vmt && vec->vmt->free)
        for (i = 0; i < count; i++)
            vec->vmt->free (vec->data [pos + i]);

    memmove (vec->data + pos, vec->data + pos + count,
             (size_t)(vec->size - (pos + count)) * sizeof (vec->data [0]));
    vec->size -= count;

    return true;
}

static inline void vector_exchange_i (vector_t *vec, int pos1, int pos2)
{
    void *tmp = vec->data [pos1];
    vec->data [pos1] = vec->data [pos2];
    vec->data [pos2] = tmp;
}

bool vector_exchange (vector_t *vec, int pos1, int pos2)
{
    assert (vec);

    if ((pos1 >= vec->size) || (pos2 >= vec->size))
        return false;

    vector_exchange_i (vec, pos1, pos2);

    return true;
}

static void vector_qsort_i (vector_t *vec, int left, int right)
{
    int i, j, x;

recurse:
    i = left;
    j = right;
    x = (left + right) / 2;
    do
    {
        while ((i != x) && (vec->vmt->compare (vec->data [i], vec->data [x]) < 0))
            i++;
        while ((j != x) && (vec->vmt->compare (vec->data [j], vec->data [x]) > 0))
            j--;

        if (i < j)
        {
            // we're guaranteed i and j are within vector boundaries
            vector_exchange_i (vec, i, j);

            if (x == i)
                x = j;
            else if (x == j)
                x = i;
        }

        if (i <= j)
        {
            i++;
            if (j > left)
                j--;
        }
    } while (i <= j);

    if (j - left < right - i)
    {
        if (left < j)
            vector_qsort_i (vec, left, j);

        if (i < right)
        {
            left = i;
            goto recurse;
        }
    }
    else
    {
        if (i < right)
            vector_qsort_i (vec, i, right);

        if (left < j)
        {
            right = j;
            goto recurse;
        }
    }
}

bool vector_qsort (vector_t *vec)
{
    assert (vec);

    if (!vec->vmt || !vec->vmt->compare)
        return false;

    if (vec->size)
        vector_qsort_i (vec, 0, vec->size - 1);

    return true;
}

int vector_insert_sorted (vector_t *vec, void *item)
{
    assert (vec);

    if (!vec->vmt || !vec->vmt->compare)
        return -1;

    int m = 0, l = 0, r = vec->size - 1;
    while (l <= r)
    {
        m = (l + r) / 2;

        int cmp = vec->vmt->compare (vec->data [m], item);
        if (cmp == 0)
        {
            vector_insert_one (vec, m, item);
            return m;
        }
        else if (cmp < 0)
            l = m + 1;
        else
            r = m - 1;
    }

    if (r == m)
        m++;

    vector_insert_one (vec, m, item);
    return m;
}

int vector_find_sorted (vector_t *vec, void *item)
{
    assert (vec);

    if (!vec->vmt || !vec->vmt->compare)
        return -1;

    int l = 0, r = vec->size - 1;
    while (l <= r)
    {
        int m = (l + r) / 2;
        int cmp = vec->vmt->compare (vec->data [m], item);

        if (cmp == 0)
            return m;
        else if (cmp < 0)
            l = m + 1;
        else
            r = m - 1;
    }

    return -1;
}

int vector_find_sorted_key (vector_t *vec, void *key)
{
    assert (vec);

    if (!vec->vmt || !vec->vmt->compare_key)
        return -1;

    int l = 0, r = vec->size - 1;
    while (l <= r)
    {
        int m = (l + r) / 2;
        int cmp = vec->vmt->compare_key (vec->data [m], key);

        if (cmp == 0)
            return m;
        else if (cmp < 0)
            l = m + 1;
        else
            r = m - 1;
    }

    return -1;
}
