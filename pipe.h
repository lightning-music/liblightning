#ifndef PIPE_H_INCLUDED
#define PIPE_H_INCLUDED

#include "stream.h"

typedef struct Pipe *Pipe;

Stream
Pipe_init(Stream in,
          Stream out);

void
Pipe_free(Pipe *p);

#endif
