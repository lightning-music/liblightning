#!/bin/bash

. pkg_install.sh

function main {
    local check=http://downloads.sourceforge.net/project/check/check/0.9.14/check-0.9.14.tar.gz
    pkg_install $check
}

main "$@"
