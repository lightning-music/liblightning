/**
 * doubly linked list
 */
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

typedef int (* Compare)(const void *x, const void *y);

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
List_push(List l,
          void *x);

/**
 * Pop a value from the end of the list
 */
List
List_pop(List l,
         void **x);

/**
 * Add a value to the front of the list
 */
List
List_shift(List l,
           void *x);

/**
 * Pop a value off the front of the list
 */
List
List_unshift(List l,
             void **x);

unsigned
List_length(List l);

/**
 * Remove a particular node in the list
 */
void *
List_remove(List l,
            void *x);

/**
 * Free the resources allocated for a list
 */
void
List_free(List *l);

#endif
