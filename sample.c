#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "clip.h"
#include "event.h"
#include "mem.h"
#include "mutex.h"
#include "sample.h"
#include "types.h"

enum {
    SAMPLE_NONE,
    SAMPLE_DONE,
    SAMPLE_REVERSED
} SampleFlags;

/*
 * TODO
 * ====
 * Need to have a static table for sample data
 * so that loading the same sample twice would result in
 * the initialization step for the second Sample_load just being
 * a memcpy from the table.
 */

struct Sample {
    const char *path;
    pitch_t pitch;
    gain_t gain;
    // channels, frames, and samplerate are pulled from SF_INFO
    channels_t channels;
    nframes_t frames;
    int samplerate;
    // buffer to hold sample data
    sample_t *framebuf;
    // track read position in file (guarded by mutex)
    nframes_t frames_read;
    Mutex frames_read_mutex;
    // flags
    int flags;
    // provide a way to synchronize a thread on the `done` event
    Event done_event;
};

// bit flag functions

static inline int
is_done(Sample samp) {
    return samp->flags & SAMPLE_DONE;
}

static inline Sample
set_done(Sample samp) {
    samp->flags |= SAMPLE_DONE;
    return samp;
}

static inline int
is_reversed(Sample samp) {
    return samp->flags & SAMPLE_REVERSED;
}

static inline Sample
set_reversed(Sample samp) {
    samp->flags |= SAMPLE_REVERSED;
    return samp;
}

/**
 * Try to set frames_read
 */
static int
set_frames_read(Sample samp, nframes_t val) {
    if (0 == Mutex_trylock(samp->frames_read_mutex)) {
        samp->frames_read = val;
        return Mutex_unlock(samp->frames_read_mutex);
    } else {
        return -1;
    }
}

/**
 * Load an audio sample.
 */
Sample
Sample_load(const char *file,
            pitch_t pitch,
            gain_t gain) {
    Sample s;
    NEW(s);

    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(file, SFM_READ, &sfinfo);

    if (sf == NULL) {
        fprintf(stderr, "%s\n", sf_strerror(sf));
        exit(EXIT_FAILURE);
    }



    /* Set pitch to a very small number if it is 0,
       otherwise clip it to a given range and
       if it is negative set the reversed bit */
    if (pitch == 0.0) {
        s->pitch = 0.0001;
    } else {
        s->pitch = clip(pitch, -32.0f, 32.0f);
        if (s->pitch < 0.0) {
            set_reversed(s);
        }
    }

    s->gain = clip(gain, 0.0f, 1.0f);
    s->frames = sfinfo.frames;
    s->channels = sfinfo.channels;
    s->samplerate = sfinfo.samplerate;

    s->done_event = Event_init(NULL);

    s->path = file;

    // frame buffer contains n samples,
    // where n = frames * channels

    s->framebuf = CALLOC(s->frames * s->channels, SAMPLE_SIZE);

    // fill the frame buffer

    sf_count_t frames = 1024 * s->channels;
    long total_frames = sf_readf_float(sf, s->framebuf, frames);

    while (total_frames < s->frames) {
        total_frames +=                         \
            sf_readf_float(sf,
                           s->framebuf + (total_frames * s->channels),
                           frames);
    }

    sf_close(sf);

    s->flags = SAMPLE_NONE;

    s->frames_read = 0;
    s->frames_read_mutex = Mutex_init();

    return s;
}

/**
 * Path to loaded file.
 */
const char *
Sample_path(Sample samp) {
    assert(samp);
    return samp->path;
}

channels_t
Sample_num_channels(Sample samp) {
    assert(samp);
    return samp->channels;
}

/**
 * Number of frames in the audio sample.
 */
nframes_t
Sample_num_frames(Sample samp) {
    assert(samp);
    return (nframes_t) samp->frames;
}

/**
 * Sample rate of the audio sample.
 */
int
Sample_samplerate(Sample samp) {
    assert(samp);
    return samp->samplerate;
}

nframes_t
Sample_frames_available(Sample samp) {
    assert(samp);
    return samp->frames - samp->frames_read;
}

int
Sample_reset(Sample samp) {
    assert(samp);
    return set_frames_read(samp, 0);
}

nframes_t
Sample_write_mono(Sample samp,
                  sample_t *ch1,
                  nframes_t frames) {
    assert(samp);
    return 0;
}

nframes_t
Sample_write_stereo(Sample samp,
                    sample_t *ch1,
                    sample_t *ch2,
                    nframes_t frames) {
    assert(samp);

    long frame = 0;
    long frames_read = 0;
    long frames_available = samp->frames - samp->frames_read;

    sample_t ch1samp, ch2samp;

    // flag to indicate we need to signal end of sample
    int hitend = 0;

    // pitch-adjusted frame index
    // will have to be cast before using to index into framebuf
    sample_count_t pfi = 0.0;

    // sample index
    long si = 0;

    // frame offset
    long offset = samp->frames_read * samp->channels;

    if (is_done(samp)) {
        return 0;
    }

    for (frame = 0; frame < frames; frame++) {
        // nudge frame index and sample index
        pfi += samp->pitch;
        frames_read = (long) pfi;
        si = frames_read * samp->channels;

        if (offset + si < samp->frames) {
            // sample values
            ch1samp = samp->framebuf[offset + si] * samp->gain;
            ch2samp = samp->framebuf[offset + si + 1] * samp->gain;

            switch(samp->channels) {
            case 1:
                ch1[frame] = ch2[frame] = ch1samp;
                break;
            case 2:
                ch1[frame] = ch1samp;
                ch2[frame] = ch2samp;
                break;
            }
        } else {
            hitend = 1;
            ch1[frame] = ch2[frame] = 0.0f;
        }
    }

    if (hitend) {
        set_frames_read(samp, samp->frames_read + frames_available);
        set_done(samp);
        // notify that we just finished reading this sample
        Event_broadcast(samp->done_event);
    } else {
        set_frames_read(samp, samp->frames_read + frames_read);
    }

    return 0;
}

int
Sample_wait(Sample samp) {
    assert(samp);
    return Event_wait(samp->done_event);
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    assert(samp && *samp);
    // free the done event
    Event_free(&(*samp)->done_event);
    // free the frames_read mutex
    Mutex_free(&(*samp)->frames_read_mutex);
    FREE((*samp)->framebuf);
}
