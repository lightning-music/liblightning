/**
 * Table class lifted straight from
 * David R. Hanson - C Interfaces and Implementations
 */
#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

typedef struct Table *Table;

Table
Table_init(int hint,
           int cmp(const void *x, const void *y),
           unsigned hash(const void *key));

int
Table_length(Table t);

void *
Table_put(Table t,
          const void *key,
          void *value);

void *
Table_get(Table t,
          const void *key);

void *
Table_remove(Table t,
             const void *key);

void
Table_map(Table t,
          void apply(const void *key, void **value, void *data),
          void *data);

void **
Table_toArray(Table t,
              void *end);

void
Table_free(Table *t);

#endif
