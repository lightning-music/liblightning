/*
 * A working example of how to play samples with jack-client
 */
#include <assert.h>
#include <pthread.h>
#include <samplerate.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../jack-client.h"
#include "../src.h"
#include "../types.h"

typedef enum {
    FP_UNSAFE,
    FP_INITIALIZED
} FP_STATE;

typedef struct FilePlayer {
    // state variable
    FP_STATE state;
    // sndfile pointer
    SNDFILE *sf;
    // number of channels
    int channels;
    // number of frames
    int frames;
    // sample rate
    nframes_t samplerate;
    // length in samples = channels * frames
    long length;
    // frames we've read
    long frames_read;
    // buffer holding the entire sample
    sample_t **framebufs;
    // mutex and conditional variable
    // used to signal sample done
    pthread_cond_t done;
    pthread_mutex_t done_lock;
    // sample rate converter
    SRC src;
    double src_ratio;
} FilePlayer;

/**
 * Initialize a FilePlayer object
 */
static void
initialize_file_player(FilePlayer *fp,
                       const char *f,
                       nframes_t output_sample_rate) {
    assert(fp);
    fp->state = FP_UNSAFE;
    // initialize condition variable
    pthread_cond_init(&fp->done, NULL);
    pthread_mutex_init(&fp->done_lock, NULL);
    // open file
    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(f, SFM_READ, &sfinfo);
    // initialize data members
    fp->channels = sfinfo.channels;
    fp->frames = sfinfo.frames;
    fp->samplerate = sfinfo.samplerate;
    fp->src_ratio = output_sample_rate / (double) fp->samplerate;
    // samples = frames * channels
    fp->length = fp->frames * fp->channels;
    // frames we have already read from the frame buffer
    fp->frames_read = 0;
    // allocate per-channel frame buffers
    fp->framebufs = calloc(fp->channels, SAMPLE_SIZE);
    int i;
    for (i = 0; i < fp->channels; i++) {
        fp->framebufs[i] = malloc(SAMPLE_SIZE * fp->frames);
    }

    assert(fp->framebufs);
    // allocate the buffer for sf_read
    sample_t framebuf[ fp->channels * fp->frames ];
    // fill frame buffer
    int frames_to_read = 4096;
    long total_frames_read = sf_readf_float(sf, framebuf, frames_to_read);
    while (total_frames_read < fp->frames) {
        // adjust buffer pointer
        total_frames_read +=                    \
            sf_readf_float(sf,
                           framebuf + (total_frames_read * fp->channels),
                           frames_to_read);
    }

    if (total_frames_read != fp->frames) {
        fprintf(stderr, "Could not read expected number of frames from %s\n", f);
        exit(EXIT_FAILURE);
    }

    // de-interleave
    int j;
    for (i = 0; i < fp->channels; i++) {
        for (j = 0; j < fp->frames; j++) {
            fp->framebufs[i][j] = framebuf[ j + i ];
        }
    }

    /* lock the done mutex */
    pthread_mutex_lock(&fp->done_lock);
    /* close file */
    sf_close(sf);
    /* initialize sample rate converter */
    fp->src = SRC_init(2);
    fp->state = FP_INITIALIZED;
}

/**
 * Destroy a FilePlayer object
 */
static void
free_file_player(FilePlayer *fp) {
    assert(fp);
    // free condition variable
    pthread_cond_destroy(&fp->done);
    // free frame buffer
    int i;
    for (i = 0; i < fp->channels; i++) {
        free(fp->framebufs[i]); fp->framebufs[i] = NULL;
    }
}

/**
 * jack client realtime callback
 */
static int
audio_callback(sample_t *ch1,
               sample_t *ch2,
               nframes_t frames,
               void *data) {
    FilePlayer *fp = (FilePlayer *) data;

    if (fp->state != FP_INITIALIZED) {
        return 0;
    }

    nframes_t frames_read = fp->frames_read;
    int channels = fp->channels;
    long offset = frames_read * channels;
    AudioData audio_data;
    int end_of_input = 0;

    audio_data.input_frames = fp->frames - frames_read;
    audio_data.output_frames = frames;

    /* process channel 1 */
    audio_data.output = ch1;
    audio_data.input = &fp->framebufs[0][ offset ];

    nframes_t input_frames_used = 0;
    nframes_t output_frames_gen = 0;

    int error = SRC_process(fp->src,
                            fp->src_ratio,
                            audio_data,
                            &input_frames_used,
                            &output_frames_gen,
                            &end_of_input);

    if (error) {
        fprintf(stderr, "Error in SRC_process: %s\n",
                SRC_strerror(error));
        exit(EXIT_FAILURE);
    }

    if (end_of_input && output_frames_gen == 0) {
        pthread_cond_broadcast(&fp->done);
    } else {
        fp->frames_read += input_frames_used;
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

    // initialize jack client

    JackClient jack_client = \
        JackClient_init(NULL, audio_callback, &fp);

    // initialize FilePlayer

    initialize_file_player(&fp, argv[1],
                           JackClient_samplerate(jack_client));

    JackClient_setup_callbacks(jack_client);
    JackClient_activate(jack_client);
    JackClient_setup_ports(jack_client);

    // wait to be done

    pthread_cond_wait(&fp.done, &fp.done_lock);

    // free FilePlayer

    free_file_player(&fp);

    // free jack client
    
    JackClient_free(&jack_client);

    return 0;
}
