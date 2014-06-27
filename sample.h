#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "types.h"

typedef struct Sample *Sample;

Sample
Sample_load(const char *file);

kc_size_t
Sample_size(Sample *samp);

void
Sample_free(Sample *samp);

#endif
