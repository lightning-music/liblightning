# liblightning
# Brian Sorahan 2014
include flags.mk
include objs.mk
include vars.mk

.PHONY: all install clean docs

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

docs:
	doxygen
	scp -r docs/html/* root@sorahan.net:/var/www/html/liblightning
