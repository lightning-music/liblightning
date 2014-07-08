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
    // TODO: get rid of sf and sfinfo
    SNDFILE *sf;
    struct SF_INFO sfinfo;
    // number of channels
    int channels;
    // number of frames
    int frames;
    // length in samples = channels * frames
    long length;
    // TODO: get rid of this var and expose methods to manipulate framep instead
    long samples_left;
    // buffer holding the entire sample
    sample_t *framebuf;
    // frame pointer
    sample_t *framep;
    // frame pointer mutex
    pthread_mutex_t framep_lock;
    // mutex and conditional variable
    // used to signal sample done
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
    // count for the samples we have left to play in this file
    fp->samples_left = fp->length;
    // allocate frame buffer
    printf("%s length = %ld samples\n", f, fp->length);
    fp->framebuf = malloc(sizeof(sample_t) * fp->length);
    // fill frame buffer
    printf("reading %s\n", f);
    int frames_to_read = 4096;
    float *p = fp->framebuf;
    long frames_read = sf_readf_float(fp->sf, p, frames_to_read);
    while (frames_read == frames_to_read) {
        /* printf("read %ld frames\n", frames_read); */
        // adjust buffer pointer
        p += frames_read * fp->channels;
        frames_read = sf_readf_float(fp->sf, p, frames_to_read);
    }
    // lock the done mutex
    pthread_mutex_lock(&fp->done_lock);
    printf("done reading %s\n", f);
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
    long len = fp->length;
    long samples_left = fp->samples_left;
    long samples_to_copy = frames * fp->channels;
    long sample_offset = len - samples_left;
    // copy frames of data to buf
    if (samples_to_copy > samples_left) {
        // copy the remaining samples in framebuf
        memcpy(buf, fp->framebuf + sample_offset, samples_left);
        // 0 out all the others
        for (p = buf + samples_left; p < buf + samples_to_copy; p++) {
            *p = 0.0f;
        }
        p = NULL;
        // signal done playing audio file
        pthread_cond_broadcast(&fp->done);
        // we've played the last samples
        fp->samples_left = 0;
    } else {
        memcpy(buf, fp->framebuf + sample_offset, samples_to_copy);
        // TODO: synchronization around decrementing samples_left
        // NOTE: this would be problematic (not good to call pthread_mutex_lock in the jack realtime callback)
        fp->samples_left -= samples_to_copy;
    }
    
    return 0;
}

int main(int argc, char **argv) {
    const char *f = "/home/brian/Audio/freesound/"
                    "marvie_baaaa_01.flac";

    FilePlayer fp;

    // initialize FilePlayer

    initialize_file_player(&fp, f);

    printf("playing %s\n", f);

    // initialize jack client

    JackClient jack_client = \
        JackClient_init(audio_callback, &fp);

    // wait to be done
    pthread_cond_wait(&fp.done, &fp.done_lock);

    // free FilePlayer

    free_file_player(&fp);

    // free jack client
    
    JackClient_free(&jack_client);

    return 0;
}
