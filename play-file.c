/*
 * Use audio-engine to play a sound file.
 */
#include <assert.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "jack-client.h"
#include "types.h"

typedef struct FilePlayer {
    SNDFILE *sf;
    struct SF_INFO sfinfo;
    long length; // in samples
    float *framebuf;
    pthread_cond_t ready;
} FilePlayer;

/**
 * Initialize a FilePlayer object
 */
static void
initialize_file_player(FilePlayer *fp,
                       const char *f) {
    assert(fp);
    // initialize condition variable
    pthread_cond_init(&fp->ready, NULL);
    // open file
    fp->sf = sf_open(f, SFM_READ, &fp->sfinfo);
    fp->length = fp->sfinfo.frames * fp->sfinfo.channels;
    // allocate frame buffer
    fp->framebuf = malloc(sizeof(sample_t) * fp->length);
    // read frame buffer
    while (sf_readf_float(fp->sf, fp->framebuf, fp->sfinfo.frames)) ;
}

/**
 * Destroy a FilePlayer object
 */
static void
free_file_player(FilePlayer *fp) {
    assert(fp);
    // close file
    sf_close(fp->sf);
    // free condition variable
    pthread_cond_destroy(&fp->ready);
    // free frame buffer
    free(fp->framebuf); fp->framebuf = NULL;
    free(fp); fp = NULL;
}

// jack realtime callback
static int
audio_callback(sample_t *buf,
               nframes_t frames,
               void *data) {
    return 0;
}

int main(int argc, char **argv) {
    const char *f = "/home/brian/Audio/freesound/playing_in_the_backyard_with_diggers.flac";

    FilePlayer fp;

    // initialize FilePlayer

    initialize_file_player(&fp, f);

    // initialize jack client

    JackClient jack_client = \
        JackClient_init(audio_callback, NULL);

    // free FilePlayer

    free_file_player(&fp);

    return 0;
}
