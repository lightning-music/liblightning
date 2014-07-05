/**
 * jack client
 * see http://jackaudio.org
 *
 * Notes:
 * AudioEngine will utilize a jack ringbuffer to
 * transfer
 */
#ifndef AUDIO_ENGINE_H_INCLUDED
#define AUDIO_ENGINE_H_INCLUDED

#include <stddef.h>

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

/**
 * Play @a frames of sample data, and assume that
 * @a sample_data has @a channels.
 * @a sample_data must be allocated by client code
 * and must be at least sizeof(sample_t) * frames * channels
 */
size_t
AudioEngine_play(AudioEngine engine,
                 sample_t *sample_data,
                 size_t frames,
                 int channels);

// free any resources being used by the audio engine
void
AudioEngine_free(AudioEngine *engine);

#endif
