#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "kit.h"
#include "safe-list.h"
#include "mem.h"
#include "sample.h"
#include "types.h"

struct Kit {
    /* actively playing Sample instances */
    SafeList active;
    /* number of samples loaded */
    unsigned int num_samples;
};

/* TODO: Kit should also spin up a thread that waits
   for sample trigger events */
Kit
Kit_load(const char *dir,
         nframes_t output_samplerate) {
    Kit kit;
    NEW(kit);
    /* TODO: better error-handling */
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

    /* kit->samples = CALLOC(MAX_SAMPLES, sizeof(Sample)); */

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

        Sample_load(sample_path, 1.0f, 1.0f, output_samplerate);

        file_index++;
    }

    kit->num_samples = file_index;

    kit->active = SafeList_init();

    return kit;
}

int
Kit_num_samples(Kit kit) {
    assert(kit);
    return kit->num_samples;
}

void
Kit_play_sample(Kit kit,
                int index) {
    assert(kit);
    if (index >= kit->num_samples) {
        fprintf(stderr, "index %d is greater than the number of samples "
                "in kit\n", index);
        exit(EXIT_FAILURE);
    } else {
        /* play sample */
    }
}

int
Kit_write(Kit kit,
          sample_t **buffers,
          channels_t channels,
          nframes_t frames) {
    int i = 0;
    int sample_write_error = 0;

    for (i = 0; i < kit->num_samples; i++) {
        // fill buffers with sample data
        /* sample_write_error = \ */
        /*     Sample_write(kit->samples[i], buffers, channels, frames); */
        if (sample_write_error) {
            return sample_write_error;
        }
    }

    return 0;
}

void
Kit_free(Kit *kit) {
    assert(kit && *kit);
}
