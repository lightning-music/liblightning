#ifndef LIGHTNING_H_INCLUDED
#define LIGHTNING_H_INCLUDED

#include "types.h"

/**
 * lightning server data type
 *
 * The server initializes a jack client and exposes
 * OSC endpoints to load and playback audio samples.
 *
 * The server also broadcasts messages over OSC
 * that provide clients a way to know what it is doing
 * internally.
 */
typedef struct Lightning *Lightning;

/**
 * Create a new lightning server.
 * If @a listenPort is NULL then liblo chooses an unused system port.
 */
Lightning
Lightning_init();

/**
 * Add a directory for lighting to search for audio files.
 */
int
Lightning_add_dir(Lightning lightning, const char *dir);

/**
 * Play a sample
 *
 * @return 0 success, nonzero failure
 */
int
Lightning_play_sample(Lightning lightning, const char *file,
                      pitch_t pitch, gain_t gain);

/**
 * Want to be able to play patterns too.
 * Playing a pattern should
 * 1. Spin up a thread
 * 2. Use a Lightning instance to trigger samples
 * 3. Samples will be triggered with an Event.
 * 
 * How to write sequencer?
 * How are all the ADT's involved related to each other?
 * Patterns are collections of Note's.
 * Note's contain the data that is actually used to trigger
 * sounds.
 * Metro master maintains a clock that invokes a callback.
 * Metro slaves emit events at an interval which is an
 * integer ratio of their master.
 * These events get caught by a Sequencer which
 * contains a set of patterns and plays them according
 * to the events it receives.
 * For the purposes of v1, we will have only sample-playback
 * events.
 */
/* int */
/* Lightning_play_pattern(Lightning lightning, Pattern pat); */

/**
 * Start exporting to an audio file
 */
int
Lightning_export_start(Lightning lightning, const char *file);

/**
 * If currently exporting, stop
 */
int
Lightning_export_stop(Lightning lightning);

/**
 * Free the resources held by a lightning server
 */
void
Lightning_free(Lightning *lightning);

#endif
