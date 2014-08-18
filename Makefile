#
# lightning
# Make music on linux lightning fast!
#
# Brian Sorahan 2014
#
JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
LIBLO_FLAGS := $(shell pkg-config --cflags --libs liblo)
SRC_FLAGS := $(shell pkg-config --cflags --libs samplerate)
CC=gcc
CFLAGS := -Wall -g -std=c99
LDLIBS := $(JACK_FLAGS) $(SNDFILE_FLAGS) \
          $(LIBLO_FLAGS) $(SRC_FLAGS) -lm

prefix=/usr/local
bindir=$(prefix)/bin

.PHONY: all install

LIBLIGHTNING_AR=liblightning.a
PROGS = lightning-engine lightning-client

EXAMPLES := metro play-file play-sample 

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

OBJS := atom.o                \
        bin-tree.o            \
        clip.o                \
        event.o               \
        export-thread.o       \
        jack-client.o         \
        list.o                \
        lightning-server.o    \
        log.o                 \
        mem.o                 \
        mutex.o               \
        osc-server.o          \
        realtime.o            \
        ringbuffer.o          \
        sample.o              \
        samples.o             \
        src.o                 \
        thread.o

all .DEFAULT: $(PROGS) $(LIBLIGHTNING_AR) $(EXAMPLES)

$(LIBLIGHTNING_AR): $(OBJS)
	$(AR) rcs $(LIBLIGHTNING_AR) $^

install: $(PROGS)
	install $(PROGS) $(DESTDIR)$(bindir)

lightning-engine: lightning-engine.c $(OBJS)

examples/play-file: examples/play-file.c $(OBJS)

examples/play-sample: examples/play-sample.c $(OBJS)

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o examples/*~
	$(MAKE) -C test clean
	rm $(LIBLIGHTNING_AR)
