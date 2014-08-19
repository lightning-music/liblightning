lightning-engine
===========

lightning-engine is the audio sample playback engine of lightning.

It is controlled with [OSC](http://opensoundcontrol.org).

[![Build Status](https://travis-ci.org/lightning/engine.svg?branch=master)](https://travis-ci.org/lightning/engine)



Build
-----

**Note** There are helper scripts for installing some of these dependencies from source in the lightning/engine repo. These scripts are named `install-*.sh`.

Prerequisites:

* [jack](http://jackaudio.org)
* [libsndfile >= 1.0.25](http://www.mega-nerd.com/libsndfile/)
* [libsamplerate >= 0.1.8](http://www.mega-nerd.com/SRC/).
* [liblo >= 0.28](http://liblo.sourceforge.net/)
* [libwebsockets 1.3-chrome37-firefox30](http://libwebsockets.org)
* [libjansson >= 2.6](http://www.digip.org/jansson/)

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


Usage
-----

Start the engine

```shell
$ lightning-engine --port 21000
```



Then visit [the OSC interface](https://github.com/lightning/engine/wiki/lightning-engine-OSC-interface).

### Discussion & Issues

lightning-music@googlegroups.com
