#ifndef METRO_H_INCLUDED
#define METRO_H_INCLUDED

#include "event.h"
#include "types.h"

typedef struct Metro *Metro;

Metro
Metro_init(Event e, tempo_t tempo);

int
Metro_start(Metro metro);

int
Metro_stop(Metro metro);

void
Metro_free(Metro *metro);

#endif
