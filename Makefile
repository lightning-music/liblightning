# lightning
# Brian Sorahan 2014
include flags.mk
include objs.mk
include vars.mk

.PHONY: all install test clean

all .DEFAULT: $(LIBLIGHTNING_AR) $(EXAMPLES)

install: $(LIBLIGHTNING_AR) $(includedir)
	install -vC $(LIBLIGHTNING_AR) $(DESTDIR)$(libdir)
	install -vC $(HEADERS) $(DESTDIR)$(includedir)

$(includedir):
	mkdir $(includedir)

uninstall:
	rm $(DESTDIR)$(libdir)/$(LIBLIGHTNING_AR)

clean:
	rm -rf $(EXAMPLES) *~ *.o examples/*~
	rm -rf $(LIBLIGHTNING_AR) $(TESTS) $(TEST_DIR)/*~
	rm -rf core *.log *.tar.gz

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
examples/play-pattern: examples/play-pattern.c $(LIBLIGHTNING_AR)
