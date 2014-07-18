#!/bin/bash

SYSTEMS='ubuntu debian'

function usage_and_exit {
    echo 1>&2 'Usage:'
    echo 1>&2 "$0" '<system type>'
    echo 1>&2
    echo 'Valid systems: ' 1>&2
    for s in $SYSTEMS; do
        echo "   $s" 1>&2
    done
    echo 1>&2
    exit 1
}

if [ -z "$1" ]; then
    usage_and_exit
fi

case "$1" in
    ubuntu|debian)
        sudo apt-get update
        sudo apt-get install -qq libsndfile1-dev \
            libjack-dev libgtk2.0-dev libsamplerate0-dev
        cd /vagrant && make && make install
        ;;
    *)
        echo 1>&2 Unrecognized system: "$1"
        echo 1>&2
        usage_and_exit
        ;;
esac
