
#include <assert.h>
#include <string.h>

#include "jack-client.h"
#include "lightning-server.h"
#include "log.h"
#include "mem.h"
#include "osc-server.h"
#include "osc-types.h"
#include "samples.h"

struct LightningServer {
    JackClient jack_client;
    OscServer osc_server;
    Samples samples;
    int listenPort;
    int broadcastPort;
};

static void
osc_error_handler(int num,
                  const char *msg,
                  const char *where);
static int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data);

/**
 * Initialize jack_client and samples,
 * and use samples as the data for the jack_client
 * realtime callback.
 */
static void
initialize_jack_client(LightningServer server);

LightningServer
LightningServer_init(const char *listenPort,
                     const char *broadcastPort,
                     const char *broadcastHost)
{
    LightningServer server;
    NEW(server);
    initialize_jack_client(server);

    /* setup OSC server */

    server->osc_server = OscServer_init(listenPort,
                                        &osc_error_handler);

    OscServer_add_method(server->osc_server,
                         "/samples",
                         "iff",
                         play_sample,
                         server->samples);

    return server;
}

LightningServer
LightningServer_multicast(const char *group,
                          const char *listenPort,
                          const char *broadcastPort,
                          const char *broadcastHost)
{
    return NULL;
}

int
LightningServer_listen_port(LightningServer server)
{
    assert(server);
    return 0;
}

int
LightningServer_broadcast_port(LightningServer server)
{
    assert(server);
    return 0;
}

void
LightningServer_listen(LightningServer server)
{
    assert(server);
    while (1) {
        OscServer_recv(server->osc_server);
    }
}

void
LightningServer_free(LightningServer *server)
{
    assert(server && *server);
    LightningServer s = *server;
    Samples_free(&s->samples);
    JackClient_free(&s->jack_client);
}

static void
osc_error_handler(int num,
                  const char *msg,
                  const char *where)
{
    Log log = Log_init(NULL);
    LOG(log, Error, "[OSC Error %d] %s %s\n", num, where, msg);
}

static int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data)
{
    assert(0 == strcmp(types, "iff"));
    assert(argc == 3);
    /* Samples kit = (Kit) data; */
    /* int *samp_idx = (int *) argv[0]; */
    /* float *pitch = (float *) argv[2]; */
    /* float *gain = (float *) argv[2]; */
    /* Kit_play_index(kit, *samp_idx, *pitch, *gain); */
    return 0;
}

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data)
{
    Samples samples = (Samples) data;
    Samples_write(samples, buffers, channels, frames);
    return 0;
}

static void
initialize_jack_client(LightningServer server)
{
    server->jack_client = JackClient_init(audio_callback, NULL);
    server->samples = Samples_init(JackClient_samplerate(server->jack_client));
    JackClient_set_data(server->jack_client, server->samples);
    JackClient_setup_callbacks(server->jack_client);
    JackClient_activate(server->jack_client);
    JackClient_setup_ports(server->jack_client);
}
