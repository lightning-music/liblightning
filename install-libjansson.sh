#!/bin/sh
wget http://www.digip.org/jansson/releases/jansson-2.6.tar.gz
tar xzf jansson-2.6.tar.gz
cd jansson-2.6
./configure
make
sudo make install
