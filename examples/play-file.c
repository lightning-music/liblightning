/*
 * A working example of how to play samples with jack-client
 */
#include <assert.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../jack-client.h"
#include "../types.h"

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
    // frames we've read
    long frames_read;
    // buffer holding the entire sample
    sample_t *framebuf;
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
    // frames we have already read from the frame buffer
    fp->frames_read = 0;
    // allocate frame buffer
    fp->framebuf = malloc(sizeof(sample_t) * fp->length);
    assert(fp->framebuf);
    // fill frame buffer
    int frames_to_read = 4096;
    long total_frames_read = sf_readf_float(fp->sf, fp->framebuf, frames_to_read);
    while (total_frames_read < fp->frames) {
        // adjust buffer pointer
        total_frames_read +=                    \
            sf_readf_float(fp->sf,
                           fp->framebuf + (total_frames_read * fp->channels),
                           frames_to_read);
    }
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

/**
 * jack client realtime callback
 */
static int
audio_callback(sample_t *ch1,
               sample_t *ch2,
               nframes_t frames,
               void *data) {
    long frame;
    FilePlayer *fp = (FilePlayer *) data;
    int channels = fp->channels;
    long offset = fp->frames_read * channels;

    if (fp->frames_read == fp->frames) {
        // we've played the whole buffer
        pthread_cond_broadcast(&fp->done);
    } else if (fp->frames_read < fp->frames - frames) {
        // we can read as many frames as jack is asking for
        switch(channels) {
        case 1:
            for (frame = 0; frame < frames; frame++) {
                ch1[frame] = ch2[frame] = fp->framebuf[offset + (frame * channels)];
            }
            break;
        case 2:
            for (frame = 0; frame < frames; frame++) {
                ch1[frame] = fp->framebuf[offset + (frame * channels)    ];
                ch2[frame] = fp->framebuf[offset + (frame * channels) + 1];
            }
            break;
        }

        fp->frames_read += frames;
    } else {
        // we can read less than we're being asked to
        // zero out the remaining samples
        long frames_available = fp->frames - fp->frames_read;
        
        switch(channels) {
        case 1:
            for (frame = 0; frame < frames_available; frame++) {
                ch1[frame] = ch2[frame] = fp->framebuf[offset + (frame * channels)];
            }
            break;
        case 2:
            for (frame = 0; frame < frames_available; frame++) {
                ch1[frame] = fp->framebuf[offset + (frame * channels)    ];
                ch2[frame] = fp->framebuf[offset + (frame * channels) + 1];
            }
            for ( ; frame < frames; frame++) {
                ch1[frame] = ch2[frame] = 0.0f;
            }
            break;
        }

        fp->frames_read += frames_available;
    }

    return 0;
}

void
usage_and_exit(char *prog) {
    fprintf(stderr, "Usage\n");
    fprintf(stderr, "$ %s path/to/audio/file\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage_and_exit(argv[0]);
    }

    FilePlayer fp;

    // initialize FilePlayer

    initialize_file_player(&fp, argv[1]);

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
