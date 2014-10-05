#include "includes.h"
#include "zhelpers.h"

int
main(int argc, char **argv)
{
    void *ctx = zmq_ctx_new();
    void *sub = zmq_socket(ctx, ZMQ_SUB);
    int rc = zmq_connect(sub, "tcp://localhost:5556");
    assert(rc == 0);
    // subscribe to zipcode, default is NYC, 10001
    char *filter = (argc > 1) ? argv[1] : "10001 ";
    rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);
    // process 100 updates
    int nupdates;
    long total_temperature = 0;
    for (nupdates = 0; nupdates < 100; nupdates++) {
        char *string = s_recv(sub);
        int zipcode, temperature, relhumidity;
        sscanf(string, "%d %d %d", &zipcode, &temperature, &relhumidity);
        total_temperature += temperature;
        free(string);
    }
    printf ("Average temperature for zipcode '%s' was %dF\n",
            filter, (int) (total_temperature / nupdates));

    zmq_close(sub);
    zmq_ctx_destroy(ctx);
    return 0;
}
