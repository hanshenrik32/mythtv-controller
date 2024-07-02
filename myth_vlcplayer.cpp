//
// vlc loader/player controller
//
#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>
#include <pthread.h>
#include "myth_vlcplayer.h"

// vlc player class stuf
// constructor

const char * const vlc_args[] = {
  "--verbose", "0",
  "--no-xlib",
  "--play-and-stop",
//  "--quiet",
  "no-video-title-show",

};

// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

vlc_controller::vlc_controller() {
  //vlc_inst = libvlc_new(5,opt);
  vlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
  is_playing=false;
  is_pause=false;
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************
vlc_controller::~vlc_controller() {
  if (vlc_inst) libvlc_release(vlc_inst);
}

// ****************************************************************************************
//
// ****************************************************************************************

bool vlc_controller::vlc_in_playing() {
  return is_playing;
};


// ****************************************************************************************
//
// do play function by use (libvlc)
//
// ****************************************************************************************

int vlc_controller::playmedia(char *path) {
  int error=0;
  libvlc_media_t *vlc_m;
  vlc_m=libvlc_media_new_path(vlc_inst,path);
  if (vlc_m) {
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
    // set playing flag in class
    is_playing=true;
    return(1);
  } else return(0);
}


// ****************************************************************************************
//
// web player
//
// ****************************************************************************************

int vlc_controller::playwebmedia(char *path) {
  int error=0;
  libvlc_media_t *vlc_m;
  //vlc_m=libvlc_media_new_path(vlc_inst,path);
  vlc_m=libvlc_media_new_location(vlc_inst,path);
  if (vlc_m) {
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
    // set playing flag in class
    is_playing=true;
    return(1);
  } else return(0);
}


// ****************************************************************************************
//
// stop play media
//
// ****************************************************************************************

void vlc_controller::stopmedia() {
  if (vlc_mp) {
    if (libvlc_media_player_is_playing(vlc_mp)) {
      libvlc_media_player_stop(vlc_mp);
      libvlc_media_player_release(vlc_mp);
      is_playing=false;
    }
  } else {
    printf("Error stop movie player\n");
    exit(1);
  }
}


// ****************************************************************************************
//
// ****************************************************************************************

float vlc_controller::jump_position(float ofset) {
  int totallengths;
  float addval;
  float pos=get_position();
  totallengths=(get_length_in_ms()/1000/60);
  addval=(float) totallengths/100;
  //printf("(addval/100) is %f \n ",(addval/100));
  if (ofset>0) {
    if (pos+addval<1.0f) pos+=pos+(addval/100); else if (pos+(addval/100)*10>0.0f) pos-=pos+((addval/100)*10);
  }
  set_position(pos);
  return(pos);
}


// ****************************************************************************************
//
// ****************************************************************************************


unsigned long vlc_controller::get_length_in_ms() {
  libvlc_time_t length;
  length=libvlc_media_player_get_length(vlc_mp);
  return((unsigned long) length);
}

// ****************************************************************************************
//
// return play pos
//
// ****************************************************************************************

float vlc_controller::get_position() {
  return(libvlc_media_player_get_position(vlc_mp));
}


// ****************************************************************************************
//
// set position
//
// ****************************************************************************************

float vlc_controller::set_position(float pos) {
  libvlc_media_player_set_position(vlc_mp,pos);
  return(pos);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

void vlc_controller::pnext_chapter() {
  libvlc_media_player_next_chapter(vlc_mp);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

void vlc_controller::plast_chapter() {
  //libvlc_media_player_last_chapter(vlc_mp);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::volume_up(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::volume_down(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::setvolume(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
// set play or pause
//
// ****************************************************************************************



void vlc_controller::pause(int pause) {
  if (vlc_mp) {
    if (is_pause) {
      libvlc_media_player_set_pause(vlc_mp,0);
      is_pause=false;
    } else {
      libvlc_media_player_set_pause(vlc_mp,1);
      is_pause=true;
    }
  }
}
