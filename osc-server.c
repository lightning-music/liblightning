#include <assert.h>
#include <lo/lo_lowlevel.h>

#include "mem.h"
#include "osc-server.h"
#include "osc-types.h"

struct OscServer {
    lo_server_thread lo_server;
};

OscServer
OscServer_init(const char *port,
               OscErrorHandler err_handler) {
    OscServer srv;
    NEW(srv);
    srv->lo_server = lo_server_new(port, err_handler);
    return srv;
}

OscMethod
OscServer_add_method(OscServer srv,
                     const char *path,
                     const char *typespec,
                     OscMethodHandler handler,
                     void *data) {
    assert(srv);
    return lo_server_add_method(srv->lo_server, path, typespec,
                                handler, data);
}

int
OscServer_recv(OscServer srv) {
    assert(srv);
    return lo_server_recv(srv->lo_server);
}

void
OscServer_free(OscServer *srv) {
    assert(srv && *srv);
    lo_server_thread_free((*srv)->lo_server);
    FREE(*srv);
}
