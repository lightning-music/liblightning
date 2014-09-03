#!/bin/bash
#
# Quick and dirty way to install libopus and opus-tools
#
function pkg_install {
    local pkg="$1"; shift
    local tarball=$(basename $pkg)
    local dir=${tarball/%.tar.gz/}
    wget $pkg
    tar xzf $tarball
    cd $dir
    ./configure && make && sudo make install
    cd ..
    rm -rf $dir $tarball
}

function main {
    local opus=http://downloads.xiph.org/releases/opus/opus-1.1.tar.gz
    local opustools=https://ftp.mozilla.org/pub/mozilla.org/opus/opus-tools-0.1.9.tar.gz
    pkg_install $opus
    pkg_install $opustools
}

main "$@"
