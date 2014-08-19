JACK_FLAGS := $(shell pkg-config --cflags --libs jack)
SNDFILE_FLAGS := $(shell pkg-config --cflags --libs sndfile)
LIBLO_FLAGS := $(shell pkg-config --cflags --libs liblo)
SRC_FLAGS := $(shell pkg-config --cflags --libs samplerate)
CHECK_FLAGS := $(shell pkg-config --cflags --libs check)
LWS_FLAGS := $(shell pkg-config --cflags --libs libwebsockets)
JSON_FLAGS := $(shell pkg-config --cflags --libs jansson)
CC=gcc
ifeq ($(DEBUG),true)
CFLAGS := -Wall -g -std=c99
else
CFLAGS := -Wall -O2 -std=c99
endif
LDFLAGS := -L.
LDLIBS := $(JACK_FLAGS) $(SNDFILE_FLAGS) \
          $(LIBLO_FLAGS) $(SRC_FLAGS) \
          $(CHECK_FLAGS) $(JSON_FLAGS) -lm -lrt
