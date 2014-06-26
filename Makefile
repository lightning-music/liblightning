#
# kidcomposer
# Brian Sorahan 2014
#
LIBCHUCK_SRC=libchuck/src
LIBCHUCK_AR=$(LIBCHUCK_SRC)/.libs/libchuck.a

GTK_FLAGS := $(shell pkg-config --cflags --libs gtk+-2.0)
CPPFLAGS := -I/usr/include/gtk-2.0
CFLAGS := $(GTK_FLAGS)
LDLIBS := $(GTK_FLAGS)

.PHONY: all

all .DEFAULT: kidcomposer packbox

kidcomposer: $(LIBCHUCK_AR) kidcomposer.cpp

packbox: packbox.c

$(LIBCHUCK_AR):
	cd libchuck && ./configure && make

clean:
	rm -rf kidcomposer packbox
