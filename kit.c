#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "jack-client.h"
#include "kit.h"
#include "mem.h"
#include "sample.h"
#include "types.h"

struct Kit {
    JackClient jack_client;
    Sample *samples;
    unsigned int num_samples;
};

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data);

/* TODO: Kit should also spin up a thread that waits
   for sample trigger events */
Kit
Kit_load(const char *dir,
         nframes_t output_samplerate) {
    Kit kit;
    NEW(kit);

    /* TODO: better error-handling with errno */
    /* read .kit */
    char kitfile_path[PATH_MAX];
    char sample_path[PATH_MAX];

    sprintf(kitfile_path, "%s/%s", dir, ".kit");

    size_t path_max = PATH_MAX;
    char *f = NULL;
    int file_index = 0;
    FILE *kitfile = fopen(kitfile_path, "r");

    if (NULL == kitfile) {
        fprintf(stderr, "Could not open %s\n", kitfile_path);
        exit(EXIT_FAILURE);
    } else {
        printf("opened %s\n", kitfile_path);
    }

    kit->samples = CALLOC(MAX_SAMPLES, sizeof(Sample));

    printf("allocated sample buffer\n");

    // read sample paths
    ssize_t read = 0;
    while (file_index < MAX_SAMPLES) {
        read = getline(&f, &path_max, kitfile);

        if (-1 == read) {
            break;
        }

        // get rid of the newline
        f[read - 1] = '\0';

        sample_path[0] = '\0';

        sprintf(sample_path, "%s/%s", dir, f);

        kit->samples[file_index] = Sample_load(sample_path, 1.0f, 1.0f,
                                               output_samplerate);

        file_index++;
    }

    kit->num_samples = file_index;

    // initialize jack client

    kit->jack_client = JackClient_init(audio_callback, kit);

    return kit;
}

int
Kit_num_samples(Kit kit) {
    assert(kit);
    return kit->num_samples;
}

Sample *
Kit_sample_list(Kit kit) {
    assert(kit);
    return kit->samples;
}

void
Kit_play_sample(Kit kit,
                Sample samp) {
    assert(kit);
}

AudioCallback
Kit_get_audio_callback(Kit kit) {
    return audio_callback;
}

void
Kit_free(Kit *kit) {
    assert(kit && *kit);
}

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data) {
    Kit kit = (Kit) data;
    int i;

    for (i = 0; i < kit->num_samples; i++) {
        // fill buffers with sample data
        Sample_write(kit->samples[i], buffers, channels, frames);
    }

    return 0;
}

