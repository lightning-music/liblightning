lib_LTLIBRARIES = liblightning.la

liblightning_la_SOURCES =     \
        atom.c                \
        bin-tree.c            \
        clip.c                \
        event.c               \
        export-thread.c       \
        jack-client.c         \
        lightning.c           \
        log.c                 \
        mem.c                 \
        mutex.c               \
        realtime.c            \
        ringbuffer.c          \
        sample.c              \
        sample-ram.c          \
        samples.c             \
        sf.c                  \
        src.c                 \
        thread.c

liblightning_la_CFLAGS = -Wall -g -O2

liblightning_la_LIBADD = -ljack -lsndfile -lsamplerate -lcheck -lrt -lm

liblightning_la_LDFLAGS = -release @PACKAGE_VERSION@ -version-info 0:0:0

nobase_include_HEADERS = lightning/lightning.h
