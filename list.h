/**
 * doubly linked list
 */
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

typedef int (* Compare)(const void *x, const void *y);
typedef void (* ListMap)(void **x, void *data, int index);

typedef struct List *List;

/**
 * Initialize a new list
 */
List
List_init(Compare cmp);

/**
 * Push a value onto the end of the list
 */
List
List_push(List l, void *x);

/**
 * Pop a value from the end of the list
 */
List
List_pop(List l, void **x);

/**
 * Add a value to the front of the list
 */
List
List_shift(List l, void *x);

/**
 * Pop a value off the front of the list
 */
List
List_unshift(List l, void **x);

/**
 * Retreive the value at a particular index.
 * O(n) time complexity, inefficient!
 */
void *
List_at(List l, int index);

unsigned
List_length(List l);

/**
 * Remove a particular node in the list
 */
void *
List_remove(List l, void *x);

/**
 * Map a function over a list
 */
void
List_map(List l, ListMap f, void *data);

/**
 * Free the resources allocated for a list
 */
void
List_free(List *l);

#endif
