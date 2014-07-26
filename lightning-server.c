#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kit.h"
#include "osc-server.h"

void
osc_error_handler(int num,
                  const char *msg,
                  const char *where) {
    fprintf(stderr, "[OSC Error] %s %s\n", where, msg);
}

int main(int argc, char **argv) {

    /* setup kit */

    const char * default_kit = "kits/default";

    Kit kit = Kit_load(default_kit);

    /* setup OSC server */

    OscServer osc_server = OscServer_init("41068",
                                          &osc_error_handler);

    /* free osc server */

    OscServer_free(&osc_server);

    /* free kit */

    Kit_free(&kit);

    return 0;
}
