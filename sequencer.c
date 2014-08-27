
#include <assert.h>

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

struct Sequencer {
    int num_patterns;
    Pattern *patterns;
    Mutex patterns_mutex;
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
    seq->clock = slave;
    Thread_create(play_samples, seq);
    return seq;
}

int
Sequencer_add_pattern(Sequencer seq, Pattern pat)
{
    assert(seq);
    if (seq->num_patterns == SEQUENCER_MAX_PATTERNS - 1) {
        if (0 == Mutex_lock(seq->patterns_mutex)) {
            seq->patterns[ seq->num_patterns++ ] = pat;
            if (0 != Mutex_unlock(seq->patterns_mutex)) {
                LOG(Warn, "could not unlock %s", "patterns_mutex");
                return 1;
            }
        } else {
            LOG(Warn, "could not lock %s", "patterns_mutex");
            return 1;
        }
    } else {
        LOG(Warn, "max patterns (%d) reached", SEQUENCER_MAX_PATTERNS);
        return 1;
    }
    return 0;
}

void
Sequencer_free(Sequencer *seq)
{
    assert(seq && *seq);
    FREE(*seq);
}

/**
 * middle C (MIDI note 60) is considered to be
 * a speed of 1 for sample playback.
 */
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
            note = Pattern_note(seq->patterns[i]);
            // TODO: play samples
        }
        if (0 != Mutex_unlock(seq->patterns_mutex)) {
            LOG(Warn, "could not unlock %s", "patterns_mutex");
        }
    } else {
        LOG(Warn, "could not lock %s", "patterns_mutex");
    }
}
