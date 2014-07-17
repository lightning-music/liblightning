#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "mem.h"
#include "stream.h"
#include "types.h"

typedef enum {
    STREAM_INITIALIZING,
    STREAM_READY,
    STREAM_PROCESSING,
    STREAM_FINISHED
} STREAM_STATE;

struct Stream {
    sample_t *buf;
    nframes_t bufsize;
    StreamCallback callback;
    STREAM_STATE state;
};

Stream
Stream_init(nframes_t bufsize,
            StreamCallback callback,
            void *data) {
    Stream s;
    NEW(s);

    s->state = STREAM_INITIALIZING;

    s->callback = callback;
    s->bufsize = bufsize;
    s->buf = CALLOC(bufsize, SAMPLE_SIZE);
    memset(s->buf, 0, bufsize);

    s->state = STREAM_READY;

    return s;
}

nframes_t
Stream_process(Stream s,
               sample_t *in,
               sample_t *out,
               nframes_t inframes,
               nframes_t outframes,
               int *hitend) {
    assert(s);
    s->state = STREAM_PROCESSING;

    nframes_t frames;

    if (s->callback == NULL) {
        return 0;
    }

    frames = s->callback(in, out, inframes, outframes, NULL);

    if (outframes != frames) {
        *hitend = 1;
        s->state = STREAM_FINISHED;
    }

    return frames;
}

void
Stream_free(Stream *s) {
    assert(s && *s);
    FREE((*s)->buf);
    FREE(*s);
}
