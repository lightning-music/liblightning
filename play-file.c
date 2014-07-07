/*
 * Use audio-engine to play a sound file.
 */
#include <assert.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jack-client.h"
#include "types.h"

typedef struct FilePlayer {
    SNDFILE *sf;
    struct SF_INFO sfinfo;
    int channels;
    int frames;
    long length; // in samples
    long samples_left;
    float *framebuf;
    pthread_cond_t done;
    pthread_mutex_t done_lock;
} FilePlayer;

/**
 * Initialize a FilePlayer object
 */
static void
initialize_file_player(FilePlayer *fp,
                       const char *f) {
    assert(fp);
    // initialize condition variable
    pthread_cond_init(&fp->done, NULL);
    pthread_mutex_init(&fp->done_lock, NULL);
    // open file
    fp->sf = sf_open(f, SFM_READ, &fp->sfinfo);
    // initialize data members
    fp->channels = fp->sfinfo.channels;
    fp->frames = fp->sfinfo.frames;
    // samples = frames * channels
    fp->length = fp->frames * fp->channels;
    fp->samples_left = fp->length;
    // allocate frame buffer
    fp->framebuf = malloc(sizeof(sample_t) * fp->length);
    // fill frame buffer
    while (sf_readf_float(fp->sf, fp->framebuf, fp->frames)) ;
    printf("locking fp->done\n");
    // lock the done mutex
    pthread_mutex_lock(&fp->done_lock);
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
    pthread_cond_destroy(&fp->done);
    // free frame buffer
    free(fp->framebuf); fp->framebuf = NULL;
}

// jack-client realtime callback
static int
audio_callback(sample_t *buf,
               nframes_t frames,
               void *data) {
    float *p = NULL;
    FilePlayer *fp = (FilePlayer *) data;
    long samples_left = fp->samples_left;
    long samples_to_copy = frames * fp->channels;
    // copy frames of data to buf
    if (samples_to_copy > samples_left) {
        // copy the remaining samples in framebuf
        memcpy(buf, fp->framebuf, samples_left);
        // 0 out all the others
        for (p = buf + samples_left; p < buf + samples_to_copy; p++) {
            *p = 0.0f;
        }
        p = NULL;
        // signal done playing audio file
        pthread_cond_broadcast(&fp->done);
    } else {    
        memcpy(buf, fp->framebuf, samples_to_copy);
    }

    // TODO: synchronization around decrementing samples_left
    fp->samples_left -= samples_to_copy;
    
    return 0;
}

int main(int argc, char **argv) {
    const char *f = "/home/brian/Audio/freesound/"
                    "marvie_baaaa_01.flac";

    FilePlayer fp;

    // initialize FilePlayer

    initialize_file_player(&fp, f);

    printf("done calling initialize_file_player\n");

    // initialize jack client

    JackClient jack_client = \
        JackClient_init(audio_callback, &fp);

    printf("reading audio file...\n");

    // wait to be done
    pthread_cond_wait(&fp.done, &fp.done_lock);

    // free FilePlayer

    free_file_player(&fp);

    return 0;
}
