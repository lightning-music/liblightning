prefix=/usr/local
bindir=$(prefix)/bin
libdir=$(prefix)/lib
# programs, examples, archive
PROGS = lightning-engine lightning-client
LIBLIGHTNING_AR=liblightning.a
EXAMPLES := metro play-file play-sample 
EXAMPLES := $(addprefix examples/,$(EXAMPLES))
# tests
TEST_DIR=test
TESTS := check-list check-metro check-json
TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))
