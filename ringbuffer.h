#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#include <stddef.h>

#include "types.h"

typedef struct Ringbuffer *Ringbuffer;

/**
 * Initialize a Ringbuffer with @a size frames for @a channels.
 */
Ringbuffer
Ringbuffer_init(size_t size);

/**
 * Read @a len samples from @a rb into @a buf.
 * Returns the number of samples read.
 */
size_t
Ringbuffer_read(Ringbuffer rb,
                char *buf,
                size_t len);

/**
 * Write @a len samples from @a buf to @a rb.
 * Returns the number of samples written.
 */
size_t
Ringbuffer_write(Ringbuffer rb,
                 const char *buf,
                 size_t len);

/**
 * Free a ringbuffer.
 */
void
Ringbuffer_free(Ringbuffer *rb);

#endif
