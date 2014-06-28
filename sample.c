#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "sample.h"
#include "types.h"

struct Sample {
    kc_size_t num_frames;
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

    s->sf = sf_open(file, SFM_READ, s->sfinfo);
    if (s->sf == NULL) {
        fprintf(stderr, "%s\n", sf_strerror(s->sf));
        exit(EXIT_FAILURE);
    }

    return s;
}

/**
 * Number of frames in the audio sample.
 */
kc_size_t
Sample_num_frames(Sample samp) {
    assert(samp);
    return (kc_size_t) samp->sf->frames;
}

/**
 * Sample rate of the audio sample.
 */
int
Sample_sample_rate(Sample samp) {
    assert(samp);
    return samp->sf->samplerate;
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    assert(samp && *samp);
    sf_close((*samp)->sf);
}
