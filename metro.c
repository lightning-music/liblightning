#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mem.h"
#include "metro.h"
#include "types.h"

extern int errno;

struct Metro {
    MetroCallback cb;
    tempo_t tempo;
    position_t pos;
    void *data;
    timer_t *timerid;
};

void
notify_func(union sigval sv)
{
    Metro metro = (Metro) sv.sival_ptr;
    metro->cb(++metro->pos, metro->data);
}

Metro
        Metro_init(MetroCallback cb, tempo_t tempo, void *data)
{
    Metro metro;
    NEW(metro);
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_notify_function = notify_func;
    sev.sigev_value.sival_ptr = cb;

    timer_t *timerid;
    NEW(timerid);
    int error = timer_create(CLOCK_REALTIME, &sev, timerid);
    if (error) {
        fprintf(stderr, "Could not create timer: %s\n", strerror(errno));
    }
    metro->cb = cb;
    metro->data = data;
    metro->timerid = timerid;
    return metro;
}

int
Metro_start(Metro metro)
{
    assert(metro);
    int error;
    struct timespec interval, expire;
    /* best we can do to make it never expire? */
    expire.tv_sec = 0;
    expire.tv_nsec = 1;
    interval.tv_sec = 1;
    interval.tv_nsec = 0;
    struct itimerspec new_it;
    new_it.it_value = expire;
    new_it.it_interval = interval;
    error = timer_settime(metro->timerid, 0, &new_it, NULL);
    if (error) {
        fprintf(stderr, "Could not set timer value=0,1 interval=1,0: %s\n",
                strerror(errno));
        return 1;
    }
    return 0;
}

int
Metro_stop(Metro metro)
{
    assert(metro);
    int error;
    struct timespec interval, expire;
    /* best we can do to make it never expire? */
    expire.tv_sec = 0;
    expire.tv_nsec = 0;
    interval.tv_sec = 1;
    interval.tv_nsec = 0;
    struct itimerspec new_it;
    new_it.it_value = expire;
    new_it.it_interval = interval;
    error = timer_settime(metro->timerid, 0, &new_it, NULL);
    if (error) {
        fprintf(stderr, "Could not set timer value=0,0 interval=1,0: %s\n",
                strerror(errno));
        return 1;
    }
    return 0;
}

void
Metro_free(Metro *metro)
{
    assert(metro && *metro);
    Metro m = *metro;
    timer_delete(*(m->timerid));
    FREE(m->timerid);
    FREE(*metro);
}
