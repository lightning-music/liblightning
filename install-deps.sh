#!/bin/sh

SYSTEMS='ubuntu debian'

if [ -z "$1" ]; then
    echo 1>&2 'Usage:'
    echo 1>&2 "$0" system
    echo 1>&2
    echo 'Valid systems: ' 1>&2
    for s in $SYSTEMS; do
        echo "   $s" 1>&2
    done
    echo 1>&2
fi

case "$1" in
    ubuntu|debian)
        sudo apt-get update
        sudo apt-get install -qq libsndfile1-dev libjack-dev libgtk2.0-dev
        ;;
    *)
        echo 1>&2 Unrecognized system: "$1"
        echo 1>&2
        echo 'Valid systems: ' 1>&2
        for s in $SYSTEMS; do
            echo "   $s" 1>&2
        done
        echo 1>&2
        ;;
esac
