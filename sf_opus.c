
#include <ogg/ogg.h>
#include <opus/opus.h>

#include "opus_header.h"

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
   OpusHeader header;

   if (opus_header_parse(op->packet, op->bytes, &header)==0)
   {
      fprintf(stderr, "Cannot parse header\n");
      return NULL;
   }

   *mapping_family = header.channel_mapping;
   *channels = header.channels;
   if(wav_format)adjust_wav_mapping(*mapping_family, *channels, header.stream_map);

   if(!*rate)*rate=header.input_sample_rate;
   /*If the rate is unspecified we decode to 48000*/
   if(*rate==0)*rate=48000;
   if(*rate<8000||*rate>192000){
     fprintf(stderr,"Warning: Crazy input_rate %d, decoding to 48000 instead.\n",*rate);
     *rate=48000;
   }

   *preskip = header.preskip;
   st = opus_multistream_decoder_create(48000, header.channels, header.nb_streams, header.nb_coupled, header.stream_map, &err);
   if(err != OPUS_OK){
     fprintf(stderr, "Cannot create decoder: %s\n", opus_strerror(err));
     return NULL;
   }
   if (!st)
   {
      fprintf (stderr, "Decoder initialization failed: %s\n", opus_strerror(err));
      return NULL;
   }

   *streams=header.nb_streams;

   if(header.gain!=0 || manual_gain!=0)
   {
      /*Gain API added in a newer libopus version, if we don't have it
        we apply the gain ourselves. We also add in a user provided
        manual gain at the same time.*/
      int gainadj = (int)(manual_gain*256.)+header.gain;
#ifdef OPUS_SET_GAIN
      err=opus_multistream_decoder_ctl(st,OPUS_SET_GAIN(gainadj));
      if(err==OPUS_UNIMPLEMENTED)
      {
#endif
         *gain = pow(10., gainadj/5120.);
#ifdef OPUS_SET_GAIN
      } else if (err!=OPUS_OK)
      {
         fprintf (stderr, "Error setting gain: %s\n", opus_strerror(err));
         return NULL;
      }
#endif
   }

   if (!quiet)
   {
      fprintf(stderr, "Decoding to %d Hz (%d channel%s)", *rate,
        *channels, *channels>1?"s":"");
      if(header.version!=1)fprintf(stderr, ", Header v%d",header.version);
      fprintf(stderr, "\n");
      if (header.gain!=0)fprintf(stderr,"Playback gain: %f dB\n", header.gain/256.);
      if (manual_gain!=0)fprintf(stderr,"Manual gain: %f dB\n", manual_gain);
   }

   return st;
}
