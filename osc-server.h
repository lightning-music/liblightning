/**
 * OSC server
 */
#ifndef OSC_SERVER_H_INCLUDED
#define OSC_SERVER_H_INCLUDED

#include "osc-types.h"

typedef struct OscServer *OscServer;

OscServer
OscServer_init(const char *port,
               OscErrorHandler err_handler);

int
OscServer_start(OscServer srv);

OscMethod
OscServer_add_method(OscServer srv,
                     const char *path,
                     const char *typespec,
                     OscMethodHandler handler,
                     void *data);
int
OscServer_stop(OscServer srv);

void
OscServer_free(OscServer *srv);

#endif
