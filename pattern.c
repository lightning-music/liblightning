
#include <assert.h>

#include "mem.h"
#include "note.h"
#include "pattern.h"

struct Pattern {
    int length;
    Note *notes;
    void *data;
};

Pattern
Pattern_init(int length, void *data)
{
    Pattern pat;
    NEW(pat);
    pat->length = length;
    pat->notes = CALLOC(length, sizeof(Note));
    pat->data = data;
    return pat;
}

Note
Pattern_note(Pattern pat, int i)
{
    assert(pat);
    assert(i > 0 && i < pat->length);
    return pat->notes[i];
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
