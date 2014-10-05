#include "includes.h"
#include "zhelpers.h"

int
main(int argc, char **argv)
{
    void *ctx = zmq_ctx_new();
    void *sink = zmq_socket(ctx, ZMQ_PULL);
    int rc = zmq_connect(sink, "tcp://localhost:5557");
    assert(rc == 0);
    while (1) {
        char *string = s_recv(sink);
        printf("%s.", string);
        fflush(stdout);
        s_sleep(atoi(string));
        free(string);
    }
    zmp_close(sink);
    zmq_ctx_destroy(ctx);
    return 0;
}
