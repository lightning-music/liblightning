liblightning
===========

liblightning is a C library for playing audio samples with JACK on Linux systems.

[![Build Status](https://travis-ci.org/lightning/liblightning.svg?branch=master)](https://travis-ci.org/lightning/liblightning)



Current features:

* play back audio samples read from disk using JACK
* control pitch and gain for each triggered sample
* export to an audio file

Ideas for the near future are:

* support opus file format for playback and export
* expose a lua API that supports dynamic code loading (a la ChucK)



Build
-----

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

* [lightning](https://github.com/lightning/lightning) - go bindings for liblightning
* [lightningd](https://github.com/lightning/lightningd) - An http/websocket server for controlling a liblightning-based backend.

### Discussion

lightning-music@googlegroups.com
