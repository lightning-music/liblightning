/**
 * jack client
 * see http://jackaudio.org
 *
 * Implementation Notes:
 * - Utilizes a jack ringbuffer to transfer data to the jack process callback.
 */
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include "jack-client.h"
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

struct JackClient {
    const char **ports;
    const char *server_name;
    void *data;
    jack_client_t *jack_client;
    jack_port_t *jack_output_port;
    jack_ringbuffer_t *rb;
    sample_data_callback callback;
    long overruns;
};

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

    JackClient client = (JackClient) arg;

    // setup output sample buffer

    sample_t *outbuf = \
        jack_port_get_buffer(client->jack_output_port, nframes);

    // write data to the output buffer

    return client->callback(outbuf,
                            (nframes_t) nframes,
                            client->data);
}

JackClient
JackClient_init(sample_data_callback callback,
                void *client_data) {
    JackClient client;
    NEW(client);

    // initialize the ringbuffer

    size_t rb_size = \
        DEFAULT_RB_SIZE * NUM_CHANNELS * sizeof(sample_t);

    client->rb = jack_ringbuffer_create(rb_size);

    // open jack client
    
    client->jack_client = jack_client_open("simplify", JackNullOption, NULL);

    if (client->jack_client == 0) {
        fprintf(stderr, "Jack server not running?\n");
        exit(EXIT_FAILURE);
    }

    // register realtime callback

    client->callback = callback;
    jack_set_process_callback(client->jack_client, process, client);

    // register shutdown callback

    jack_on_shutdown(client->jack_client, jack_shutdown, NULL);

    if (jack_activate(client->jack_client)) {
        fprintf(stderr, "Could not activate JACK client\n");
        exit(EXIT_FAILURE);
    }

    // register output port

    client->jack_output_port = \
        jack_port_register(client->jack_client,
                           "output",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    return client;
}

void
JackClient_free(JackClient *client) {
    assert(client && *client);
    // close jack client
    jack_client_close((*client)->jack_client);
    FREE(*client);
}

