package engine

// #cgo CFLAGS: -Wall -g
// #cgo LDFLAGS: -L. -llightning -lm -ljack -lsndfile -llo -lpthread -lsamplerate
// #include "samples.h"
import "C"

type Samples C.Samples

var samples Samples = C.Samples_init()
