cc -pthread  -march=native -O0 -ggdb -o mythtv-controller main.cpp myctrl_readwebfile.cpp myctrl_stream.cpp myctrl_music.cpp myctrl_mplaylist.cpp myctrl_radio.cpp myth_setupsql.cpp  myctrl_recorded.cpp myctrl_movie.cpp myctrl_tvprg.cpp myth_setup.cpp utility.cpp readjpg.cpp loadpng.cpp myth_saver.cpp myth_picture.cpp myth_ttffont.cpp checknet.cpp dds_loader.cpp myctrl_xbmc.cpp myth_vlcplayer.cpp myctrl_spotify.cpp myctrl_tidal2.cpp  mongoose-master/mongoose.c json-parser/json.c -I /usr/include/GL -I/usr/include/libical  -I/usr/local/include/fmodex/ -I/usr/include/lirc -I/usr/local/include -I/usr/include/SDL/ -I/usr/local/lib/ -I/usr/lib -I/usr/include/mysql -I/usr/include/GL/ -L/usr/X11R6/lib  -L/usr/lib -L/usr/lib/mysql -L/usr/lib/vlc -lmysqlclient  /usr/lib/aarch64-linux-gnu/libical.so /opt/mythtv-controller/fmodstudioapi20218linux/api/core/lib/arm64/libfmod.so /usr/lib/aarch64-linux-gnu/libstdc++.so.6  /usr/lib/aarch64-linux-gnu/libGL.so -lsqlite3 -lvlc -lfontconfig /usr/lib/aarch64-linux-gnu/libfreetype.so /usr/lib/aarch64-linux-gnu/libGLC.so -lXrandr -I/usr/include/libxml2 -lX11 -lglut -lGLU -lm -lIL -lSDL -lSDL -lSDL_image -lpthread -lxml2 -lcurl