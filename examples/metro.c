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
    const char *path0 = "/lightning/kits/default/samples/0";
    const char *path1 = "/lightning/kits/default/samples/1";
    const char *path2 = "/lightning/kits/default/samples/2";
    
    while (1) {
        /* sent = lo_send(addr, path0, "ff", 1.0, 1.0); */
        /* if (sent == -1) { */
        /*     fprintf(stderr, "Could not send OSC message (%s)\n", */
        /*             lo_address_errstr(addr)); */
        /*     return EXIT_FAILURE; */
        /* } */
        sent = lo_send(addr, path2, "ff", 1.0, 1.0);
        if (sent == -1) {
            fprintf(stderr, "Could not send OSC message (%s)\n",
                    lo_address_errstr(addr));
            return EXIT_FAILURE;
        }
        usleep(ms * 250);
    }
    
    return 0;
}
