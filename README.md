liblightning
===========

liblightning is an audio sample engine

[![Build Status](https://travis-ci.org/lightning/liblightning.svg?branch=master)](https://travis-ci.org/lightning/liblightning)



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



### Discussion & Issues

lightning-music@googlegroups.com
