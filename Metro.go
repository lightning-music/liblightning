package engine

import (
	"time"
)

// tempo in bpm
type Tempo uint32

type Pos uint64

type Metro interface {
	Ticker() chan Pos
	Stop()
}

type metroImpl struct {
	t *time.Ticker
	c chan Pos
}

func (m *metroImpl) Ticker() chan Pos {
	return m.c
}

func (m *metroImpl) Stop() {
	m.t.Stop()
}

func count(m *metroImpl) {
	var i Pos = 0
	for _ = range m.t.C {
		i++
		m.c <- i
	}
}

func NewMetro(t Tempo) Metro {
	m := new(metroImpl)
	m.t = time.NewTicker(time.Duration(1000000))
	m.c = make(chan Pos)
	go count(m)
	return m
}
