#ifndef GAIN_H_INCLUDED
#define GAIN_H_INCLUDED

#include "types.h"

/**
 * Apply gain to a sample buffer
 */
void
gain(gain_t gain,
     sample_t *buf,
     nframes_t frames);

#endif
