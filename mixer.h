#ifndef MIXER_H_INCLUDED
#define MIXER_H_INCLUDED

#include "audio-engine.h"
#include "types.h"

/*
 * Provide a way to mix multiple audio signals into one.
 */
typedef struct Mixer *Mixer;

/**
 * Initialize a new Mixer object
 *
 * @param  {AudioEngine} engine - An audio engine
 * @param  {unsigned int} num_channels - Number of channels
 * @return {Mixer} A new mixer object
 */
Mixer
Mixer_init(AudioEngine engine,
           unsigned int num_channels);

/**
 * Get number of mixer channels
 *
 * @param  {Mixer} m - A mixer object
 * @return {unsigned int} Number of channels in m
 */
unsigned int
Mixer_num_channels(Mixer m);

/**
 * Register a callback on a mixer channel.
 *
 * This action overrides any previous callbacks
 * that have been registered on the channel.
 *
 * @param  {Mixer}                   
 * @param  {unsigned int} channel - The channel number (must be less than Mixer_num_channels(m)
 * @param  {audio_process_callback} callback - The callback used to feed audio data into the channel
 * @return {int} 0 on success, nonzero for failure
 */
int
Mixer_connect(Mixer m,
              unsigned int channel,
              audio_process_callback callback);

/**
 * Free resources associated with a mixer object
 * @param   {Mixer *} m - Pointer to a mixer object. Must not be NULL
 */
void
Mixer_free(Mixer *m);

#endif
