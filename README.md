liblightning
===========

liblightning is a C library for playing audio
samples with JACK.

[![Build Status](https://travis-ci.org/lightning/liblightning.svg?branch=master)](https://travis-ci.org/lightning/liblightning)

It is a very young project, but can already

* play back audio samples read from disk
* control pitch and gain for each triggered sample
* export its output to an audio file

Ideas for the near future are:

* support opus file format for playback and export
* expose a sequencer API



Build
-----

**Note** There are helper scripts named `install-*.sh` for installing some of the dependencies, the rest should be available in your distro's package manager.

Prerequisites:

* [jack](http://jackaudio.org)
* [libsndfile >= 1.0.25](http://www.mega-nerd.com/libsndfile/)
* [libsamplerate >= 0.1.8](http://www.mega-nerd.com/SRC/).

```shell
$ make
```

Test
----

Prerequisites:

* [check](http://check.sourceforge.net/)

```shell
$ make test
```

Install
-------

```shell
$ make install
```



### Related projects

* [lightning-go](https://github.com/lightning/lightning-go) - go bindings for liblightning
* [lightningd](https://github.com/lightning/lightningd) - An http/websocket server for controlling a liblightning-based backend.

### Discussion & Issues

lightning-music@googlegroups.com
