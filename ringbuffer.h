#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#include <stddef.h>

#include "types.h"

typedef struct Ringbuffer *Ringbuffer;

/**
 * Initialize a ringbuffer with two channels and
 * a reasonable size.
 */
Ringbuffer
Ringbuffer_default();

/**
 * Initialize a Ringbuffer with @a size frames for @a channels.
 */
Ringbuffer
Ringbuffer_init(size_t size, channels_t channels);

/**
 * Read @a len samples from @a rb into @a buf.
 * Returns the number of samples read.
 */
nframes_t
Ringbuffer_read(Ringbuffer rb,
                sample_t *buf,
                size_t len);

/**
 * Write @a len samples from @a buf to @a rb.
 * Returns the number of samples written.
 */
nframes_t
Ringbuffer_write(Ringbuffer rb,
                 sample_t *buf,
                 size_t len);

/**
 * Free a ringbuffer.
 */
void
Ringbuffer_free(Ringbuffer *rb);

#endif
