LIGHTNING_FLAGS := $(shell pkg-config --cflags --libs jack sndfile samplerate check ogg opus)
CC=gcc
ifeq ($(DEBUG),true)
CFLAGS := -Wall -g -std=c99
else
CFLAGS := -Wall -O2 -std=c99
endif
LDFLAGS := -L.
LDLIBS := -llightning $(LIGHTNING_FLAGS)
