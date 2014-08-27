
#include <assert.h>

#include "event.h"
#include "mem.h"
#include "metro.h"
#include "note.h"
#include "pattern.h"
#include "types.h"

struct Pattern {
    int flags;
    int length;
    Note *notes;
    void *data;
    PatternMode mode;
};

/**
 *
 */
static int
play_callback(position_t pos, void *data);

Pattern
Pattern_init(int length, void *data, PatternMode mode)
{
    Pattern pat;
    NEW(pat);
    pat->length = length;
    pat->notes = CALLOC(length, sizeof(Note));
    pat->data = data;
    /* will be used at play time */
    pat->mode = mode;
    return pat;
}

Note
Pattern_note(Pattern pat, int i)
{
    assert(pat);
    assert(i > 0 && i < pat->length);
    return pat->notes[i];
}

Event
Pattern_play(Pattern pat, Metro metro)
{
    Event event = Event_init();
    return event;
}

void
Pattern_free(Pattern *pat)
{
    assert(pat && *pat);
    Pattern p = *pat;
    int i;
    for (i = 0; i < p->length; i++) {
        Note_free(&p->notes[i]);
    }
    FREE(p->notes);
    FREE(*pat);
}

static int
play_callback(position_t pos, void *data)
{
    return 0;
}
