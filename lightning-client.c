#include <lo/lo.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

    lo_address addr = lo_address_new(NULL, "41068");

    if (lo_send(addr, "/lightning/kit/1/samples/1", "i", 1) == -1) {
        fprintf(stderr, "Could not send OSC message (%s)\n",
                lo_address_errstr(addr));
        return EXIT_FAILURE;
    }
    
    return 0;
}