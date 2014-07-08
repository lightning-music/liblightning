#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>
#include <jack/jack.h>

#define SAMPLE_SIZE sizeof(sample_t)

/**
 * default sample size
 */
typedef jack_default_audio_sample_t sample_t;

/**
 * sample count
 */
typedef unsigned long sample_count_t;

/**
 * alias of jack_nframes_t (type used to represent sample frame counts)
 */
typedef jack_nframes_t nframes_t;

/**
 * callback for getting @a frames of sample data
 * return 0 for success, nonzero for failure
 */
typedef int (* sample_data_callback)(sample_t *buf,
                                     nframes_t frames,
                                     void *client_data);

#endif
