/* The Cook project
 * Plain C string management and manipulation library
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __STR_H__
#define __STR_H__

#include "useful.h"

/**
 * This object encapsulates a non-zero-terminated string
 * with explicit length and copy-on-write semantics.
 *
 * A zero character is always ensured to be present after
 * the end of the string (except cases when the string
 * is a non-zero-terminated constant).
 */
typedef struct _str_t
{
    /// A pointer to string data
    char *data;
    /// String size in bytes (not characters!)
    int size;
    /// The amount of memory allocated, or 0 if memory was not allocated
    int allocated;
    /// Reference counter is shared by all instances of same string data
    int refcnt;
} str_t;

/// This value says that string is not shared
#define REFCNT_UNUSED	0

/**
 * A simple macro to declare initialized str_t objects.
 * Usage like this: static const str_t x = STR_INIT_C ("blah");
 */
#define STR_INIT_C(s)   { (char *)s, sizeof (s) - 1, 0, REFCNT_UNUSED }

/**
 * Empty string initializer.
 * In fact, filling str_t with zeros using memset is enough.
 */
#define STR_INIT_EMPTY  { NULL, 0, 0, REFCNT_UNUSED }

/**
 * Initialize an empty string.
 *
 * @param str The preallocated string object.
 */
extern void str_init (str_t *str);

/**
 * Create an empty string.
 *
 * @return The created empty string object.
 */
extern str_t *str_new (void);

/**
 * Initialize a string from a const char * reference
 * (e.g. a constant in .text segment). The initialized string object
 * doesn't have to be freed in any way, but if you do, use str_done()
 * rather than str_free().
 *
 * @param str The preallocated string object.
 * @param cstr A pointer to a readonly C string.
 * @param size String size in bytes (not including terminating \0)
 * or -1 to use strlen()
 */
extern void str_init_c_const (str_t *str, const char *cstr, int size);

/**
 * Create a new string from a const char * reference
 * (e.g. a constant in .text segment).
 *
 * @param cstr A pointer to a readonly C string.
 * @param size String size in bytes (not including terminating \0)
 * or -1 to use strlen()
 * @return The allocated string object or NULL if not enough memory
 */
extern str_t *str_new_c_const (const char *cstr, int size);

/**
 * Initialize a string from a preallocated data buffer.
 * The initialized string should be freed with str_done().
 *
 * @param str The preallocated string object.
 * @param cstr The preallocated string data.
 * @param size String size in bytes (not including terminating \0)
 * or -1 to use strlen()
 */
extern void str_init_c_prealloc (str_t *str, char *cstr, int size);

/**
 * Create a new string from a preallocated data buffer.
 *
 * @param cstr A preallocated with malloc() zero-terminated C string.
 * Buffer ownership is passed to the newly-initialized object.
 * @param size String size in bytes or -1 to use strlen()
 * @return The allocated string object or NULL if not enough memory
 */
extern str_t *str_new_c_prealloc (char *cstr, int size);

/**
 * Initialize the string object with a copy of the passed C string.
 *
 * @param str The preallocated string object.
 * @param cstr A pointer to a zero-terminated C string.
 * @param size String size in bytes or -1 to use strlen()
 * @return false if memory allocation failed.
 */
extern bool str_init_c_copy (str_t *str, const char *cstr, int size);

/**
 * Make a copy of the passed C string.
 *
 * @param cstr A pointer to a zero-terminated C string.
 * @param size String size in bytes or -1 to use strlen()
 * @return The allocated string object or NULL if not enough memory
 */
extern str_t *str_new_c_copy (const char *cstr, int size);

/**
 * Initialize a string object with a copy of the passed string.
 * The string data is shared between instances. Unless you
 * don't perform changes to the string, it will stay shared.
 *
 * @param str The string to initialize.
 * @param copy A pointer to a string to copy.
 * @return false if not enough memory.
 */
extern bool str_init_copy (str_t *str, str_t *copy);

/**
 * Make a copy of the passed string.
 * The string data is shared between instances. Unless you
 * don't perform changes to the string, it will stay shared.
 *
 * @param copy A pointer to a string to copy.
 * @return The allocated string object or NULL if not enough memory.
 */
extern str_t *str_new_copy (str_t *copy);

/**
 * Initialize a string with a part of other string.
 * The strings will be shared.
 *
 * @param str The string to initialize.
 * @param src A part of this string will be assigned to @a str.
 * @param pos The byte position of the substring, starting from 0.
 * @param size Substring length in bytes.
 * @return false if not enough memory.
 */
extern bool str_init_substr (str_t *str, str_t *src, int pos, int size);

/**
 * Create a new string, initializing it with a part of other string.
 * The strings will be shared.
 *
 * @param src A part of this string will be assigned to result.
 * @param pos The byte position of the substring, starting from 0.
 * @param size Substring length in bytes.
 * @return The allocated string object or NULL if not enough memory.
 */
extern str_t *str_new_substr (str_t *src, int pos, int size);

/**
 * Finalize the string data, but doesn't free the string structure itself.
 * This is half of the work that str_free does.
 *
 * @param str The string which's data must be freed.
 */
extern void str_done (str_t *str);

/**
 * Free a string object.
 *
 * @param str The string to free.
 */
extern void str_free (str_t *str);

/**
 * Convert a str_t back into a C zero-terminated string.
 * The function will return an empty string if str_t object
 * is not zero-terminated.
 *
 * @param str The string to return a pointer to.
 * @return A standard C char* pointing to a zero-terminated
 * string. The returned string lies within str_t buffer,
 * so caller does not have to free it. The function may
 * return NULL if str was initialized with str_c_const()
 * and is not zero-terminated.
 */
extern const char *str_c (const str_t *str);

/**
 * Make string data non-shared.
 *
 * @param str The string to make non-shared
 * @return false on (memory allocation) failure
 */
extern bool str_unshare (str_t *str);

/**
 * Make sure there are at least @a xsize bytes after the end of the string.
 * If string is shared, it is made non-shared.
 *
 * @param str The string to expand.
 * @param xsize Extra size to allocate.
 * @return
 */
extern bool str_expand (str_t *str, int xsize);

/**
 * Assign one string to another. Usually it will create
 * a shared copy of the source string, so it's cheap.
 *
 * @param to The string to assign to. Old string will be freed.
 * @param from The string to assign from.
 * @return false on memory allocation failure
 */
extern bool str_set (str_t *to, str_t *from);

/**
 * Compare two strings.
 *
 * @param str1 The first string to compare
 * @param str2 The second string to compare
 * @return -1 if str1 is less than str2, 0 if equal, +1 if greater.
 */
extern int str_cmp (const str_t *str1, const str_t *str2);

/**
 * Insert a substring into a string.
 * Take care with multibyte characters, the function works at byte level.
 *
 * @param str The string to insert into
 * @param pos The byte position to insert at, starting from 0.
 * If pos is larger than target string length, function returns false.
 * @param insert The substring to insert
 * @return false on failure (either memory allocation
 * or target position is out of boundaries).
 */
extern bool str_insert (str_t *str, int pos, const str_t *insert);

/**
 * Append a string to another.
 *
 * @param str The string to append to
 * @param tail The tail to append to str
 * @return false on (memory allocation) failure.
 */
extern bool str_append (str_t *str, const str_t *tail);

/**
 * Append a C string to a string object.
 *
 * @param str The string object to append to.
 * @param cstr The C string to append.
 * @param size The size of the C string, in bytes, or -1 for strlen.
 * @return false on (memory allocation) failure.
 */
extern bool str_append_c_const (str_t *str, const char *cstr, int size);

/**
 * Replace a substring with another.
 *
 * @param str The string to modify.
 * @param pos The position where replaced text starts.
 * @param size The size of replaced text in bytes. If size extends past the end
 * of the string, it is truncated to comprise the whole string.
 * @param repl The string that will replace the text.
 * @return false on (memory allocation) failure.
 */
extern bool str_replace (str_t *str, int pos, int size, str_t *repl);

/**
 * Remove a substring from the string.
 *
 * @param str The string to modify.
 * @param pos The position to start deleting at.
 * @param size The size of replaced text in bytes. If size extends past the end
 * of the string, it is truncated to comprise the whole string.
 * @return false on (memory allocation) failure.
 */
extern bool str_delete (str_t *str, int pos, int size);

/**
 * Find a character in input string. This function updates the @a pos
 * paramter no matter if the character was or was not found. If character
 * is not found, pos will be equal to str->size.
 *
 * @param str The input string.
 * @param pos The position to start at and the variable to store
 * the final position.
 * @param find The character to find.
 * @return true if character found, false if end of string hit.
 */
extern bool str_find_char (str_t *str, int *pos, char find);

/**
 * Finalize the string library, freeing global allocated variables.
 * Call this after you have freed all string objects, otherwise it
 * will print warnings regarding hanging string data references.
 */
extern void str_finalize (void);

#endif /* __STR_H__ */
