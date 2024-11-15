#!/bin/sh

CMD="/usr/bin/vlc --spdif --mouse-hide-timeout 500 --no-video-title-show -f --intf dummy --extraintf lirc"

echo $CMD "$@"
exec $CMD "$@" vlc://quit
