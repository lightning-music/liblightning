#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "jack-client.h"
#include "sample.h"

void
usage_and_exit(char *prog) {
    printf("Usage\n");
    printf("%s path/to/audio/file\n", prog);
    exit(EXIT_FAILURE);
}

int
main(int argc, char **argv) {
    if (argc < 2) {
        usage_and_exit(argv[0]);
    }

    Sample s = Sample_load(argv[1]);

    Sample_free(&s);

    return 0;
}
