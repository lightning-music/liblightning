TEST_DIR=test
TESTS := check-list check-metro
CHECK_FLAGS := $(shell pkg-config --cflags --libs check)
LDLIBS := $(CHECK_FLAGS) -llightning

TESTS := $(addprefix $(TEST_DIR)/, $(TESTS))

all .DEFAULT: $(LIBLIGHTNING_AR) $(TESTS)

check-metro: check-metro.c $(LIBLIGHTNING_AR)
check-list: check-list.c $(LIBLIGHTNING_AR)
