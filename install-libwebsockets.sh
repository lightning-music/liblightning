#!/bin/sh
wget http://git.libwebsockets.org/cgi-bin/cgit/libwebsockets/snapshot/libwebsockets-1.3-chrome37-firefox30.tar.gz
tar xzf libwebsockets-1.3-chrome37-firefox30.tar.gz
cd libwebsockets-1.3-chrome37-firefox30
mkdir build
cd build
cmake ..
make
sudo make install
