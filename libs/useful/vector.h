/* The Cook project
 * Universal C vector implementation
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "useful.h"

/**
 * This table contains pointers to some functions
 * needed by vector class to implement some extra functionality
 * (automatic data freeing, sorting, binary search).
 */
typedef struct
{
    /**
     * Free a element of the vector.
     * @param item The element to free.
     */
    void (*free) (void *item);

    /**
     * Compare two elements of the array.
     */
    int (*compare) (const void *item1, const void *item2);

    /**
     * Compare element array with a search key.
     * The key must be of same type which is used
     * to sort the array.
     */
    int (*compare_key) (const void *item, const void *key);
} vector_vmt_t;

/**
 * A linear array with pointers to any types of objects.
 * The vector can be sorted, in which case it can be used
 * for quick lookup of vector elements.
 *
 * A vector will only grow, not shrink, even if you remove
 * items from it. The cost of reallocation is substantially
 * higher than the cost of some unused pointers.
 */
typedef struct
{
    /// Virtual method table or NULL for simple vectors
    const vector_vmt_t *vmt;
    /// Array of pointers to array elements
    void **data;
    /// Number of allocated pointers
    int allocated;
    /// Number of actually used pointers
    int size;
} vector_t;

/// Empty vector initializer
#define VECTOR_INIT_EMPTY   { NULL, NULL, 0, 0 }

/**
 * Initialize a empty vector object, given an estimate of vector size.
 * When done, free it with vector_done().
 *
 * @param vec The vector to initialize.
 * @param size Estimated number of elements in the vector.
 *     Can be 0 in which case vector will grow as needed.
 */
extern void vector_init (vector_t *vec, int size);

/**
 * Create and initialize a empty vector object, given an estimate of
 * vector size. When done, free it with vector_free().
 *
 * @param size Estimated number of elements in the vector.
 *     Can be 0 in which case vector will grow as needed.
 * @return A newly allocated empty vector.
 */
extern vector_t *vector_new (int size);

/**
 * Free all elements of the vector (using the 'free' method, if it is defined)
 * and set element count to 0.
 *
 * @param vec The vector to clear.
 */
extern void vector_clear (vector_t *vec);

/**
 * Free all elements of the vector, and internal data of a vector object.
 * The object itself is not freed, use vector_free if you want this.
 * The vector may be reused, you may either vector_init first, or just
 * directly use it.
 *
 * @param vec The vector to free.
 */
extern void vector_done (vector_t *vec);

/**
 * Free internal data of a vector object and free the structure itself.
 *
 * @param vec The vector to free.
 */
extern void vector_free (vector_t *vec);

/**
 * Ensure vector is large enough to hold at least size elements.
 * The actual size of the vector is not modified.
 * If currently vector has allocated more than requested, nothing is done.
 *
 * @param vec The vector to resize.
 * @param size The expected vector size.
 * @return false if memory allocation failed.
 */
extern bool vector_allocate (vector_t *vec, int size);

/**
 * Get a element of the vector.
 *
 * @param vec The vector to extract the element from.
 * @param pos The position of the element.
 * @return The element or NULL if pos is out of bounds.
 */
extern void *vector_get (vector_t *vec, int pos);

/**
 * Set a element of the vector. If there's already an elemnt at pos,
 * it is freed via the 'free' method, if defined. If pos is equal to
 * vector size, the vector is enlengthed by one.
 *
 * @param vec The vector to put the element into.
 * @param pos The position to put the element at.
 * @param item The element to put into the vector.
 * @return false if pos is out of bounds.
 */
extern bool vector_set (vector_t *vec, int pos, void *item);

/**
 * Insert elements at given position.
 * The list of elements must be NULL-terminated.
 * This means you can't insert NULL elements with this function.
 *
 * @param vec The vector to insert into.
 * @param pos The position to insert at.
 * @return false if insertion failed for some reason
 *     (wrong pos or memory allocation failed).
 */
extern bool vector_insert (vector_t *vec, int pos, ...);

/**
 * Insert a single element at given position.
 *
 * @param vec The vector to insert into.
 * @param pos The position to insert at.
 * @param item The item to insert.
 * @return false if operation failed for some reason
 *     (wrong pos or memory allocation failed).
 */
extern bool vector_insert_one (vector_t *vec, int pos, void *item);

/**
 * Append a item to the vector.
 * @param vec The vector to append to.
 * @param item The item to append.
 * @return false if operation failed for some reason
 *     (wrong pos or memory allocation failed).
 */
extern bool vector_append (vector_t *vec, void *item);

/**
 * Pop a item from the tail of vector.
 * The item is not freed, the calling code becomes item owner.
 *
 * @param vec The vector to remove last item from.
 * @return The last item from the vector.
 */
extern void *vector_pop (vector_t *vec);

/**
 * Append all elements of one vector to another.
 * No copy is made, all elements are moved from one array to another.
 * On exit, the source vector remains empty.
 *
 * @param vec_to The array to append elements to.
 * @param vec_from The array with elements to append.
 * @return false on memory allocation failure.
 */
extern bool vector_join (vector_t *vec_to, vector_t *vec_from);

/**
 * Delete some elements of the vector. Elements are freed using the 'free'
 * method, if it is defined.
 *
 * @param vec The vector to delete from.
 * @param pos The position to start deleting at.
 * @param count The amount of elements to delete.
 * @return false if deletion failed (wrong pos or too big count).
 */
extern bool vector_delete (vector_t *vec, int pos, int count);

/**
 * Exchange places of two vector elements.
 *
 * @param vec The vector in which to exchange elements.
 * @param pos1 The position of 1st element.
 * @param pos2 The position of 2nd element.
 * @return false if position of any element is out of bounds.
 */
extern bool vector_exchange (vector_t *vec, int pos1, int pos2);

/**
 * Sort the vector using the quicksort algorithm.
 * This uses the 'compare' method which must be defined.
 *
 * @param vec The vector to sort.
 * @return false if vector sorting failed.
 */
extern bool vector_qsort (vector_t *vec);

/**
 * Insert a element into a sorted vector, maintaining sorting order.
 * If vector contains equal elements, the position at which insertion
 * will be done is not steady (it may be before or after equal elements).
 * This uses the 'compare' method which must be defined.
 *
 * @param vec The vector to insert into.
 * @param item The item to insert.
 * @return The index at which item was inserted, or -1 on error.
 */
extern int vector_insert_sorted (vector_t *vec, void *item);

/**
 * Quickly find a element in a sorted vector using binary search.
 *
 * @param vec The vector to search in.
 * @param item The item to look for.
 * @return The index of the item found or -1 if not found.
 */
extern int vector_find_sorted (vector_t *vec, void *item);

/**
 * Find a element in a sorted vector using binary search by key.
 *
 * @param vec The vector to search in.
 * @param key The key that identifies the record we're looking for.
 * @return The index of the item found or -1 if not found.
 */
extern int vector_find_sorted_key (vector_t *vec, void *key);

#endif /* __VECTOR_H__ */
