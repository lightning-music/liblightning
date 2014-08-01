/**
 * A kit is responsible for loading and playing samples.
 */
#ifndef KIT_H_INCLUDED
#define KIT_H_INCLUDED

#include "list.h"
#include "sample.h"
#include "types.h"

/**
 * Maximum number of samples a kit can load.
 */
#define MAX_SAMPLES 32

typedef struct Kit *Kit;

/**
 * load all the samples in a given directory
 * expects a .kit file containing a list
 * of samples to load
 * uses @a engine for audio playback
 */
Kit
Kit_load(const char *name,
         const char *dir,
         nframes_t output_samplerate);

/**
 * Return the number of samples successfully loaded.
 */
int
Kit_num_samples(Kit kit);

/**
 * Get the samples that are a part of this kit.
 */
List
Kit_samples(Kit kit);

/**
 * Play an audio sample. Lookup with filename.
 */
void
Kit_play_file(Kit kit,
              const char *file,
              pitch_t pitch,
              gain_t gain);

/**
 * Play an audio sample. Lookup with index.
 * Index will be relative to how the samples were loaded.
 */
void
Kit_play_index(Kit kit,
               int index,
               pitch_t pitch,
               gain_t gain);

/**
 * Realtime callback.
 */
int
Kit_write(Kit kit,
          sample_t **buffers,
          channels_t channels,
          nframes_t frames);

/**
 * Free the samples.
 */
void
Kit_free(Kit *kit);

#endif
