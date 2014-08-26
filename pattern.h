#ifndef PATTERN_H_INCLUDED
#define PATTERN_H_INCLUDED

#include "note.h"

/**
 * Pattern is a container for Note's
 * It can be played with Lightning using
 * a metro.
 */
typedef struct Pattern *Pattern;

Pattern
Pattern_init(int length, const char *sample);

Note
Pattern_note(Pattern pat, int i);

void
Pattern_free(Pattern *pat);

#endif
