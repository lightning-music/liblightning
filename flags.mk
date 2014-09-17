LIGHTNING_FLAGS := $(shell pkg-config --cflags --libs jack sndfile samplerate check ogg opus)
CC=gcc
ifeq ($(DEBUG),true)
CFLAGS := -Wall -g
else
CFLAGS := -Wall -O2
endif
LDFLAGS := -L.
LDLIBS := -llightning $(LIGHTNING_FLAGS) -lrt -lm
