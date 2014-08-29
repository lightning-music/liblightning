/**
 * Top-level module for lightning.
 */
#include <assert.h>
#include <string.h>

#include "jack-client.h"
#include "lightning.h"
#include "log.h"
#include "mem.h"
#include "metro.h"
#include "samples.h"
#include "sequencer.h"

struct Lightning {
    JackClient jack_client;
    Sequencer seq;
    Samples samples;
};

/* realtime callback */
static int
audio_callback(sample_t **buffers, channels_t channels,
               nframes_t frames, void *data);
               
/**
 * Initialize jack_client and samples,
 * and use samples as the data for the jack_client
 * realtime callback.
 */
static void
initialize_jack_client(Lightning lightning);

Lightning
Lightning_init()
{
    Lightning lightning;
    NEW(lightning);
    initialize_jack_client(lightning);
    /* lightning->metro = Metro_init(initial_tempo); */
    /* kind of arbitrary */
    /* Slave clock = Metro_slave(lightning->metro, "1/16"); */
    /* lightning->seq = Sequencer_init(lightning, clock); */
    /* if (Sequencer_start(lightning->seq)) { */
    /*     LOG(Error, "could not start %s", "sequencer"); */
    /*     return NULL; */
    /* } */
    return lightning;
}

int
Lightning_add_dir(Lightning lightning, const char *dir)
{
    assert(lightning);
    return Samples_add_dir(lightning->samples, dir);
}

/**
 * Play a sample
 */
int
Lightning_play_sample(Lightning lightning,
                      const char *file,
                      pitch_t pitch,
                      gain_t gain)
{
    assert(lightning && lightning->samples);
    return NULL != Samples_play(lightning->samples, file, pitch, gain);
}

/* int */
/* Lightning_play_pattern(Lightning lightning, Pattern pat) */
/* { */
/*     assert(lightning); */
/*     return Sequencer_add_pattern(lightning->seq, pat); */
/* } */

/**
 * Start exporting to an audio file
 */
int
Lightning_export_start(Lightning lightning, const char *file)
{
    assert(lightning);
    return JackClient_export_start(lightning->jack_client, file);
}

/**
 * If currently exporting, stop
 */
int
Lightning_export_stop(Lightning lightning)
{
    assert(lightning);
    return JackClient_export_stop(lightning->jack_client);
}

void
Lightning_free(Lightning *lightning)
{
    assert(lightning && *lightning);
    Lightning s = *lightning;
    Samples_free(&s->samples);
    JackClient_free(&s->jack_client);
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
initialize_jack_client(Lightning lightning)
{
    lightning->jack_client =                    \
        JackClient_init(audio_callback, NULL);

    lightning->samples =                                                \
        Samples_init(JackClient_samplerate(lightning->jack_client));
    
    JackClient_set_data(lightning->jack_client, lightning->samples);
    JackClient_setup_callbacks(lightning->jack_client);
    JackClient_activate(lightning->jack_client);
    JackClient_setup_ports(lightning->jack_client);
}
