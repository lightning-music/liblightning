lightning-engine
===========

lightning-engine is the audio sample playback engine of lightning.

It is controlled with [OSC](4).

[![Build Status](https://travis-ci.org/briansorahan/lightning-engine.svg?branch=master)](https://travis-ci.org/briansorahan/lightning-engine)

Build
-----

Prerequisites:

* [jack](1)
* [libsndfile](2)
* [libsamplerate](3).
* [liblo](6)

Usage
-----

Start the engine

```shell
$ lightning-engine --port 21000
...
```

Then use the [OSC interface](https://github.com/briansorahan/lightning-engine/wiki/lightning-engine-OSC-interface).

### Discussion & Issues

lightning-music@googlegroups.com

[1]: http://jackaudio.org
[2]: http://www.mega-nerd.com/libsndfile/
[3]: http://www.mega-nerd.com/SRC/
[4]: http://opensoundcontrol.org
[5]: https://github.com/briansorahan/lightning-engine/wiki/lightning-engine-OSC-interface
[6]: http://liblo.sourceforge.net/
