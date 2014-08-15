/**
 * TODO: fix this implementation
 *
 * We need to
 * - start/stop a thread that is writing audio data to disk
 * - this thread will read data from a ringbuffer that
 *   is written to in the JACK realtime thread
 * - different thread(s) will be starting/stopping the disk thread
 *
 * start action algorithm:
 * - JackClient starts the disk thread
 * - stores a reference to the ExportThread
 * - immediately starts writing to the disk thread's ringbuffer
 * - disk thread immediately starts writing data from the ringbuffer to disk
 *
 * stop action algorithm:
 * (1) signal the disk thread that it is time to stop writing
 *     and close the file
 * (2) free the ExportThread structure
 *
 * the tricky part is how to synchronize with JackClient to
 * let it know when the stop action algorithm is complete, so
 * that it can set its export_thread pointer to NULL
 *
 * Alternative
 * -----------
 * Maybe the way to solve these problems is to always have the
 * disk thread running.
 *
 * In this scenario, there is no need to free the export thread structure.
 * We start up the disk thread after JackClient registers its process callback,
 * then we
 * (1) wait for a "start" event
 * (2) open an audio file for writing, and stream data to this file from a ringbuffer
 * (3) tell the disk thread to stop writing and close the file
 *
 * The start/stop mechanism could be acheived with a boolean variable
 * that is guarded by a mutex.
 *
 * The JACK realtime thread would never need to change the value of this boolean variable,
 * so there is no concern about blocking the realtime thread to access this variable.
 *
 * Call the boolean variable "exporting".
 *
 * JackClient would do the following in the realtime callback:
 * (1)  call ExportThread_write, which
 *   (1.1)  checks the value of exporting variable
 *   (1.2)  if it is false, do nothing
 *   (1.3)  otherwise
 *     (1.3.1)  write samples to the ringbuffer
 *     (1.3.2)  signal the data event
 *
 * The start algorithm:
 * (1)  signal the start event (with a filename value)
 * (2)  lock exporting mutex
 * (3)  set exporting to 1
 * (4)  unlock exporting mutex
 *
 * The stop algorithm:
 * (1)  lock exporting mutex
 * (2)  set exporting to 0
 * (3)  unlock exporting mutex
 *
 * The disk thread:
 * (1)  initializes local variables
 * (2)  waits for start event
 * (3)  opens file
 * (4)  enters infinite loop where it
 *   (4.1)  checks the value of exporting
 *   (4.2)  if true
 *     (4.2.1)  wait for data event
 *     (4.2.2)  write data from ringbuffer to disk
 *   (4.3)  if false
 *     (4.3.1)  close file
 *     (4.3.2)  go back to (2)
 *
 * JackClient_start_exporting should ideally check if the disk thread
 * is already running an export job and fail if it is.
 * Could expose a function
 * int ExportThread_is_exporting(...)
 * for JackClient to use to determine this.
 */
#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <string.h>

#include "event.h"
#include "export-thread.h"
#include "log.h"
#include "mem.h"
#include "mutex.h"
#include "ringbuffer.h"
#include "thread.h"
#include "types.h"

static int
ExportThread_set_exporting(ExportThread thread, int val);

static int
ExportThread_is_exporting(ExportThread thread);

/* data for threads that write audio data to disk */
struct ExportThread {
    /* ringbuffer to read data from */
    Ringbuffer rb;
    /* file name */
    char *file;
    /* start event, should be signalled with a filename */
    Event start_event;
    /* Event that signals there is data to write */
    Event data_event;
    /* JACK sample rate */
    nframes_t output_sr;
    /* output channels */
    channels_t channels;
    /* reference to the thread */
    Thread thread;
    /* flag to tell whether we are currently exporting,
       and associated mutex to protect concurrent reads/writes */
    int exporting;
    Mutex exporting_mutex;
};

static void *
export_thread(void *arg);

/**
 * Start a thread for writing to a file
 */
ExportThread
ExportThread_create(nframes_t output_sr, channels_t channels)
{
    ExportThread thread;
    NEW(thread);
    thread->exporting = 0;
    thread->channels = channels;
    thread->exporting_mutex = Mutex_init();
    thread->output_sr = output_sr;
    thread->start_event = Event_init(NULL);
    thread->data_event = Event_init(NULL);
    thread->rb = Ringbuffer_init(4096 * SAMPLE_SIZE * channels);
    thread->thread = Thread_create(export_thread, thread);
    return thread;
}

/**
 * Write some samples to the export thread's ring buffer
 * Beware that this function must be realtime safe!
 */
nframes_t
ExportThread_write(ExportThread thread, sample_t **bufs, nframes_t frames)
{
    assert(thread);

    int is_exporting = ExportThread_is_exporting(thread);

    if (is_exporting) {
        channels_t chan = 0;
        nframes_t frame = 0;
        size_t bytes_total = frames * thread->channels * SAMPLE_SIZE;

        /* allocate and fill interleaved buffer */

        sample_t ibuf[ frames * thread->channels * SAMPLE_SIZE ];
        for (chan = 0; chan < thread->channels; chan++) {
            for (frame = 0; frame < frames; frame++) {
                ibuf[ chan + (frame * thread->channels) ] = bufs[chan][frame];
            }
        }

        /* write interleaved data to ringbuffer */

        size_t bytes_written =                          \
            Ringbuffer_write(thread->rb, ibuf, bytes_total);

        Event_broadcast(thread->data_event, NULL);

        return bytes_written / (thread->channels * SAMPLE_SIZE);
    } else {
        return 0;
    }
}

static int
ExportThread_set_exporting(ExportThread thread, int val)
{
    if (Mutex_lock(thread->exporting_mutex)) {
        return 1;
    }
    thread->exporting = val;
    return Mutex_unlock(thread->exporting_mutex);
}

static int
ExportThread_is_exporting(ExportThread thread)
{
    int result;
    if (Mutex_lock(thread->exporting_mutex)) {
        return 0;
    }
    result = thread->exporting != 0;
    Mutex_unlock(thread->exporting_mutex);
    return result;
}

int
ExportThread_start(ExportThread thread, const char *file)
{
    assert(thread && thread->start_event);
    Event_signal(thread->start_event, (void *) file);
    return ExportThread_set_exporting(thread, 1);
}

int
ExportThread_stop(ExportThread thread)
{
    assert(thread);
    return ExportThread_set_exporting(thread, 0);
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
    Event_free(&t->start_event);
    Event_free(&t->data_event);
    Mutex_free(&t->exporting_mutex);
    FREE(t->file);
}

static void *
export_thread(void *arg)
{
    static const nframes_t samples_wanted = 512;

    ExportThread thread = (ExportThread) arg;
    const size_t bytes_wanted = samples_wanted * SAMPLE_SIZE * thread->channels;
    sample_t buf[samples_wanted * thread->channels];

    /* wait for the start event */
    Event_wait(thread->start_event);

    LOG(Debug, "received %s event", "start");
    
    char *file = (char *) Event_value(thread->start_event);
    SF_INFO sfinfo;
    sfinfo.samplerate = thread->output_sr;
    sfinfo.channels = thread->channels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    LOG(Debug, "exporting to %s", file);
    SNDFILE *sf = sf_open(file, SFM_WRITE, &sfinfo);
    FREE(file);

    if (sf == NULL) {
        LOG(Error, "%s", sf_strerror(sf));
        return NULL;
    }

    while (1) {

        LOG(Debug, "waiting for %s event", "data");

        /* wait for data */
        Event_wait(thread->data_event);

        LOG(Debug, "received %s event", "data");

        /* read data from the ringbuffer and write it to the file,
           assume data in ringbuffer is interleaved */
        size_t bytes_read = 0;
        nframes_t frames_read = 0;
        do {
            bytes_read = Ringbuffer_read(thread->rb, (void *) buf, bytes_wanted);
            frames_read = bytes_read / (SAMPLE_SIZE * thread->channels);
            sf_writef_float(sf, buf, frames_read);
        } while (bytes_read == bytes_wanted);
    }

    sf_close(sf);

    return NULL;
}
