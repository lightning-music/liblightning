#include <assert.h>

#include "audio-engine.h"
#include "mem.h"
#include "mixer.h"
#include "types.h"

// Mixer Channel data

typedef struct Channel {
} Channel;

// Mixer data

struct Mixer {
    AudioEngine engine;
    unsigned int num_channels;
    Channel *channels;
    Channel master;
};

Mixer
Mixer_init(AudioEngine engine,
           unsigned int num_channels) {
    Mixer mixer;

    NEW(mixer);

    // initialize fields of new mixer

    mixer->engine = engine;
    mixer->num_channels = num_channels;
    mem_calloc(num_channels, (long) mixer->channels, __FILE__, __LINE__);

    return mixer;
}

unsigned int
Mixer_num_channels(Mixer m) {
    assert(m);
    return m->num_channels;
}

int
Mixer_connect(Mixer m,
              unsigned int channel,
              audio_process_callback callback) {
    return 0;
}

void
Mixer_free(Mixer *m) {
    assert(m && *m);
    int i;
    for (i = 0; i < (*m)->num_channels; i++) {
    }
    FREE(*m);
}
