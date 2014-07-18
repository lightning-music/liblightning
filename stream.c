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
    nframes_t frames;
    channels_t channels;
    StreamCallback callback;
    STREAM_STATE state;
    void *data;
};

Stream
Stream_init(nframes_t frames,
            channels_t channels,
            StreamCallback callback,
            void *data) {
    Stream s;
    NEW(s);

    s->state = STREAM_INITIALIZING;

    s->callback = callback;
    s->frames = frames;
    s->channels = channels;
    s->buf = CALLOC(frames, SAMPLE_SIZE * channels);
    memset(s->buf, 0, SAMPLE_SIZE * frames * channels);
    s->data = data;

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

    frames = s->callback(in, out, inframes, outframes, s->data);

    if (outframes != frames) {
        *hitend = 1;
        s->state = STREAM_FINISHED;
    }

    return frames;
}

nframes_t
Stream_frames(Stream s) {
    assert(s);
    return s->frames;
}

channels_t
Stream_channels(Stream s) {
    assert(s);
    return s->channels;
}

void
Stream_free(Stream *s) {
    assert(s && *s);
    FREE((*s)->buf);
    FREE(*s);
}
