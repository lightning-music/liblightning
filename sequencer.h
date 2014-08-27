#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

#include "lightning.h"
#include "pattern.h"
#include "slave.h"

#define SEQUENCER_MAX_PATTERNS 16

/**
 * Sequencer is an ADT that manages patterns, a clock,
 * and uses a lightning instance to play samples.
 * It spawns a thread when initialized that will use the
 * ticks emitted by the clock to play the
 * notes in the patterns it contains.
 * It uses a reference to a lightning instance
 * to actually play the patterns.
 */
typedef struct Sequencer *Sequencer;

Sequencer
Sequencer_init(Lightning lightning, Slave clock);

/**
 * Add a pattern to the sequencer.
 * If max patterns has already been reached, return nonzero.
 * Return 0 on success.
 */
int
Sequencer_add_pattern(Sequencer seq, Pattern pat);

void
Sequencer_free(Sequencer *seq);

#endif
