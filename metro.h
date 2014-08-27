#ifndef METRO_H_INCLUDED
#define METRO_H_INCLUDED

#include <stdint.h>

#include "event.h"
#include "slave.h"
#include "types.h"

typedef struct Metro *Metro;

Metro
Metro_init(MetroCallback cb, tempo_t tempo, void *data);

int
Metro_start(Metro metro);

int
Metro_set_tempo(Metro metro, tempo_t tempo);

/**
 * Should we return Event here?
 * We want to feed off just 1 timer_t, which ticks
 * n times per second.
 * We want clients to be able to synchronize with either:
 * - Event (they will have to spin up a thread to consume the events)
 * - Callback
 */
Slave
Metro_slave(Metro master, const char *divisor);

int
Metro_stop(Metro metro);

void
Metro_free(Metro *metro);

#endif
