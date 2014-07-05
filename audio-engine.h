/**
 * jack client
 * see http://jackaudio.org
 */
#ifndef AUDIO_ENGINE_H_INCLUDED
#define AUDIO_ENGINE_H_INCLUDED

#include "types.h"

/**
 * AudioEngine data structure.
n */
typedef struct AudioEngine *AudioEngine;

/**
 * Initialize an audio engine.
 */
AudioEngine
AudioEngine_init();

int
AudioEngine_play_samples(sample_t *frames,
                         nframes_t nframes);

// free any resources being used by the audio engine
void
AudioEngine_free(AudioEngine *engine);

#endif
