/**
 * Stream provides an abstraction to ease processing of sample
 * buffers.
 *
 * History
 * =======
 * When I first started implementing the Sample
 * interface I had never played with realtime audio
 * on such a low level before.
 * At first it was exciting just to be able to use
 * the jack c api to stream sample data from memory to
 * my dac.
 * As soon as I could play back a file, though, I wanted to
 * do more. The first thing I decided I would do was play
 * files back at different speeds.
 * I modified my play-sample example to accept a speed
 * argument and modified my Sample implementation to
 * play back at different speeds.
 * This involved inserting some extra processing in
 * between reading sample data from a buffer and feeding
 * to my jack output port.
 * This implementation worked well, and I was now more
 * excited than before!
 * I started experimenting with lots of different audio
 * files I had been using (all from freesound.org), and
 * soon discovered that there was a problem.
 * When I tried to play files that had a different sample
 * rate than the sr jack was using, the playback speed was
 * being altered.
 */
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
