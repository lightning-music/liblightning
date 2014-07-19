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
    StreamCallbackMono mono_callback;
    StreamCallbackStereo stereo_callback;
    STREAM_STATE state;
    void *data;
};

Stream
Stream_init(nframes_t frames,
            channels_t channels,
            StreamCallbackMono mono_callback,
            StreamCallbackStereo stereo_callback,
            void *data) {
    Stream s;
    NEW(s);

    s->state = STREAM_INITIALIZING;

    s->mono_callback = mono_callback;
    s->stereo_callback = stereo_callback;
    s->frames = frames;
    s->channels = channels;
    s->buf = CALLOC(frames, SAMPLE_SIZE * channels);
    memset(s->buf, 0, SAMPLE_SIZE * frames * channels);
    s->data = data;

    s->state = STREAM_READY;

    return s;
}

nframes_t
Stream_process_mono(Stream s,
                    sample_t *in,
                    sample_t *out,
                    nframes_t inframes,
                    nframes_t outframes,
                    int *hitend) {
    assert(s);
    s->state = STREAM_PROCESSING;

    nframes_t frames;

    if (s->mono_callback == NULL) {
        return 0;
    }

    frames = s->mono_callback(in, out, inframes, outframes, hitend, s->data);

    if (*hitend) {
        s->state = STREAM_FINISHED;
    }

    return frames;
}

nframes_t
Stream_process_stereo(Stream s,
                      sample_t *in,
                      sample_t *ch1,
                      sample_t *ch2,
                      nframes_t inframes,
                      nframes_t outframes,
                      int *hitend) {
    assert(s);
    s->state = STREAM_PROCESSING;

    nframes_t frames;

    if (s->stereo_callback == NULL) {
        return 0;
    }

    frames = s->stereo_callback(in, ch1, ch2, inframes, outframes,
                                hitend, s->data);

    if (*hitend) {
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
