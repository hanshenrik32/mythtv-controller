You need imageconvert/imagemagic to enable the icon update.

tv module to use in the future

https://tvheadend.org/projects/tvheadend/wiki/Faq

if you wants to make the deb file you have to install debhelper

Version.

mythtv-controller ver 0.41.0

tidal working. need some code fix to make it stable.

mythtv-controller ver 0.40.7

New logfile mythtv-controller.log in homedir. Spotify loader fixed. Not able to select last icon in music overview fixed.

mythtv-controller ver 0.39.0

tty default log is now write to logfile mythtv-controller.log

mythtv-controller ver 0.38.4

install dir move to /opt/mythtv-controller

mythtv-controller ver 0.38.1

Spotify player added.

mythtv-controller ver 0.36.7

mythtv-controller ver 0.36.4

new rss feed default installed

mythtv-controller ver 0.36.2

mythtv-controller ver 0.36.1

uv music meter fixed after fmod upgrade All databases is now auto created by the system.

mythtv-controller ver 0.36.0

New xbmc/kodi loader,faster tread loader on all types

mythtv-controller ver 0.32.6

Now 32/64B version

mythtv-controller ver 0.31.6

Howto install.

The ease way is to download the deb version by dubble click on it.

The source version install is like this.

This packets need to be installed on Ubuntu.

Ver 12.xx

g++ make libdevil-dev libcurl-ocaml-dev libsdl1.2-dev glutg3-dev libmysqlclient-dev libsdl-image1.2-dev lirc-modules-source dialog imagemagick liblircclient-dev xmltv libglc-dev libical-dev unzip libical-dev unzip libical-dev lirc

Do command to install packets.

sudo sh apt-get_12.10.sh

Ver 10.04

g++ make libcurl-ocaml-dev libdevil-dev libsdl1.2-dev glutg3-dev libmysqlclient-dev libsdl-image1.2-dev lirc-modules-source dialog imagemagick liblircclient-dev xmltv libglc-dev libical-dev unzip libical-dev unzip libical-dev lirc

Do command to install packets.

sudo sh apt-get.sh

You can install them by 'sudo sh apt-get.sh' when you have downloaded and untar the install file.

mkdir mythtv-controller cd mythtv-controller tar -zxf mythtv-controller-0.33.6.tar.gz

Now you have to create a database, called mythtvcontroller use mysql to create the database. Rember to check the password in the config file. 'createdb.sql' textfelt 'PASSWORD HERE' BEFORE run.

mysql -u root -p < createdb.sql

Now it is time to install mythtv-controller by doing.

Possible targets: 'sudo sh apt-get.sh' - Install software required by mythtv-controller 'sudo make installsound' - Install FMOD sound system 'make compile' - Compile mythtv-controller 'sudo make install' - Install mythtv-controller in /usr/share/mythtv-controller

You can uninstall by.

'sudo make uninstall' - Uninstall mythtv-controller FMOD/irrKlang/ libraries and headers

screen saver types

analog - Analog clock digital - Digital clock 3D - 3d cover 3D2 - PICTURE3D - Pictures in Photos directory in homedir none - None screen saver MUSICMETER - Music meter

First start is slow, if your mythtv is setup right. The program have to convert some gfx/cdcover/dvdcover file to internal format.

After start. First jump to F1 menu for setup. Check the database setup for connect to mythtv backend. After setup is okay.. restart the program to load the data from the mythtv system the radio stations will be installed by the mythtv-controller program.

Se install.txt for arch linux install.

Programmed by Hans-Henrik Grüner-Pedersen release under GPL hanshenrik32@gmail.com
