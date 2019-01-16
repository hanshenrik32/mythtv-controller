#ifndef MYCTRL_MAIN
#define MYCTRL_MAIN

const char *SHOWVER={"0.36.7"};

// debug modes
// 1  = wifi net
// 2  = music
// 4  = stream
// 8  = keyboard/mouse move
// 16 = movie
// 32 = search
// 64 =
// 128= stream search
// 256 = tv program stuf
// 512 = media importer
// 1024 = flag loader

// Keys
//
// music overview 'S' Save playlist
//
//

//
// xmltv update interval in sec
// default 2 * 60 min
//
const int doxmltvupdateinterval=60*60*2;
const int dorssupdateinterval=60*60;

// screen saver options

// 3D         - Do not work right now
// 3D2        - Do not work right now
// digital    - Digital Watch
// analog     - Analog Watch
// PICTURE3D  - Do not work right now

//

// end,danish
//
// Sprog struktur.
//
// English, danish, france, tysk, Arabic

// time in language

const char *music_timename[]={"Time","Tid","time","time","time"};

// music stuf
const char *music_now_playing[]={"Now Playing.","Spiller nu.","Lecture en cours.","Now Playing.","قائمة الأغاني"};
const char *music_artist[]={"Artist","Artist","Artiste","kunstenaar","فنان"};
const char *music_album[]={"Album","Album","l'album","album","الألبوم"};
const char *music_songname[]={"Song name","Sang navn","nom de la chanson","nom de la chanson","اسم الأغنية"};
const char *music_play_time[]={"Play time","Afspilet tid","temps de jeu","speeltijd","لعب وقت"};
const char *music_samplerate[]={"Samplerate","Samplerate","taux d'échantillonnage","sample rate","معدل عينة"};
const char *music_playsong[]={"PLay song","Afspil sang","","",""};
const char *music_nextsong[]={"Next song","Næste sang","","",""};
const char *music_nowplaying[]={"Now Playing.","Spiller nu","","",""};
const char *music_unsupported[]={"Unsupported file or audio format.","Fil eller lyd format fejl.","","",""};
const char *music_authentication[]={"Play req. authentication.","Play req. authentication.","","",""};
const char *music_filenotfound[]={"File not found.","Sang ikke fundet.","","",""};
const char *music_ioerror[]={"Song %s can not be played No access.","IO error kan ikke afspilles.","","",""};
const char *music_cannotplay[]={"Song %s can not be played No access.","Sang %s kan ikke afspilles.","","",""};
const char *music_songloaderror[]={"Song load error. %d/%s ","Sang load fejl. %d/%s ","","",""};
const char *music_noartistfound[]={"No artist id found","Ingen artist id fundet","","",""};
const char *music_nomberofsongs[]={"# of songs: %2d Artist : %s","# of sange: %2d Artist : %s","","",""};
const char *music_nomberofdirs[]={"Numbers of directorys is :%d","Antal af directorys er :%d","","",""};
const char *music_numberinplaylist[]={"%d/%d in playlist ","%d/%d i playlisten",""};

// radio stuf
const char *radio_artistname[]={"Artist name","Artist navn","","",""};
const char *radio_songname[]={"Song name","Sang navn","","",""};
const char *radio_stationid[]={"Station id","Stations id","","",""};
const char *radio_playtimetxt[]={"Play time","Afspil. tid","","",""};
const char *radio_status[]={"Status","Status","","",""};
const char *radio_nowplaying[]={"Now Playing.","Spiller nu","","",""};

const char *radio_connecting[]={"Connecting..","Connecting..","","",""};
const char *radio_buffing[]={"Buffering.. %d %%","Buffering.. %d %%","","",""};
const char *radio_playing[]={"Playing..     ","Afspiller. ","","",""};

// movie stuf
const char *movie_rating[]={"Rating","Rating","Rating","Rating","Rating"};
const char *movie_length[]={"Length","Længde","Length","Length","Length"};
const char *movie_title[]={"Title","Title","Title","Title","Title"};
const char *movie_genre[]={"Genre","Genre","Genre","Genre","Genre"};
const char *movie_cast[]={"Cast","Cast","Case","Case","Case"};
const char *movie_description[]={"Description","Beskrivelse","Description","Description","Description"};
const char *movie_year[]={"Year","År","Year","Year","Year"};

void *radio_check_statusloader(void *data);
void *update_xmltv_phread_loader();

void *datainfoloader_stream(void *data);
void *update_rss_phread_loader();

void *datainfoloader_movie(void *data);
void *xbmcdatainfoloader_movie(void *data);

#endif
