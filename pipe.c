#include <assert.h>
#include <string.h>

#include "mem.h"
#include "pipe.h"
#include "stream.h"
#include "types.h"

struct Pipe {
    /* input stream */
    Stream in;
    /* output stream */
    Stream out;
    nframes_t frames;
    channels_t channels;
    /* buffer to hold intermediate sample data */
    sample_t *buf;
};

nframes_t
stream_pipe(sample_t *in,
            sample_t *out,
            nframes_t inframes,
            nframes_t outframes,
            void *data) {
    Pipe p = (Pipe) data;
    /* clear out the buffer */
    memset(p->buf, 0, SAMPLE_SIZE * p->frames * p->channels);
    /* flag that will tell us if we hit the end of the input buffer */
    int hitend = 0;
    nframes_t frames1 = 0, frames2 = 0;
    /* let the first stream process the data */
    frames1 = Stream_process(p->in, in, p->buf,
                             inframes, p->frames, &hitend);

    if (hitend) {
        return frames1;
    }

    frames2 = Stream_process(p->out, p->buf, out,
                             p->frames, outframes, &hitend);

    /* there is probably a better way to calculate the
       return value */
    return frames1;
}

Stream
Pipe_init(Stream in,
          Stream out) {
    Pipe p;
    Stream s;

    NEW(p);

    p->in = in;
    p->out = out;

    nframes_t frames = Stream_frames(in);
    channels_t channels = Stream_channels(in);
    /* is this necessary? */
    assert(frames == Stream_frames(out));
    assert(channels == Stream_channels(out));
    p->frames = frames;
    p->channels = channels;
    /* allocate pipe buffer */
    p->buf = CALLOC(frames, SAMPLE_SIZE * channels);
    memset(p->buf, 0, SAMPLE_SIZE * frames * channels);
    /* initialize and return new Stream */
    s = Stream_init(Stream_bufsize(in), stream_pipe, p);
    return s;
}

void
Pipe_free(Pipe *p) {
    assert(p && *p);
    FREE(p->in);
    FREE(p->out);
    FREE(p);
}
