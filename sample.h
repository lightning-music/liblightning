#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "types.h"

typedef struct Sample *Sample;

/**
 * Load and initialize a new Sample object.
 */
Sample
Sample_load(const char *file);

/**
 * Get the path this sample was loaded from.
 */
const char *
Sample_path(Sample samp);

/**
 * Get the number of channels for a Sample.
 */
channels_t
Sample_num_channels(Sample samp);

/**
 * Get the number of frames for a Sample.
 */
nframes_t
Sample_num_frames(Sample samp);

/**
 * Get the sample rate of the Sample.
 */
int
Sample_sample_rate(Sample samp);

/**
 * Write sample data to a mono output buffer.
 * Returns the number of frames written.
 */
nframes_t
Sample_write_mono(sample_t *ch1,
                  nframes_t frames);

/**
 * Write sample data to a stereo output buffers.
 * Returns the number of frames written.
 */
nframes_t
Sample_write_stereo(sample_t *ch1,
                    sample_t *ch2,
                    nframes_t frames);

/**
 * Make current thread wait for the sample to finish playing.
 */
int
Sample_wait();

/**
 * Free resources associated with a Sample.
 */
void
Sample_free(Sample *samp);

#endif
