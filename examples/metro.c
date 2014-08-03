/* feature test macro for usleep(2) */
#define _XOPEN_SOURCE 500

#include <lo/lo.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
usage(char *prog) {
    fprintf(stderr, "Usage");
    fprintf(stderr, "$ %s command [options]\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "commands\n");
    fprintf(stderr, "    play <sample id> [pitch] [gain]\n");
    fprintf(stderr, "\n");
}

int main(int argc, char **argv) {

    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int sent;
    long ms = 1000;
    lo_address addr = lo_address_new(NULL, "41068");
    const char *path = "/lightning/kits/default/samples";

    int sample_index = 0;
    int max_samples = 4;
    while (1) {
        sample_index = (sample_index + 1) % max_samples;
        sent = lo_send(addr, path, "iff", sample_index, 1.0, 1.0);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
        usleep(ms * 250);
    }
    
    return 0;
}
