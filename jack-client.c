/**
 * jack client
 * see http://jackaudio.org
 */
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jack/jack.h>

#include "jack-client.h"
#include "mem.h"
#include "ringbuffer.h"
#include "types.h"

struct JackClient {
    const char **ports;
    const char *server_name;
    void *data;
    jack_client_t *jack_client;
    jack_port_t *jack_output_port_1;
    jack_port_t *jack_output_port_2;
    sample_data_callback callback;
    Ringbuffer rb;
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

    // setup output sample buffers

    sample_t *ch1 = \
        jack_port_get_buffer(client->jack_output_port_1, nframes);

    sample_t *ch2 = \
        jack_port_get_buffer(client->jack_output_port_2, nframes);

    // write data to the output buffer

    return client->callback(ch1,
                            ch2,
                            (nframes_t) nframes,
                            client->data);
}

JackClient
JackClient_init(sample_data_callback callback,
                void *client_data) {
    JackClient client;
    NEW(client);

    // initialize the ringbuffer

    client->rb = Ringbuffer_default();

    // open jack client
    
    client->jack_client = jack_client_open("simplify", JackNullOption, NULL);

    if (client->jack_client == 0) {
        fprintf(stderr, "Jack server not running?\n");
        exit(EXIT_FAILURE);
    }

    // register realtime callback

    client->data = client_data;
    client->callback = callback;
    jack_set_process_callback(client->jack_client, process, client);

    // register shutdown callback

    jack_on_shutdown(client->jack_client, jack_shutdown, NULL);

    if (jack_activate(client->jack_client)) {
        fprintf(stderr, "Could not activate JACK client\n");
        exit(EXIT_FAILURE);
    }

    // register output ports

    client->jack_output_port_1 = \
        jack_port_register(client->jack_client,
                           "output_1",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    client->jack_output_port_2 = \
        jack_port_register(client->jack_client,
                           "output_2",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    const char *playback1 = "system:playback_1";
    const char *playback2 = "system:playback_2";

    // connect playback_1

    if (jack_connect(client->jack_client,
                     jack_port_name(client->jack_output_port_1),
                     playback1)) {
        fprintf(stderr, "Could not connect %s to %s\n",
                jack_port_name(client->jack_output_port_1),
                playback1);
        exit(EXIT_FAILURE);
    }
    // connect playback_2

    if (jack_connect(client->jack_client,
                     jack_port_name(client->jack_output_port_2),
                     playback2)) {
        fprintf(stderr, "Could not connect %s to %s\n",
                jack_port_name(client->jack_output_port_2),
                playback2);
        exit(EXIT_FAILURE);
    }

    return client;
}

int
JackClient_playback_ports(JackClient jack) {
}

void
JackClient_free(JackClient *client) {
    assert(client && *client);
    // close jack client
    jack_client_close((*client)->jack_client);
    FREE(*client);
}

