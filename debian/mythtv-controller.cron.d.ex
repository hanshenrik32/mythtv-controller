#
# Regular cron jobs for the mythtv-controller package
#
0 4	* * *	root	[ -x /usr/bin/mythtv-controller_maintenance ] && /usr/bin/mythtv-controller_maintenance
