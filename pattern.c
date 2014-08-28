
#include <assert.h>

#include "atom.h"
#include "event.h"
#include "mem.h"
#include "metro.h"
#include "note.h"
#include "pattern.h"
#include "types.h"

struct Pattern {
    int flags;
    int length;
    int num_notes;
    Note *notes;
    /* void *data; */
    /* PatternMode mode; */
    const char *file;
};

Pattern
Pattern_init(int length, const char *file)
{
    Pattern pat;
    NEW(pat);
    pat->length = length;
    pat->notes = CALLOC(length, sizeof(Note));
    pat->file = Atom_string(file);
    pat->num_notes = 0;
    return pat;
}

Note
Pattern_note(Pattern pat, int i)
{
    assert(pat);
    assert(i > 0);
    return pat->notes[ i % pat->length ];
}

int
Pattern_append_note(Pattern pat, Note note)
{
    assert(pat);
    if (pat->num_notes == pat->length) {
        return 1;
    }
    pat->notes[ pat->num_notes++ ] = note;
    return 0;
}

int
Pattern_set_note(Pattern pat, Note note, int i)
{
    assert(pat);
    if (i < 0 || i > pat->length - 1) {
        return 1;
    }
    Note prev = pat->notes[i];
    pat->notes[i] = note;
    if (prev != NULL) {
        Note_free(&prev);
    }
    return 0;
}

const char *
Pattern_sample(Pattern pat)
{
    assert(pat);
    return pat->file;
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
