#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#include "types.h"

/**
 * Opaque struct to hold Stream data
 */
typedef struct Stream *Stream;

/**
 * Initialize a Stream resource
 * If callback is NULL, the stream will
 * be a passthru.
 */
Stream
Stream_init(nframes_t bufsize,
            StreamCallback callback,
            void *data);

/**
 * Process an input sample buffer and
 * write data to an output sample buffer.
 * If this function exhausts the input buffer,
 * then hitend will be > 0. Otherwise it will be 0.
 */
nframes_t
Stream_process(Stream s,
               sample_t *in,
               sample_t *out,
               nframes_t inframes,
               nframes_t outframes,
               int *hitend);

/**
 * Free a Stream resource
 */
void
Stream_free(Stream *s);

#endif
