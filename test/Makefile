DEPTH := ..
PROGS := check-list
OBJS := list.o mem.o
OBJS := $(addprefix $(DEPTH)/,$(OBJS))
CHECK_LIBS := $(shell pkg-config --cflags --libs check)
LDLIBS := $(CHECK_LIBS) $(OBJS)

all .DEFAULT: $(PROGS)

check-list: check-list.c

clean:
	rm -f $(PROGS) *~ *.o
