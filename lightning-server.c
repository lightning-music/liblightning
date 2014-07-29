/* feature test macros
   - sigaction */
#define _POSIX_SOURCE

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jack-client.h"
#include "kit.h"
#include "log.h"
#include "osc-server.h"
#include "osc-types.h"

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

void
osc_error_handler(int num,
                  const char *msg,
                  const char *where);

int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data);

int main(int argc, char **argv) {

    setup_signal_handlers();
    log = Log_init(NULL);
    LOG(log, Info, "Welcome to %s!", "lightning");

    JackClient jack_client = JackClient_init(audio_callback, NULL);

    /* setup kit */

    const char * default_kit = "kits/default";

    Kit kit = Kit_load(default_kit, JackClient_samplerate(jack_client));

    JackClient_set_data(jack_client, kit);

    /* register a callback for if the jack output sample
       rate changes */

    JackClient_setup_callbacks(jack_client);

    JackClient_activate(jack_client);
    JackClient_setup_ports(jack_client);

    /* setup OSC server */

    OscServer osc_server = OscServer_init("41068",
                                          &osc_error_handler);

    OscServer_add_method(osc_server,
                         "/lightning/kit/1/samples/1",
                         "i",
                         play_sample,
                         kit);

    /* OSC receive loop */

    while (1) {
        OscServer_recv(osc_server);
    }

    /* free osc server */

    OscServer_free(&osc_server);

    /* free kit */

    Kit_free(&kit);

    /* free jack client */

    JackClient_free(&jack_client);

    /* free logger */

    Log_free(&log);

    return 0;
}

void
osc_error_handler(int num,
                  const char *msg,
                  const char *where) {
    Log log = Log_init(NULL);
    LOG(log, Error, "[OSC Error %d] %s %s\n", num, where, msg);
}

int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data) {
    printf("path %s\n", path);
    printf("types %s\n", types);
    return 0;
}

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data) {
    Kit kit = (Kit) data;
    Kit_write(kit, buffers, channels, frames);
    return 0;
}
