
#include <assert.h>

#include "mem.h"
#include "slave.h"
#include "types.h"

struct Slave {
    Event emitter;
    position_t pos;
    position_t count;
    int divisor;
};

Slave
Slave_init(int divisor)
{
    Slave slave;
    NEW(slave);
    slave->emitter = Event_init();
    slave->pos = 0;
    slave->divisor = divisor;
    return slave;
}

Event
Slave_event(Slave slave)
{
    assert(slave);
    
    return slave->emitter;
}

void
Slave_tick(Slave slave)
{
    assert(slave);
    if (slave->pos % slave->divisor == 0) {\
        slave->count++;
        Event_broadcast(slave->emitter, &slave->count);
        slave->pos = 0;
    } else {
        slave->pos++;
    }
}

/**
 * Hallelujah Amen
 */
void
Slave_free(Slave *slave)
{
    assert(slave && *slave);
    Slave s = *slave;
    Event_free(&s->emitter);
    FREE(*slave);
}
