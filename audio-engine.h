#ifndef AUDIO_ENGINE_H_INCLUDED
#define AUDIO_ENGINE_H_INCLUDED
// Play sound files with libsndfile and jack.
// http://mega-nerd.com/libsndfile
// http://jackaudio.org

// opaque data structure
typedef struct AudioEngine *AudioEngine;

// initialize the audio engine
AudioEngine
AudioEngine_init();

// free any resources being used by the audio engine
void
AudioEngine_free(AudioEngine *ae);

#endif
