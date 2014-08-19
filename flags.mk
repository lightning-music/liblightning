LIGHTNING_FLAGS := $(shell pkg-config --cflags --libs jack sndfile liblo samplerate check libwebsockets jansson)
CC=gcc
ifeq ($(DEBUG),true)
CFLAGS := -Wall -g -std=c99
else
CFLAGS := -Wall -O2 -std=c99
endif
LDFLAGS := -L.
LDLIBS := -llightning $(LIGHTNING_FLAGS)
