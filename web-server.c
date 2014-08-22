
#include <assert.h>
#include <libwebsockets.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    creation_info.iface = NULL;
    creation_info.port = port;
    creation_info.protocols = protocols;
    creation_info.extensions = libwebsocket_get_internal_extensions();
    creation_info.ssl_cert_filepath = NULL;
    creation_info.ssl_private_key_filepath = NULL;
    creation_info.ssl_ca_filepath = NULL;
    creation_info.ssl_cipher_list = NULL;
    creation_info.http_proxy_address = NULL;
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
    switch(reason) {
    case LWS_CALLBACK_CLIENT_WRITEABLE: {
        printf("connection established\n");
        break; }
    case LWS_CALLBACK_HTTP: {
        char *request_uri = (char *) in;
        printf("request uri: %s\n", request_uri);

        if (0 == strcmp(request_uri, "/")) {
            void *universal_response = "Hello, World!";
            libwebsocket_write(wsi, universal_response,
                               strlen(universal_response), LWS_WRITE_HTTP);
        } else {
            char cwd[1024];
            char *resource_path;

            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                resource_path = malloc(strlen(cwd) + strlen(request_uri));

                sprintf(resource_path, "%s%s", cwd, request_uri);
                printf("resource path: %s\n", resource_path);

                char *extension = strrchr(resource_path, '.');
                char *mime;

                if (extension == NULL) {
                    mime = "text/plain";
                } else if (0 == strcmp(extension, ".png")) {
                    mime = "image/png";
                } else if (0 == strcmp(extension, ".jpg")) {
                    mime = "image/jpg";
                } else if (0 == strcmp(extension, ".gif")) {
                    mime = "image/gif";
                } else if (0 == strcmp(extension, ".html")) {
                    mime = "text/html";
                } else if (0 == strcmp(extension, ".css")) {
                    mime = "text/css";
                } else if (0 == strcmp(extension, ".js")) {
                    mime = "text/javascript";
                } else {
                    mime = "text/plain";
                }

                const char *additional_headers = NULL;
                libwebsockets_serve_http_file(context, wsi,
                                              resource_path, mime,
                                              additional_headers);
            }
        }

        /* this may be unnecessary */
        /* libwebsocket_close_and_free_session(context, wsi, */
        /*                                     LWS_CLOSE_STATUS_NORMAL); */

        break; }
    default: {
        /* printf("unhandled callback type\n"); */
        break; }
    }
    return 0;
}
