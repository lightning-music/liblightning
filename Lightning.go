package engine

// go bindings for the lightning.h entry point

// #cgo CFLAGS: -Wall -O2
// #cgo LDFLAGS: -L. -llightning -lm -ljack -lsndfile -lpthread -lsamplerate
// #include "lightning.h"
// #include "types.h"
import "C"

type impl struct {
	handle C.Lightning
}

func (this *impl) AddDir(file string) int {
	return int(C.Lightning_add_dir(this.handle, C.CString(file)))
}

func (this *impl) PlaySample(file string, speed float32, gain float32) int {
	return int(C.Lightning_play_sample(
		this.handle, C.CString(file), C.pitch_t(speed), C.gain_t(gain),
	))
}

func (this *impl) ExportStart(file string) int {
	return int(C.Lightning_export_start(
		this.handle, C.CString(file),
	))
}

func (this *impl) ExportStop() int {
	return int(C.Lightning_export_stop(this.handle))
}

type Lightning interface {
	AddDir(file string) int
	PlaySample(file string, speed float32, gain float32) int
	ExportStart(file string) int
	ExportStop() int
}

func New() Lightning {
	instance := new(impl)
	instance.handle = C.Lightning_init()
	return instance
}
