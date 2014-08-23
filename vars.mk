prefix=/usr/local
bindir=$(prefix)/bin
includedir=$(prefix)/include/lightning
libdir=$(prefix)/lib
# headers
HEADERS = lightning.h types.h
# examples, archive
LIBLIGHTNING_AR=liblightning.a
EXAMPLES := play-file play-sample
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# tests
TEST_DIR=test
TESTS := check-list
TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))
