#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>
#include "myth_vlcplayer.h"

// vlc player class stuf
// constructor

const char * const vlc_args[] = {
  "--verbose", "1",
  "--no-xlib",
  "--play-and-stop",
  "no-video-title-show"
};


vlc_controller::vlc_controller() {
  //vlc_inst = libvlc_new(5,opt);
  vlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
  is_playing=false;
}


// destructor

vlc_controller::~vlc_controller() {
  if (vlc_inst) libvlc_release(vlc_inst);
}


// do play function by use (libvlc)

int vlc_controller::playmovie(char *path) {
  int error=0;
  libvlc_media_t *vlc_m;
  vlc_m=libvlc_media_new_path(vlc_inst,path);
  if (vlc_m) {
    // set playing flag in class
    is_playing=true;
    // Create a media player playing environement
    vlc_mp=libvlc_media_player_new_from_media(vlc_m);
    libvlc_media_add_option(vlc_m,"no-video-title-show");
    // set fullscreen
    libvlc_set_fullscreen(vlc_mp,true);
    // enable ketboard input to vlc player
    libvlc_video_set_key_input(vlc_mp,true);
    libvlc_video_set_mouse_input(vlc_mp,true);
    //libvlc_media_add_option(vlc_m,":fullscreen");
    //libvlc_media_add_option(vlc_m,":sout-all");
    // <gdk/gdkx.h>
    // Bind to xwindows

    //libvlc_media_player_set_xwindow(vlc_mp, 0);

    //libvlc_media_player_set_xwindow(mp, GDK_WINDOW_XID(gtk_widget_get_window(b_window)));
    if (!(vlc_mp)) error=1;
    libvlc_media_release(vlc_m);
    #if 0
         /* This is a non working code that show how to hooks into a window,
          * if we have a window around */
          libvlc_media_player_set_xwindow (mp, xid);
         /* or on windows */
          libvlc_media_player_set_hwnd (mp, hwnd);
         /* or on mac os */
          libvlc_media_player_set_nsobject (mp, view);
    #endif
    // start play
    libvlc_media_player_play(vlc_mp);
    return(1);
  } else return(0);
}


void vlc_controller::stopmovie() {
  if (vlc_mp) {
    libvlc_media_player_stop(vlc_mp);
    libvlc_media_player_release(vlc_mp);
  } else {
    printf("Error stop movie player\n");
    exit(1);
  }
}


// return pos

float vlc_controller::get_position() {
  return(libvlc_media_player_get_position(vlc_mp));
}

// set play on pause

void vlc_controller::pause() {
  if (vlc_mp) libvlc_media_player_pause(vlc_mp);
}
