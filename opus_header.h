/* Copyright (C)2012 Xiph.Org Foundation
   File: opus_header.h

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OPUS_HEADER_H_INCLUDED
#define OPUS_HEADER_H_INCLUDED

typedef struct OpusHeader *OpusHeader;

OpusHeader
OpusHeader_parse(const unsigned char *header, int len);

int
OpusHeader_to_packet(const OpusHeader h, unsigned char *packet, int len);

int
OpusHeader_version(OpusHeader h);

int
OpusHeader_channels(OpusHeader h);

int
OpusHeader_preskip(OpusHeader h);

ogg_uint32_t
OpusHeader_input_sr(OpusHeader h);

int
OpusHeader_gain(OpusHeader h);

int
OpusHeader_channel_mapping(OpusHeader h);

int
OpusHeader_num_streams(OpusHeader h);

int
OpusHeader_num_coupled(OpusHeader h);

unsigned char *
OpusHeader_stream_map(OpusHeader h);

extern const int wav_permute_matrix[8][8];

#endif