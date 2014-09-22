# liblightning
# Brian Sorahan 2014
include flags.mk
include objs.mk
include vars.mk

.PHONY: all install clean

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
	rm -rf $(LIBLIGHTNING_AR)
	rm -rf core *.log *.tar.gz

test/check-list: test/check-list.c $(LIBLIGHTNING_AR)

$(LIBLIGHTNING_AR): $(OBJS)
	$(AR) rcs $(LIBLIGHTNING_AR) $^

examples/play-file: examples/play-file.c $(LIBLIGHTNING_AR)
examples/play-sample: examples/play-sample.c $(LIBLIGHTNING_AR)
