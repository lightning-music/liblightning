#ifndef SF_H_INCLUDED
#define SF_H_INCLUDED

#include "types.h"

typedef enum {
    SF_MODE_R,
    SF_MODE_W,
    SF_MODE_RW
} SF_MODE;

typedef struct SF *SF;

/**
 * Open a sound file.
 *
 * @param file - The path to a file
 * @param mode - Open mode (read, write, or read/write)
 */
SF
SF_open(const char *file,  SF_MODE mode);

/**
 * Get the channels of a sound file.
 */
channels_t
SF_channels(SF sf);

/**
 * Get the frames of a sound file.
 */
nframes_t
SF_frames(SF sf);

/**
 * Get the sample rate of a sound file.
 */
nframes_t
SF_samplerate(SF sf);

/**
 * Read @a frames frames from a sound file, and
 * store them in @a buf. @a buf must be large enough
 * to hold at least @a frames frames of data.
 *
 * @return Number of frames actually read
 */
nframes_t
SF_read(SF sf, sample_t *buf, nframes_t frames);

/**
 * Write @a frames frames of data to a sound file.
 */
nframes_t
SF_write(SF sf, sample_t *buf, nframes_t frames);

/**
 * Get a string that describes the last error that occured
 * with the given SF object.
 */
const char *
SF_strerror(SF sf);

/**
 * Close the soundfile.
 */
void
SF_close(SF *sf);

#endif
