C = gcc
# CFLAGS for 32bits -m32 / 64 bits -m64
# -Wall
CFLAGS = -Wformat-truncation -pthread -m32 -Wformat-overflow -std=c++11

PROG       = mythtv-controller
EXECUTABLE = mythtv-controller
CONFIG_FILE= mythtv-controller.conf
DESTDIR    = /usr/share/mythtv-controller
DESTDIRBIN = /usr/local/bin
DESTIMG    = /usr/share/mythtv-controller/images
DESTLIBDIR = /usr/local/lib
DESTHDRDIR = /usr/local/include/fmodex
ETCDIR     = /etc
FMODFILE   = fmodstudioapi11011linux.tar.gz
BINPROG    = /usr/bin/mythtv-controller
FREETYPELIB= /usr/lib/x86_64-linux-gnu/libfreetype.so
LBITS := $(shell getconf LONG_BIT)

BUILD_NUMBER_FILE=build-number.txt

# check 64 bits ver
# download from http://www.fmod.org/download/#StudioAPIDownloads

LIRCSOURCES := $(shell find /usr/lib/ -name 'liblirc_client.so')

LIBICAL:=$(shell find /usr/lib/ -name 'libical.so')

ifeq ($(LBITS),64)
	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi11011linux/api/lowlevel/lib/x86_64/libfmod.so

#	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi10605linux/api/lowlevel/lib/x86_64/libfmod.so

	CFLAGS = -pthread -m64
	FREETYPELIB = /usr/lib/x86_64-linux-gnu/libfreetype.so
else
	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi11011linux/api/lowlevel/lib/x86/libfmod.so
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

SRCS = main.cpp myctrl_readwebfile.cpp myctrl_stream.cpp myctrl_music.cpp myctrl_mplaylist.cpp myctrl_radio.cpp myth_setupsql.cpp  myctrl_recorded.cpp myctrl_movie.cpp myctrl_tvprg.cpp myth_setup.cpp utility.cpp readjpg.cpp loadpng.cpp myth_saver.cpp myth_picture.cpp myth_ttffont.cpp checknet.cpp dds_loader.cpp myctrl_xbmc.cpp myth_vlcplayer.cpp myctrl_spotify.cpp mongoose-master/mongoose.c

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lX11 -lglut -lGLU -lm -lIL -lSDL `sdl-config --libs` -lSDL_image -lpthread -lxml2
endif

all:
	@echo "mythtv-controller ver 0.37.4 \nPossible targets:"
	@echo "'sudo sh apt-get.sh'       - Install software required by mythtv-controller"
	@echo "'sudo make installsound'   - Install FMOD/irrklang sound system"
	@echo "'make compile'             - Compile mythtv-controller"
	@echo "'sudo make install'        - Install mythtv-controller in /usr/share/mythtv-controller"
	@echo "'sudo make uninstall'      - Uninstall mythtv-controller FMOD/irrKlang libraries and headers"


compile: $(PROG)
	@if ! test -d ~/.config/lirc/; then \
	mkdir  ~/.config/lirc/; \
		cp lirc/* ~/.config/lirc/; \
	fi
	@if test -e ~/.xmltv; then echo "xmltv config exist. No update"; else cp xmltv_config/* ~/.xmltv/; fi

$(PROG): $(SRCS)
	$(CC) $(CFLAGS) $(BUILD_NUMBER_LDFLAGS) -ggdb -o $(PROG) $(SRCS) $(OPTS) $(LIBS)

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
	tar -zxvf $(FMODFILE) -C /usr/share/mythtv-controller/
	cp xmltv_config/*  ~/.xmltv/
	chmod 666 ~/.xmltv/*
	#@ln -s /usr/share/mythtv-controller/fmodstudioapi11011linux/api/lowlevel/lib/x86_64/libfmod.so.10.8 /usr/lib/libfmod.so.10
	#@ln -s /usr/share/mythtv-controller/fmodstudioapi11011linux/api/lowlevel/lib/x86_64/libfmod.so.10.8 /usr/lib/libfmod.so
	@ln -s /usr/share/mythtv-controller/fmodstudioapi11011linux/api/lowlevel/lib/x86_64/libfmod.so.10.11 /usr/lib/libfmod.so.10
	@echo "Done installing fmod32/64 version 4.44.41"
	@echo "Sound system installed."


install:
	@echo "Installing mythtv-controller ver 0.37.x in /usr/share/mythtv-controller."
	@mkdir -p /usr/share/mythtv-controller/images/radiostations
	@mkdir -p /usr/share/mythtv-controller/convert/hires
	@cp charset $(DESTDIR)
	@if test -e /etc/mythtv-controller.conf; then echo "mythtv-controller config exist. No update"; else cp $(CONFIG_FILE) ${ETCDIR}; fi
	@chmod 777 /etc/mythtv-controller.conf
	@mkdir -p /usr/share/mythtv-controller/images/mythnetvision
	@chmod 777 /usr/share/mythtv-controller/images/mythnetvision
	@cp -r -p images tema1 tema2 tema3 tema4 tema5 tema6 tema7 tema8 tema9 tema10 $(DESTDIR)
	@cp -r xmltv_config $(DESTDIR)	
	@cp mythtv-controller $(DESTDIRBIN)
	@cp mythtv-controller.png  /usr/share/mythtv-controller/mythtv-controller.png
	@cp mythtv-controller.desktop /usr/share/applications/
	@cp mythtv-controller.desktop  ~/.local/share/applications
	@cp mythtv-controller.desktop ~/Desktop
	@chmod 777 /usr/share/mythtv-controller/tema1 /usr/share/mythtv-controller/tema2 /usr/share/mythtv-controller/tema3 /usr/share/mythtv-controller/tema4 /usr/share/mythtv-controller/tema5 /usr/share/mythtv-controller/tema6 /usr/share/mythtv-controller/tema7 /usr/share/mythtv-controller/tema8 /usr/share/mythtv-controller/tema9 /usr/share/mythtv-controller/tema10
	@if ! test -e ~/.lirc; then \
	  mkdir -p ~/.lirc/; \
	  mkdir ~/.lircrc; \
	  cp lirc/mythtv-controller* ~/.lirc/; fi


include buildnumber.mak
