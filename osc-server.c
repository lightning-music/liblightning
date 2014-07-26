#include <assert.h>
#include <lo/lo_lowlevel.h>
#include <lo/lo_serverthread.h>

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
    srv->lo_server = lo_server_thread_new(port, err_handler);
    return srv;
}

int
OscServer_start(OscServer srv) {
    assert(srv);
    return lo_server_thread_start(srv->lo_server);
}

OscMethod
OscServer_add_method(OscServer srv,
                     const char *path,
                     const char *typespec,
                     OscMethodHandler handler,
                     void *data) {
    assert(srv);
    return lo_server_thread_add_method(srv->lo_server, path, typespec,
                                       handler, data);
}

int
OscServer_stop(OscServer srv) {
    assert(srv);
    return lo_server_thread_stop(srv->lo_server);
}

void
OscServer_free(OscServer *srv) {
    assert(srv && *srv);
    
    FREE(*srv);
}
