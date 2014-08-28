
#include <assert.h>
#include <math.h>

#include "event.h"
#include "lightning.h"
#include "log.h"
#include "mem.h"
#include "mutex.h"
#include "note.h"
#include "pattern.h"
#include "sequencer.h"
#include "slave.h"
#include "thread.h"
#include "types.h"

struct Sequencer {
    int num_patterns;
    Pattern *patterns;
    Mutex patterns_mutex;
    volatile int playing;
    Mutex playing_mutex;
    Lightning lightning;
    Slave clock;
};

static void *
play_samples(void *arg);

Sequencer
Sequencer_init(Lightning lightning, Slave clock)
{
    Sequencer seq;
    NEW(seq);
    seq->num_patterns = 0;
    seq->patterns = CALLOC(SEQUENCER_MAX_PATTERNS, sizeof(Pattern));
    seq->lightning = lightning;
    seq->patterns_mutex = Mutex_init();
    seq->clock = clock;
    seq->playing = 0;
    seq->playing_mutex = Mutex_init();
    Thread_create(play_samples, seq);
    return seq;
}

int
Sequencer_add_pattern(Sequencer seq, Pattern pat)
{
    assert(seq);
    int error;
    if (seq->num_patterns == SEQUENCER_MAX_PATTERNS - 1) {
        LOG(Warn, "max patterns (%d) reached", SEQUENCER_MAX_PATTERNS);
        return 1;
    }
    /* acquire mutex */
    error = Mutex_lock(seq->patterns_mutex);
    if (error) {
        LOG(Warn, "could not lock %s", "patterns_mutex");
        return error;
    }
    /* add pattern */
    seq->patterns[ seq->num_patterns++ ] = pat;
    /* release mutex */
    error = Mutex_unlock(seq->patterns_mutex);
    if (error) {
        LOG(Warn, "could not unlock %s", "patterns_mutex");
        return 1;
    }
    return 0;
}

static int
Sequencer_set_playing(Sequencer seq, int val)
{
    assert(seq);
    /* acquire mutex */
    int error = Mutex_lock(seq->playing_mutex);
    if (error) {
        LOG(Warn, "could not lock %s", "playing_mutex");
        return error;
    }
    seq->playing = val;
    /* release mutex */
    error = Mutex_unlock(seq->playing_mutex);
    if (error) {
        LOG(Warn, "could not unlock %s", "playing_mutex");
        return error;
    }
    return 0;
}

int
Sequencer_start(Sequencer seq)
{
    return Sequencer_set_playing(seq, 1);
}

int
Sequencer_stop(Sequencer seq)
{
    return Sequencer_set_playing(seq, 0);
}

void
Sequencer_free(Sequencer *seq)
{
    assert(seq && *seq);
    FREE(*seq);
}

/**
 * mapping MIDI notes to sample playback rate
 * 48  => 0.5
 * 60  => 1.0
 * 72  => 2.0
 */
static pitch_t
get_pitch(Note note)
{
    return pow(2.0, (Note_number(note) - 60.0) / 12.0);
}

static gain_t
get_gain(Note note)
{
    return Note_velocity(note) / 127.0;
}

static void *
play_samples(void *arg)
{
    Sequencer seq = (Sequencer) arg;
    Note note;
    int i;
    Event sync = Slave_event(seq->clock);
    Event_wait(sync);
    int *pos = Event_value(sync);
    if (0 == Mutex_lock(seq->patterns_mutex)) {
        for (i = 0; i < seq->num_patterns; i++) {
            note = Pattern_note(seq->patterns[i], *pos);

            Lightning_play_sample(seq->lightning,
                                  Pattern_sample(seq->patterns[i]),
                                  get_pitch(note),
                                  get_gain(note));
        }
        if (0 != Mutex_unlock(seq->patterns_mutex)) {
            LOG(Warn, "could not unlock %s", "patterns_mutex");
        }
    } else {
        LOG(Warn, "could not lock %s", "patterns_mutex");
    }
    return (void *) NULL;
}
