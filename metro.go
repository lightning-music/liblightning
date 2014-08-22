package main

// #cgo CFLAGS: -Wall -g
// #cgo LDFLAGS: -L. -llightning -lm -ljack -lsndfile -llo -lpthread -lsamplerate
// #include "lightning.h"
import "C"

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"time"
)

func main() {
	srv := C.LightningServer_init(C.CString("41068"), nil, nil, 0, nil)
	defer C.LightningServer_free(&srv)

	dur, err := time.ParseDuration("100ms")

	if err != nil {
		fmt.Fprintln(os.Stderr, "Could not parse duration string")
	}

	ticker := time.NewTicker(dur)

	var snap *C.char = C.CString("audio/snap.flac")
	var kick *C.char = C.CString("audio/kick.flac")
	
	for _ = range ticker.C {
		chooser := rand.Float64()
		scale := (2.0 * rand.Float64()) - 1.0
		var speed C.pitch_t = C.pitch_t(math.Pow(2.0, scale))
		if chooser > 0.5 {
			C.LightningServer_play_sample(srv, snap, speed, 1.0)
		} else {
			C.LightningServer_play_sample(srv, kick, speed, 1.0)
		}
	}
}
