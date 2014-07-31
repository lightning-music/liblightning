/**
 * A ringbuffer specially made for audio data
 */
#ifndef SAMPLE_BUFFER_H_INCLUDED
#define SAMPLE_BUFFER_H_INCLUDED

typedef struct SampleBuffer *SampleBuffer;

#include "types.h"

/**
 * Initialize a new sample buffer
 */
SampleBuffer
SampleBuffer_init(nframes_t sz);

/**
 * Read @a len samples from @a rb into @a buf.
 * Returns the number of samples read.
 */
nframes_t
SampleBuffer_read(SampleBuffer rb,
                  const sample_t *buf,
                  nframes_t len);

/**
 * Write @a len samples from @a buf to @a rb.
 * Returns the number of samples written.
 */
nframes_t
SampleBuffer_write(SampleBuffer rb,
                   sample_t *buf,
                   nframes_t len);

/**
 * Free a sample buffer
 */
void
SampleBuffer_free(SampleBuffer *buf);

#endif
