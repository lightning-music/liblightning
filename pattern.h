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

/* typedef enum { */
/*     PatternLoop, */
/*     PatternOnce */
/* } PatternMode; */

/**
 * Initialize a pattern with a given length.
 *
 * @param {void *} data - Pointer to data that will be used to process the notes in the pattern
 */
Pattern
Pattern_init(int length, const char *file);

/**
 * Append a note to the pattern.
 * Fails if the pattern is full.
 *
 * @return 0 on success, nonzero on failure
 */
int
Pattern_append_note(Pattern pat, Note note);

/**
 * Set a pattern's note at a particular index.
 * Fails if the i < 0 or i > length.
 *
 * @return 0 on success, nonzero on failure
 */
int
Pattern_set_note(Pattern pat, Note note, int i);

/**
 * Get pattern note at a particular position.
 * Raises an exception if i > pat->length or i < 0.
 */
Note
Pattern_note(Pattern pat, int i);

/**
 * Return the sample associated with this pattern
 */
const char *
Pattern_sample(Pattern pat);

void
Pattern_free(Pattern *pat);

#endif
