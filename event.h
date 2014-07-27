#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

typedef struct Event *Event;

Event
Event_init(void *val);

int
Event_wait(Event e);

int
Event_timedwait(Event e,
                long ns);

int
Event_signal(Event e);

int
Event_broadcast(Event e);

void *
Event_value(Event e);

void
Event_free(Event *e);

#endif
