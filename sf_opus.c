
#include <math.h>
#include <ogg/ogg.h>
#include <opus/opus.h>
#include <opus/opus_multistream.h>
#include <stdio.h>

#include "mem.h"
#include "opus_header.h"
#include "sf_opus.h"
#include "wav_io.h"

struct SF_Opus {
    ogg_sync_state *oy;
};

SF_Opus
SF_open_read_opus(const char *file)
{
    SF_Opus sf;
    NEW(sf);
    return sf;
}

SF_Opus
SF_open_write_opus(const char *file, channels_t channels,
                   nframes_t samplerate)
{
    SF_Opus sf;
    NEW(sf);
    return sf;
}

/*Process an Opus header and setup the opus decoder based on it.
  It takes several pointers for header values which are needed
  elsewhere in the code.*/
static OpusMSDecoder *
process_header(ogg_packet *op, opus_int32 *rate, int *mapping_family,
               int *channels, int *preskip, float *gain,
               float manual_gain, int *streams, int wav_format,
               int quiet)
{
   int err;
   OpusMSDecoder *st;
   OpusHeader header = OpusHeader_parse(op->packet, op->bytes);

   if (header == NULL) {
      fprintf(stderr, "Cannot parse header\n");
      return NULL;
   }

   *mapping_family = OpusHeader_channel_mapping(header);
   *channels = OpusHeader_channels(header);
   if (wav_format) {
       adjust_wav_mapping(*mapping_family, *channels,
                          OpusHeader_stream_map(header));
   }

   if(! *rate) {
       *rate = OpusHeader_input_sr(header);
   }

   /*If the rate is unspecified we decode to 48000*/
   if(*rate == 0) {
       *rate = 48000;
   }

   if(*rate < 8000 || *rate > 192000) {
       fprintf(stderr,"Warning: Crazy input_rate %d, "
               "decoding to 48000 instead.\n", *rate);
       *rate = 48000;
   }

   *preskip = OpusHeader_preskip(header);
   st = opus_multistream_decoder_create(48000,
                                        OpusHeader_channels(header),
                                        OpusHeader_num_streams(header),
                                        OpusHeader_num_coupled(header),
                                        OpusHeader_stream_map(header),
                                        &err);
   if(err != OPUS_OK) {
     fprintf(stderr, "Cannot create decoder: %s\n",
             opus_strerror(err));
     return NULL;
   }

   if (!st) {
      fprintf (stderr, "Decoder initialization failed: %s\n",
               opus_strerror(err));
      return NULL;
   }

   *streams = OpusHeader_num_streams(header);

   if(OpusHeader_gain(header) != 0 || manual_gain != 0) {
       /*Gain API added in a newer libopus version, if we don't have it
         we apply the gain ourselves. We also add in a user provided
         manual gain at the same time.*/
       int gainadj = (int) (manual_gain * 256.0) + OpusHeader_gain(header);
#ifdef OPUS_SET_GAIN
       err = opus_multistream_decoder_ctl(st, OPUS_SET_GAIN(gainadj));
       if (err == OPUS_UNIMPLEMENTED) {
#endif
           *gain = pow(10.0, gainadj / 5120.0);
#ifdef OPUS_SET_GAIN
       } else if (err != OPUS_OK) {
           fprintf (stderr, "Error setting gain: %s\n", opus_strerror(err));
           return NULL;
       }
#endif
   }

   if (!quiet) {
       fprintf(stderr, "Decoding to %d Hz (%d channel%s)", *rate,
               *channels, *channels > 1 ? "s" : "");

       if (OpusHeader_version(header) != 1) {
           fprintf(stderr, ", Header v%d", OpusHeader_version(header));
       }

       fprintf(stderr, "\n");

       if (OpusHeader_gain(header) != 0) {
           fprintf(stderr,"Playback gain: %f dB\n",
                   OpusHeader_gain(header) / 256.0);
       }

       if (manual_gain != 0) {
           fprintf(stderr,"Manual gain: %f dB\n", manual_gain);
       }
   }

   return st;
}
