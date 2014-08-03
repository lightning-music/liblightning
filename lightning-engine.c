/* feature test macro for sigaction(2) */
#define _POSIX_SOURCE

/* feature test macro for getopt_long(3) */
#define _GNU_SOURCE

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lightning-server.h"
#include "log.h"

static Log log = NULL;

static void
signal_handler(int signum) {
    if (signum == SIGINT) {
        LOG(log, Info, "Received signal %d... Exiting\n", signum);
        Log_free(&log);
        exit(1);
    } else {
        exit(1);
    }
}

static void
setup_signal_handlers(void) {
    struct sigaction old_action, new_action;
    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGINT, &new_action, NULL);
}

int main(int argc, char **argv)
{
    setup_signal_handlers();
    log = Log_init(NULL);
    LOG(log, Info, "Welcome to %s!", "lightning");

    LightningServer server = LightningServer_init("41068", NULL, NULL);

    LightningServer_listen(server);

    LightningServer_free(&server);

    /* free logger */

    Log_free(&log);

    return 0;
}
