#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "jack-client.h"
#include "sample.h"
#include "types.h"

int
audio_callback(sample_t *ch1,
               sample_t *ch2,
               nframes_t frames,
               void *data) {
    Sample sample = (Sample) data;

    Sample_write_stereo(sample, ch1, ch2, frames);

    return 0;
}

void
usage_and_exit(char *prog) {
    printf("Usage\n");
    printf("%s path/to/audio/file\n", prog);
    exit(EXIT_FAILURE);
}

int
main(int argc, char **argv) {
    char *f;
    pitch_t pitch = 1.0;
    gain_t gain = 1.0;

    // require path to audio file

    if (argc < 2) {
        usage_and_exit(argv[0]);
    }

    f = argv[1];

    // see if pitch and gain were provided

    switch(argc) {
    case 4:
        gain = atof(argv[3]);
    case 3:
        pitch = atof(argv[2]);
        break;
    }

    // initialize sample and jack client

    Sample s = Sample_load(f, pitch, gain);
    JackClient jack_client = JackClient_init(audio_callback, s);

    // wait for sample to finish playing

    Sample_wait(s);

    // free sample and jack client

    Sample_free(&s);
    JackClient_free(&jack_client);

    return 0;
}
