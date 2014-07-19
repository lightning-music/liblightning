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

function provision {
    local system_type="$1"; shift

    if [ -z "$system_type" ]; then
        usage_and_exit
    fi

    case "$system_type" in
        ubuntu|debian)
            # assume make is installed
            sudo apt-get update
            sudo apt-get install -qq libsndfile1-dev \
                libjack-dev libgtk2.0-dev libsamplerate0-dev
            ;;
        *)
            echo 1>&2 Unrecognized system: "$system_type"
            echo 1>&2
            usage_and_exit
            ;;
    esac

    [ -d /vagrant ] && cd /vagrant && make && make install
}

provision "$@"
