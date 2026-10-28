You need imageconvert/imagemagic to enable the icon update.

tv module to use in the future

https://tvheadend.org/projects/tvheadend/wiki/Faq

if you wants to make the deb file you have to install debhelper.

Run on most debian based systems, like Ubuntu.

# First clone mythtv.controller

git clone https://github.com/hanshenrik32/mythtv-controller.git

build by cmake.

# First install needed packed.

sudo apt-get install g++ make curl libcurl-ocaml-dev libdevil-dev libsdl1.2-dev freeglut3-dev libsdl-image1.2-dev dialog imagemagick xmltv libglc-dev libical-dev unzip libical-dev unzip libical-dev lirc libsqlite3-dev libvlc-dev libxrandr-dev libxml2-dev libhttp-ocaml-dev libmediainfo-dev libfreetype-dev libfreetype6 libfreetype6-dev libfmt-dev  libglfw3 libglfw3-dev libjsoncpp-dev libtorrent-rasterbar-dev libtorrent-dev libfmt-dev libmediainfo-dev liblirc-dev ffmpeg libgtk-3-dev libglew-dev libmariadb-dev

# Then cd to source files.

cd mythtv-controller

# Start install mythtv-controller.

cmake -S . -B build -DFMOD_ROOT="$PWD/fmodstudioapi20311linux"

### If you get a error about
### CMake Error in CMakeLists.txt:
### ...
### "/usr/include/sysprof-6"
### ...
### Then reinstall packed sysprof by.
### sudo apt update
### sudo apt install --reinstall libsysprof-capture-4-dev

# Then install fmod api

sudo cmake --build build --target installsound

# After fmod api install build app.

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"

# Install mythtv-controller in /opt/mythtv-controller
sudo cmake --install build

### Rember to install mariadb and change settings in /etc/mythtv-controller.conf

### CREATE USER 'mythtv'@'localhost' IDENTIFIED BY 'password';
### FLUSH PRIVILEGES;
### create database mythtvcontroller;

# Version history.

mythtv-controller ver 0.55.1 Tema update and more access to settings in tema json config files.

mythtv-controller ver 0.54.1 you can see tvguide overlaw on movies playing by pressing t - for tv guide. All opengl calls is 500% faster now. high framerate > 120

mythtv-controller ver 0.50.0 - New plasma screen saver. Other errors is fixed to.

mythtv-controller ver 0.46.1 - new scroll functions.

mythtv-controller ver 0.45.1 - movie struct now use vector

mythtv-controller ver 0.44.4 

mythtv-controller ver 0.43.4

mythtv-controller ver 0.42.6

mythtv-controller ver 0.41.0

Tidal working. need some code fix to make it stable.

tidal-dl installed in /usr/local/bin NOT IN USE ANY MORE.

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

Se install.txt for arch linux install.

Programmed by Hans-Henrik Grüner-Pedersen release under GPL hanshenrik32@gmail.com
