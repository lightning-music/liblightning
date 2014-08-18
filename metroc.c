#define _POSIX_C_SOURCE 199309L

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern int errno;

void
notify_func(union sigval sv)
{
    printf("notified\n");
}

int main(int argc, char **argv)
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    timer_t timerid;
    int error = timer_create(CLOCK_REALTIME, &sev, &timerid);
    if (error) {
        fprintf(stderr, "Could not create timer: %s\n", strerror(errno));
    }
    struct timespec interval, expire;
    /* best we can do to make it never expire? */
    expire.tv_sec = UINT64_MAX;
    interval.tv_sec = 1;
    const struct itimerspec new_it = {
        interval,
        expire
    };
    error = timer_settime(timerid, 0, &new_it, NULL);
    if (error) {
        fprintf(stderr, "Could not set timer: %s\n", strerror(errno));
    }
    return 0;
}
