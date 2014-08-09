lightning-engine
===========

lightning-engine is the audio sample playback engine of lightning.

It is controlled with [OSC](http://opensoundcontrol.org).

[![Build Status](https://travis-ci.org/lightning/engine.svg?branch=master)](https://travis-ci.org/lightning/engine)

Build
-----

Prerequisites:

* [jack](jackaudio.org)
* [libsndfile](http://www.mega-nerd.com/libsndfile/)
* [libsamplerate](http://www.mega-nerd.com/SRC/).
* [liblo](http://liblo.sourceforge.net/)

Usage
-----

Start the engine

```shell
$ lightning-engine --port 21000
...
```

Play a sample from the audio folder

`./play <file> [gain] [pitch]`

```shell
$ ./play snap 
```

Then use the [OSC interface](https://github.com/lightning/engine/wiki/lightning-engine-OSC-interface).

### Discussion & Issues

lightning-music@googlegroups.com
