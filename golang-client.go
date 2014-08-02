package main

import "github.com/hypebeast/go-osc/osc"

func main() {
	host := "localhost"
	port := 41068
	client := osc.NewOscClient(host, port)
	base := "/lightning/kits/default/samples"
	msg := osc.NewOscMessage(base + "/3")
	msg.Append(1.0)
	msg.Append(1.0)
	client.Send(msg)
	msg = osc.NewOscMessage(base + "/2")
	msg.Append(1.0)
	msg.Append(1.0)
	client.Send(msg)
}
