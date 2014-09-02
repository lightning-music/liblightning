
#include <assert.h>
#include <sndfile.h>
#include <stddef.h>

#include "mem.h"
#include "sf.h"
#include "types.h"

struct SF {
    SNDFILE *sfp;
    nframes_t frames;
    channels_t channels;
};

SF
SF_open(const char *file,  SF_MODE mode)
{
    SF sf;
    NEW(sf);

    int sndfile_mode;
    switch (mode) {
    case SF_MODE_R:  sndfile_mode = SFM_READ;  break;
    case SF_MODE_W:  sndfile_mode = SFM_WRITE; break;
    case SF_MODE_RW: sndfile_mode = SFM_RDWR;  break;
    default: sndfile_mode = SFM_RDWR;
    }

    SF_INFO sfinfo;
    sf->sfp = sf_open(file, sndfile_mode, &sfinfo);

    return sf;
}

nframes_t
SF_read(SF sf, sample_t *buf, nframes_t frames)
{
    return 0;
}

nframes_t
SF_write(SF sf, sample_t *buf, nframes_t frames)
{
    return 0;
}

void
SF_close(SF *sf)
{
}
