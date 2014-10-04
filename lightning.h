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
 * Connect lightning to a pair of JACK sinks.
 *
 * @param  lightning   {Lightning}
 * @param  ch1         {const char *}
 * @param  ch2         {const char *}
 *
 * @return 0 (success), nonzero (failure)
 */
int
Lightning_connect_to(Lightning lightning, const char *ch1, const char *ch2);

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
