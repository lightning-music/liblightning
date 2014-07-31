#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "clip.h"
#include "event.h"
#include "gain.h"
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
    const char *path;
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

/* cache samples */
static Table cache = NULL;

static void
Sample_cache(Sample samp) {
    Table_put(cache, samp->path, samp);
}

/**
 * Create a new sample and cache it
 */
static Sample
Sample_load_new(const char *file,
                pitch_t pitch,
                gain_t gain,
                nframes_t output_samplerate);

/**
 * Clone a cached sample. Pulls the cached data, but
 * will play back with different pitch and gain.
 */
static Sample
Sample_load_cached(Sample cached_sample,
                   pitch_t pitch,
                   gain_t gain,
                   nframes_t output_samplerate);

/* static utility functions */

static void
initialize_state(Sample s);

static void
allocate_frame_buffers(Sample s);

static void
copy_frame_buffers(Sample dest,
                   Sample src);

static void
allocate_src(Sample s);

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
Sample_is_processing(Sample samp) {
    return samp->state == Processing;
}

/**
 * Load an audio sample.
 * Either loads actual sample data into the cache (for playing
 * samples from memory) or loads a file descriptor (for
 * playing from disk).
 */
static Sample
Sample_load(const char *file,
            pitch_t pitch,
            gain_t gain,
            nframes_t output_samplerate) {
    if (cache == NULL) {
        cache = Table_init(16, NULL, NULL);
    }

    Log log = Log_init(NULL);
    Sample cached = (Sample) Table_get(cache, file);
    Sample s;

    if (NULL == cached) {
        LOG(log, Info, "%s was not cached", file);
        s = Sample_load_new(file, pitch, gain, output_samplerate);
        return Sample_load_cached(s, pitch, gain, output_samplerate);
    } else {
        return Sample_load_cached(cached, pitch, gain, output_samplerate);
    }
}

static Sample
Sample_load_cached(Sample cached_sample,
                   pitch_t pitch,
                   gain_t gain,
                   nframes_t output_samplerate) {
    Sample s;

    NEW(s);
    initialize_state(s);

    s->gain = pitch;
    s->pitch = gain;
    s->frames = cached_sample->frames;
    s->channels = cached_sample->channels;
    s->samplerate = cached_sample->samplerate;
    s->src_ratio = output_samplerate / (double) cached_sample->samplerate;
    s->done_event = Event_init(NULL);
    s->path = cached_sample->path;

    allocate_frame_buffers(s);
    copy_frame_buffers(s, cached_sample);

    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;

    /* initialize sample rate converters */

    allocate_src(s);

    return s;
}

static Sample
Sample_load_new(const char *file,
                pitch_t pitch,
                gain_t gain,
                nframes_t output_samplerate) {
    Sample s;

    Log log = Log_init(NULL);

    /* initialize state mutex and set state to Processing */

    NEW(s);
    initialize_state(s);

    /* open audio file */

    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(file, SFM_READ, &sfinfo);

    if (sf == NULL) {
        LOG(log, Error, "%s\n", sf_strerror(sf));
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
    s->frames = sfinfo.frames;
    s->channels = sfinfo.channels;
    s->samplerate = sfinfo.samplerate;
    s->src_ratio = output_samplerate / (double) s->samplerate;
    s->done_event = Event_init(NULL);
    s->path = file;

    /* stereo buffers */

    allocate_frame_buffers(s);

    /* read the file */

    sample_t *framebuf = ALLOC( s->frames * s->channels * SAMPLE_SIZE );
    sf_count_t frames = (4096 / SAMPLE_SIZE) / s->channels;
    long total_frames = sf_readf_float(sf, framebuf, frames);

    while (total_frames < s->frames) {
        total_frames +=                         \
            sf_readf_float(sf,
                           framebuf + (total_frames * s->channels),
                           frames);
    }

    sf_close(sf);

    /* de-interleave data */

    int i;
    unsigned int j;
    if (s->channels == 1) {
        for (j = 0; j < s->frames; j++) {
            s->framebufs[0][j] = s->framebufs[1][j] = framebuf[j];
        }
    } else if (s->channels == 2) {
        for (j = 0; j < s->frames; j++) {
            for (i = 0; i < s->channels; i++) {
                s->framebufs[i][j] = framebuf[ (j * s->channels) + i ];
            }
        }
    } else {
        LOG(log, Error, "Unsupported number of channels (%d). "
            "Only stereo and mono are supported.\n", s->channels);
        return NULL;
    }

    FREE(framebuf);

    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;

    /* initialize sample rate converters */

    allocate_src(s);
    Sample_cache(s);

    LOG(log, Debug, "Sample_load_new: done loading %s", file);

    return s;
}

/**
 * Path to loaded file.
 */
const char *
Sample_path(Sample samp) {
    assert(samp);
    return samp->path;
}

Sample
Sample_play(const char *file,
            pitch_t pitch,
            gain_t gain,
            nframes_t output_samplerate) {
    Sample s = Sample_load(file, pitch, gain, output_samplerate);
    /* set framep to 0 and state to Processing */
    if (Sample_set_state(s, Processing)) {
        fprintf(stderr, "could not set sample state to processing\n");
    } else {
        printf("set sample state to processing\n");
    }
    return s;
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

    int chans = (int) samp->channels;
    long offset = samp->framep;
    int end_of_input = 0;
    int chan = 0;
    int error = 0;
    nframes_t input_frames_used = 0;
    nframes_t output_frames_gen = 0;
    AudioData audio_data;

    audio_data.input_frames = samp->frames - samp->framep;
    audio_data.output_frames = frames;

    for (chan = 0; chan < chans; chan++) {
        audio_data.output = buffers[chan];
        audio_data.input = &samp->framebufs[chan][offset];

        input_frames_used = 0;
        output_frames_gen = 0;

        error = SRC_process(samp->src[chan],
                            samp->src_ratio / samp->pitch,
                            audio_data,
                            &input_frames_used, &output_frames_gen,
                            &end_of_input);

        if (error) {
            fprintf(stderr, "Error in SRC_process: %s\n",
                    SRC_strerror(error));
            exit(EXIT_FAILURE);
        }

        /* apply gain */

        gain(samp->gain, buffers[chan], frames);
    }

    if (end_of_input) {
        printf("sample finished\n");
        Sample_set_state(samp, Finished);
        Event_broadcast(samp->done_event, NULL);
    } else {
        samp->framep += input_frames_used;
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
    /* free the sample rate converters */
    SRC_free(&(*samp)->src[0]);
    SRC_free(&(*samp)->src[1]);
    FREE((*samp)->src);
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
allocate_frame_buffers(Sample s) {
    s->framebufs = CALLOC(2, sizeof(sample_t*));
    s->framebufs[0] = CALLOC(s->frames, SAMPLE_SIZE);
    s->framebufs[1] = CALLOC(s->frames, SAMPLE_SIZE);
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

static void
allocate_src(Sample s) {
    s->src = CALLOC(2, sizeof(SRC));
    s->src[0] = SRC_init();
    s->src[1] = SRC_init();
}
