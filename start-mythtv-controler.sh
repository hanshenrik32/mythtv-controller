#!/bin/sh
#Lock cursor in main display
##/usr/local/bin/Jail > /dev/null 2>&1 &
# Kill the kicker
#kill -9 `ps -ef | grep kicker | grep -v grep | head -1 | awk '{print $2}'`

# exit 2 = exit controller else error (start igen)

export DISPLAY=:0.0
mythtv_cer=0
#while [ "$mythtv_cer" -ne "2" ] 
#do
while [ "$mythtv_cer" -ne "2" ] 
do
if ps ax | grep -v grep | grep ~/mythtv-controller/mythtv-controller > /dev/null
then 
 echo "mythtv 3d controller is running " > mythtv-controller.log
else
 cd /home/hans/mythtv-controller
 if  ps ax | grep -v grep | grep /usr/bin/mythfrontend > /dev/null
  then
   echo "mythfrontend is running kill it"
   pkill -f /usr/bin/mythfrontend.real
 else
   export DISPLAY=:0.0
   /usr/bin/xrandr --output default --mode 13600x768
   mythtv-controller
   mythtv_cer=$?
   /usr/bin/xrandr --output default --mode 1920x1080
 fi
 echo "Exit=" $mythtv_cer
fi
done
