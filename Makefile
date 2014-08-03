#
# lightning
# Make music on linux lightning fast!
#
# Brian Sorahan 2014
#
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
LIBLO_FLAGS := $(shell pkg-config --cflags --libs liblo)
SRC_FLAGS := $(shell pkg-config --cflags --libs samplerate)
CPPFLAGS := -I/usr/include/gtk-2.0
CC=gcc
CFLAGS := -Wall -g $(GTK_FLAGS) -std=c99
LDLIBS := $(GTK_FLAGS) $(JACK_FLAGS) $(SNDFILE_FLAGS) \
          $(LIBLO_FLAGS) $(SRC_FLAGS) -lm

prefix=/usr/local
bindir=$(prefix)/bin

.PHONY: all install

PROGS = lightning-engine lightning-client

EXAMPLES := metro play-file play-sample 

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

all .DEFAULT: $(PROGS)

examples: $(EXAMPLES)

install: $(PROGS)
	install $(PROGS) $(DESTDIR)$(bindir)

lightning-engine: lightning-engine.c                                \
                  lightning-server.o lightning-server.h             \
                  samples.o samples.h                               \
                  list.o list.h                                     \
                  log.o log.h                                       \
                  clip.o clip.h                                     \
                  table.o table.h                                   \
                  thread.o thread.h                                 \
                  mem.o mem.h                                       \
                  jack-client.o jack-client.h                       \
                  realtime.o realtime.h                             \
                  ringbuffer.o ringbuffer.h                         \
                  event.o event.h                                   \
                  mutex.o mutex.h                                   \
                  osc-server.o osc-server.h                         \
                  gain.o gain.h                                     \
                  src.o src.h                                       \
                  sample.o sample.h

lightning-client: lightning-client.c

jack-client.o: ringbuffer.o mem.o
ringbuffer.o: mem.o
sample.o: mem.o event.o gain.o clip.o mutex.o src.o table.o
event.o: mem.o
kit.o: list.o

examples/play-file: examples/play-file.c       \
           mem.o mem.h                         \
           ringbuffer.o ringbuffer.h           \
           mutex.o mutex.h                     \
           src.o src.h                         \
           jack-client.o jack-client.h

examples/play-sample: examples/play-sample.c   \
             log.o log.h                       \
             clip.o clip.h                     \
             mem.o mem.h                       \
             realtime.o realtime.h             \
             ringbuffer.o ringbuffer.h         \
             thread.o thread.h                 \
             event.o event.h                   \
             mutex.o mutex.h                   \
             src.o src.h                       \
             gain.o gain.h                     \
             table.o table.h                   \
             samples.o samples.h               \
             sample.o sample.h                 \
             jack-client.o jack-client.h

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o examples/*~
	$(MAKE) -C test clean
