#include <assert.h>

#include "mem.h"
#include "pipe.h"
#include "stream.h"
#include "types.h"

struct Pipe {
    Stream in;
    Stream out;
};

nframes_t
stream_pipe(sample_t *in,
            sample_t *out,
            nframes_t inframes,
            nframes_t outframes,
            void *data) {
    Pipe p = (Pipe) data;
    return 0;
}

Stream
Pipe_init(Stream in,
          Stream out) {
    Pipe p;
    Stream s;
    NEW(p);

    p->in = in;
    p->out = out;

    assert(Stream_bufsize(in) == Stream_bufsize(out));

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
