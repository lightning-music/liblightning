lightning-engine
===========

lightning-engine is an audio sample playback server that can be controlled with
[OSC](http://opensoundcontrol.org), HTTP, and [websockets](http://tools.ietf.org/html/rfc6455).

[![Build Status](https://travis-ci.org/lightning/engine.svg?branch=master)](https://travis-ci.org/lightning/engine)



Build
-----

**Note** There are helper scripts for installing some of these dependencies from source in the lightning/engine repo. These scripts are named `install-*.sh`.

Prerequisites:

* [jack](http://jackaudio.org)
* [libsndfile >= 1.0.25](http://www.mega-nerd.com/libsndfile/)
* [libsamplerate >= 0.1.8](http://www.mega-nerd.com/SRC/).
* [go >= 1.3.1](golang.org)

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
