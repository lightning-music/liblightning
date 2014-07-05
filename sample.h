#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "types.h"

typedef struct Sample *Sample;

Sample
Sample_load(const char *file);

const char *
Sample_path(Sample samp);

nframes_t
Sample_num_frames(Sample samp);

void
Sample_free(Sample *samp);

#endif
