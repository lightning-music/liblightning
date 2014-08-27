#ifndef SLAVE_H_INCLUDED
#define SLAVE_H_INCLUDED

#include "event.h"

typedef struct Slave *Slave;

Slave
Slave_init(int divisor);

/**
 * Getter for the slave's event.
 */
Event
Slave_event(Slave slave);

/**
 * Advance the position.
 * Trigger the event if the position is an integer
 * multiple of divisor.
 */
void
Slave_tick(Slave slave);

void
Slave_free(Slave *slave);

#endif
