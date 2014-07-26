#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kit.h"
#include "osc-server.h"
#include "osc-types.h"

void
osc_error_handler(int num,
                  const char *msg,
                  const char *where);

int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

int main(int argc, char **argv) {

    /* setup kit */

    const char * default_kit = "kits/default";

    Kit kit = Kit_load(default_kit);

    /* setup OSC server */

    OscServer osc_server = OscServer_init("41068",
                                          &osc_error_handler);

    OscServer_add_method(osc_server,
                         "/lightning/kit/1/samples/1",
                         "i",
                         play_sample,
                         kit);

    /* OSC receive loop */

    while (1) {
        OscServer_recv(osc_server);
    }

    /* free osc server */

    OscServer_free(&osc_server);

    /* free kit */

    Kit_free(&kit);

    return 0;
}

void
osc_error_handler(int num,
                  const char *msg,
                  const char *where) {
    fprintf(stderr, "[OSC Error] %s %s\n", where, msg);
}

int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data) {
    printf("path %s\n", path);
    printf("types %s\n", types);
    return 0;
}
