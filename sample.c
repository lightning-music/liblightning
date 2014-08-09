#include <assert.h>
#include <math.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clip.h"
#include "event.h"
#include "log.h"
#include "mem.h"
#include "mutex.h"
#include "sample.h"
#include "src.h"
#include "table.h"
#include "types.h"

typedef enum {
    Initializing,
    /* ready for processing */
    Processing,
    /* cleaning up after playing */
    Finished,
} State;

struct Sample {
    char *path;
    pitch_t pitch;
    gain_t gain;
    // channels, frames, and samplerate are pulled from SF_INFO
    channels_t channels;
    nframes_t frames;
    int samplerate;
    // buffers to hold sample data (one per channel)
    sample_t **framebufs;
    // track read position in file (guarded by mutex)
    nframes_t framep;
    Mutex framep_mutex;
    nframes_t total_frames_written;
    // provide a way to synchronize a thread on the `done` event
    Event done_event;
    /* sample rate converters */
    SRC *src;
    double src_ratio;
    /* sample state and associated mutex */
    State state;
    Mutex state_mutex;
};

/* static utility functions */

static void
initialize_state(Sample s);

static void
allocate_frame_buffers(Sample s,
                       nframes_t frames);

static void
copy_frame_buffers(Sample dest,
                   Sample src);

static int
Sample_set_state(Sample samp,
                 State state);

static void
Sample_set_state_or_exit(Sample samp,
                         State state);

static const char *
state_string(State state) {
    switch(state) {
    case Initializing:
        return "Initializing";
        break;
    case Processing:
        return "Processing";
        break;
    case Finished:
        return "Finished";
        break;
    default:
        assert(0);
    }
}

static inline int
Sample_is_processing(Sample samp)
{
    return samp->state == Processing;
}

/**
 * Set a sample's path.
 */
static void
Sample_set_path(Sample samp,
                const char *path)
{
    /* copy string to path member */
    size_t path_bytes = strlen(path);
    samp->path = ALLOC(path_bytes + 1);
    memcpy(samp->path, path, path_bytes);
    samp->path[path_bytes] = '\0';
}

/**
 * De-interleave and resample a buffer that has
 * been read from disk.
 */
static int
Sample_set_buffers(Sample samp,
                   sample_t *buf,
                   nframes_t output_sr)
{
    SRC srcs[2];
    srcs[0] = SRC_init();
    srcs[1] = SRC_init();
    /* de-interleave data */
    sample_t *di_bufs[2];
    di_bufs[0] = ALLOC( samp->frames * SAMPLE_SIZE );
    di_bufs[1] = ALLOC( samp->frames * SAMPLE_SIZE );
    int i;
    unsigned int j;
    if (samp->channels == 1) {
        for (j = 0; j < samp->frames; j++) {
            di_bufs[0][j] = di_bufs[1][j] = buf[j];
        }
    } else if (samp->channels == 2) {
        for (j = 0; j < samp->frames; j++) {
            for (i = 0; i < samp->channels; i++) {
                di_bufs[i][j] = buf[ (j * samp->channels) + i ];
            }
        }
    } else {
        LOG(Error, "Unsupported number of channels (%d). "
            "Only stereo and mono are supported.\n", samp->channels);
        return 1;
    }
    /* perform sample rate conversion */
    double src_ratio = output_sr / (double) samp->samplerate;
    nframes_t output_frames = (nframes_t) ceil(samp->frames * src_ratio);
    int error, end_of_input = 0;
    AudioData audio_data;
    nframes_t frames_consumed = 0, frames_produced = 0;
    nframes_t input_frames_used, output_frames_gen;
    while (frames_consumed < samp->frames) {
        input_frames_used = 0;
        output_frames_gen = 0;
        audio_data.input_frames = samp->frames - frames_consumed;
        audio_data.output_frames = output_frames - frames_produced;
        for (i = 0; i < 2; i++) {
            audio_data.output = samp->framebufs[i + frames_produced];
            audio_data.input = di_bufs[i + frames_consumed];
            error = SRC_process(srcs[i], src_ratio, audio_data,
                                &input_frames_used, &output_frames_gen,
                                &end_of_input);
            if (error) {
                LOG(Error, "Error in sample rate conversion: %s",
                    SRC_strerror(error));
            }
        }
        if (end_of_input) {
            break;
        } else {
            frames_consumed += input_frames_used;
            frames_produced += output_frames_gen;
        }
    }

    FREE(di_bufs[0]);
    FREE(di_bufs[1]);
    FREE(srcs[0]);
    FREE(srcs[1]);

    return 0;
}

/**
 * Read a sound file, de-interleave the channels if necessary,
 * perform sample rate conversion, then cache this data in memory.
 */
Sample
Sample_init(const char *file,
            pitch_t pitch,
            gain_t gain,
            nframes_t output_sr)
{
    Sample s;
    /* initialize state mutex and set state to Processing */
    NEW(s);
    initialize_state(s);
    Sample_set_path(s, file);
    /* open audio file */
    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(file, SFM_READ, &sfinfo);
    if (sf == NULL) {
        LOG(Error, "%s\n", sf_strerror(sf));
        return NULL;
    }
    /* Set pitch to a very small number if it is 0,
       otherwise clip it to a given range and
       if it is negative set the reversed bit */
    if (pitch == 0.0) {
        s->pitch = 0.0001;
    } else {
        s->pitch = clip(pitch, -32.0f, 32.0f);
    }
    s->gain = clip(gain, 0.0f, 1.0f);
    s->channels = sfinfo.channels;
    s->frames = sfinfo.frames;
    s->samplerate = sfinfo.samplerate;
    s->done_event = Event_init(NULL);
    /* allocate stereo buffers */
    double src_ratio = output_sr / (double) s->samplerate;
    nframes_t output_frames = (nframes_t) ceil(s->frames * src_ratio);
    allocate_frame_buffers(s, output_frames);
    /* read the file */
    sample_t *framebuf = ALLOC( s->frames * s->channels * SAMPLE_SIZE );
    const sf_count_t frames = (4096 / SAMPLE_SIZE) / s->channels;
    long total_frames = sf_readf_float(sf, framebuf, frames);
    while (total_frames < s->frames) {
        total_frames +=                         \
            sf_readf_float(sf,
                           framebuf + (total_frames * s->channels),
                           frames);
    }
    assert(total_frames == s->frames);
    sf_close(sf);

    Sample_set_buffers(s, framebuf, output_sr);

    FREE(framebuf);

    /* set frames member to the number of frames that are
       actually in framebuf after resampling */
    s->frames = output_frames;
    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;
    Sample_set_state_or_exit(s, Processing);
    LOG(Debug, "Sample_init: done loading %s", file);
    return s;
}

Sample
Sample_clone(Sample orig,
             pitch_t pitch,
             gain_t gain,
             nframes_t output_sr)
{
    Sample s;
    NEW(s);
    initialize_state(s);
    s->gain = pitch;
    s->pitch = gain;
    s->frames = orig->frames;
    s->channels = orig->channels;
    s->samplerate = orig->samplerate;
    s->src_ratio = output_sr / (double) orig->samplerate;
    s->done_event = Event_init(NULL);
    s->path = orig->path;
    allocate_frame_buffers(s, s->frames);
    copy_frame_buffers(s, orig);
    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;
    /* initialize sample rate converters */
    /* allocate_src(s); */
    Sample_set_state_or_exit(s, Processing);
    return s;
}

/**
 * Path to loaded file.
 */
const char *
Sample_path(Sample samp)
{
    assert(samp);
    return samp->path;
}

nframes_t
Sample_write(Sample samp,
             sample_t **buffers,
             channels_t channels,
             nframes_t frames) {
    assert(samp);

    if (! Sample_is_processing(samp)) {
        fprintf(stderr, "sample is not processing\n");
        return 0;
    }

    nframes_t len = samp->frames;
    int chans = (int) samp->channels;
    nframes_t offset = samp->framep;
    int chan = 0;
    int frame = 0;
    int at_end = 0;
    nframes_t frames_used = 0;

    for (frame = 0; frame < frames; frame++) {
        for (chan = 0; chan < chans; chan++) {
            if (offset + frame < len) {
                buffers[chan][frame] =                                  \
                    samp->gain * samp->framebufs[chan][offset + frame];
            } else {
                at_end = 1;
                buffers[chan][frame] = 0.0f;
            }
            /* gain(samp->gain, buffers[chan], frames); */
        }
        if (!at_end) {
            frames_used++;
        }
    }

    if (at_end) {
        Sample_set_state(samp, Finished);
        Event_broadcast(samp->done_event, NULL);
    } else {
        samp->framep += frames_used;
    }

    return 0;
}

int
Sample_done(Sample samp) {
    /* int done = 0; */
    /* if (!Mutex_lock(samp->state_mutex)) { */
    /*     done = samp->state == Finished; */
    /*     Mutex_unlock(samp->state_mutex); */
    /* } */
    /* return done; */
    return samp->state == Finished;
}

int
Sample_wait(Sample samp) {
    assert(samp);
    return Event_wait(samp->done_event);
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp) {
    int i;
    assert(samp && *samp);
    /* free the done event */
    Event_free(&(*samp)->done_event);
    /* free the framep mutex */
    Mutex_free(&(*samp)->framep_mutex);
    for (i = 0; i < (*samp)->channels; i++) {
        FREE((*samp)->framebufs[i]);
    }
    FREE((*samp)->framebufs);
    FREE(*samp);
}

static int
Sample_set_state(Sample samp,
                 State state) {
    assert(samp->state_mutex);
    int not_locked = Mutex_lock(samp->state_mutex);
    if (not_locked) {
        return not_locked;
    } else {
        samp->state = state;
        return Mutex_unlock(samp->state_mutex);
    }
}

static void
Sample_set_state_or_exit(Sample s,
                         State state) {
    if (Sample_set_state(s, state)) {
        fprintf(stderr, "Could not set Sample state to %s\n",
                state_string(state));
        exit(EXIT_FAILURE);
    }
}

static void
initialize_state(Sample s) {
    s->state_mutex = Mutex_init();
    Sample_set_state_or_exit(s, Initializing);
}

static void
allocate_frame_buffers(Sample s, nframes_t frames) {
    s->framebufs = CALLOC(2, sizeof(sample_t*));
    s->framebufs[0] = CALLOC(frames, SAMPLE_SIZE);
    s->framebufs[1] = CALLOC(frames, SAMPLE_SIZE);
}

/**
 * This function is used for loading cached samples.
 * It doesn't matter if we use src or dest in the loop
 * conditions.
 */
static void
copy_frame_buffers(Sample dest,
                   Sample src) {
    int i;
    unsigned int j;
    for (j = 0; j < dest->frames; j++) {
        for (i = 0; i < dest->channels; i++) {
            dest->framebufs[i][j] = src->framebufs[i][j];
        }
    }
}
