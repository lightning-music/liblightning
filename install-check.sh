#!/bin/bash
wget http://downloads.sourceforge.net/project/check/check/0.9.14/check-0.9.14.tar.gz
cd check-0.9.14
./configure
make
sudo make install
