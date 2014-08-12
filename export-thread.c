#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <string.h>

#include "event.h"
#include "export-thread.h"
#include "log.h"
#include "mem.h"
#include "ringbuffer.h"
#include "thread.h"
#include "types.h"

/* data for threads that write audio data to disk */
struct ExportThread {
    /* ringbuffer to read data from */
    Ringbuffer rb;
    /* file name */
    char *file;
    /* Event that signals there is data to write
       if the value associated with the event is nonzero,
       that means that the write thread has no more data */
    Event data_event;
    /* JACK sample rate */
    nframes_t output_sr;
    /* output channels */
    channels_t channels;
    /* reference to the thread */
    Thread thread;
};

typedef enum {
    WORKING = 10,
    DONE = 20
} State;

static void *
export_thread(void *arg);

/**
 * Start a thread for writing to a file
 */
ExportThread
ExportThread_create(const char *file, nframes_t output_sr, channels_t channels)
{
    ExportThread thread;
    NEW(thread);
    size_t len = strlen(file);
    thread->file = ALLOC(len + 1);
    memcpy(thread->file, file, len);
    thread->file[len] = '\0';
    thread->output_sr = output_sr;
    State state = WORKING;
    thread->data_event = Event_init(&state);
    thread->rb = Ringbuffer_init(4096 * SAMPLE_SIZE * channels);
    thread->thread = Thread_create(export_thread, thread);
    return thread;
}

/**
 * Write some samples to the export thread's ring buffer
 */
nframes_t
ExportThread_write(ExportThread thread, sample_t **bufs,
                   nframes_t frames, channels_t channels)
{
    assert(thread);

    channels_t chan = 0;
    nframes_t frame = 0;
    size_t bytes_total = frames * channels * SAMPLE_SIZE;

    /* allocate and fill interleaved buffer */

    sample_t *ibuf = CALLOC(frames * channels, SAMPLE_SIZE);
    for (chan = 0; chan < channels; chan++) {
        for (frame = 0; frame < frames; frame++) {
            ibuf[ chan + (frame * channels) ] = bufs[chan][frame];
        }
    }

    /* write interleaved data to ringbuffer */

    size_t bytes_written =                          \
        Ringbuffer_write(thread->rb, ibuf, bytes_total);

    /* warn if we couldn't write all the data */

    LOG(Warn, "attempted to write %ld bytes, but could only write %ld",
        bytes_total, bytes_written);

    FREE(ibuf);

    return bytes_written / (channels * SAMPLE_SIZE);
}

/**
 * Destroy an export thread
 */
void
ExportThread_free(ExportThread *thread)
{
    assert(thread && *thread);
    ExportThread t = *thread;
    Ringbuffer_free(&t->rb);
    Event_free(&t->data_event);
    FREE(t->file);
}

static void *
export_thread(void *arg)
{
    static const nframes_t samples_wanted = 512;

    ExportThread thread = (ExportThread) arg;
    const size_t bytes_wanted = samples_wanted * SAMPLE_SIZE * thread->channels;
    sample_t buf[samples_wanted * thread->channels];
    SF_INFO sfinfo;
    sfinfo.samplerate = thread->output_sr;
    sfinfo.channels = thread->channels;
    SNDFILE *sf = sf_open(thread->file, SFM_WRITE, &sfinfo);

    if (sf == NULL) {
        return NULL;
    }

    /* read data from the ringbuffer,
       assume that it is interleaved */
    size_t bytes_read = Ringbuffer_read(thread->rb, (void *) buf, bytes_wanted);
    nframes_t frames_read = 0;

    while (bytes_read == bytes_wanted) {
        /* wait for data */
        Event_wait(thread->data_event);
 
       /* read data from the ringbuffer */
        bytes_read = Ringbuffer_read(thread->rb, (void *) buf, bytes_wanted);

        /* write to file */
        frames_read = bytes_read / (SAMPLE_SIZE * thread->channels);
        sf_writef_float(sf, buf, frames_read);
    }

    sf_close(sf);
    ExportThread_free(&thread);

    return NULL;
}
