LIGHTNING_FLAGS := $(shell pkg-config --cflags --libs jack sndfile samplerate check)
CC=gcc
CFLAGS := -Wall -g
LDFLAGS := -L.
LDLIBS := -llightning $(LIGHTNING_FLAGS) -lrt -lm
