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
JackClient_init(MonoCallback mono_callback,
                StereoCallback stereo_callback,
                void *client_data);

int
JackClient_setup_callbacks(JackClient client);

int
JackClient_activate(JackClient client);

int
JackClient_setup_ports(JackClient client);

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
 * Set the mono callback
 */
int
JackClient_set_mono_callback(JackClient jack,
                             MonoCallback mcb,
                             void *data);

/**
 * Set the stereo callback
 */
int
JackClient_set_stereo_callback(JackClient jack,
                               StereoCallback scb,
                               void *data);

// free any resources being used by the audio engine
void
JackClient_free(JackClient *jack);

#endif
