#
# lightning
# Make music on linux lightning fast!
#
# Brian Sorahan 2014
#
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
SAMPLERATE=libsamplerate-0.1.8
CPPFLAGS := -I/usr/include/gtk-2.0 -I$(SAMPLERATE)/src
CC=gcc
CFLAGS := -Wall -g $(GTK_FLAGS)
LDLIBS := $(GTK_FLAGS) $(JACK_FLAGS) $(SNDFILE_FLAGS) \
          -L$(SAMPLERATE)/src/.libs -lsamplerate -lm

prefix=/usr/local
bindir=$(prefix)/bin

.PHONY: all install

PROGS = lightning

EXAMPLES := packbox table-pack simple_client \
        capture_client play-file play-sample

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

all .DEFAULT: $(PROGS)

examples: $(EXAMPLES)

install: $(PROGS)
	install $(PROGS) $(DESTDIR)$(bindir)

lightning: lightning.c                         \
           kit.o kit.h                         \
           clip.o clip.h                       \
           mem.o mem.h                         \
           jack-client.o jack-client.h         \
           ringbuffer.o ringbuffer.h           \
           event.o event.h                     \
           mutex.o mutex.h                     \
           stream.o stream.h                   \
           sample.o sample.h

jack-client.o: ringbuffer.o mem.o
ringbuffer.o: mem.o
sample.o: mem.o event.o
event.o: mem.o

examples/packbox: examples/packbox.c
examples/table-pack: examples/table-pack.c
examples/simple_client: examples/simple_client.c
examples/capture_client: examples/capture_client.c

examples/play-file: examples/play-file.c       \
           mem.o mem.h                         \
           ringbuffer.o ringbuffer.h           \
           mutex.o mutex.h                     \
           jack-client.o jack-client.h

examples/play-sample: examples/play-sample.c   \
             clip.o clip.h                     \
             mem.o mem.h                       \
             ringbuffer.o ringbuffer.h         \
             event.o event.h                   \
             mutex.o mutex.h                   \
             stream.o stream.h                 \
             sample.o sample.h                 \
             jack-client.o jack-client.h

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o
