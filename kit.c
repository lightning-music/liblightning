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
#include "mem.h"
#include "sample.h"

struct Kit {
    Sample *samples;
    unsigned int num_samples;
};

Kit
Kit_load(const char *dir,
         AudioEngine engine) {
    int i;
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

        kit->samples[file_index] = Sample_load(sample_path);

        file_index++;
    }

    kit->num_samples = file_index;

    // initialize samples

    /* for (i = 0; i < file_index; i++) { */
    /*     kit->samples[i] = Sample_load(files[file_index]); */
    /* } */

    return kit;
}

unsigned int
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

/**
 * Free the samples.
 */
void
Kit_free(Kit *kit) {
    assert(kit && *kit);
}
