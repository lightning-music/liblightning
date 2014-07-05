/*
 * Use audio-engine to play a sound file.
 */
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio-engine.h"
#include "types.h"

float *framebuf;

int main(int argc, char **argv) {
    AudioEngine engine = AudioEngine_init();
    const char *f = "/home/brian/Audio/freesound/playing_in_the_backyard_with_diggers.flac";
    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(f, SFM_READ, &sfinfo);
    long samples = sfinfo.frames * sfinfo.channels;

    // allocate frame buffer
    framebuf = malloc(sizeof(float) * samples);

    // read frame buffer
    while (sf_readf_float(sf, framebuf, sfinfo.frames)) ;

    // free frame buffer
    free(framebuf); framebuf = NULL;

    return 0;
}
