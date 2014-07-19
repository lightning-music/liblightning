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
Stream_init(nframes_t frames,
            channels_t channels,
            StreamCallbackMono mono_callback,
            StreamCallbackStereo stereo_callback,
            void *data);

/**
 * Process an input sample buffer and
 * write data to an output sample buffer.
 * If this function exhausts the input buffer,
 * then hitend will be > 0. Otherwise it will be 0.
 */
nframes_t
Stream_process_mono(Stream s,
                    sample_t *in,
                    sample_t *out,
                    nframes_t inframes,
                    nframes_t outframes,
                    int *hitend);
/**
 * Process an input sample buffer and
 * write data to an output sample buffer.
 * If this function exhausts the input buffer,
 * then hitend will be > 0. Otherwise it will be 0.
 */
nframes_t
Stream_process_stereo(Stream s,
                      sample_t *in,
                      sample_t *ch1,
                      sample_t *ch2,
                      nframes_t inframes,
                      nframes_t outframes,
                      int *hitend);


/**
 * Return the buffer size in frames for a Stream.
 */
nframes_t
Stream_frames(Stream s);

/**
 * Return the number of channels for a Stream.
 */
channels_t
Stream_channels(Stream s);

/**
 * Free a Stream resource
 */
void
Stream_free(Stream *s);

#endif
