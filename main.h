#ifndef MYCTRL_MAIN
#define MYCTRL_MAIN

const char *SHOWVER={"0.40.x"};

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
// Spotify        'D' Select device to play on
//

//
// xmltv update interval in sec
// default 2 * 60 min
// value in minute
//
const int doxmltvupdateinterval=60*60*2;
const int dorssupdateinterval=60*60;

// max string length search spotify stuf
const int search_string_max_length=15;

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
const char *music_filenotfound[]={"Song not found.","Sang ikke fundet.","Chanson introuvable","Lied wurde nicht gefunden","لم يتم العثور على الاغنية"};
const char *music_ioerror[]={"Song %s can not be played No access.","IO error %s kan ikke afspilles.","","",""};
const char *music_cannotplay[]={"Song %s can not be played No access.","Sang %s kan ikke afspilles.","","",""};
const char *music_songloaderror[]={"Song load error. %d/%s ","Sang load fejl. %d/%s ","","",""};
const char *music_noartistfound[]={"No artist id found","Ingen artist id fundet","Aucun identifiant d'artiste trouvé","Keine Künstler-ID gefunden","لم يتم العثور على معرف الفنان"};
const char *music_nomberofsongs[]={"# of songs: %2d Artist : %s","# of sange: %2d Artist : %s","","",""};
const char *music_nomberofdirs[]={"Numbers of directorys is :%d","Antal af directorys er :%d","","",""};
const char *music_numberinplaylist[]={"%d/%d in playlist ","%d/%d i playlisten",""};

// Sprog struktur.
//
// English, danish, france, tysk, Arabic
// radio stuf
const char *radio_artistname[]={"Artist name","Artist navn","Artiste navn","Künstler navn","الفنانة نافن"};
const char *radio_songname[]={"Song name","Sang navn","Titre de chanson","Liedtitel",""};
const char *radio_stationid[]={"Station id","Stations id","","",""};
const char *radio_playtimetxt[]={"Play time","Afspil. tid","Récréation","Spielzeit","وقت اللعب"};
const char *radio_status[]={"Status","Status","Status","Status","حالة"};
const char *radio_nowplaying[]={"Now Playing.","Spiller nu","Lecture en cours.","Läuft gerade.","الان العب."};

const char *radio_connecting[]={"Connecting..","Connecting..","","",""};
const char *radio_buffing[]={"Buffering.. %d %%","Buffering.. %d %%","","",""};
const char *radio_playing[]={"Playing..     ","Afspiller. ","","",""};

// Sprog struktur.
//
// English, danish, france, tysk, Arabic
// movie stuf
const char *movie_rating[]={"Rating","Rating","Notation","Bewertung","تقييم"};
const char *movie_length[]={"Length","Længde","Longueur","Länge","طول"};
const char *movie_title[]={"Title","Title","Titre","Title","لقب"};
const char *movie_genre[]={"Genre","Genre","Genre","Genre","النوع"};
const char *movie_cast[]={"Cast","Cast","Jeter","Besetzung","يقذف"};
const char *movie_description[]={"Description","Beskrivelse","Description","Beschreibung","وصف"};
const char *movie_year[]={"Year","År","An","Jahr","سنة"};

// Sprog struktur.
//
// English, danish, france, tysk, Arabic
// spotify stuf
const char *spotify_firsttime_line1[]={"Do you wants to start the first time spotify update ?","Ønsker du at starte første gangs spotify opdatering ?","Voulez-vous démarrer la première mise à jour de Spotify ?","Möchten Sie das Spotify-Update zum ersten starten?"};
const char *spotify_firsttime_line2[]={"It can take some time to complete.","Det kan godt tage lidt tid at gemmenføre.","It can take some time to complete.","It can take some time to complete."};
const char *spotify_firsttime_line3[]={"It will download you playlist collection.","Det henter din playliste samling.","Il vous téléchargera la collection de playlist.","Es wird Ihre Wiedergabelistensammlung herunterladen."};
const char *spotify_firsttime_line4[]={"You can use '*' now to start the update now.","Du skal bruge * til at starte opdateringen.","You can use '*' now to start the update now.","You can use '*' now to start the update now."};
const char *spotify_firsttime_pleasewait[]={"Please wait.","Vent venlist.","Please wait.","Please wait."};

// Sprog struktur.
//
// English, danish, france, tysk, Arabic
// tidal stuf
const char *tidal_firsttime_line1[]={"Do you wants to start the first time spotify update ?","Ønsker du at starte første gangs spotify opdatering ?","Voulez-vous démarrer la première mise à jour de Spotify ?","Möchten Sie das Spotify-Update zum ersten starten?"};
const char *tidal_firsttime_line2[]={"It can take some time to complete.","Det kan godt tage lidt tid at gemmenføre.","It can take some time to complete.","It can take some time to complete."};
const char *tidal_firsttime_line3[]={"It will download you playlist collection.","Det henter din playliste samling.","Il vous téléchargera la collection de playlist.","Es wird Ihre Wiedergabelistensammlung herunterladen."};
const char *tidal_firsttime_line4[]={"You can use '*' now to start the update now.","Du skal bruge * til at starte opdateringen.","You can use '*' now to start the update now.","You can use '*' now to start the update now."};
const char *tidal_firsttime_pleasewait[]={"Please wait.","Vent venlist.","Please wait.","Please wait."};

void *radio_check_statusloader(void *data);
void *update_xmltv_phread_loader();

void *datainfoloader_stream(void *data);
void *update_rss_phread_loader();

void *datainfoloader_movie(void *data);
void *xbmcdatainfoloader_movie(void *data);


// start (button)
//
float uvcolortable2[]={0.8,0.0,0.8, \
                      0.8,0.0,0.8, \
                      0.7,0.0,0.7, \
                      0.7,0.0,0.7, \
                      0.4,0.0,0.6, \
                      0.4,0.0,0.6, \
                      0.4,0.0,0.5, \
                      0.4,0.0,0.5, \
                      0.4,0.0,0.4, \
                      0.4,0.0,0.4, \
                      0.4,0.0,0.3, \
                      0.4,0.0,0.3, \
                      0.8,0.0,0.1, \
                      0.8,0.0,0.1};
// (top of uv)

#endif
