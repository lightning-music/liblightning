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
    MonoCallback mono_callback;
    StereoCallback stereo_callback;
    Ringbuffer rb;
};

static int
samplerate_callback(nframes_t sr,
                    void *data) {
    /* Notify client code that depends on the output
       sample rate */
    return 0;
}

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

    // write data to the output buffer with registered callbacks
    // TODO: use mono callback if there is only one playback port

    if (client->stereo_callback != NULL) {
        return client->stereo_callback(ch1,
                                       ch2,
                                       (nframes_t) nframes,
                                       client->data);
    } else if (client->mono_callback != NULL) {
        return client->mono_callback(ch1,
                                     (nframes_t) nframes,
                                     client->data);
    } else {
        return 0;
    }
}

JackClient
JackClient_init(MonoCallback mono_callback,
                StereoCallback stereo_callback,
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
    client->mono_callback = mono_callback;
    client->stereo_callback = stereo_callback;
    jack_set_process_callback(client->jack_client, process, client);

    // register a callback for when jack changes the output
    // sample rate
    if (jack_set_sample_rate_callback(client->jack_client,
                                      samplerate_callback,
                                      client)) {
        fprintf(stderr, "Could not register samplerate callback\n");
        exit(EXIT_FAILURE);
    }

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

nframes_t
JackClient_samplerate(JackClient jack) {
    assert(jack);
    return jack_get_sample_rate(jack->jack_client);
}

nframes_t
JackClient_buffersize(JackClient jack) {
    assert(jack);
    return jack_get_buffer_size(jack->jack_client);
}

// FIXME
int
JackClient_playback_ports(JackClient jack) {
    return 2;
}

/* How would the set_*_callback functions ever fail? */

int
JackClient_set_mono_callback(JackClient jack,
                             MonoCallback mcb,
                             void *data) {
    assert(jack);
    jack->mono_callback = mcb;
    return 0;
}

int
JackClient_set_stereo_callback(JackClient jack,
                               StereoCallback scb,
                               void *data) {
    assert(jack);
    jack->stereo_callback = scb;
    return 0;
}

void
JackClient_free(JackClient *jack) {
    assert(jack && *jack);
    // close jack client
    jack_client_close((*jack)->jack_client);
    FREE(*jack);
}

