#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "event.h"
#include "log.h"
#include "mem.h"
#include "metro.h"
#include "types.h"

#define MAX_SLAVES 16

extern int errno;

struct Metro {
    tempo_t tempo;
    position_t pos;
    timer_t *timerid;
    int num_slaves;
    Slave *slaves;
};

void
notify_func(union sigval sv);

static int
parse_divisor(const char *divisor);

Metro
Metro_init(tempo_t tempo)
{
    Metro metro;
    NEW(metro);
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_notify_function = notify_func;
    sev.sigev_value.sival_ptr = metro;

    timer_t *timerid;
    NEW(timerid);
    int error = timer_create(CLOCK_REALTIME, &sev, timerid);
    if (error) {
        LOG(Error, "Could not create timer: %s\n", strerror(errno));
    }

    metro->tempo = tempo;
    metro->timerid = timerid;
    metro->num_slaves = 0;
    metro->slaves = CALLOC(MAX_SLAVES, sizeof(Slave));
    Metro_set_tempo(metro, metro->tempo);

    return metro;
}

int
Metro_start(Metro metro)
{
    return Metro_set_tempo(metro, metro->tempo);
}

int
Metro_set_tempo(Metro metro, tempo_t tempo)
{
    assert(metro);
    int error;
    /* calculate ns interval for given tempo
       we tick 384 times per bar */
    double nspertick = (1000000000 * (240 / metro->tempo)) / 384.0;
    struct timespec interval, expire;
    /* best we can do to make it never expire? */
    expire.tv_sec = 0;
    expire.tv_nsec = 1;
    interval.tv_sec = 0;
    interval.tv_nsec = (long) nspertick;
    struct itimerspec new_it;
    new_it.it_value = expire;
    new_it.it_interval = interval;
    error = timer_settime(metro->timerid, 0, &new_it, NULL);
    if (error) {
        LOG(Error, "Could not set timer value=0,1 interval=1,0: %s\n",
            strerror(errno));
        return 1;
    }
    return 0;
}

Slave
Metro_slave(Metro master, const char *divisor)
{
    Slave slave;
    if (master->num_slaves == MAX_SLAVES) {
        return NULL;
    }
    slave = Slave_init( parse_divisor(divisor) );
    master->slaves[ master->num_slaves++ ] = slave;
    return slave;
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
        LOG(Error, "Could not set timer value=0,0 interval=1,0: %s\n",
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

void
notify_func(union sigval sv)
{
    Metro metro = (Metro) sv.sival_ptr;
    int i;
    for (i = 0; i < metro->num_slaves; i++) {
        if (NULL != metro->slaves[i]) {
            Slave_tick(metro->slaves[i]);
        }
    }
}

static int
parse_divisor(const char *divisor)
{
    int d;
    sscanf(divisor, "1/%d", &d);
    assert(d);
    return d;
}
