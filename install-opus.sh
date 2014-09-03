#!/bin/bash
#
# Quick and dirty way to install libopus and opus-tools
#
. pkg_install.sh

function main {
    local opus=http://downloads.xiph.org/releases/opus/opus-1.1.tar.gz
    local opustools=https://ftp.mozilla.org/pub/mozilla.org/opus/opus-tools-0.1.9.tar.gz
    pkg_install $opus
    pkg_install $opustools
}

main "$@"
