#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "mutex.h"
#include "stream.h"
#include "types.h"

typedef enum {
    StreamOutput_Mono,
    StreamOutput_Stereo
} StreamOutput;

typedef enum {
    StreamState_Initializing,
    StreamState_Processing,
    StreamState_Finished
} StreamState;

struct Stream {
    sample_t *buf;
    nframes_t frames;
    channels_t channels;
    StreamCallbackMono mono_callback;
    StreamCallbackStereo stereo_callback;
    /* stream state guarded by mutex */
    StreamState state;
    Mutex state_mutex;
    void *data;
};

inline static int
Stream_set_state(Stream s,
                 StreamState state) {
    int not_locked = Mutex_lock(s->state_mutex);
    if (not_locked) {
        return not_locked;
    } else {
        s->state = state;
        return Mutex_unlock(s->state_mutex);
    }
}

inline static int
Stream_is_processing(Stream s) {
    return s->state == StreamState_Processing;
}

Stream
Stream_init(nframes_t frames,
            channels_t channels,
            StreamCallbackMono mono_callback,
            StreamCallbackStereo stereo_callback,
            void *data) {
    Stream s;
    NEW(s);

    /* initialize state mutex and set to Initializing */

    s->state_mutex = Mutex_init();
    if (Stream_set_state(s, StreamState_Initializing)) {
        fprintf(stderr, "Could not set Stream state to Initializing\n");
        exit(EXIT_FAILURE);
    }

    s->mono_callback = mono_callback;
    s->stereo_callback = stereo_callback;
    s->frames = frames;
    s->channels = channels;
    s->buf = CALLOC(frames, SAMPLE_SIZE * channels);
    memset(s->buf, 0, SAMPLE_SIZE * frames * channels);
    s->data = data;

    if (Stream_set_state(s, StreamState_Processing)) {
        fprintf(stderr, "Could not set Stream state to Processing\n");
        exit(EXIT_FAILURE);
    }

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

    /* early out if stream is not in processing state */

    if (! Stream_is_processing(s)) {
        return 0;
    }

    nframes_t frames;

    if (s->mono_callback == NULL) {
        return 0;
    }

    frames = s->mono_callback(in, out, inframes, outframes, hitend, s->data);

    if (*hitend) {
        if (Stream_set_state(s, StreamState_Finished)) {
            fprintf(stderr, "Could not set Stream state to Finished\n");
            exit(EXIT_FAILURE);
        }
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

    /* early out if stream is not in processing state */

    if (! Stream_is_processing(s)) {
        return 0;
    }

    nframes_t frames;

    if (s->stereo_callback == NULL) {
        return 0;
    }

    frames = s->stereo_callback(in, ch1, ch2, inframes, outframes,
                                hitend, s->data);

    if (*hitend) {
        if (Stream_set_state(s, StreamState_Finished)) {
            fprintf(stderr, "Could not set Stream state to Finished\n");
            exit(EXIT_FAILURE);
        }
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
