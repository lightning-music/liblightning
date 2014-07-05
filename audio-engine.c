#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>

#include "audio-engine.h"
#include "mem.h"
#include "types.h"

struct AudioEngine {
    const char **ports;
    const char *server_name;
    jack_client_t *jack_client;
    jack_port_t *jack_output_port;
};

/**
 * Audio engine singleton
 */
static AudioEngine g_engine = NULL;

/**
 * JACK shutdown callback
 */
void
jack_shutdown(void *arg) {
    fprintf(stderr, "JACK shutdown\n");
    abort();
}

/**
 * JACK process callback
 */
int
process(jack_nframes_t nframes,
        void *arg) {

    // output sample buffer
    sample_t *outbuf = \
        jack_port_get_buffer(g_engine->jack_output_port, nframes);

    return 0;
}

AudioEngine
AudioEngine_init() {
    if (g_engine != NULL) {
        return g_engine;
    }

    AudioEngine engine;

    NEW(engine);

    // open jack client
    
    engine->jack_client = jack_client_open("simplify", JackNullOption, NULL);

    if (engine->jack_client == 0) {
        fprintf(stderr, "Jack server not running?\n");
        exit(EXIT_FAILURE);
    }

    // register realtime callback
    
    jack_set_process_callback(engine->jack_client, process, NULL);

    // register shutdown callback

    jack_on_shutdown(engine->jack_client, jack_shutdown, NULL);

    // register output port

    engine->jack_output_port = \
        jack_port_register(engine->jack_client,
                           "output",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    if (jack_activate(engine->jack_client)) {
        fprintf(stderr, "Could not activate JACK client\n");
        exit(EXIT_FAILURE);
    }

    return g_engine = engine;
}

void
AudioEngine_free(AudioEngine *engine) {
    assert(engine && *engine);
    // close jack client
    jack_client_close((*engine)->jack_client);
    FREE(*engine);
}

