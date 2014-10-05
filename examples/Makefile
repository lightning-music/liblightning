# only experimental examples will be built by this Makefile
# usable examples are built by top-level make
CPPFLAGS=-I/usr/local/include
LDLIBS=-lzmq $(shell pkg-config --libs lua)
PROGS = luaserver luaclient
all: $(PROGS)
clean:
	rm -rf *~ $(PROGS)
