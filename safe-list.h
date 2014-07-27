#ifndef SAFE_LIST_H_INCLUDED
#define SAFE_LIST_H_INCLUDED

/**
 * A list whose reads/writes are protected by a mutex
 */
typedef struct SafeList *SafeList;

/**
 * Initialize a new SafeList
 */
SafeList
SafeList_init();

/**
 * Push a value onto the end of the list
 */
int
SafeList_push(SafeList l,
              void *x);

/**
 * Pop a value from the end of the list
 */
int
SafeList_pop(SafeList l,
             void **x);

/**
 * Add a value to the front of the list
 */
int
SafeList_shift(SafeList l,
               void *x);

/**
 * Pop a value off the front of the list
 */
int
SafeList_unshift(SafeList l,
                 void **x);

/**
 * Remove a particular node in the list
 */
int
SafeList_remove(SafeList l,
                void *x);

/**
 * Free the resources allocated for a list
 */
void
SafeList_free(SafeList *l);

#endif
