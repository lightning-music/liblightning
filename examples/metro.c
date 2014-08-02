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

/**
 * Broken because I am not adding signals properly.
 * Next question is how to implement Kit signal summing.
 * Requires an aux buffer:
 * for all i in 0..n-1 aux[i] = buf1[i] + buf2[i]
 * One way to do this would be to have JackClient
 * allocate a certain number of sample buffers when
 * it is initialized that clients could check out to
 * use as auxiliary buffers for doing things like summing.
 * 
 * Kit will have to use two auxiliary buffers.
 * The first will be for storing the result of the
 * cumulative summing.
 * The second will be to collect the output of each active
 * sample.
 *
 * Could kit just allocate these buffers itself?
 * We could just allocate buffers that would each be able
 * to hold up to 2048 samples.
 *
 * The algorithm using this approach would be
 * Let each sample write to collect buffer,
 * and after each pass through the collect buffer,
 * sum it into sum buffer.
 */
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
