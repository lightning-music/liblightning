/* feature test macro for sigaction(2) */
#define _POSIX_SOURCE

/* feature test macro for getopt_long(3) */
#define _GNU_SOURCE

#include <assert.h>
#include <getopt.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lightning-server.h"
#include "log.h"
#include "mem.h"

extern char *optarg;
extern int optind, opterr, optopt;

static void
signal_handler(int signum);

static void
setup_signal_handlers(void);

static char **
parse_search_dirs(char *str);

int main(int argc, char * const argv[])
{
    enum Option {
        OPTION_HOST,
        OPTION_PORT,
        OPTION_SEARCH_DIRS
    };

    char **search_dirs = NULL;
    
    setup_signal_handlers();
    LOG(Info, "Welcome to %s!", "lightning");

    int c = 0;
    int option_index = 0;
    struct option opts[] = {
        {"host",            required_argument, 0,  OPTION_HOST},
        {"port",            required_argument, 0,  OPTION_PORT},
        {"search-dirs",     required_argument, 0,  OPTION_SEARCH_DIRS},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "h:p:s:", opts, &option_index)) != -1) {
        switch(c) {
        case OPTION_HOST:
            break;
        case OPTION_PORT:
            break;
        case OPTION_SEARCH_DIRS:
            search_dirs = parse_search_dirs(optarg);
            break;
        }
    }

    int i = 0;
    if (search_dirs != NULL)
        while (search_dirs[i++] != NULL) ;

    LightningServer server =                                        \
        LightningServer_init("41068", NULL, NULL, i, search_dirs);
    /* blocking */
    LightningServer_listen(server);
    LightningServer_free(&server);

    /* free search dirs */

    int num_dirs = i;
    for (i = 0; i < num_dirs; i++) {
        FREE(search_dirs[i]);
    }
    FREE(search_dirs);

    return 0;
}

static void
signal_handler(int signum)
{
    if (signum == SIGINT) {
        LOG(Info, "Received signal %d... Exiting\n", signum);
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

static char **
parse_search_dirs(char *str)
{
    const long limit = 32;
    const char *delim = ":";
    char **strings = CALLOC(limit, sizeof(char*));
    char *c = strtok(str, delim);
    int i = 0;
    while (NULL != c) {
        strings[i] = CALLOC(strlen(c) + 1, sizeof(char));
        strings[i++] = c;
        c = strtok(NULL, delim);
    }
    strings[i] = NULL;
    return strings;
}
