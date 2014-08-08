
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "event.h"
#include "list.h"
#include "mem.h"
#include "realtime.h"
#include "ringbuffer.h"
#include "sample.h"
#include "samples.h"
#include "table.h"
#include "thread.h"
#include "types.h"

#define ASSUMED_CHANNELS 2

struct Samples {
    /* output sample rate */
    nframes_t output_sr;
    /* sample cache */
    Table tab;
    /* sample that are actively playing on any
       given audio cycle */
    Sample active[MAX_POLYPHONY];
    /* buffer where we pull samples that need to go
       in the active array */
    Ringbuffer play_buf;
    /* event used to signal that a Sample needs
       to be added to the play buffer */
    Event play_event;
    /* thread used to push samples into the play buffer
       every time there is a play event */
    Thread play_thread;
    /* state for objects being used in the realtime thread */
    Realtime state;
    /* Sample pointer used for pushing samples into the play
       ringbuffer */
    Sample new_sample;
    /* summing buffers */
    sample_t **sum_bufs;
    /* sample collecting buffers */
    sample_t **collect_bufs;
};

void *
play_new_samples(void *arg);

/**
 * Data for the thread we start that adds
 * Sample instances to a ringbuffer every
 * time we are requested to play one.
 * We spin up a dedicated thread just for this
 * task because jack ringbuffers are only considered
 * thread-safe if there is exactly one writer thread
 * and exactly one reader thread and that these threads
 * never change.
 */
typedef struct ThreadData {
    Ringbuffer buf;
    Event event;
} *ThreadData;

Samples
Samples_init(nframes_t output_sr)
{
    int i = 0;
    Samples samps;
    NEW(samps);
    samps->state = Realtime_init();
    samps->tab = Table_init(32, NULL, NULL);

    /* allocate auxiliary buffers
       each buffer will be able to hold 2048 samples
       and will be mlock'ed into RAM
       there is one per channel and we just assume stereo */

    const size_t aux_buf_size = 2048;

    samps->sum_bufs = CALLOC(ASSUMED_CHANNELS, sizeof(sample_t*));
    samps->collect_bufs = CALLOC(ASSUMED_CHANNELS, sizeof(sample_t*));

    for (i = 0; i < ASSUMED_CHANNELS; i++) {
        samps->sum_bufs[i] = CALLOC(aux_buf_size, SAMPLE_SIZE);
        samps->collect_bufs[i] = CALLOC(aux_buf_size, SAMPLE_SIZE);

        if (0 != mlock(samps->sum_bufs[i], aux_buf_size * SAMPLE_SIZE)) {
            fprintf(stderr, "Could not lock memory into RAM\n");
            exit(EXIT_FAILURE);
        }

        if (0 != mlock(samps->collect_bufs[i], aux_buf_size * SAMPLE_SIZE)) {
            fprintf(stderr, "Could not lock memory into RAM\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < MAX_POLYPHONY; i++) {
        samps->active[i] = NULL;
    }

    samps->output_sr = output_sr;

    ThreadData tdata;
    NEW(tdata);
    samps->play_buf = Ringbuffer_init(sizeof(Sample) * MAX_POLYPHONY);
    if (0 != Ringbuffer_mlock(samps->play_buf)) {
        fprintf(stderr, "Could not mlock ringbuffer\n");
        exit(EXIT_FAILURE);
    }
    samps->play_event = Event_init();
    tdata->buf = samps->play_buf;
    tdata->event = samps->play_event;
    samps->play_thread = Thread_create(play_new_samples, tdata);

    samps->new_sample = ALLOC(sizeof(Sample));

    if (Realtime_set_processing(samps->state)) {
        fprintf(stderr, "Could not set Samples state to processing\n");
        exit(EXIT_FAILURE);
    }
    return samps;
}

Sample
Samples_load(Samples samps,
             const char *path)
{
    assert(samps);
    printf("looking up %s in sample table\n", path);
    Sample cached = (Sample) Table_get(samps->tab, path);
    if (NULL == cached) {
        /* initialize and cache it */
        Sample samp = Sample_init(path, 1.0, 1.0, samps->output_sr);
        printf("sample %s was not cached\n", Sample_path(samp));
        Table_put(samps->tab, path, samp);
        return samp;
    } else {
        printf("returning cached sample %s\n", Sample_path(cached));
        return cached;
    }
}

/**
 * Get a new instance of the sample specified by path.
 * If the sample was not in the cache and had to be loaded from disk,
 * it will be stored in the cache so that subsequent loads
 * will be from memory.
 */
Sample
Samples_play(Samples samps,
             const char *path,
             pitch_t pitch,
             gain_t gain)
{
    assert(samps);
    printf("playing %s\n", path);
    Sample cached = Samples_load(samps, path);
    printf("loaded %s\n", Sample_path(cached));
    Sample samp = Sample_clone(cached, pitch, gain, samps->output_sr);
    printf("cloned %s\n", Sample_path(samp));
    Event_broadcast(samps->play_event, samp);
    return samp;
}

int
Samples_write(Samples samps,
              sample_t **buffers,
              channels_t channels,
              nframes_t frames)
{
    assert(samps);

    int i = 0;
    int chan = 0;
    int frame = 0;
    int sample_write_error = 0;

    if (!Realtime_is_processing(samps->state)) {
        return 0;
    }

    /* zero out sum buffers */

    for (chan = 0; chan < channels; chan++) {
        memset(samps->sum_bufs[chan], 0, frames * SAMPLE_SIZE);
    }

    /* add any new samples to the active list */

    i = 0;
    while (Ringbuffer_read(samps->play_buf,
                           (void *) &samps->new_sample,
                           sizeof(Sample))) {
        for ( ; i < MAX_POLYPHONY; i++) {
            if (samps->active[i] == NULL) {
                /* assign to the open sample slot and read another
                   new sample from the ring buffer */
                samps->active[i] = samps->new_sample;
                break;
            }
        }
    }

    /* write samples to output buffers */
    /* TODO: sum the samples into the output buffers
       instead of overwriting each time through the loop */

    for (i = 0; i < MAX_POLYPHONY; i++) {
        if (samps->active[i] == NULL)
            continue;

        /* fill buffers with sample data */
        sample_write_error =                                            \
            Sample_write(samps->active[i], samps->collect_bufs, channels, frames);

        if (sample_write_error) {
            return sample_write_error;
        }

        for (chan = 0; chan < ASSUMED_CHANNELS; chan++) {
            for (frame = 0; frame < frames; frame++) {
                samps->sum_bufs[chan][frame] += samps->collect_bufs[chan][frame];
            }
        }

        if (Sample_done(samps->active[i])) {
            /* remove from the active list and free the sample */
            Sample_free(&samps->active[i]);
            samps->active[i] = NULL;
        }
    }

    /* copy sum buffers to output buffers */

    for (chan = 0; chan < channels; chan++) {
        memcpy(buffers[chan], samps->sum_bufs[chan], frames * SAMPLE_SIZE);
    }

    return 0;
}

void
Samples_free(Samples *samps)
{
    assert(samps && *samps);
    int i = 0;
    Samples s = *samps;
    Event_free(&s->play_event);
    Ringbuffer_free(&s->play_buf);
    Table_free(&s->tab);
    /* free auxiliary buffers */
    for (i = 0; i < ASSUMED_CHANNELS; i++) {
        FREE(s->sum_bufs[i]);
        FREE(s->collect_bufs[i]);
    }
    FREE(s->sum_bufs);
    FREE(s->collect_bufs);
}

void *
play_new_samples(void *arg)
{
    ThreadData data = (ThreadData) arg;
    Event event = data->event;
    Ringbuffer rb = data->buf;
    while (1) {
        Event_wait(event);
        Sample samp = (Sample) Event_value(event);
        Ringbuffer_write(rb, (void *) &samp, sizeof(Sample));
    }
}
