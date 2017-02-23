# mythtv-controller 
# packs to install to compile on Ubuntu 10.xx/12.xx
# Updated 01/11/2014
#

Version.

mythtv-controller ver 0.36.1

uv music meter fixed after fmod upgrade
All databases is now auto created by the system.

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

g++ make libdevil-dev libcurl-ocaml-dev libsdl1.2-dev glutg3-dev 
libmysqlclient-dev libsdl-image1.2-dev lirc-modules-source dialog 
imagemagick liblircclient-dev xmltv libglc-dev libical-dev unzip 
libical-dev unzip libical-dev lirc

Do command to install packets.

sudo sh apt-get_12.10.sh

Ver 10.04

g++ make libcurl-ocaml-dev libdevil-dev libsdl1.2-dev glutg3-dev
libmysqlclient-dev libsdl-image1.2-dev lirc-modules-source dialog
imagemagick liblircclient-dev xmltv libglc-dev libical-dev unzip libical-dev
unzip libical-dev lirc

Do command to install packets.

sudo sh apt-get.sh



You can install them by 'sudo sh apt-get.sh' when you have downloaded and
untar the install file.

mkdir mythtv-controller
cd mythtv-controller
tar -zxf mythtv-controller-0.33.6.tar.gz

Rember to check the password in the config file (/etc/mythtv-controller.conf) BEFORE run

Now it is time to install mythtv-controller by doing.

Possible targets:
'sudo sh apt-get.sh'       - Install software required by mythtv-controller
'sudo make installsound'   - Install FMOD sound system
'make compile'             - Compile mythtv-controller
'sudo make install'        - Install mythtv-controller in /usr/share/mythtv-controller

You can uninstall by.

'sudo make uninstall'      - Uninstall mythtv-controller FMOD/irrKlang/ libraries and headers


 First start is slow, if your mythtv is setup right. The program have to
 convert all the gfx/cdcover/dvdcover file to internal format.

 After start. First jump to F1 menu for setup. 
 Check the database setup for connect to mythtv or kodi backend. You can change the system type by spacekey.

 After setup is okay.. restart the program to load the data from the mythtv

 the radio stations db will be installed by the mythtv-controller by first run if able to create db (again check password in config file.)

# Se install.txt for arch linux install.
#
# Programmed by Hans-Henrik Grüner-Pedersen
# release under GPL
# hanshenrik32@gmail.com
#

