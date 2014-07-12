/**
 * jack client
 * see http://jackaudio.org
 *
 * Goals:
 * - Expose an interface
 */
#ifndef JACK_CLIENT_H_INCLUDED
#define JACK_CLIENT_H_INCLUDED

#include <stddef.h>

#include "types.h"

/**
 * AudioEngine data structure.
n */
typedef struct JackClient *JackClient;

/**
 * Initialize an audio engine.
 * @param realtime callback used to fill frame buffer
 * @client_data pointer to data passed to callback
 */
JackClient
JackClient_init(sample_data_callback callback,
                void *client_data);

int
JackClient_playback_ports(JackClient jack);

// free any resources being used by the audio engine
void
JackClient_free(JackClient *jack);

#endif
