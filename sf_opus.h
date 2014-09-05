#ifndef SF_OPUS_H_INCLUDED
#define SF_OPUS_H_INCLUDED

#include "types.h"

// here is a helpful comment taken straight from
// opus-tools-0.1.9/src/opusdec.c

/* .opus files use the Ogg container to provide framing and timekeeping.
 http://tools.ietf.org/html/draft-terriberry-oggopus
 The easiest way to decode the Ogg container is to use libogg, so
  thats what we do here.
 Using libogg is fairly straight forward-- you take your stream of bytes
  and feed them to ogg_sync_ and it periodically returns Ogg pages, you
  check if the pages belong to the stream you're decoding then you give
  them to libogg and it gives you packets. You decode the packets. The
  pages also provide timing information. */
typedef struct SF_Opus *SF_Opus;

SF_Opus
SF_open_read_opus(const char *file);

SF_Opus
SF_open_write_opus(const char *file, channels_t channels,
                   nframes_t samplerate);

#endif
