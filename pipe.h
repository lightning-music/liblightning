#ifndef PIPE_H_INCLUDED
#define PIPE_H_INCLUDED

#include "stream.h"

typedef struct Pipe *Pipe;

/**
 * Pipe two Stream's together.
 */
Stream
Pipe_init(Stream in,
          Stream out);

/**
 * Free resources associated with this pipe
 */
void
Pipe_free(Pipe *p);

#endif
