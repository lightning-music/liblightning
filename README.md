lightning
===========

Make music on linux lightning fast!

[![Build Status](https://travis-ci.org/briansorahan/lightning.svg?branch=master)](https://travis-ci.org/briansorahan/lightning)

lightning is composed of two components:

- lightning-engine
- lightning-control

lightning-engine is responsible for playing audio samples.
It does this with [jack](1), [libsndfile](2), and [libsamplerate](3).
You can find documentation about the lightning-engine OSC interface [here](5).

lightning-control uses the OSC interface of lightning-engine and provides
users with the ability to:
- Organizing groups of samples into kits
- Sequencing samples with patterns

### Discussion & Issues

lightning-music@googlegroups.com

[1]: http://jackaudio.org
[2]: http://www.mega-nerd.com/libsndfile/
[3]: http://www.mega-nerd.com/SRC/
[4]: http://opensoundcontrol.org
[5]: https://github.com/briansorahan/lightning/wiki/lightning-engine-OSC-interface
