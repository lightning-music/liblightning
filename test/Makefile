DEPTH := ..
PROGS := check-list lo-test
OBJS := list.o mem.o
OBJS := $(addprefix $(DEPTH)/,$(OBJS))
LIBLO_FLAGS := $(shell pkg-config --cflags --libs liblo)
CHECK_FLAGS := $(shell pkg-config --cflags --libs check)
LDLIBS := $(CHECK_FLAGS) $(LIBLO_FLAGS) $(OBJS)

all .DEFAULT: $(PROGS)

check-list: check-list.c

clean:
	rm -f $(PROGS) *~ *.o
