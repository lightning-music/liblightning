#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "types.h"

typedef struct Sample *Sample;

/**
 * Play a sample.
 * This will either load cached sample data (fast) or
 * read sample data from disk (slow).
 * Reading sample data from disk should only happen the first
 * time you load a particular sample. After that it should
 * be cached and subsequent calls to Sample_play should
 * be much faster.
 */
Sample
Sample_play(const char *file,
            pitch_t pitch,
            gain_t gain,
            nframes_t output_samplerate);

/**
 * Get the path this sample was loaded from.
 */
const char *
Sample_path(Sample samp);

/**
 * Write sample data to some buffers.
 * Returns the number of frames written.
 */
nframes_t
Sample_write(Sample samp,
             sample_t **buffers,
             channels_t channels,
             nframes_t frames);

int
Sample_done(Sample samp);

/**
 * Make current thread wait for the sample to finish playing.
 */
int
Sample_wait(Sample samp);

/**
 * Free resources associated with a Sample.
 */
void
Sample_free(Sample *samp);

#endif
