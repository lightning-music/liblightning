#include <jack/jack.h>
#include "mem.h"
#include "audio-engine.h"

struct AudioEngine {
    const char **ports;
    const char *client_name;
    const char *server_name;
    jack_options_t jack_options;
    jack_status_t jack_status;
};

AudioEngine
AudioEngine_init() {
}

void
AudioEngine_free(AudioEngine *ae) {
}

