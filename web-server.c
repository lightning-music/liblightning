
#include <assert.h>
#include <libwebsockets.h>
#include <stddef.h>

#include "log.h"
#include "mem.h"
#include "web-server.h"

#define TIMEOUT_MS 20

struct WebServer {
    struct libwebsocket_context *context;
};

static int
callback_http(struct libwebsocket_context *context,
              struct libwebsocket *wsi,
              enum libwebsocket_callback_reasons reason,
              void *user,
              void *in,
              size_t len);

static struct libwebsocket_protocols protocols[] = {
    {"http-only",      callback_http,            0},
    { NULL,            NULL,                     0}
};

WebServer
WebServer_init(int port)
{
    WebServer ws;
    NEW(ws);
    struct lws_context_creation_info creation_info;
    /* creation_info.iface = NULL; */
    creation_info.port = port;
    creation_info.protocols = protocols;
    creation_info.extensions = libwebsocket_get_internal_extensions();
    /* creation_info.ssl_cert_filepath = NULL; */
    /* creation_info.ssl_private_key_filepath = NULL; */
    /* creation_info.ssl_ca_filepath = NULL; */
    /* creation_info.ssl_cipher_list = NULL; */
    /* creation_info.http_proxy_address = NULL; */
    creation_info.options = 0;
    ws->context = libwebsocket_create_context(&creation_info);

    if (ws->context == NULL) {
        LOG(Error, "could not create %s context", "libwebsockets");
        return NULL;
    }

    return ws;
}

void
WebServer_listen(WebServer ws)
{
    while (0 == libwebsocket_service(ws->context, TIMEOUT_MS)) ;
}

void
WebServer_free(WebServer *ws)
{
    assert(ws && *ws);
    libwebsocket_context_destroy((*ws)->context);
    FREE(*ws);
}

static int
callback_http(struct libwebsocket_context *context,
              struct libwebsocket *wsi,
              enum libwebsocket_callback_reasons reason,
              void *user,
              void *in,
              size_t len)
{
    return 0;
}
