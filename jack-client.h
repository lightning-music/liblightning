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
 */
typedef struct JackClient *JackClient;

/**
 * Initialize an audio engine.
 * @param realtime callback used to fill frame buffer
 * @client_data pointer to data passed to callback
 */
JackClient
JackClient_init(AudioCallback audio_callback, void *client_data);

int
JackClient_setup_callbacks(JackClient client);

int
JackClient_activate(JackClient client);

int
JackClient_setup_ports(JackClient client);

void
JackClient_set_data(JackClient client, void *data);

nframes_t
JackClient_samplerate(JackClient jack);

int
JackClient_set_samplerate_callback(JackClient jack,
                                   SampleRateCallback callback,
                                   void *arg);

nframes_t
JackClient_buffersize(JackClient jack);

int
JackClient_playback_ports(JackClient jack);

/**
 * Start exporting to an audio file
 *
 * @return 0 on success, nonzero on failure
 */
int
JackClient_export_start(JackClient client, const char *file);

/**
 * Stop recording output to audio file
 *
 * @return 0 on success, nonzero on failure
 */
int
JackClient_export_stop(JackClient client);

// free any resources being used by the audio engine
void
JackClient_free(JackClient *jack);

#endif
