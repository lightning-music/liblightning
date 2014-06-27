#
# kidcomposer
# Brian Sorahan 2014
#
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
ALSA_FLAGS := $(shell pkg-config --cflags --libs alsa)
JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
CPPFLAGS := -I/usr/include/gtk-2.0
CFLAGS := $(GTK_FLAGS)
LDLIBS := $(GTK_FLAGS) $(ALSA_FLAGS) $(JACK_FLAGS) \
          $(SNDFILE_FLAGS) -lm

.PHONY: all

PROGS = kidcomposer packbox table-pack pcm-min simple_client \
        capture_client

all .DEFAULT: $(PROGS)

kidcomposer: kidcomposer.c
packbox: packbox.c
table-pack: table-pack.c
pcm-min: pcm-min.c
simple_client: simple_client.c
capture_client: capture_client.c

clean:
	rm -rf $(PROGS) *~
