/**
 * kidcomposer jack client
 * see http://jackaudio.org
 */
#ifndef AUDIO_ENGINE_H_INCLUDED
#define AUDIO_ENGINE_H_INCLUDED

/**
 * AudioEngine data structure.
 */
typedef struct AudioEngine *AudioEngine;

/**
 * Initialize an audio engine.
 */
AudioEngine
AudioEngine_init();

// free any resources being used by the audio engine
void
AudioEngine_free(AudioEngine *ae);

#endif
