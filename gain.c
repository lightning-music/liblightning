
#include "gain.h"
#include "types.h"

/**
 * Apply gain to a sample buffer
 */
void
gain(gain_t gain,
     sample_t *buf,
     nframes_t frames) {
    int i;
    for (i = 0; i < frames; i++)
        buf[i] = buf[i] * gain;
}

