
#include <assert.h>

#include "mem.h"
#include "note.h"

struct Note {
    int number;
    int velocity;
};

Note
Note_init()
{
    Note note;
    NEW(note);
    return note;
}

int
Note_number(Note n)
{
    assert(n);
    return n->number;
}

int
Note_velocity(Note n)
{
    assert(n);
    return n->velocity;
}

void
Note_free(Note *note)
{
    assert(note && *note);
    FREE(*note);
}
