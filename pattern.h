#ifndef PATTERN_H_INCLUDED
#define PATTERN_H_INCLUDED

#include "note.h"

/**
 * Pattern is a container for Note's
 * It can be played with Lightning using
 * a metro.
 */
typedef struct Pattern *Pattern;

/**
 * Initialize a pattern with a given length.
 *
 * @param {void *} data - Pointer to data that will be used to process the notes in the pattern
 */
Pattern
Pattern_init(int length, void *data);

Note
Pattern_note(Pattern pat, int i);

void
Pattern_free(Pattern *pat);

#endif
