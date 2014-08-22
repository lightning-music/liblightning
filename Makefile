# lightning
# Brian Sorahan 2014
include flags.mk
include objs.mk
include vars.mk

.PHONY: all install test clean

all .DEFAULT: $(LIBLIGHTNING_AR) $(PROGS) $(EXAMPLES)

install: $(PROGS) $(LIBLIGHTNING_AR)
	install $(PROGS) $(DESTDIR)$(bindir)
	install $(LIBLIGHTNING_AR) $(DESTDIR)$(libdir)

clean:
	rm -rf $(PROGS) $(EXAMPLES) *~ *.o examples/*~
	rm -rf $(LIBLIGHTNING_AR) $(TESTS) $(TEST_DIR)/*~
	rm -rf core *.log *.tar.gz

lightning-engine: lightning-engine.c $(LIBLIGHTNING_AR)

test: $(TESTS)
	$(TEST_DIR)/run

test-clean:
	rm -rf $(TESTS)

test/check-metro: test/check-metro.c $(LIBLIGHTNING_AR)
test/check-list: test/check-list.c $(LIBLIGHTNING_AR)

$(LIBLIGHTNING_AR): $(OBJS)
	$(AR) rcs $(LIBLIGHTNING_AR) $^

examples/play-file: examples/play-file.c $(LIBLIGHTNING_AR)
examples/play-sample: examples/play-sample.c $(LIBLIGHTNING_AR)
examples/webs: examples/webs.c $(LIBLIGHTNING_AR)
