
#include "add_buffer.h"
#include "types.h"

/**
 * Add one sample buffer to another
 * Both buffers should contain at least @a len samples
 */
nframes_t
add_buffers(sample_t *dest,
            sample_t *src,
            nframes_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        dest[i] = dest[i] + src[i];
    }
}

