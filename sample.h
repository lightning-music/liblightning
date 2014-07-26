#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "types.h"

typedef struct Sample *Sample;

/**
 * Load and initialize a new Sample object.
 */
Sample
Sample_load(const char *file,
            pitch_t pitch,
            gain_t gain);

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
Sample_samplerate(Sample samp);

int
Sample_samplerate_callback(nframes_t sr,
                           void *arg);

/**
 * Return the number of samples available for reading.
 */
nframes_t
Sample_frames_available(Sample samp);

/**
 * Reset the internal sample frame pointer.
 */
int
Sample_reset(Sample samp);

/**
 * Write sample data to some buffers.
 * Returns the number of frames written.
 */
nframes_t
Sample_write(Sample samp,
             sample_t **buffers,
             channels_t channels,
             nframes_t frames);

/**
 * Total number of frames that have been written in one of
 * the audio callbacks.
 */
nframes_t
Sample_total_frames_written(Sample samp);

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
