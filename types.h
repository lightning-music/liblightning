#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>
#include <jack/jack.h>

/**
 * default sample size
 */
typedef jack_default_audio_sample_t sample_t;

/**
 * alias of jack_nframes_t (type used to represent sample frame counts)
 */
typedef jack_nframes_t nframes_t;

/**
 * callback for getting @a frames of sample data
 */
typedef nframes_t (* get_sample_data)(sample_t *buf,
                                      nframes_t frames);

#endif
