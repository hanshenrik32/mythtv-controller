CC = gcc
# CFLAGS for 32bits -m32 / 64 bits -m64
CFLAGS = -Wall -pthread -m32
#LIBDIR   = fmodapi44006linux/api/lib
#HDRDIR   = fmodapi44006linux/api/inc
PROG     = mythtv-controller
EXECUTABLE = mythtv-controller
CONFIG_FILE= mythtv-controller.conf
DESTDIR    = /usr/share/mythtv-controller
DESTIMG    = /usr/share/mythtv-controller/images
DESTLIBDIR = /usr/local/lib
DESTHDRDIR = /usr/local/include/fmodex
ETCDIR     = /etc
BINPROG    = /usr/bin/mythtv-controller
FREETYPELIB= /usr/lib/x86_64-linux-gnu/libfreetype.so
LBITS := $(shell getconf LONG_BIT)
# check 64 bits ver
# download from http://www.fmod.org/download/#StudioAPIDownloads

LIRCSOURCES := $(shell find /usr/lib/ -name 'liblirc_client.so')

LIBICAL:=$(shell find /usr/lib/ -name 'libical.so')

ifeq ($(LBITS),64)
	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86_64/libfmod.so 

#	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi10605linux/api/lowlevel/lib/x86_64/libfmod.so

	CFLAGS = -pthread -m64
	FREETYPELIB = /usr/lib/x86_64-linux-gnu/libfreetype.so
else
	LIBFMOD    = /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86/libfmod.so
        CFLAGS = -pthread -m32
	FREETYPELIB = /usr/lib/i386-linux-gnu/libfreetype.so
endif


ifeq ($(LBITS),64)
      STDCLIB = /usr/lib/x86_64-linux-gnu/libstdc++.so.6
      GLLIB = /usr/lib/x86_64-linux-gnu/mesa/libGL.so
else
	STDCLIB = /usr/lib/i386-linux-gnu/libstdc++.so.6
	GLLIB = /usr/lib/i386-linux-gnu/mesa/libGL.so
endif



OPTS =  -I"/usr/include/libical"  -I"/usr/local/include/fmodex/" -I"/usr/include/lirc" -I"/usr/local/include" -I"/usr/include/SDL/" -I"/usr/local/lib/" -I"/usr/lib" -I"/usr/include/mysql" -I/usr/include/GL/ -L/usr/X11R6/lib  -L"/usr/lib" -L"/usr/lib/mysql" -L"/usr/lib/vlc" -lmysqlclient $(LIRCSOURCES) $(LIBICAL) $(LIBFMOD) $(STDCLIB) $(GLLIB) /usr/lib/libGLC.so -lsqlite3 -lvlc -lfontconfig $(FREETYPELIB) -lXrandr

SRCS = main.cpp myctrl_readwebfile.cpp myctrl_stream.cpp myctrl_music.cpp myctrl_mplaylist.cpp myctrl_radio.cpp myth_setupsql.cpp  myctrl_recorded.cpp myctrl_movie.cpp myctrl_tvprg.cpp myth_setup.cpp utility.cpp readjpg.cpp loadpng.cpp myth_saver.cpp myth_picture.cpp myth_ttffont.cpp checknet.cpp dds_loader.cpp myctrl_xbmc.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lX11 -lglut -lGLU -lm -lIL -lSDL `sdl-config --libs` -lSDL_image -lcurl -lSDL_mixer -lpthread
endif

all:
	@echo "mythtv-controller ver 0.32.1 \nPossible targets:"
	@echo "'sudo sh apt-get.sh'       - Install software required by mythtv-controller"
	@echo "'sudo make installsound'   - Install FMOD/irrklang sound system"
	@echo "'make compile'             - Compile mythtv-controller"
	@echo "'sudo make install'        - Install mythtv-controller in /usr/share/mythtv-controller"
	@echo "'sudo make uninstall'      - Uninstall mythtv-controller FMOD/irrKlang libraries and headers"


compile: $(PROG)

$(PROG): $(SRCS)
	$(CC) $(CFLAGS) -ggdb -o $(PROG) $(SRCS) $(OPTS) $(LIBS)

#$(CC) $(CFLAGS) -ggdb -o $(PROG) $(SRCS) $(OPTS) $(LIBS)

uninstall:
	rm -f $(DESTDIR)
	rm -f $(BINPROG)


installsound:
	@echo "Install fmod sound system ver 4.44.41"
	mkdir -p $(DESTDIR)
#	cp fmodstudioapi10605linux.tar.gz $(DESTDIR)
	cp fmodstudioapi10811linux.tar.gz $(DESTDIR)	
	cd $(DESTDIR)
	touch /etc/mythtv-controller.conf
	chmod 777 /etc/mythtv-controller.conf
	tar -zxf fmodstudioapi10811linux.tar.gz -C /usr/share/mythtv-controller/
	ln -s /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86_64/libfmod.so.8.11 /usr/lib/libfmod.so.8.11
	ln -s /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86_64/libfmodL.so.8.11 /usr/lib/libfmodL.so.8.11
	ln -s /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86_64/libfmod.so.8.11 /usr/lib/libfmod.so
	ln -s /usr/share/mythtv-controller/fmodstudioapi10811linux/api/lowlevel/lib/x86_64/libfmod.so.8.11 /usr/lib/libfmod.so.8
	@echo "Done installing fmod32/64 version 4.44.41"
	@echo "Sound system installed."


install:
	echo "Installing mythtv-controller ver 0.32.x in /usr/share/mythtv-controller."
	mkdir -p /usr/share/mythtv-controller/images/radiostations
	mkdir -p /usr/share/mythtv-controller/convert/hires
	cp charset $(DESTDIR)
	@if test -e /etc/mythtv-controller.conf; then echo "config exist. No update"; else cp $(CONFIG_FILE) ${ETCDIR}; fi
	@chmod 777 /etc/mythtv-controller.conf
	@mkdir -p /usr/share/mythtv-controller/images/mythnetvision
	@chmod 777 /usr/share/mythtv-controller/images/mythnetvision
	cp $(PROG) checkwakeup.sh startmovie.sh /usr/bin/
	@chmod 755 /usr/bin/startmovie.sh
	cp -r -p images tema1 tema2 tema3 tema4 tema5 tema6 tema7 tema8 tema9 tema10 $(DESTDIR)
	@chmod 777 /usr/share/mythtv-controller/tema1 /usr/share/mythtv-controller/tema2 /usr/share/mythtv-controller/tema3 /usr/share/mythtv-controller/tema4 /usr/share/mythtv-controller/tema5 /usr/share/mythtv-controller/tema6 /usr/share/mythtv-controller/tema7 /usr/share/mythtv-controller/tema8 /usr/share/mythtv-controller/tema9 /usr/share/mythtv-controller/tema10
	@if ! test -e ~/.lirc; then \
	  mkdir -p ~/.lirc/; \
	  mkdir ~/.lircrc; \
	  cp lirc/mythtv-controller ~/.lirc/; fi
