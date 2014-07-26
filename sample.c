#include <assert.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "clip.h"
#include "event.h"
#include "mem.h"
#include "mutex.h"
#include "sample.h"
#include "src.h"
#include "types.h"

typedef enum {
    SampleState_Initializing,
    SampleState_Processing,
    SampleState_Finishing,
} SampleState;

/*
 * TODO
 * ====
 * Need to have a static table for sample data
 * so that loading the same sample twice would result in
 * the initialization step for the second Sample_load just being
 * a memcpy from the table.
 */

/*
 * TODO
 * ====
 * Dynamic sample rate conversion.
 * jack-client is notified with a callback any time
 * the server changes the output sample rate.
 * Any samples that are currently playing will also need to
 * be notified of such changes to ensure maximum audio quality.
 * This would either be done with a callback or with an
 * Event.
 * I'm probably being too defensive at this stage in the game [bs].
 */

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
    SampleState state;
    Mutex state_mutex;
};

/**
 * Try to set framep
 */
static int
set_framep(Sample samp,
                nframes_t val);

int
Sample_set_state(Sample samp,
                 SampleState state);

static inline int
Sample_is_processing(Sample samp) {
    return samp->state == SampleState_Processing;
}

/**
 * Load an audio sample.
 */
Sample
Sample_load(const char *file,
            pitch_t pitch,
            gain_t gain,
            nframes_t output_samplerate) {
    Sample s;
    NEW(s);

    /* initialize state mutex and set state to Processing */

    s->state_mutex = Mutex_init();
    if (Sample_set_state(s, SampleState_Initializing)) {
        fprintf(stderr, "Could not set Sample state to Initializing\n");
        exit(EXIT_FAILURE);
    }

    /* open audio file */

    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(file, SFM_READ, &sfinfo);

    if (sf == NULL) {
        fprintf(stderr, "%s\n", sf_strerror(sf));
        exit(EXIT_FAILURE);
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

    s->framebufs = CALLOC(2, sizeof(sample_t*));
    s->framebufs[0] = CALLOC(s->frames, SAMPLE_SIZE);
    s->framebufs[1] = CALLOC(s->frames, SAMPLE_SIZE);

    /* read the file */

    sample_t framebuf[ s->frames * s->channels ];
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

    int i, j;
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
        fprintf(stderr, "Unsupported number of channels (%d). "
                "Only stereo and mono are supported.\n", s->channels);
        exit(EXIT_FAILURE);
    }

    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;

    /* initialize sample rate converters */

    s->src = CALLOC(2, sizeof(SRC));
    s->src[0] = SRC_init();
    s->src[1] = SRC_init();

    /* set state to processing */

    if (Sample_set_state(s, SampleState_Processing)) {
        fprintf(stderr, "Could not set Sample state to Processing\n");
        exit(EXIT_FAILURE);
    }

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

channels_t
Sample_num_channels(Sample samp) {
    assert(samp);
    return samp->channels;
}

/**
 * Number of frames in the audio sample.
 */
nframes_t
Sample_num_frames(Sample samp) {
    assert(samp);
    return (nframes_t) samp->frames;
}

/**
 * Sample rate of the audio sample.
 */
int
Sample_samplerate(Sample samp) {
    assert(samp);
    return samp->samplerate;
}

int
Sample_samplerate_callback(nframes_t sr,
                           void *arg) {
    assert(arg);
    Sample s = (Sample) arg;
    /* printf("got new sample rate   = %ld\n", sr); */
    s->src_ratio = ((double) sr) / ((double) s->samplerate);
    return 0;
}

nframes_t
Sample_frames_available(Sample samp) {
    assert(samp);
    return samp->frames - samp->framep;
}

int
Sample_reset(Sample samp) {
    assert(samp);
    /* set framep to 0 and state to Processing */
    return set_framep(samp, 0) &&          \
        Sample_set_state(samp, SampleState_Processing);
}

nframes_t
Sample_write(Sample samp,
             sample_t **buffers,
             channels_t channels,
             nframes_t frames) {
    assert(samp);

    if (! Sample_is_processing(samp)) {
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

        error = SRC_process(samp->src[chan], samp->src_ratio, audio_data,
                            &input_frames_used, &output_frames_gen,
                            &end_of_input);

        if (error) {
            fprintf(stderr, "Error in SRC_process: %s\n",
                    SRC_strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    if (end_of_input) {
        Sample_set_state(samp, SampleState_Finishing);
        Event_broadcast(samp->done_event);
    } else {
        samp->framep += input_frames_used;
    }

    return 0;
}

nframes_t
Sample_total_frames_written(Sample samp) {
    assert(samp);
    return samp->total_frames_written;
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

/**
 * Try to set framep
 */
static int
set_framep(Sample samp, nframes_t val) {
    if (0 == Mutex_trylock(samp->framep_mutex)) {
        samp->framep = val;
        return Mutex_unlock(samp->framep_mutex);
    } else {
        return -1;
    }
}

int
Sample_set_state(Sample samp,
                 SampleState state) {
    assert(samp->state_mutex);
    int not_locked = Mutex_lock(samp->state_mutex);
    if (not_locked) {
        return not_locked;
    } else {
        samp->state = state;
        return Mutex_unlock(samp->state_mutex);
    }
}
