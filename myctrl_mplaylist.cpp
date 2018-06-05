#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdio.h>
//#include <irrKlang.h>
#include <iostream>   // one of two
#include <mysql.h>
#include "myctrl_mplaylist.h"

//using namespace irrklang;


int mplaylist::m_play_playlist(char *path,int recnr) {
    if (recnr<=MAX_IN_PLAYLIST-1) {
        strcpy(path,aktiv_playlist[recnr].filename);			// return file name to mp3file
        return(1);
    } else return(0);
}

int mplaylist::m_add_playlist(char *file,char *songid,char *artistid,char *albumname,char *songname,char *artistname,char *songlength,int position,GLuint texture) {
    if (playlist_length<MAX_IN_PLAYLIST-1) {
        strcpy(aktiv_playlist[playlist_length].filename,file);
        strcpy(aktiv_playlist[playlist_length].artistname,artistname);
        strcpy(aktiv_playlist[playlist_length].songname,songname);
        aktiv_playlist[playlist_length].length=atoi(songlength);
        aktiv_playlist[playlist_length].song_id=atoi(songid);
        aktiv_playlist[playlist_length].artist_id=atoi(artistid);
        strncpy(aktiv_playlist[playlist_length].albumname,albumname,99);
        aktiv_playlist[playlist_length].albumname[99]=0;
        aktiv_playlist[playlist_length].textureId=texture;
        aktiv_playlist[playlist_length].selected=true;
        playlist_length++;
        return(playlist_length);
    } else return(0);
}

int mplaylist::numbers_in_playlist() {
    return(playlist_length);
}


bool mplaylist::get_selected(int recnr) {
    if (recnr<playlist_length) {
        if (aktiv_playlist[recnr].selected) return(true); else return(false);
    } else return(false);
}

bool mplaylist::set_selected(int recnr) {
    if (recnr<playlist_length) {
        aktiv_playlist[recnr].selected=!(aktiv_playlist[recnr].selected);
        return(true);
    } else return(false);
}

unsigned int mplaylist::get_songid(int recnr) {
    if (recnr<playlist_length) return(aktiv_playlist[recnr].song_id); else return(0);
}

GLuint mplaylist::get_textureid(int nr) {
    if (nr<playlist_length) return(aktiv_playlist[nr].textureId); else return(0);
}


unsigned int mplaylist::get_artistid(int recnr) {
    if (recnr<playlist_length)
        return(aktiv_playlist[recnr].artist_id);
    else return(0);
}

int mplaylist::get_artistname(char *resl,int recnr) {
    if (recnr<playlist_length) {
        strcpy(resl,aktiv_playlist[recnr].artistname);
        return(1);
    } else return(0);
}


int mplaylist::get_albumname(char *resl,int recnr) {
    if (recnr<playlist_length) {
        strcpy(resl,aktiv_playlist[recnr].albumname);
        return(1);
    } else return(0);
}

unsigned int mplaylist::get_length(int recnr) {
    if (recnr<playlist_length) {
        return(aktiv_playlist[recnr].length);
    } else return(0);
}


int mplaylist::get_songname(char *songname,int recnr) {
    strcpy(songname,aktiv_playlist[recnr].songname);
    if (recnr<playlist_length) return(1); else return(0);
}

void mplaylist::clean_playlist() {
    int n=0;
    while (n<MAX_IN_PLAYLIST-1) {
        strcpy(aktiv_playlist[n].filename,"");
        strcpy(aktiv_playlist[n].songname,"");
        aktiv_playlist[n].length=0;
        aktiv_playlist[n].textureId=0;
        aktiv_playlist[n].song_id=0;
        aktiv_playlist[n].artist_id=0;
        strcpy(aktiv_playlist[n].albumname,"");
        aktiv_playlist[n].selected=true;		// alle sange er default selected
        n++;
    }
    playlist_length=0;
}

// init empty playlist

mplaylist::mplaylist() {
    clean_playlist();
    playlist_length=0;
}


mplaylist::~mplaylist() {

}
