#!/bin/sh
git clone git://git.libwebsockets.org/libwebsockets
cd libwebsockets
mkdir build
cd build
cmake
make
sudo make install

