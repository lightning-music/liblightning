#ifndef METRO_H_INCLUDED
#define METRO_H_INCLUDED

#include <stdint.h>

#include "types.h"

typedef struct Metro *Metro;

Metro
Metro_init(MetroCallback cb, tempo_t tempo, void *data);

int
Metro_start(Metro metro);

int
Metro_set_tempo(Metro metro, tempo_t tempo);

int
Metro_stop(Metro metro);

void
Metro_free(Metro *metro);

#endif
