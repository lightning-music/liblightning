#include <assert.h>
#include <limits.h>
#include <stddef.h>

#include "mem.h"
#include "table.h"

struct Table {
    int size;
    int (* cmp)(const void *x, const void *y);
    unsigned (* hash)(const void *key);
    int length;
    unsigned timestamp;
    struct binding {
        struct binding *link;
        const void *key;
        void *value;
    } **buckets;
};

static int
cmpatom(const void *x, const void *y) {
    return x != y;
}

static unsigned
hashatom(const void *key) {
    return (unsigned long) key >> 2;
}

Table
Table_init(int hint,
           int cmp(const void *x, const void *y),
           unsigned hash(const void *key)) {
    Table t;
    int i;

    static int primes[] = {
        509, 509, 1021, 2053,
        4093, 8191, 16381, 32771, 65521, INT_MAX
    };

    assert(hint >= 0);
    for (i = 1; primes[i] < hint; i++) ;
    t = ALLOC(sizeof(*t) + primes[i - 1] * sizeof(t->buckets[0]));
    t->size = primes[i - 1];
    t->cmp = cmp ? cmp : cmpatom;
    t->hash = hash ? hash : hashatom;
    t->buckets = (struct binding **)(t + 1);
    for (i = 0; i < t->size; i++)
        t->buckets[i] = NULL;
    t->length = 0;
    t->timestamp = 0;

    return t;
}

int
Table_length(Table t) {
    assert(t);
    return t->length;
}

void *
Table_put(Table t,
          const void *key,
          void *value) {
    int i;
    struct binding *p;
    void *prev;
    assert(t);
    assert(key);
    i = (*t->hash)(key) % t->size;
    for (p = t->buckets[i]; p; p = p->link)
        if ((*t->cmp)(key, p->key) == 0)
            break;
    if (p == NULL) {
        NEW(p);
        p->key = key;
        p->link = t->buckets[i];
        t->buckets[i] = p;
        t->length++;
        prev = NULL;
    } else {
        prev = p->value;
    }
    p->value = value;
    t->timestamp++;
    return prev;
}

void *
Table_get(Table t,
          const void *key) {
    int i;
    struct binding *p;
    assert(t);
    assert(key);
    i = (*t->hash)(key) % t->size;
    for (p = t->buckets[i]; p; p = p->link)
        if ((*t->cmp)(key, p->key) == 0)
            break;
    return p ? p->value : NULL;
}

void *
Table_remove(Table t,
             const void *key) {
    int i;
    struct binding **pp;
    assert(t);
    assert(key);
    t->timestamp++;
    i = (*t->hash)(key) % t->size;
    for (pp = &t->buckets[i]; *pp; pp = &(*pp)->link)
        if ((*t->cmp)(key, (*pp)->key) == 0) {
            struct binding *p = *pp;
            void *value = p->value;
            *pp = p->link;
            FREE(p);
            t->length--;
            return value;
        }
    return NULL;
}

void
Table_map(Table t,
          void apply(const void *key, void **value, void *data),
          void *data) {
    int i;
    unsigned stamp;
    struct binding *p;

    assert(t);
    assert(apply);
    stamp = t->timestamp;
    for (i = 0; i < t->size; i++)
        for (p = t->buckets[i]; p; p = p->link) {
            apply(p->key, &p->value, data);
            assert(t->timestamp == stamp);
        }
}

void **
Table_toArray(Table t,
              void *end) {
    int i, j = 0;
    void **array;
    struct binding *p;
    assert(t);
    array = ALLOC((2 * t->length + 1) * sizeof(*array));
    for (i = 0; i < t->size; i++)
        for (p = t->buckets[i]; p; p = p->link) {
            array[j++] = (void *)p->key;
            array[j++] = p->value;
        }
    array[j] = end;
    return array;
}

void
Table_free(Table *t) {
    assert(t && *t);
    if ((*t)->length > 0) {
        int i;
        struct binding *p, *q;
        for (i = 0; i < (*t)->size; i++)
            for (p = (*t)->buckets[i]; p; p = q) {
                q = p->link;
                FREE(p);
            }
    }
    FREE(*t);
}
