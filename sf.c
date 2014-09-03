// TODO: opus support
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
    nframes_t samplerate;
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
    sf->channels = sfinfo.channels;
    sf->frames = sfinfo.frames;
    sf->samplerate = sfinfo.samplerate;

    return sf;
}

channels_t
SF_channels(SF sf)
{
    assert(sf);
    return sf->channels;
}

nframes_t
SF_frames(SF sf)
{
    assert(sf);
    return sf->frames;
}

nframes_t
SF_samplerate(SF sf)
{
    assert(sf);
    return sf->samplerate;
}

nframes_t
SF_read(SF sf, sample_t *buf, nframes_t frames)
{
    assert(sf);
    return (nframes_t) sf_readf_float(sf->sfp, buf, frames);
}

nframes_t
SF_write(SF sf, sample_t *buf, nframes_t frames)
{
    assert(sf);
    return (nframes_t) sf_writef_float(sf->sfp, buf, frames);
}

const char *
SF_strerror(SF sf)
{
    assert(sf);
    return sf_strerror(sf->sfp);
}

void
SF_close(SF *sf)
{
    assert(sf && *sf);
    sf_close((*sf)->sfp);
    FREE(*sf);
}
