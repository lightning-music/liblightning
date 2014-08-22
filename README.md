liblightning
===========

liblightning is an audio sample engine

[![Build Status](https://travis-ci.org/lightning/liblightning.svg?branch=master)](https://travis-ci.org/lightning/liblightning)



Build
-----

**Note** There are helper scripts for installing some of these dependencies from source in the lightning/engine repo. These scripts are named `install-*.sh`.

Prerequisites:

* [jack](http://jackaudio.org)
* [libsndfile >= 1.0.25](http://www.mega-nerd.com/libsndfile/)
* [libsamplerate >= 0.1.8](http://www.mega-nerd.com/SRC/).
* [go >= 1.3.1](http://golang.org)

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
