#include <assert.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "clip.h"
#include "mem.h"
#include "sample.h"
#include "types.h"

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
    channels_t channels;
    nframes_t frames;
    int samplerate;
    // buffer to hold sample data
    sample_t *framebuf;
    // track read position in file
    nframes_t frames_read;
    // provide a way to synchronize a thread on the
    // `done` event
    int is_done;
    pthread_cond_t done;
    pthread_mutex_t done_lock;
};

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

    s->pitch = clip(pitch, -32.0f, 32.0f);
    s->gain = clip(gain, 0.0f, 1.0f);
    s->frames = sfinfo.frames;
    s->channels = sfinfo.channels;
    s->samplerate = sfinfo.samplerate;

    pthread_cond_init(&s->done, NULL);
    pthread_mutex_init(&s->done_lock, NULL);

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

    s->is_done = 0;
    pthread_mutex_lock(&s->done_lock);

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

    // pitch-adjusted frame index
    // will have to be cast before using to index into framebuf
    sample_count_t pfi = 0.0;

    // sample index
    long si = 0;

    // frame offset
    long offset = samp->frames_read * samp->channels;

    if (samp->is_done) {
        return 0;
    }

    if (samp->frames_read < samp->frames - (frames * samp->pitch)) {
        // we can fill the whole buffer with sample data
        switch (samp->channels) {
        case 1:
            // fill stereo buffers with mono data
            for (frame = 0; frame < frames; frame++) {
                pfi += samp->pitch;
                frames_read = (long) pfi;
                si = frames_read * samp->channels;

                ch1[frame] = ch2[frame] =                               \
                    samp->framebuf[offset + si] * samp->gain;
            }
            break;
        case 2:
            // de-interleave
            for (frame = 0; frame < frames; frame++) {
                pfi += samp->pitch;
                frames_read = (long) pfi;
                si = frames_read * samp->channels;

                ch1[frame] =                                            \
                    samp->framebuf[offset + si] * samp->gain;

                ch2[frame] =                                            \
                    samp->framebuf[offset + si + 1] * samp->gain;
            }
            break;
        }

        samp->frames_read += frames_read;
    } else {
        // read the remaining samples, mark the sample as done,
        // and fill the rest of the buffer with zeroes
        long frames_available = samp->frames - samp->frames_read;

        switch (samp->channels) {
        case 1:
            // fill stereo buffers with mono data
            for (frame = 0; frame < frames_available; frame++) {
                pfi += samp->pitch;
                frames_read = (long) pfi;
                si = frames_read * samp->channels;

                ch1[frame] = ch2[frame] =                               \
                    samp->framebuf[offset + si];
            }
            break;
        case 2:
            // de-interleave
            for (frame = 0; frame < frames_available; frame++) {
                pfi += samp->pitch;
                frames_read = (long) pfi;
                si = frames_read * samp->channels;

                if (offset + si < samp->frames) {
                    ch1[frame] =                                    \
                        samp->framebuf[offset + si] * samp->gain;
                    
                    ch2[frame] =                                        \
                        samp->framebuf[offset + si + 1] * samp->gain;
                } else {
                    ch1[frame] = ch2[frame] = 0.0f;
                }
            }
            break;
        }

        for ( ; frame < frames; frame++) {
            ch1[frame] = ch2[frame] = 0.0f;
        }

        samp->frames_read += frames_available;
        samp->is_done = 1;
        // note that done_lock was locked in Sample_load
        pthread_cond_broadcast(&samp->done);
    }

    return 0;
}

int
Sample_is_done(Sample samp) {
    assert(samp);
    return samp->is_done;
}

int
Sample_wait(Sample samp) {
    assert(samp);
    return pthread_cond_wait(&samp->done,
                             &samp->done_lock);
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    assert(samp && *samp);

    pthread_cond_destroy(&(*samp)->done);
    pthread_mutex_destroy(&(*samp)->done_lock);

    FREE((*samp)->framebuf);
}
