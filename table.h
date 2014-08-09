/**
 * Table class lifted straight from
 * David R. Hanson - C Interfaces and Implementations
 */
#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "types.h"

typedef struct Table *Table;

typedef unsigned (* HashFunction)(const void *key);

typedef void (* TableMapFunction)(const void *key, void **value, void *data);

Table
Table_init(int hint, CmpFunction cmp, HashFunction hash);

int
Table_length(Table t);

void *
Table_put(Table t, const void *key, void *value);

void *
Table_get(Table t, const void *key);

void *
Table_remove(Table t, const void *key);

void
Table_map(Table t, TableMapFunction f, void *data);

void **
Table_toArray(Table t, void *end);

void
Table_free(Table *t);

#endif
