#
# lightning
# Make music on linux lightning fast!
#
# Brian Sorahan 2014
#
include flags.mk

prefix=/usr/local
bindir=$(prefix)/bin

.PHONY: all install test clean

PROGS = lightning-engine lightning-client

EXAMPLES := metro play-file play-sample 

EXAMPLES := $(addprefix examples/,$(EXAMPLES))

include objs.mk
include ar.mk
include test.mk

all .DEFAULT: $(PROGS) $(LIBLIGHTNING_AR) $(EXAMPLES)

install: $(PROGS)
	install $(PROGS) $(DESTDIR)$(bindir)

lightning-engine: lightning-engine.c $(OBJS)

examples/play-file: examples/play-file.c $(OBJS)

examples/play-sample: examples/play-sample.c $(OBJS)

test: $(TESTS)
	$(TEST_DIR)/run

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o examples/*~
	rm $(LIBLIGHTNING_AR) $(TESTS) $(TEST_DIR)/*~
