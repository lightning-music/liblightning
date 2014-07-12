#include <assert.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "sample.h"
#include "types.h"

struct Sample {
    const char *path;
    channels_t channels;
    nframes_t frames;
    int samplerate;
    sample_t *framebuf;
    pthread_cond_t done;
    pthread_mutex_t done_lock;
};

/**
 * Load an audio sample.
 */
Sample
Sample_load(const char *file) {
    Sample s;
    NEW(s);

    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(file, SFM_READ, &sfinfo);

    if (sf == NULL) {
        fprintf(stderr, "%s\n", sf_strerror(sf));
        exit(EXIT_FAILURE);
    }

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
        total_frames += sf_readf_float(sf,
                                       s->framebuf + (total_frames * s->channels),
                                       frames);
    }

    sf_close(sf);

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
Sample_sample_rate(Sample samp) {
    assert(samp);
    return samp->samplerate;
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    assert(samp && *samp);
}
