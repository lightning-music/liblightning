#include <assert.h>
#include <pthread.h>
#include <stddef.h>

#include "event.h"
#include "mem.h"

typedef enum {
    EventState_NotReady,
    EventState_Ready
} EventState;

struct Event {
    void *val;
    EventState state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

Event
Event_init(void *val) {
    Event e;
    NEW(e);
    pthread_mutex_init(&e->mutex, NULL);
    pthread_cond_init(&e->cond, NULL);
    e->state = EventState_NotReady;
    e->val = val;
    pthread_mutex_lock(&e->mutex);
    return e;
}

int
Event_wait(Event e) {
    assert(e);
    e->state = EventState_NotReady;
    int result = pthread_cond_wait(&e->cond, &e->mutex);
    if (e->state != EventState_Ready) {
        return Event_wait(e);
    } else {
        return result;
    }
}

int
Event_signal(Event e) {
    assert(e);
    e->state = EventState_Ready;
    return pthread_cond_signal(&e->cond);
}

int
Event_broadcast(Event e) {
    assert(e);
    e->state = EventState_Ready;
    return pthread_cond_broadcast(&e->cond);
}

void *
Event_value(Event e) {
    assert(e);
    return e->val;
}

void
Event_free(Event *e) {
    assert(e && *e);
    FREE(*e);
}

