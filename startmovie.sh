#!/bin/sh

CMD="/usr/bin/mplayer -fs "

echo $CMD "$@"
exec $CMD "$@" vlc://quit
