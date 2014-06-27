#include <assert.h>
#include <dirent.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
    DIR *dirp = opendir(dir);
    if (NULL == dirp) {
        fprintf(stderr, "Could not open %s\n", dir);
        exit(EXIT_FAILURE);
    }

    struct dirent *ent = readdir(dirp);
    while (NULL != ent) {
    }

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
