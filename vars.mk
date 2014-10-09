prefix=/usr/local
bindir=$(prefix)/bin
includedir=$(prefix)/include/lightning
libdir=$(prefix)/lib
# headers
HEADERS := lightning.h types.h
HEADERS := $(addprefix lightning/,$(HEADERS))
# examples, archive
LIBLIGHTNING_AR=liblightning.a
EXAMPLES := play-file play-sample lightning-play-sample
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# tests
TEST_DIR=test
TESTS := check-list check-metro
TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))
# API docs
API_DOCS_DIR=/var/www/html/liblightning
