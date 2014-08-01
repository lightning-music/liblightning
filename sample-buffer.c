#include <assert.h>

#include "mem.h"
#include "ringbuffer.h"
#include "sample-buffer.h"
#include "types.h"

struct SampleBuffer {
    Ringbuffer rb;
};

/**
 * Initialize a new sample buffer
 */
SampleBuffer
SampleBuffer_init(nframes_t sz)
{
    SampleBuffer buf;
    NEW(buf);
    buf->rb = Ringbuffer_init(SAMPLE_SIZE * sz);
    return buf;
}

/**
 * Read @a len samples from @a rb into @a buf.
 * Returns the number of samples read.
 */
nframes_t
SampleBuffer_read(SampleBuffer sb,
                  const sample_t *buf,
                  nframes_t len)
{
    assert(rb);
    return SAMPLE_SIZE * Ringbuffer_read(sb->rb,
                                         (const void *) buf,
                                         SAMPLE_SIZE * len);
}

/**
 * Write @a len samples from @a buf to @a rb.
 * Returns the number of samples written.
 */
nframes_t
SampleBuffer_write(SampleBuffer sb,
                   sample_t *buf,
                   nframes_t len)
{
    assert(rb);
    return SAMPLE_SIZE * Ringbuffer_write(sb->rb,
                                          (void *) buf,
                                          SAMPLE_SIZE * len);
}

/**
 * Free a sample buffer
 */
void
SampleBuffer_free(SampleBuffer *buf)
{
    assert(buf && *buf);
    Ringbuffer_free(&(*buf)->rb);
    FREE(*buf);
}

