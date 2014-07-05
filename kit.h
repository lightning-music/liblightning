#ifndef KIT_H_INCLUDED
#define KIT_H_INCLUDED

#include "sample.h"
#include "audio-engine.h"

/*
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
Kit_load(const char *dir,
         AudioEngine engine);

/**
 * Return the number of samples successfully loaded.
 */
unsigned int
Kit_num_samples(Kit kit);

/**
 * Return a list of the samples used in this kit.
 */
Sample *
Kit_sample_list(Kit kit);

/**
 * Play the audio sample using the given audio engine.
 * Uses the default audio engine if @a engine is NULL.
 */
void
Kit_play_sample(Kit kit,
                Sample samp);

void
Kit_free(Kit *kit);

#endif
