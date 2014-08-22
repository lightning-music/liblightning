prefix=/usr/local
bindir=$(prefix)/bin
libdir=$(prefix)/lib
# examples, archive
LIBLIGHTNING_AR=liblightning.a
EXAMPLES := play-file play-sample
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# tests
TEST_DIR=test
TESTS := check-list
TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))
