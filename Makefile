C = gcc
# CFLAGS for 32bits -m32 / 64 bits -m64
# -Wall
CFLAGS = -Wformat-truncation -pthread -m64 -o0 -ggdb -std=c++17 -Wno-format-overflow Wformat-truncation -Wformat-truncation=2
LDFLAGS= 

PROG       = mythtv-controller
EXECUTABLE = mythtv-controller
CONFIG_FILE= mythtv-controller.conf
DESTDIR    = /opt/mythtv-controller
DESTDIRBIN = /usr/bin
DESTIMG    = /opt/mythtv-controller/images
DESTLIBDIR = /usr/local/lib
DESTHDRDIR = /usr/local/include/fmodex
ETCDIR     = /etc
FMODFILE   = fmodstudioapi20218linux.tar.gz
BINPROG    = /usr/bin/mythtv-controller
FREETYPELIB= /usr/lib/x86_64-linux-gnu/libfreetype.so
LBITS := $(shell getconf LONG_BIT)

BUILD_NUMBER_FILE=build-number.txt

# check 64 bits ver
# download from http://www.fmod.org/download/#StudioAPIDownloads

LIRCSOURCES := $(shell find /usr/lib/ -name 'liblirc_client.so')

LIBICAL:=$(shell find /usr/lib/ -name 'libical.so')

ifeq ($(LBITS),64)
	LIBFMOD    = $(shell find /opt/mythtv-controller/fmodstudioapi20218linux/api/core/lib/x86_64/ -name 'libfmod.so')
	CFLAGS = -pthread -m64
	FREETYPELIB = /usr/lib/x86_64-linux-gnu/libfreetype.so
else
	LIBFMOD    = $(shell find /opt/mythtv-controller/fmodstudioapi20218linux/api/core/lib/x86/ -name 'libfmod.so')
        CFLAGS = -pthread -m32
	FREETYPELIB = /usr/lib/i386-linux-gnu/libfreetype.so
endif


ifeq ($(LBITS),64)
	STDCLIB = /usr/lib/x86_64-linux-gnu/libstdc++.so.6
	LIBGL:=$(shell find /usr/lib/ -name 'libGL.so')
	LIBGLC:=$(shell find /usr/lib/ -name 'libGLC.so')
else
	STDCLIB = /usr/lib/i386-linux-gnu/libstdc++.so.6
	LIBGL:=$(shell find /usr/lib/ -name 'libGL.so')
	LIBGLC:=$(shell find /usr/lib/ -name 'libGLC.so')
endif


OPTS = -I "/usr/include/GL" -I"/usr/include/libical"  -I"/usr/local/include/fmodex/" -I"/usr/include/lirc" -I"/usr/local/include" -I"/usr/include/SDL/" -I"/usr/local/lib/" -I"/usr/lib" -I"/usr/include/mysql" -I/usr/include/GL/ -L/usr/X11R6/lib  -L"/usr/lib" -L"/usr/lib/mysql" -L"/usr/lib/vlc" -lmysqlclient $(LIRCSOURCES) $(LIBICAL) $(LIBFMOD) $(STDCLIB) $(GLLIB) $(LIBGL) -lsqlite3 -lvlc -lfontconfig $(FREETYPELIB) $(LIBGLC) -lXrandr -I/usr/include/libxml2

SRCS = main.cpp myctrl_readwebfile.cpp myctrl_stream.cpp myctrl_music.cpp myctrl_mplaylist.cpp myctrl_radio.cpp myth_setupsql.cpp  myctrl_recorded.cpp myctrl_movie.cpp myctrl_tvprg.cpp myth_setup.cpp utility.cpp readjpg.cpp loadpng.cpp myth_saver.cpp myth_picture.cpp myth_ttffont.cpp checknet.cpp dds_loader.cpp myctrl_xbmc.cpp myth_vlcplayer.cpp myctrl_spotify.cpp myctrl_tidal2.cpp  mongoose-master/mongoose.c json-parser/json.c

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lX11 -lglut -lGLU -lm -lIL -lSDL `sdl-config --libs` -lSDL_image -lpthread -lxml2 -lcurl
endif

all:
	@echo "mythtv-controller ver 0.38.1 \nPossible targets:"
	@echo "'sudo sh apt-get.sh'       - Install software required by mythtv-controller"
	@echo "'sudo make installsound'   - Install FMOD/irrklang sound system"
	@echo "'make compile'             - Compile mythtv-controller"
	@echo "'sudo make install'        - Install mythtv-controller in /usr/share/mythtv-controller"
	@echo "'sudo make uninstall'      - Uninstall mythtv-controller FMOD/irrKlang libraries and headers"


compile: $(PROG)
	@if ! test -f build-number.txt; then touch build-number.txt; fi
	tar -zxvf json-parser.tar.gz
	cd json-parser \
	./configure \
	make
	@if ! test -d ~/.config/lirc/; then \
	mkdir  ~/.config/lirc/; \
		cp lirc/* ~/.config/lirc/; \
		mkdir -p ~/.xmltv/; \
	fi
	#@if test -e ~/.xmltv; then echo "xmltv config exist. No update"; else cp xmltv_config/* ~/.xmltv/; fi
	@if test -e build-number.txt; then echo $$(($$(cat build-number.txt) + 1)) > build-number.txt; fi

$(PROG): $(SRCS) $(BUILD_NUMBER_FILE)
	$(CC) $(CFLAGS) -march=native -O0 -ggdb -o $(PROG) $(SRCS) $(OPTS) $(LIBS) $(LDFLAGS)

#$(CC) $(CFLAGS) -ggdb -o $(PROG) $(SRCS) $(OPTS) $(LIBS)


Debug: $(PROG)

mysqlfix:
	echo 'sql_mode = "STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION"' > /etc/mysql/mysql.conf.d/mysqld.cnf
	service mysql restart        



uninstall:
	rm -f $(DESTDIR)
	rm -f $(BINPROG)

clean:
	@if test -e mythtv-controller; then rm mythtv-controller; fi

installsound:
	@echo "Install fmod sound system ver 4.44.41"
	#@if test -e /etc/mythtv-controller.conf; then echo "mythtv-controller config exist. No update"; else cp $(CONFIG_FILE) ${ETCDIR}; fi
	mkdir -p $(DESTDIR)
	cp $(FMODFILE) $(DESTDIR)
	cd $(DESTDIR)
	touch /etc/mythtv-controller.conf
	chmod 777 /etc/mythtv-controller.conf
	tar -zxvf $(FMODFILE) -C /opt/mythtv-controller/
	#remove old link
	if test -e /usr/lib/libfmod.so.12; then rm /usr/lib/libfmod.so.12; fi
	ln -s /opt/mythtv-controller/fmodstudioapi20218linux/api/core/lib/x86_64/libfmod.so /usr/lib/libfmod.so.13
	@echo "Done installing fmod32/64 bit version 4.44.41"
	@echo "Sound system installed."


install:
	@echo "Installing mythtv-controller ver 0.38.x in /opt/mythtv-controller."
	@mkdir -p /opt/mythtv-controller/images/radiostations	
	@mkdir -p /opt/mythtv-controller/convert/hires
	@mkdir -p /opt/mythtv-controller/images/mythnetvision	
	@if test -e /etc/mythtv-controller.conf; then echo "mythtv-controller config exist. No update"; else cp $(CONFIG_FILE) ${ETCDIR}; fi
	@chmod 777 /etc/mythtv-controller.conf
	@cp -r -p images tema1 tema2 tema3 tema4 tema5 tema6 tema7 tema8 tema9 tema10 $(DESTDIR)
	#@cp -r xmltv_config $(DESTDIR)	
	@cp mythtv-controller $(DESTDIRBIN)
	@cp mythtv-controller.png  /opt/mythtv-controller/mythtv-controller.png
	@cp mythtv-controller.desktop /usr/share/applications/
	if test -e ~/.local/share/applications; then cp mythtv-controller.desktop  ~/.local/share/applications; fi
	@cp mythtv-controller.desktop ~/Desktop
	#@cp xmltv_config/*  ~/.xmltv/
	#@chmod 666 ~/.xmltv/*
	@chmod 777 /opt/mythtv-controller/tema*
	@if ! test -e ~/.lirc; then \
	  mkdir -p ~/.lirc/; \
	  mkdir ~/.lircrc; \
	  cp lirc/mythtv-controller* ~/.lirc/; fi
	# create random password
	PASSWDDB="$(openssl rand -base64 12)"
	# replace "-" with "_" for database username
	MAINDB="mythtvcontroller"
	# If /root/.my.cnf exists then it won't ask for root password
	#if [ -f /root/.my.cnf ]; then
	#    mysql -e "CREATE DATABASE ${MAINDB} /*\!40100 DEFAULT CHARACTER SET utf8 */;"
	#    mysql -e "CREATE USER ${MAINDB}@localhost IDENTIFIED BY '${PASSWDDB}';"
	#    mysql -e "GRANT ALL PRIVILEGES ON ${MAINDB}.* TO '${MAINDB}'@'localhost';"
	#    mysql -e "FLUSH PRIVILEGES;"
	# If /root/.my.cnf doesn't exist then it'll ask for root password   
	#else
	#    echo "Please enter root user MySQL password!"
	#    echo "Note: password will be hidden when typing"
	#    read -sp rootpasswd
	#    mysql -uroot -p${rootpasswd} -e "CREATE DATABASE ${MAINDB} /*\!40100 DEFAULT CHARACTER SET utf8 */;"
	#    mysql -uroot -p${rootpasswd} -e "CREATE USER ${MAINDB}@localhost IDENTIFIED BY '${PASSWDDB}';"
	#    mysql -uroot -p${rootpasswd} -e "GRANT ALL PRIVILEGES ON ${MAINDB}.* TO '${MAINDB}'@'localhost';"
	#    mysql -uroot -p${rootpasswd} -e "FLUSH PRIVILEGES;"
	#fi

