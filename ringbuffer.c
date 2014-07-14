#include <assert.h>
#include <jack/ringbuffer.h>
#include <stddef.h>

#include "mem.h"
#include "ringbuffer.h"
#include "types.h"

struct Ringbuffer {
    jack_ringbuffer_t *jrb;
};

Ringbuffer
Ringbuffer_default() {
    return Ringbuffer_init(16384, 2);
}

Ringbuffer
Ringbuffer_init(size_t size, channels_t channels) {
    Ringbuffer rb;
    NEW(rb);

    size_t bytes = size * channels * SAMPLE_SIZE;

    rb->jrb = jack_ringbuffer_create(bytes);

    return rb;
}

nframes_t
Ringbuffer_read(Ringbuffer rb,
                sample_t *buf,
                size_t len) {
    assert(rb);
    return jack_ringbuffer_read(rb->jrb, (void *) buf, len);
}

nframes_t
Ringbuffer_write(Ringbuffer rb,
                 sample_t *buf,
                 size_t len) {
    assert(rb);
    return jack_ringbuffer_write(rb->jrb, (void *) buf, len);
}

void
Ringbuffer_free(Ringbuffer *rb) {
    assert(rb && *rb);
    jack_ringbuffer_free((*rb)->jrb);
    FREE(*rb);
}
