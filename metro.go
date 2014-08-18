package main

// #cgo CFLAGS: -Wall -g
// #cgo LDFLAGS: -L. -llightning -lm -ljack -lsndfile -llo -lpthread -lsamplerate
// #include <stddef.h>
// #include "lightning-server.h"
import (
	"C"
)

import (
	"fmt"
	// "github.com/hypebeast/go-osc/osc"
	"os"
	"time"
)

func main() {
	// address := "127.0.0.1"
	// port := 8765
	// server := osc.NewOscServer(address, port)

	// server.AddMsgHandler("/sample", func(msg *osc.OscMessage) {
	// 	osc.PrintOscMessage(msg)
	// })

	srv := C.LightningServer_init(C.CString("41068"), nil, nil, 0, nil)
	defer C.LightningServer_free(&srv)

	dur, err := time.ParseDuration("200ms")

	if err != nil {
		fmt.Fprintln(os.Stderr, "Could not parse duration string")
	}

	ticker := time.NewTicker(dur)
	
	for _ = range ticker.C {
		C.LightningServer_play_sample(srv, C.CString("audio/snap.flac"), 1.0, 1.0)
	}

	// server.ListenAndDispatch()
	// C.LightningServer_listen(srv)
}
