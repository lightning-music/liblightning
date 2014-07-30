/* feature test macros */
#define _GNU_SOURCE

#include <assert.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "event.h"
#include "kit.h"
#include "list.h"
#include "mem.h"
#include "realtime.h"
#include "ringbuffer.h"
#include "sample.h"
#include "thread.h"
#include "types.h"

typedef struct PlayThreadData {
    Ringbuffer play_buffer;
    Event play_event;
} *PlayThreadData;

struct Kit {
    /* number of samples loaded */
    unsigned int num_samples;
    /* output sample rate */
    nframes_t output_samplerate;
    /* actively playing Sample instances */
    Sample active[MAX_SAMPLES];
    /* ringbuffer to hold samples that need to go in the active list */
    Ringbuffer play_buffer;
    /* event signalled when there are samples in the play buffer */
    Event play_event;
    /* thread that puts new samples in the ring buffer */
    Thread play_thread;
    /* guard against jack realtime thread using a kit
       before it has been initialized*/
    Realtime state;
};

void *
play_new_samples(void *arg);

/* TODO: Kit should also spin up a thread that waits
   for sample done events and removes these samples from
   the active list */
Kit
Kit_load(const char *dir,
         nframes_t output_samplerate) {
    Kit kit;
    NEW(kit);
    kit->state = Realtime_init();
    /* TODO: better error-handling */
    /* read .kit */
    char kitfile_path[PATH_MAX];
    char sample_path[PATH_MAX];

    sprintf(kitfile_path, "%s/%s", dir, ".kit");

    size_t path_max = PATH_MAX;
    char *f = NULL;
    int file_index = 0;
    FILE *kitfile = fopen(kitfile_path, "r");

    if (NULL == kitfile) {
        fprintf(stderr, "Could not open %s\n", kitfile_path);
        exit(EXIT_FAILURE);
    } else {
        printf("opened %s\n", kitfile_path);
    }

    /* read sample paths */
    Sample s = NULL;
    ssize_t read = 0;
    while (file_index < MAX_SAMPLES) {
        read = getline(&f, &path_max, kitfile);
        if (-1 == read) {
            break;
        }
        /* get rid of the newline */
        f[read - 1] = '\0';
        sample_path[0] = '\0';
        sprintf(sample_path, "%s/%s", dir, f);
        /* cache sample data */
        s = Sample_play(sample_path, 1.0f, 1.0f, output_samplerate);
        Sample_free(&s);
        file_index++;
    }

    kit->num_samples = file_index;
    int i;
    for (i = 0; i < MAX_SAMPLES; i++) {
        kit->active[i] = NULL;
    }

    kit->output_samplerate = output_samplerate;

    PlayThreadData play_thread_data;
    NEW(play_thread_data);
    kit->play_buffer = Ringbuffer_init(sizeof(Sample) * MAX_SAMPLES);
    kit->play_event = Event_init();
    play_thread_data->play_buffer = kit->play_buffer;
    play_thread_data->play_event = kit->play_event;
    kit->play_thread = Thread_create(play_new_samples, play_thread_data);

    Realtime_set_processing(kit->state);
    
    return kit;
}

int
Kit_num_samples(Kit kit) {
    assert(kit);
    return kit->num_samples;
}

/**
 * Add a sample to the active list and tell it to start playing.
 */
void
Kit_play_sample(Kit kit,
                const char *file,
                pitch_t pitch,
                gain_t gain) {
    assert(kit);
    Sample s = Sample_play(file, pitch, gain, kit->output_samplerate);
    /* reached the end of the sample list
       start looking for an empty slot from the beginning */
    int i = 0;
    while (kit->active[i] != NULL) ;
    if (i == MAX_SAMPLES - 1) {
        /* no available sample slots */
        return;
    }
    kit->active[i] = s;
}

/**
 * Ideas for sample list management...
 *
 * kit loops through active list writing each of
 * the samples to the sample buffers
 *
 * if any of these sample buffers reach the end of the sample
 * they should be removed from the active list
 *
 * could implement this as a two-pass algorithm: kit first plays
 * each of the samples, any that are done get added to a 'done' list
 * then it frees all the samples on the done list
 *
 * either way this should all happen in the jack realtime thread
 * because if we do it from a different thread we will have to
 * start locking mutexes to keep data uncorrupted
 *
 * note that not reading samples from memory is not a fix for this
 * problem because in that case the active list is actually pointing
 * to a list of samples that are writing to ringbuffers
 */
int
Kit_write(Kit kit,
          sample_t **buffers,
          channels_t channels,
          nframes_t frames) {
    int i = 0;
    int sample_write_error = 0;

    /* add any new samples */

    /* write samples to output buffers */

    for (i = 0; i < MAX_SAMPLES; i++) {
        if (kit->active[i] == NULL)
            continue;

        /* fill buffers with sample data */
        sample_write_error =                                            \
            Sample_write(kit->active[i], buffers, channels, frames);

        if (sample_write_error) {
            return sample_write_error;
        } else if (Sample_done(kit->active[i])) {
            /* remove from the active list and free the sample */
            kit->active[i] = NULL;
            Sample_free(&kit->active[i]);
        }
    }

    return 0;
}

void
Kit_free(Kit *kit) {
    assert(kit && *kit);
    FREE(*kit);
}

void *
play_new_samples(void *arg)
{
    PlayThreadData data = (PlayThreadData) arg;
    Event event = data->play_event;
    Ringbuffer rb = data->play_buffer;

    while (1) {
        Event_wait(event);
        Sample samp = (Sample) Event_value(event);
        Ringbuffer_write(rb, (void *) samp, sizeof(Sample));
    }
}
