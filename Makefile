#
# kidcomposer
# Brian Sorahan 2014
#
GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
CPPFLAGS := -I/usr/include/gtk-2.0
CFLAGS := $(GTK_FLAGS)
LDLIBS := $(GTK_FLAGS)

.PHONY: all

all .DEFAULT: kidcomposer     \
              packbox         \
              table-pack

kidcomposer: kidcomposer.cpp
packbox: packbox.c
table-pack: table-pack.c

clean:
	rm -rf kidcomposer packbox table-pack *~
