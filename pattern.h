#ifndef PATTERN_H_INCLUDED
#define PATTERN_H_INCLUDED

#include "event.h"
#include "metro.h"
#include "note.h"

/**
 * Pattern is a container for Note's
 * It can be played with Lightning using
 * a metro.
 * The question is, what will be doing the playing?
 * Another ADT on top of Pattern, or Pattern itself?
 * If handled by Pattern, maybe
 *
 * // Spins up a thread that emits notes
 * Event
 * Pattern_play(Pattern pat, Metro metro);
 *
 */
typedef struct Pattern *Pattern;

typedef enum {
    PatternLoop,
    PatternOnce
} PatternMode;

/**
 * Initialize a pattern with a given length.
 *
 * @param {void *} data - Pointer to data that will be used to process the notes in the pattern
 */
Pattern
Pattern_init(int length, void *data, PatternMode mode);

/**
 * Get pattern note at a particular position.
 * Raises an exception if i > pat->length or i < 0.
 */
Note
Pattern_note(Pattern pat, int i);

/**
 * All Pattern_play* functions spin up a thread that emits notes.
 *
 * @return Event - must be free'd after usage
 */
Event
Pattern_play(Pattern pat, Metro metro);

void
Pattern_free(Pattern *pat);

#endif
