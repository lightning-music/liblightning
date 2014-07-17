#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>
#include <jack/jack.h>

#define SAMPLE_SIZE sizeof(sample_t)

/**
 * Gain type float in [0, 1]
 */
typedef float gain_t;

/**
 * Pitch type
 */
typedef float pitch_t;

/**
 * default sample size (jack currently uses 32-bit float)
 */
typedef jack_default_audio_sample_t sample_t;

/**
 * number of channels
 */
typedef int channels_t;

/**
 * sample count
 */
typedef double sample_count_t;

/**
 * alias of jack_nframes_t (type used to represent sample frame counts)
 */
typedef jack_nframes_t nframes_t;

/**
 * callback for getting @a frames of mono sample data
 * return 0 for success, nonzero for failure
 */
typedef int (* MonoCallback)(sample_t *ch1,
                             nframes_t frames,
                             void *client_data);

/**
 * callback for getting @a frames of stereo sample data
 * return 0 for success, nonzero for failure
 */
typedef int (* StereoCallback)(sample_t *ch1,
                               sample_t *ch2,
                               nframes_t frames,
                               void *client_data);

/**
 * Callback that allows us to customize the behavior
 * of streams.
 */
typedef nframes_t (* StreamCallback)(sample_t *in,
                                     sample_t *out,
                                     nframes_t inframes,
                                     nframes_t outframes,
                                     void *data);

#endif
