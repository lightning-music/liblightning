
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

/* OSC handlers */

static void
osc_error_handler(int num,
                  const char *msg,
                  const char *where);

static int
add_dir(const char *path,
        const char *types,
        OscArgument **argv,
        int argc,
        OscMessage msg,
        void *data);

static int
load_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

static int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

static int
export_start(const char *path,
             const char *types,
             OscArgument **argv,
             int argc,
             OscMessage msg,
             void *data);

static int
export_stop(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data);

/* realtime callback */
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

/**
 * Setup OSC handlers
 */
static void
setup_osc_handlers(LightningServer server);

LightningServer
LightningServer_init(const char *listenPort,
                     const char *broadcastPort,
                     const char *broadcastHost,
                     int num_dirs,
                     char **search_dirs)
{
    LightningServer server;
    NEW(server);
    initialize_jack_client(server);

    /* setup OSC server */

    server->osc_server = OscServer_init(listenPort,
                                        &osc_error_handler);

    setup_osc_handlers(server);

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

/**
 * Play a sample
 */
int
LightningServer_play_sample(LightningServer server,
                            const char *file,
                            pitch_t pitch,
                            gain_t gain)
{
    assert(server && server->samples);
    return NULL != Samples_play(server->samples, file, pitch, gain);
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
    LOG(Error, "[OSC Error %d] %s %s\n", num, where, msg);
}

static int
add_dir(const char *path,
        const char *types,
        OscArgument **argv,
        int argc,
        OscMessage msg,
        void *data)
{
    assert(0 == strcmp(types, "s"));
    assert(argc == 1);
    Samples samps = (Samples) data;
    const char *dir = (const char *) argv[0];
    Samples_add_dir(samps, dir);
    return 0;
}

static int
load_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data)
{
    assert(0 == strcmp(types, "s"));
    assert(argc == 1);
    Samples samps = (Samples) data;
    const char *file = (const char *) argv[0];
    Samples_load(samps, file);
    return 0;
}

static int
play_sample(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data)
{
    assert(0 == strcmp(types, "sff"));
    assert(argc == 3);
    Samples samps = (Samples) data;
    const char *file = (const char *) argv[0];
    pitch_t *pitch = (pitch_t *) argv[1];
    gain_t *gain = (gain_t *) argv[2];
    Samples_play(samps, file, *pitch, *gain);
    return 0;
}

static int
export_start(const char *path,
             const char *types,
             OscArgument **argv,
             int argc,
             OscMessage msg,
             void *data)
{
    assert(0 == strcmp(types, "s"));
    assert(argc == 1);
    JackClient jack = (JackClient) data;
    const char *file = (const char *) argv[0];
    LOG(Debug, "export start %s", file);
    return JackClient_export_start(jack, file);
}

static int
export_stop(const char *path,
            const char *types,
            OscArgument **argv,
            int argc,
            OscMessage msg,
            void *data)
{
    JackClient jack = (JackClient) data;
    LOG(Debug, "export %s", "stop");
    int error = JackClient_export_stop(jack);
    if (error) {
        LOG(Error, "could not stop %s", "export");
    }
    return error;
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

static void
setup_osc_handlers(LightningServer server)
{
    /* Add Directory handler */
    OscServer_add_method(server->osc_server,
                         "/sample/search",
                         "s",
                         add_dir,
                         server->samples);

    /* Load Sample handler */
    OscServer_add_method(server->osc_server,
                         "/sample/load",
                         "s",
                         load_sample,
                         server->samples);

    /* Play Sample handler */
    OscServer_add_method(server->osc_server,
                         "/sample",
                         "sff",
                         play_sample,
                         server->samples);

    /* Export Start handler */
    OscServer_add_method(server->osc_server,
                         "/export/start",
                         "s",
                         export_start,
                         server->jack_client);

    /* Export Stop handler */
    OscServer_add_method(server->osc_server,
                         "/export/stop",
                         "",
                         export_stop,
                         server->jack_client);
}
