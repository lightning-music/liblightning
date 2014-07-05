#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include "audio-engine.h"
#include "mem.h"
#include "types.h"

/**
 * Default ringbuffer size
 */
#define DEFAULT_RB_SIZE 16384

/**
 * Only support stereo output.
 */
#define NUM_CHANNELS 2

struct AudioEngine {
    const char **ports;
    const char *server_name;
    jack_client_t *jack_client;
    jack_port_t *jack_output_port;
    long overruns;
};

static jack_ringbuffer_t *g_ringbuffer = NULL;

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

    size_t rb_size = \
        DEFAULT_RB_SIZE * NUM_CHANNELS * sizeof(sample_t);

    if (g_ringbuffer == NULL) {
        g_ringbuffer = jack_ringbuffer_create(rb_size);
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

size_t
AudioEngine_play(AudioEngine engine,
                 sample_t *sample_data,
                 size_t frames,
                 int channels) {
    assert(engine);
    size_t cnt = sizeof(sample_t) * frames * channels;
    return jack_ringbuffer_write(g_ringbuffer, (void *) sample_data, cnt);
}

void
AudioEngine_free(AudioEngine *engine) {
    assert(engine && *engine);
    // close jack client
    jack_client_close((*engine)->jack_client);
    FREE(*engine);
}

