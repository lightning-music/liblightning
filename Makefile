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
        capture_client

all .DEFAULT: $(PROGS)

simplify: simplify.c                         \
          kit.o kit.h                        \
          mem.o mem.h                        \
          audio-engine.o audio-engine.h      \
          mixer.o mixer.h

packbox: packbox.c
table-pack: table-pack.c
simple_client: simple_client.c
capture_client: capture_client.c

clean:
	rm -rf $(PROGS) *~ *.o
