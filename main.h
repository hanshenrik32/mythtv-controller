#ifndef MYCTRL_MAIN
#define MYCTRL_MAIN

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

// other anim stuf

const char *music_icon_anim_files[]={"music","music1"};
const char *movie_icon_anim_files[]={"video","video1"};
const char *radio_icon_anim_files[]={"radio","radio1"};
const char *other_icon_anim_files[]={"recorded","lille_knap1_1"};
const char *tv_icon_anim_files[]={"tv","tv1"};

void *radio_check_statusloader(void *data);

void *update_xmltv_phread_loader();

float music_icon_anim_files_ofset=0.0f;
int music_icon_anim_files_numbers=19;

float tv_icon_anim_files_ofset=0.0f;
int tv_icon_anim_files_numbers=19;

float movie_icon_anim_files_ofset=0.0f;
int movie_icon_anim_files_numbers=19;
const int SINTABLE_SIZE=11*22*20;

const char *music_zoom_anim_files[]={"stor_knap3_2_mask_10.jpg","stor_knap3_2_mask_9.jpg","stor_knap3_2_mask_8.jpg","stor_knap3_2_mask_7.jpg","stor_knap3_2_mask_6.jpg","stor_knap3_2_mask_5.jpg","stor_knap3_2_mask_4.jpg","stor_knap3_2_mask_3.jpg","stor_knap3_2_mask_2.jpg","stor_knap3_2_mask_1.jpg","stor_knap3_2_mask.jpg"};

#endif
