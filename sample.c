#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "sample.h"
#include "types.h"

struct Sample {
    const char *path;
    sample_t *framebuf;
    SNDFILE *sf;
    SF_INFO *sfinfo;
};

/**
 * Load an audio sample.
 */
Sample
Sample_load(const char *file) {
    Sample s;
    NEW(s);
    NEW(s->sfinfo);

    s->sf = sf_open(file, SFM_READ, s->sfinfo);

    if (s->sf == NULL) {
        fprintf(stderr, "%s\n", sf_strerror(s->sf));
        exit(EXIT_FAILURE);
    }

    s->path = file;

    // frame buffer contains n samples,
    // where n = frames * channels

    s->framebuf = CALLOC(s->sfinfo->frames * s->sfinfo->channels,
                         sizeof(sample_t));

    // fill the frame buffer

    sf_count_t frames = 1024 * s->sfinfo->channels;
    while (sf_readf_float(s->sf, s->framebuf, frames)) ;

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
    return (nframes_t) samp->sfinfo->frames;
}

/**
 * Sample rate of the audio sample.
 */
int
Sample_sample_rate(Sample samp) {
    assert(samp);
    return samp->sfinfo->samplerate;
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    assert(samp && *samp);
    sf_close((*samp)->sf);
}
