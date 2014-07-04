#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>
#include <jack/jack.h>

typedef jack_default_audio_sample_t sample_t;
typedef jack_nframes_t nframes_t;
typedef int (* audio_process_callback)(nframes_t nframes, void *arg);

#endif
