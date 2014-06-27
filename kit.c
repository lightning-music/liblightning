#include <assert.h>
#include <dirent.h>
#include <limits.h>
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
Kit_load(const char *dir) {
    // TODO: better error-handling with errno

    /* read .kit */
    char kitfile_path[PATH_MAX];
    sprintf(kitfile_path, "%s/%s", dir, ".kit");
    size_t path_max = PATH_MAX;
    /* char f[PATH_MAX]; */
    char *f;
    int file_index = 0;
    char files[MAX_SAMPLES][PATH_MAX];
    FILE *kitfile = fopen(kitfile_path, "r");
    if (NULL == kitfile) {
        fprintf(stderr, "Could not open %s\n", kitfile_path);
        exit(EXIT_FAILURE);
    } else {
        printf("reading samples from %s\n", kitfile_path);
    }

    ssize_t chars_read = 0;
    while (file_index < MAX_SAMPLES) {
        chars_read = getline(&f, &path_max, kitfile);
        if (-1 == chars_read) {
            break;
        }
        f[chars_read - 2] = '\0';
        memcpy(files[file_index], f, PATH_MAX);
        printf("read file %d\n", file_index);
        file_index++;
    }

    int i;
    for (i = 0; i < file_index; i++) {
        printf("found sample %s\n", files[i]);
    }

    /* Sample *samples; */

    Kit kit;
    NEW(kit);

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
                Sample samp,
                AudioEngine engine) {
    assert(kit);
}

/**
 * Free the samples.
 */
void
Kit_free(Kit *kit) {
    assert(kit && *kit);
}
