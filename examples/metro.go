package main

import (
	"fmt"
	"github.com/hypebeast/go-osc/osc"
	"os"
	"time"
)

func main() {
	dur, err := time.ParseDuration("250ms")
	if err != nil {
		fmt.Fprintln(os.Stderr, "Could not parse duration string")
	}
	ticker := time.NewTicker(dur)
	host := "localhost"
	port := 41068
	client := osc.NewOscClient(host, port)
	base := "/lightning/kits/default/samples"
	msg := osc.NewOscMessage(base + "/0")
	msg.Append(1.0)
	msg.Append(1.0)
	for _ = range ticker.C {
		client.Send(msg)
	}
}
