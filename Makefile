.PHONY: libchuck

kidcomposer .DEFAULT: libchuck

libchuck:
	cd libchuck && ./configure && make

