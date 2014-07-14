#
# simplify
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

PROGS = simplify packbox table-pack simple_client \
        capture_client play-file play-sample

all .DEFAULT: $(PROGS)

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

packbox: packbox.c
table-pack: table-pack.c
simple_client: simple_client.c
capture_client: capture_client.c
play-file: play-file.c                         \
           mem.o mem.h                         \
           ringbuffer.o ringbuffer.h           \
           jack-client.o jack-client.h

play-sample: play-sample.c                     \
             clip.o clip.h                     \
             mem.o mem.h                       \
             ringbuffer.o ringbuffer.h         \
             sample.o sample.h                 \
             jack-client.o jack-client.h

clean:
	rm -rf $(PROGS) *~ *.o
