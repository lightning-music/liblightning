#
# simplify
# Easily make sample-based music on Linux
#
# Brian Sorahan 2014
#
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
CPPFLAGS := -I/usr/include/gtk-2.0
CC=gcc
CFLAGS := -Wall -g $(GTK_FLAGS)
LDLIBS := $(GTK_FLAGS) $(JACK_FLAGS) $(SNDFILE_FLAGS) -lm

.PHONY: all

PROGS = simplify

EXAMPLES := packbox table-pack simple_client \
        capture_client play-file play-sample

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

all .DEFAULT: $(PROGS)

examples: $(EXAMPLES)

simplify: simplify.c                          \
          kit.o kit.h                         \
          clip.o clip.h                       \
          mem.o mem.h                         \
          jack-client.o jack-client.h         \
          ringbuffer.o ringbuffer.h           \
          sample.o sample.h

jack-client.o: ringbuffer.o mem.o
ringbuffer.o: mem.o
sample.o: mem.o

examples/packbox: examples/packbox.c
examples/table-pack: examples/table-pack.c
examples/simple_client: examples/simple_client.c
examples/capture_client: examples/capture_client.c

examples/play-file: examples/play-file.c       \
           mem.o mem.h                         \
           ringbuffer.o ringbuffer.h           \
           jack-client.o jack-client.h

examples/play-sample: examples/play-sample.c   \
             clip.o clip.h                     \
             mem.o mem.h                       \
             ringbuffer.o ringbuffer.h         \
             sample.o sample.h                 \
             jack-client.o jack-client.h

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o
