#!/bin/sh
#
# checkwakeup.sh by Hans-Henrik Grüner-pedersen
#
# checker om mythtv-controller lock is found
# if found no close down.
# 
#
#if ps ax | grep /usr/sbin/mythtv-controller | grep -v grep

if [ -f /usr/share/mythtv-controller/mythtv-controller.lock ]
then
	echo "mythtv-controller.lock no close down"
	exit 1
else	
	if ps ax | grep mythtv-controller | grep -v grep
	then
		echo "mythtv-controller prg found no closedown"
		exit 1
	else
		exit 0
	fi
fi
