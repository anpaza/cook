/* The Cook project
 * Plain C string management and manipulation library
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "str.h"
#include "useful.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* The library is not thread-safe. It's easy to make it so,
 * but would require fiddling with spinlocks and all that
 * non-portable stuff, so this is left as an exercise
 * to the reader. Cook doesn't need thread safety.
 */

// An empty string
static str_t empty_str = STR_INIT_C ("");

/* Decide how much to allocate to store a string.
 * Currently we round allocation size up to nearest larger power of two.
 */
static int str_alloc_size (int size)
{
    if (size)
    {
        // reserve 1 char for zero-termination (str_c())
        uint fls = fls32 ((uint32_t)size + 1);
        if (fls < 5)
            return 1 << 5;
        if (fls < 31)
            return 1 << (fls + 1);
    }

    return 0;
}

/* The global string reference counter pool.
 *
 * str_t.refcnt is a index (offset by 1) into str_ref[],
 * which contains the number of references to a particular string.
 *
 * A string cannot be modified if its reference counter is > 1.
 * If that needs to be done, the strings has to be unshared first.
 */
static int *str_ref = NULL;
static int str_ref_count = 0;

/* Get the shared string reference counter.
 */
int str_getref (int ref)
{
    if (ref == REFCNT_UNUSED)
        return 1;

    assert ((ref > 0) && (ref <= str_ref_count));

    return str_ref [ref - 1];
}

static int str_newref ()
{
    /* @OPT@ A more optimal approach would be to keep the free refcnt's
     * in a chained list. We have to profile it yet to see
     * if that makes sense.
     */
    int ref;
    for (ref = 0; ref < str_ref_count; ref++)
        if (str_ref [ref] == 0)
            goto leave;

    int new_ref_count = (str_ref_count < 256) ? 256 : str_ref_count * 2;
    str_ref = realloc (str_ref, (size_t)new_ref_count * sizeof (str_ref [0]));
    // if realloc returns NULL here, we're in a big trouble...
    assert (str_ref);

    memset (str_ref + str_ref_count, 0,
            sizeof (str_ref [0]) * (size_t)(new_ref_count - str_ref_count));

    ref = str_ref_count;
    str_ref_count = new_ref_count;

leave:
    str_ref [ref] = 1;
    return ref + 1;
}

/**
 * Increment the string reference counter.
 * Allocate a new one if not already.
 * @param idx_p A pointer to shared string data refcnt index.
 * @return the index of the incremented reference counter.
 */
static int str_incref (int *ref_p)
{
    int ref = *ref_p;

    // allocate a new refcnt?
    if (ref == REFCNT_UNUSED)
        *ref_p = ref = str_newref ();

    if (ref == REFCNT_UNUSED)
        return REFCNT_UNUSED;

    assert ((ref > 0) && (ref <= str_ref_count));

    str_ref [ref - 1]++;
    return ref;
}

/**
 * Decrement the shared string reference counter,
 * @param idx The refcnt index.
 * @return the new (decremented) value.
 */
static int str_decref (int ref)
{
    if (ref == REFCNT_UNUSED)
        return 0;

    assert ((ref > 0) && (ref <= str_ref_count));

    return --str_ref [ref - 1];
}

void str_finalize ()
{
    for (int ref = 0; ref < str_ref_count; ref++)
        if (str_ref [ref] != 0)
            fprintf (stderr, "%s: Refcount %u has value %u\n",
                     __FUNCTION__, ref, str_ref [ref]);

    free (str_ref);
    str_ref = NULL;
    str_ref_count = 0;
}

// --------------------------------------------------------------- //

void str_init (str_t *str)
{
    memset (str, 0, sizeof (str_t));
    /*
    str->data = NULL;
    str->size = str->allocated = 0;
    str->refcnt = REFCNT_UNUSED;
    */
}

str_t *str_new ()
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    str_init (ret);

    return ret;
}

void str_init_c_const (str_t *str, const char *cstr, int size)
{
    str->size = (size >= 0) ? size : (cstr ? (int)strlen (cstr) : 0);
    str->allocated = 0;
    // we're not going to break const since (allocated == 0).
    str->data = (char *)cstr;
    str->refcnt = REFCNT_UNUSED;
}

str_t *str_new_c_const (const char *cstr, int size)
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    str_init_c_const (ret, cstr, size);

    return ret;
}

static void str_c_detect (const char *cstr, int *psize, int *pallocated)
{
    int size = *psize;
    int allocated = size;
    if (size < 0)
    {
        if (cstr)
        {
            size = (int)strlen (cstr);
            allocated = size + 1;
        }
        else
            allocated = size = 0;
    }
    else
        while (size && (cstr [size - 1] == '\0'))
            size--;

    *psize = size;
    if (pallocated)
        *pallocated = allocated;
}

void str_init_c_prealloc (str_t *str, char *cstr, int size)
{
    int allocated;
    str_c_detect (cstr, &size, &allocated);

    str->size = size;
    str->allocated = allocated;
    str->data = cstr;
    str->refcnt = REFCNT_UNUSED;
}

str_t *str_new_c_prealloc (char *cstr, int size)
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    str_init_c_prealloc (ret, cstr, size);

    return ret;
}

bool str_init_c_copy (str_t *str, const char *cstr, int size)
{
    str_c_detect (cstr, &size, NULL);

    str->size = size;
    str->allocated = str_alloc_size (size);
    if (str->allocated)
    {
        str->data = malloc ((size_t)str->allocated);
        if (!str->data)
            return false;

        memcpy (str->data, cstr, (size_t)size);
        str->data [size] = '\0';
    }
    else
        str->data = NULL;

    str->refcnt = REFCNT_UNUSED;
    return true;
}

str_t *str_new_c_copy (const char *cstr, int size)
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    if (!str_init_c_copy (ret, cstr, size))
    {
        free (ret);
        return NULL;
    }

    return ret;
}

bool str_init_copy (str_t *str, str_t *copy)
{
    // str_set->str_done will not free anything if allocated == 0
    str->allocated = 0;
    return str_set (str, copy);
}

str_t *str_new_copy (str_t *copy)
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    if (!str_init_copy (ret, copy))
    {
        free (ret);
        return NULL;
    }

    return ret;
}

bool str_init_substr (str_t *str, str_t *src, int pos, int size)
{
    if (pos < 0 || pos >= src->size)
    {
        str_init (str);
        return true;
    }

    if (pos + size > src->size)
        size = src->size - pos;

    // If source string is not allocated, create an unallocated copy
    if (src->allocated == 0)
    {
        str->allocated = 0;
        str->size = size;
        str->data = src->data + pos;
        str->refcnt = REFCNT_UNUSED;
        return true;
    }

    // If the "substring" is really the whole string, do a smart copy
    if (pos == 0 && size == src->size)
        return str_init_copy (str, src);

    // Otherwise we have to make a complete copy
    return str_init_c_copy (str, src->data + pos, size);
}

str_t *str_new_substr (str_t *src, int pos, int size)
{
    str_t *ret = malloc (sizeof (str_t));
    if (!ret)
        return NULL;

    if (!str_init_substr (ret, src, pos, size))
    {
        free (ret);
        return NULL;
    }

    return ret;
}

void str_done (str_t *str)
{
    if (str->allocated)
    {
        int refs = str_decref (str->refcnt);
        if (refs == 0)
            free (str->data);

        str->allocated = 0;
    }

    str->size = 0;
    str->refcnt = REFCNT_UNUSED;
}

void str_free (str_t *str)
{
    if (!str)
        return;

    str_done (str);
    free (str);
}

const char *str_c (const str_t *str)
{
    if (!str->size)
        return "";

    // Check if it is already zero-terminated
    // (also covers static const char* strings)
    if (((str->allocated == 0) ||
         (str->allocated > str->size)) &&
        (str->data [str->size] == 0))
        return str->data;

    // Fine, we can't do anything here since it's a const
    return "";
}

static bool str_unshare_expand (str_t *str, int xsize)
{
    if (!str || (xsize < 0))
        return false;

    if (str_getref (str->refcnt) > 1)
    {
        int allocated = str_alloc_size (str->size + xsize);
        if (allocated)
        {
            char *data = malloc ((size_t)allocated);
            if (!data)
                return false;

            memcpy (data, str->data, (size_t)str->size + 1);
            str->data = data;
        }

        str->allocated = allocated;
        str_decref (str->refcnt);
        str->refcnt = REFCNT_UNUSED;
    }

    return true;
}

bool str_unshare (str_t *str)
{
    return str_unshare_expand (str, 0);
}

bool str_expand (str_t *str, int xsize)
{
    // we have to unshare the string since we're going to modify it
    if (!str_unshare_expand (str, xsize))
        return false;

    // perhaps we already did it
    int allocated = str_alloc_size (str->size + xsize);
    if (allocated <= str->allocated)
        return true;

    if (str->allocated == 0)
    {
        char *data = malloc ((size_t)allocated);
        if (!data)
            return false;

        memcpy (data, str->data, (size_t)str->size);
        data [str->size] = '\0';
        str->data = data;
    }
    else
        str->data = realloc (str->data, (size_t)allocated);

    str->allocated = allocated;

    if (str->data)
        return true;

    // whoops, we lost it
    str->size = 0;
    str->allocated = 0;
    return false;
}

bool str_set (str_t *to, str_t *from)
{
    str_done (to);

    to->size = from->size;
    to->allocated = from->allocated;

    // if string is not allocated, do not allocate the copy
    if (to->allocated == 0)
    {
        to->refcnt = REFCNT_UNUSED;
        to->data = from->data;
    }
    else
    {
        to->refcnt = str_incref (&from->refcnt);

        if (to->refcnt == REFCNT_UNUSED)
            // Sharing failed, do a regular copy
            return str_init_c_copy (to, from->data, from->size);
        else
            to->data = from->data;
    }

    return true;
}

int str_cmp (const str_t *str1, const str_t *str2)
{
    if (str1 == str2)
        return 0;

    if (!str1)
        return -1;
    if (!str2)
        return +1;

    // If strings are shared, the allocated size is always equal
    if (str1->allocated == str2->allocated)
    {
        if (!str1->allocated)
            return 0;

        if (str1->data == str2->data)
            return 0;
    }

    uint ml = umin ((uint)str1->size, (uint)str2->size);
    int res = memcmp (str1->data, str2->data, ml);
    if (res != 0)
        return res;

    return (int)str1->size - (int)str2->size;
}

bool str_insert (str_t *str, int pos, const str_t *insert)
{
    if (!insert || (pos > str->size))
        return false;

    if (insert->size == 0)
        return true;

    // Optimization: appending an adjoined unallocated string to another
    if (!str->allocated && !insert->allocated &&
        (pos == str->size) && (str->data + str->size == insert->data))
    {
        str->size += insert->size;
        return true;
    }

    // Make sure we have at least insert->size extra bytes after data tail
    if (!str_expand (str, insert->size))
        return false;

    // appending or inserting ?
    if (pos < str->size)
        memmove (str->data + pos + insert->size, str->data + pos,
                 (size_t)(str->size + 1 - pos));

    memcpy (str->data + pos, insert->data, (size_t)insert->size);
    str->size += insert->size;
    str->data [str->size] = '\0';

    return true;
}

bool str_append (str_t *str, const str_t *tail)
{
    return str_insert (str, str->size, tail);
}

bool str_append_c_const (str_t *str, const char *cstr, int size)
{
    if (str->size == 0)
    {
        // Optimization: append to empty string
        str_done (str);
        str_init_c_const (str, cstr, size);
        return true;
    }

    int allocated;
    str_c_detect (cstr, &size, &allocated);

    if (cstr == str->data + str->size)
    {
        // Optimization: expand string in-place
        str->size += size;
        if (str->allocated)
            str->allocated += allocated;

        return true;
    }

    if (!str_expand (str, size))
        return false;

    assert (str->allocated > str->size);

    memcpy (str->data + str->size, cstr, (size_t)size);
    str->size += size;
    str->data [str->size] = '\0';

    return true;
}

bool str_replace (str_t *str, int pos, int size, str_t *repl)
{
    if (!repl || (pos > str->size))
        return false;

    if ((pos + size < pos) || (pos + size > str->size))
        size = str->size - pos;

    if (!str_expand (str, repl->size - size))
        return false;

    if (size != repl->size)
        memmove (str->data + pos + repl->size, str->data + pos + size,
                 (size_t)(str->size - pos - size));

    memcpy (str->data + pos, repl->data, (size_t)repl->size);
    str->size = str->size - size + repl->size;
    str->data [str->size] = '\0';

    return true;
}

bool str_delete (str_t *str, int pos, int size)
{
    return str_replace (str, pos, size, &empty_str);
}

bool str_find_char (str_t *str, int *pos, char find)
{
    char *cp = memchr (str->data + *pos, find,
                       (size_t)(str->size - *pos));
    if (!cp)
    {
        *pos = str->size;
        return false;
    }

    *pos = (int)(cp - str->data);

    return true;
}
