#include <assert.h>
#include <stddef.h>

#include "list.h"
#include "mem.h"
#include "mutex.h"
#include "safe-list.h"

struct SafeList {
    Mutex mutex;
    List list;
};

/**
 * Initialize a new SafeList
 */
SafeList
SafeList_init() {
    SafeList list;
    NEW(list);
    list->mutex = Mutex_init();
    list->list = List_init(NULL);
    return list;
}

/**
 * Push a value onto the end of the list
 */
int
SafeList_push(SafeList l,
              void *x) {
    assert(l);
    int locked = Mutex_lock(l->mutex);
    if (locked) {
        return locked;
    } else {
        List_push(l->list, x);
        return Mutex_unlock(l->mutex);
    }
}

/**
 * Pop a value from the end of the list
 */
int
SafeList_pop(SafeList l,
             void **x) {
    assert(l);
    int locked = Mutex_lock(l->mutex);
    if (locked) {
        return locked;
    } else {
        List_pop(l->list, x);
        return Mutex_unlock(l->mutex);
    }
}

/**
 * Add a value to the front of the list
 */
int
SafeList_shift(SafeList l,
               void *x) {
    assert(l);
    int locked = Mutex_lock(l->mutex);
    if (locked) {
        return locked;
    } else {
        List_shift(l->list, x);
        return Mutex_unlock(l->mutex);
    }
}

/**
 * Pop a value off the front of the list
 */
int
SafeList_unshift(SafeList l,
                 void **x) {
    assert(l);
    int locked = Mutex_lock(l->mutex);
    if (locked) {
        return locked;
    } else {
        List_shift(l->list, x);
        return Mutex_unlock(l->mutex);
    }
}

/**
 * Remove a particular node in the list
 */
int
SafeList_remove(SafeList l,
                void *x) {
    assert(l);
    int locked = Mutex_lock(l->mutex);
    if (locked) {
        return locked;
    } else {
        List_remove(l->list, x);
        return Mutex_unlock(l->mutex);
    }
}

/**
 * Free the resources allocated for a list
 */
void
SafeList_free(SafeList *l) {
    assert(l && *l);
    Mutex_free(&(*l)->mutex);
    List_free(&(*l)->list);
    FREE(*l);
}
