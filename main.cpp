//
// main code
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// opengl
#include <GL/gl.h>
#include <GL/glut.h>                     // print
#include <IL/il.h>
#include <math.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <dirent.h>                     // dir functions
#include <netinet/in.h>                 // hostname
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <lirc/lirc_client.h>           // lirc client lib
#include <mysql.h>                      // mysql stuf
#include <sys/socket.h>
#include <unistd.h>                     // get hostname
#include <sqlite3.h>                    // sqlite interface to xbmc(kodi)
// vlc plugin
#include <vlc/vlc.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
// time
#include <ctime>
#include <sys/timeb.h>
// file io
#include <iostream>
#include <pthread.h>                      // multi thread support

// type used
using namespace std;

// if defined the support will be enabled

#define ENABLE_TIDAL
#define ENABLE_SPOTIFY



//

// web server
static bool runwebserver=true;
bool do_open_spotifyplaylist=false;
bool do_select_device_to_play=false;
bool ask_save_playlist = false;
bool save_ask_save_playlist = false;
bool stream_jump = false;

// Set sound system used
//#define USE_SDL_MIXER 1
#define USE_FMOD_MIXER 1

#if defined USE_SDL_MIXER
#include <SDL_mixer.h>
#endif
#if defined __APPLE__ && defined __MACH__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>                    // Header File For The GLUT Library linux
#include <GL/glc.h>                     // glc true type font system
#endif

#ifdef ENABLE_TIDAL
#include "myctrl_tidal.h"
#endif
// sound system include fmod
#if defined USE_FMOD_MIXER
#include "/opt/mythtv-controller/fmodstudioapi20107linux/api/core/inc/fmod.hpp"
#include "/opt/mythtv-controller/fmodstudioapi20107linux/api/core/inc/fmod_errors.h"
#endif

#include "mongoose-master/mongoose.h"

// glut fonts list

// GLUT_BITMAP_8_BY_13 - A variable-width font with every character fitting in a rectangle of 13 pixels high by at most 8 pixels wide.
// GLUT_BITMAP_9_BY_15 - A variable-width font with every character fitting in a rectangle of 15 pixels high by at most 9 pixels wide.
// GLUT_BITMAP_TIMES_ROMAN_10 - A 10-point variable-width Times Roman font.
// GLUT_BITMAP_TIMES_ROMAN_24 - A 24-point variable-width Times Roman font.
// GLUT_BITMAP_HELVETICA_10 - A 10-point variable-width Helvetica font.
// GLUT_BITMAP_HELVETICA_12 - A 12-point variable-width Helvetica font.
// GLUT_BITMAP_HELVETICA_18 - A 18-point variable-width Helvetica font.


extern float sinofsetz[];

bool tv_guide_firsttime_update = false;                                           //
int radio_oversigt_loaded_nr=0;                                                  //
bool radio_oversigt_loaded_begin = false;                                         // true first time radio is loaded
int radio_oversigt_antal=0;                                                      //
int music_oversigt_loaded_nr=0;                                                  //
int movie_oversigt_loaded_nr=0;                                                  //
bool movie_oversigt_gfx_loading = false;
bool show_status_update = false;
bool spotify_oversigt_loaded_begin = false;                                      // true then spotify update is started

#ifdef USE_FMOD_MIXER
FMOD::DSP* dsp = 0;                   // fmod Sound device
#endif

// screen saver uv stuf
float spectrum[2000];                                                           // used for spectium
float spectrum_left[2000];                                                      // used for spectium
float spectrum_right[2000];                                                     // used for spectium
float uvmax_values[1024];
int frequencyOctaves[15];                                                       // 15 octaver

static char huskname[1024];

// from compiler (debuger)
extern char __BUILD_DATE;
extern char __BUILD_NUMBER;
//

#include "main.h"
#include "myctrl_storagedef.h"
#include "myth_saver.h"
#include "myth_setup.h"
#include "myth_setupsql.h"
#include "myth_picture.h"
#include "myctrl_xbmc.h"
#include "myctrl_movie.h"
#include "myctrl_music.h"
#include "myctrl_radio.h"
#include "myctrl_tvprg.h"
#include "myctrl_stream.h"
#include "myctrl_recorded.h"
#include "myctrl_mplaylist.h"
#ifdef ENABLE_SPOTIFY
#include "myctrl_spotify.h"
#endif
//#ifdef ENABLE_TIDAL
//#include "myctrl_tidal.h"
//#endif
#include "checknet.h"
#include "myth_ttffont.h"
#include "readjpg.h"
// #include "myth_picture.h"

extern rss_stream_class rssstreamoversigt;

#ifdef ENABLE_SPOTIFY
spotify_class spotify_oversigt;
static bool do_update_spotify_playlist = false;           // do it first time thread
#endif

// tidal music class
#ifdef ENABLE_TIDAL
  tidal_class *tidal_oversigt;
#endif

char debuglogdata[1024];                                  // used by log system

// struct used by keyboard config of functions keys

struct configkeytype {
    char cmdname[200];
    unsigned int scrnr;
};

int numCPU;                                             // have the numbers of cpu cores
char systemcommand[2000];                               // shell command to do to play recorded program mplayer/vlc eks.
                                                        // (vlc player buildin. DEFAULT player)
const char *dbname="mythtvcontroller";                  // internal database name in mysql (music,movie,radio)
// koki db names (by version)
const char *kodiver[7]={"MyMusic70.db","MyMusic60.db","MyMusic56.db","MyMusic52.db","MyMusic48.db","MyMusic46.db","MyMusic32.db"};
const char *kodivermovie[7]={"MyVideos109.db","MyVideos107.db","MyVideos104.db","MyVideos99.db","MyVideos99.db","MyVideos99.db","MyVideos99.db"};
// ************************************************************************************************
char keybuffer[512];                                    // keyboard buffer
unsigned int keybufferindex=0;                          // keyboard buffer index
int findtype=0;					                              	// bruges af search kunster/sange
unsigned int do_show_setup_select_linie=0;              // bruges af setup
bool do_save_config = false;                              // flag to save config

channel_list_struct channel_list[MAXCHANNEL_ANTAL];     // channel_list array used in setup graber (default max 400) if you wats to change it look in myth_setup.h
channel_configfile  xmltv_configcontrol;                //

bool firsttime_xmltvupdate = true;                      // update tvguide xml files first start (force)
char playlistfilename[80];                              // name to use thewn save playlist
char movie_search_name[80];                             // name to use thewn search for movies
// ************************************************************************************************
char configmysqluser[256];                              // /mythtv/mysql access info
char configmysqlpass[256];                              //
char configmysqlhost[256];                              //
// ************************************************************************************************
char configxbmcuser[256];                               // /mythtv/mysql access info
char configxbmcpass[256];                               //
char configxbmchost[256];                               //
// ************************************************************************************************
bool hent_tv_channels = false;                            // if false update tv guide
long configtvguidelastupdate=0;                         // last date /unix time_t type) tvguide update
float configdefaulttvguidefontsize=18;                  // default font size tv tvguide
float configdefaultradiofontsize=18;                    // default font size
float configdefaultmusicfontsize=18;                    // default font size
float configdefaultstreamfontsize=18;                   // default font size
float configdefaultmoviefontsize=18;                    // default font size
char configdefaultmusicpath[256];                       // internal db for music
char configdefaultmoviepath[256];                       // internal db for movie
char configbackend_tvgraber[256];                       // internal tv graber to use
char configbackend_tvgraber_old[256];                   // rember last select tvgraber (used in selector in keyboard controler)
char configbackend_tvgraberland[2048];                  // internal tv graber to use
char configbackend[20];			                        		// backend system xbmc/mythtv
char configmythhost[256];			                        	// host of mythtv master
char configmysqlip[256];				                        // mysql server ip
char confighostname[256];                               // this hostname
char confighostip[256];				                          // this host ip adress
char confighostwlanname[256];				                    // wlan netid name
char configpicturepath[256];                            // path til pictures gallery source (den laver et dir under som hedder "mythc_gallery"
char configmusicpath[256];                              // path til music source
char configmusicmypath[256];                            // mythtv-controller internal path
char configmoviepath[256];                              //
char configrecordpath[256];                             //
char configmythsoundsystem[256];	                   		// selected soundsystem output device 0 = default
char avalible_device[10][256];			                    // sound system devises list
char configsoundoutport[256];			                    	// sound output port (hdmi/spdif/analog)
char configclosemythtvfrontend[256];		              	// close mythtvfront end on startup
char configaktivescreensavername[256];			            // screen saver name
char configdvale[256];			                         		// kan vi gå i dvale (mythtv kontrol) gen lock file
char configmouse[256];			                         		// mouse enable
storagedef configstoragerecord[storagegroupantal];    	// storage array (for recorded programs)
char configdeviceid[256];                               // music device name
float configsoundvolume=1.0f;                           // default sound volume
char configscreensavertimeout[256];			                // default screensaver timeout
char configfontname[200];				                        // default ttf font name to load and use
configkeytype configkeyslayout[12];	                		// functions keys startfunc
char configuse3deffect[20];			                      	// use 3d effects
// ************************************************************************************************
char configvideoplayer[200];                            // default video player
char configdefaultplayer[256];				                  // default player
int configdefaultplayer_screenmode;				              // default player screeb mode
// ************************************************************************************************
int configuvmeter;
// ************************************************************************************************
int configland=0;
const int configlandantal=5;
char *configlandsprog[]={(char *) "English",(char *) "Dansk",(char *) "France",(char *) "Tysk",(char *) "Arabic"};
// ************************************************************************************************
int tema=3;       					                            // aktiv tema (default is 3)
int screen_size=3;		                            			// default screen size
bool do_shift_song = false;				                        // jump to next song to play
bool do_stop_music = false;                               // stop all play
bool do_stop_radio = false;                               // stop all play
int soundsystem=0;		      		                      	  // used sound system 1=FMOD 2=OPENSOUND
int numbersofsoundsystems=0;                              // antal devices
unsigned int musicoversigt_antal=0;                       // antal aktive sange
unsigned int spotifycoversigt_antal=0;                    // antal aktive sange
bool firsttimespotifyupdate=true;                         // default show the option to update spotify db
int do_zoom_music_cover_remove_timeout=0;
int showtimeout=600;
int orgwinsizex,orgwinsizey;
                                                          // 1  = wifi net
                                                          // 2  = music
                                                          // 4  = stream
                                                          // 8  = keyboard/mouse move
                                                          // 16 = movie
                                                          // 32 = searcg
int debugmode=0;                                          // 64 = radio station land icon loader
                                                          // 128= stream search
                                                          // 256 = tv program stuf
                                                          // 512 = media importer
                                                          // 1024 = flag loader

char music_db_update_loader[256];                         //
bool loading_tv_guide = false;                            // loading_tv_guide true if loading
int tvchannel_startofset=0;                                 // ofset used in tv_graber config (line offset)
bool showfps = true;
int configmythtvver=0;            			                  // mythtv config found version
bool stopmovie = false;
int film_key_selected=1;                                  // den valgte med keyboard i film oversigt
int vis_volume_timeout=0;
int music_key_selected=1;                                 // default music selected
int spotify_key_selected=1;                                 // default music selected
bool ask_open_dir_or_play = false;                        // ask open dir or play it ?
bool ask_open_dir_or_play_aopen = false;                  // auto open dir
bool ask_open_dir_or_play_spotify = false;                //
bool ask_open_dir_or_play_tidal = false;                //
bool do_swing_music_cover = true;                         // default swing music cover
int music_selected_iconnr=0;                              // default valgt icon i music oversigt
float _angle=0.00;                                        // bruges af 3d screen saver
bool full_screen = true;
int do_play_music_aktiv_table_nr=0;                       // aktiv sang nr
int do_play_music_aktiv_nr_select_array=0;
bool show_uv = true;                                      // default show uv under menu
bool vis_uv_meter = false;                                // uv meter er igang med at blive vist
bool hent_music_search = false;                           // skal vi søge efter music
bool keybufferopenwin = false;                            // er vindue open
bool do_play_music_cover = false;                         // start play music
bool do_play_spotify_cover = false;                       // start play spotify song/playlist
bool do_find_playlist = false;                            // do find play list
bool do_play_music_aktiv_play = false;                    //

bool do_play_music_aktiv = false;                         //
bool do_stop_music_all = false;                           //
char aktivplay_music_path[1024];                          //

bool hent_radio_search = false;                           // skal vi søge efter music
bool hent_film_search = false;                            // skal vi søge efter film title
bool hent_stream_search = false;                          // skal vi søge efter stream
bool hent_spotify_search = false;                          // skal vi søge efter spotify stuf in local db

bool do_show_spotify_search_oversigt=false;
bool hent_spotify_search_online=false;                    // skal vi starte search online
bool do_hent_spotify_search_online=false;                 // skal vi starte search online (do it)
bool search_spotify_string_changed=false;
int do_music_icon_anim_icon_ofset=0;                  	  // sin scrool ofset for show fast music
int sinusofset=0;      					                          // bruges af show_music_oversigt
bool show_volume_info = false;                        	  // show vol info
int show_volume_info_timeout=0;                   		    // time out to close vol info again
bool stream_loadergfx_started = false;                	  // thread stream gfx loader running ?
bool stream_loadergfx_started_done = false;            	  // thread stream gfx loader running ?
bool stream_loadergfx_started_break = false;
bool vis_music_oversigt = false;                          // vis music player
bool vis_film_oversigt = false;				                    // vis film oversigt
bool vis_recorded_oversigt = false;                       // vis recorded oversigt
bool vis_tv_oversigt = false;                             // vis tv oversigt
bool vis_radio_oversigt = false;                          // vis radio player
bool vis_spotify_oversigt = false;                        // vis spotify player
bool vis_tidal_oversigt = false;                          // vis tidal player if supported and enabled
bool vis_old_recorded = false;                            //
bool vis_tvrec_list = false;                              // show tv program need record
bool saver_irq = false;                                   // er screen saver aktiv
bool radio_oversigt_loaded = false;                       //
bool radio_oversigt_loaded_done=0;                        //
unsigned int do_show_play_open_select_line=0;             // bruges til at vælge hvilken music filer som skal spilles
unsigned int do_show_play_open_select_line_ofset=0;       // bruges til at vælge hvilken music filer som skal spilles (start ofset)
bool show_radio_options = false;                          //
int radio_select_iconnr=0;                                //
float _rangley;                                           //
bool do_show_setup = false;                               // show setup menu
bool do_show_setup_sound = false;                         // Show sound setup view
bool do_show_setup_screen = false;                        // Show screen setuo view
bool do_show_setup_tema = false;                          // show tema setup view
bool do_show_setup_sql = false;                           // Show sql db setup view
bool do_show_setup_network = false;                       // Show network setuo view
bool do_show_setup_font = false;                          // font setup view
bool do_show_setup_keys = false;                          // show keys setup view
bool do_show_setup_rss = false;                           // Setup rss posdcast view
bool do_show_setup_spotify = false;                       // setup spotify menu
bool do_save_setup_rss = false;                           // update db flag to do it (call func)
bool do_show_videoplayer = false;                         // show player config
bool do_show_tvgraber = false;                            // show tv graber/channel view config
bool do_show_rss = false;                                 // show rss config
bool use3deffect = false;                                 // use 3d scroll effect default no
bool do_zoom_music_cover = false;                         // show music conver
bool do_zoom_tidal = false;                               //
bool do_zoom_radio = false;                               //
bool do_zoom_spotify_cover = false;                       // show spotify cover
bool do_zoom_stream = false;                              //

bool show_wlan_select = false;                            //

bool do_zoom_film_cover = false;                          //
bool do_zoom_stream_cover = false;                        // show played cover
bool vis_movie_options = false;                           //
bool vis_movie_sort_option = false;                       //

bool vis_stream_oversigt = false;
bool startstream = false;
bool do_play_stream = false;
bool do_stop_stream = false;
bool stopstream = false;

bool do_pause_stream = false;                             // pause play

int rknapnr=0;                                            // buttons vars
int sknapnr=0;                                            // stream button
int mknapnr=0;                                            // music
int tvknapnr=0;                                           // tv
int spotifyknapnr=1;                                      // spotify
int tidalknapnr=1;                                        // tidal support
int fknapnr=0;                                            // movie
int swknapnr=0;                                           //

// aktiv stream play
int stream_playnr=0;                                      //
char stream_playing_name[80];                             //
char stream_playing_desc[80];                             //
GLuint stream_playing_icon=0;                             //

int stream_key_selected=1;                                //
int stream_select_iconnr=0;                               //
int do_zoom_tvprg_aktiv_nr=0;                             //

int PRGLIST_ANTAL=0;                                      // used in tvguide xml program selector

int music_select_iconnr;                                  // selected icon
int spotify_select_iconnr;                                // selected icon
int spotify_selected_startofset=0;                                 //
int antal_songs=0;                                        //

int _sangley;                                             //
int _mangley;                                             //
int _angley;                                              //
int _spangley;                                             //

int music_icon_anim_icon_ofset=0;                         //
int music_icon_anim_icon_ofsety=0;                        //


int do_play_music_aktiv_nr=0;                           // den aktiv dirid som er trykket på


int screenx=1920;                                         // default screen size
int screeny=1080;                                         // default screen size


// xbmc/kodi db version files
int kodiverfound=0;


const float textcolor[3]={0.8f,0.8f,0.8f};
const float selecttextcolor[3]={0.4f,0.4f,0.4f};


float _fangley=0.0f;					// bruges af vis_film_oversigt glob
int do_zoom_film_aktiv_nr=0;
bool vis_radio_or_music_oversigt = false;
bool vis_stream_or_movie_oversigt = false;
bool global_use_internal_music_loader_system = false;     // use internal db for musicdb or mysql/kodi/
bool ask_tv_record = false;
bool do_play_radio = false;
bool do_play_spotify = false;
bool do_play_tidal = false;
GLint ctx, myFont;
bool do_swing_movie_cover = false;                        // do anim
bool vis_nyefilm_oversigt = true;                         // start med at vise nye film
// stream
stream_class streamoversigt;
bool show_stream_options = false;
bool startmovie = false;                  		// start play movie
int sleep_ok=0;
int sleeper=1;
unsigned int percent;
bool starving;
char aktivsongname[40];                         	// song name
char aktivartistname[40];                      		// navn på aktiv artist (som spilles)
bool check_radio_thread = true;            			  	// DO NOT check radio station online status
const unsigned int ERROR_TIMEOUT=120;                    // show error timeout
int vis_error_timeout=ERROR_TIMEOUT;
bool vis_error = false;
int vis_error_flag=0;
int vis_error_songnr;
int do_stream_icon_anim_icon_ofset=0;                   //
int stream_icon_anim_icon_ofset=0;                      //
unsigned int configrss_ofset=0;
unsigned int realrssrecordnr=0;                         //
int do_radio_icon_anim_icon_ofset=0;                    //
int radio_icon_anim_icon_ofset=0;                       //
unsigned int radiomoversigt_antal=0;                            // antal radio stationer
int do_movie_icon_anim_icon_ofset=0;
int film_select_iconnr=0;
int movie_icon_anim_icon_ofset=0;
// ************************************************************************************************
int visvalgtnrtype=1;
int visvalgttype=1;
// ************************************************************************************************
int do_play_recorded_aktiv_nr=0;                        // flag for start play af recorded
int valgtrecordnr=0;                                    // valgte recorded program
int subvalgtrecordnr=0;                                 // valgte sub recorded program som skal vises
int tvvalgtrecordnr=0;                                  // valgte tv recorded program                                   (SIMPLE)
int tvsubvalgtrecordnr=0;                               // valgte tv sub recorded program som skal vises                (SIMPLE)
int tvvisvalgtnrtype=1;
int tvvisvalgttype=1;
char aktivsongstatus[40];
// fmod stuf
#if defined USE_FMOD_MIXER
FMOD_OPENSTATE openstate;
int fmodbuffersize=16*1024;
#endif
#if defined USE_SDL_MIXER
Mix_Music *sdlmusicplayer=0;
int audio_buffers=1024;
int audio_rate=44100;
Uint16 audio_format=MIX_DEFAULT_FORMAT;
int audio_channels;
int sdlmusic;
#endif
// ************************************************************************************************
tv_oversigt     aktiv_tv_oversigt;
tv_graber_config  aktiv_tv_graber;
earlyrecorded aktiv_crecordlist;
GLuint tvoversigt;
GLuint canalnames;
int tvstartxofset=0;
// ************************************************************************************************
extern mplaylist aktiv_playlist;                                                // music play list

struct dirmusic_list_type {
    char name[200];
    unsigned int songlength;
    int aktiv;
};

struct dirmusic_dirs_type {
    char dirname[20];
    int dirid;
    GLuint _textureId;
};


const int dirliste_size=512;

void loadgfx();
void freegfx();


// class to playlist gfx *****************************************************************************

class dirmusictype {
    private:
        unsigned int listesize;			      		// antal elementer i liste
        dirmusic_dirs_type *dirliste;		   		// antal sange i playliste
        dirmusic_list_type *songliste;				// directory navne i playliste
        int numbersofsongs;				           	// numbers of songs in songlist array
        int numbersofdirs;			          		// numbers of under directorys in songlist array
        int artist_id;				            		// artist id from mythtv mysql
    public:
        GLuint textureId;		             			// directorys texture
        int emtydirmusic() {
            // for(unsigned int i=0;i<listesize;i++) {			// reset all music info
            for(int i=0;i<listesize;i++) {			// reset all music info
              strcpy(songliste[i].name,"");
              songliste[i].songlength=0;
              strcpy(dirliste[i].dirname,"");
              dirliste[i].dirid=0;
              dirliste[i]._textureId=0;			// reset Opengl cover id
            }
            numbersofsongs=0;
            numbersofdirs=0;
            return(true);
        }
        // constructor
        dirmusictype(unsigned int antal) { 				// constructor
            listesize=antal;
            dirliste=new dirmusic_dirs_type[antal];
            songliste=new dirmusic_list_type[antal];
            emtydirmusic();
            //for(unsigned int i=0;i<listesize;i++) songliste[i].aktiv = true;	// set play flag for all music
            for(int i=0;i<listesize;i++) songliste[i].aktiv = true;	// set play flag for all music
        }

        // destructor
        ~dirmusictype() {
            listesize=0;
            delete [] dirliste;
            delete [] songliste;
        }

        bool set_songaktiv(bool aktiv,int nr) {
            songliste[nr].aktiv=aktiv;
            return(true);
        }
        bool get_songaktiv(int nr) {						// get aktiv (overfør til aktiv playlist)
            return(songliste[nr].aktiv);
        }
        int pushsong(char *name,char *artist);
        int popsong(char *name,bool *aktiv,int nr);
        int numbersinlist() {
            return(numbersofsongs);
        }
        int pushdir(char *name,char *dirid);
        int popdir(char *name,int nr);
        int settexture(GLuint texture,int nr) {
	         if (nr<dirliste_size) {
	            dirliste[nr]._textureId=texture;
	             return(1);
	          } else return(0);
        }
        GLuint gettexture(int nr) {
            if (nr<dirliste_size) return(dirliste[nr]._textureId); else return(0);
        }
        int numbersindirlist() {
             return(numbersofdirs);
        }
};

// ************************************************************************************************

dirmusictype dirmusic(dirliste_size);					// oversigt over dirs/songs som skal spilles

int dirmusictype::pushsong(char *name,char *artist) {
    if (numbersofsongs<200) {
        strcpy(songliste[numbersofsongs].name,name);
//        songliste[numbersofsongs].aktiv=true;
        artist_id=atoi(artist);
        numbersofsongs++;
        return(numbersofsongs);
    } else return(0);
}


int dirmusictype::popsong(char *name,bool *aktiv,int nr) {
    if (nr<=numbersofsongs) {
        strcpy(name,songliste[nr].name);
        *aktiv=songliste[nr].aktiv;
        return(1);
    } else return(0);
}


int dirmusictype::pushdir(char *name,char *dirid) {
    if (numbersofsongs<200) {
        strcpy(dirliste[numbersofdirs].dirname,name);
        dirliste[numbersofdirs].dirid=atoi(dirid);
        numbersofdirs++;
        return(numbersofdirs);
    } else return(0);
}


int dirmusictype::popdir(char *name,int nr) {
    if (nr<=numbersofdirs) {
        strcpy(name,dirliste[nr].dirname);
        return(1);
    } else return(0);
}


// remote control

struct lirc_config *lircconfig;
int sock=0;                                     // lirc socket

#if defined USE_FMOD_MIXER
// *************************************************************************************************
// setup fmod sound device and program vars
FMOD::System    *sndsystem;
FMOD::Sound     *sound=0;
FMOD::Channel   *channel=0;
FMOD_RESULT     result;
unsigned int    fmodversion;
#endif
bool            playing = 0;
int             snd=0;

// Screen saver setup ******************************************************************************
int visur=0;
const int DIGITAL=1;
const int ANALOG=2;
const int SAVER3D=3;
const int SAVER3D2=4;
const int PICTURE3D=5;
const int MUSICMETER=6;

int urtype=1;                                   // set default screen saver

// *************************************************************************************************

recorded_overigt recordoversigt;                // optaget programer oversigt
bool reset_recorded_texture = false;

// *************************************************************************************************

wifinetdef wifinets;                            // wifi net class

// *************************************************************************************************

musicoversigt_class musicoversigt;              // Music class

// *************************************************************************************************
// film oversigt type class
film_oversigt_typem film_oversigt(FILM_OVERSIGT_TYPE_SIZE+1);

// Screen saver box ********************************************************************************

boxarray mybox;                                 // 3D screen saver

// *************************************************************************************************

// radio
radiostation_class radiooversigt;         // lav top radio statio oversigt
int aktiv_radio_station=0;                //
int radio_key_selected=1;                 // default
int radiooversigt_antal=0;                // antal aktive sange
GLint cur_avail_mem_kb = 0;               // free nvidia memory (hvis 0 så ændres gfx zoom lidt så det passer på ati/intel)
GLuint _textureutvbgmask;                 // background in tv guide programs
GLuint _defaultdvdcover;                	// The id of the texture
GLuint _texturemovieinfobox;	            //  movie image
GLuint _textureId_dir; 	                    // folder image
GLuint _textureId_song; 	                    // folder image
GLuint _texturemusicplayer; 	            // music image		// show player
GLuint _textureId9_askbox; 	              // askbox image
GLuint _textureId9_2; 	                  // askbox music image
GLuint _textureIdplayicon; 	              // play icon
GLuint _textureopen; 	                    // open icon
GLuint _textureclose; 	                  // close icon
GLuint _textureswap; 	                    // swap icon
GLuint _textureId11; 	                    // tv program oversigt logo
GLuint _textureId12; 	                    // background 1
GLuint _textureId12_1; 	                  // background 2
GLuint _textureId14; 	                    // pause knap
GLuint _texture_nocdcover; 	              // cdnocover big icon
GLuint _textureId20; 	                    // mask movie options box
GLuint _textureId22; 	                    // move options box
GLuint _textureId23; 	                    // movie options box
GLuint _textureId24; 	                    // movie options box
GLuint _textureId26; 	                    //
GLuint _textureId27; 	                    //
GLuint _textureId28; 	                    // dir playlist_icon
GLuint _textureIdback; 	                  //
GLuint _errorbox;	                        //
GLuint _textureIdreset_search;            // used in movie vi
GLuint _textureexit;                      // exit button
GLuint big_search_bar_playlist;                    // big search bar used by sporify search
GLuint big_search_bar_track;                    // big search bar used by sporify search
GLuint big_search_bar_albumm;                    // big search bar used by sporify search
GLuint big_search_bar_artist;                    // big search bar used by sporify search

// radio view icons
GLuint onlineradio;                       //
GLuint onlineradio_empty;                 //
GLuint onlineradio192;                    //
GLuint onlineradio320;                    //
GLuint radiooptions;                      //
GLuint radiobutton;                       //
GLuint tidalbutton;                       //
GLuint spotifybutton;                     //
GLuint spotify_askplay;                   //
GLuint spotify_askopen;                   //
GLuint spotify_search;                    // button in spotify search
GLuint spotify_search_back;               // back button in spotify search
GLuint spotify_ecover;                    //
GLuint spotify_pil;                       // pil bruges i spotify search nederst på skærmen midt for
GLuint musicbutton;                       //
GLuint streambutton;                      //
GLuint onlinestream;                      // stream default icon
GLuint onlinestream_empty;                // stream default icon
GLuint onlinestream_empty1;               // stream default icon
GLuint moviebutton;                       //

GLuint gfxlandemask;                      //
GLuint gfxlande[80];                      //
GLuint texturedot;                        //
GLuint texturedot1;                       //
// loading window
GLuint _textureIdloading;                 // loading window
// setup menu textures
GLuint setupupdatebutton;                 //
GLuint setuptexture;                      //
GLuint setuptvgraberback;
GLuint setupsoundback;
GLuint setupsqlback;
GLuint setupnetworkback;
GLuint setupnetworkwlanback;
GLuint setupscreenback;
GLuint setuptemaback;
GLuint setupfontback;
GLuint setupkeysback;
GLuint setuprssback;
GLuint _texturesetupclose;
GLuint mobileplayer_icon;                   // mobile pplayer icon
GLuint pcplayer_icon;                       // pc player icon
GLuint unknownplayer_icon;

// setup menu textures
GLuint _texturesoundsetup;                  // setup icon
GLuint _texturesourcesetup;                 //
GLuint _textureimagesetup;                  //
GLuint _texturetemasetup;                   //
GLuint _texturemythtvsql;                   //
GLuint _texturesetupfont;                   //
GLuint _texturekeyssetup;                   //
GLuint _texturekeysrss;                     //
GLuint _texturespotify;
GLuint _texturetidal;
GLuint spotify_icon_border;                 // spotify border icon for spotify icon
GLuint _texturevideoplayersetup;            //
GLuint _texturetvgrabersetup;               //

GLuint _texturesaveplaylist;                  // setup icon

GLuint _texturelock;

GLuint setupkeysbar1;                       // bruges af setup
GLuint setupkeysbar2;                       // bruges af setup

GLuint screenshot1,screenshot2,screenshot3,screenshot4,screenshot5,screenshot6,screenshot7,screenshot8,screenshot9,screenshot10;

GLuint _textureIdback_main=0,_textureIdback_music=0;            // main background
GLuint _textureIdback_setup;
GLuint  screensaverbox;
GLuint  screensaverbox1;
GLuint _textureIdmusic_aktiv;
GLuint _tvbar1,_tvbar3;
GLuint _mainlogo;
GLuint _tvnewprgrecordedbutton;
GLuint _tvrecordcancelbutton;

GLuint _tvrecordbutton;
GLuint _tvoldprgrecordedbutton;
GLuint _tvprgrecorded;
GLuint _tvprgrecordedr;
GLuint tvprginfobig;
GLuint _tvoverskrift;             // overskrift window
GLuint _texturesetupmenu_select;
GLuint _texturesetupmenu;
GLuint _textureIdclose;
GLuint _textureIdclose1;
GLuint _texturemplay;
GLuint _texturempause;
GLuint _texturemstop;
GLuint _texturemnext;
GLuint _textureIdradiosearch;
GLuint _textureIdplayinfo;
GLuint _textureIdpil;
GLuint _textureIdmusicsearch1;
GLuint _textureIdmusicsearch;
GLuint _textureIdmoviesearch;
GLuint _textureIdrecorded_aktiv;
GLuint _textureIdpup;
GLuint _textureIdpdown;
GLuint _textureIdback_other;
GLuint _texturemlast2;
GLuint _texturemlast;
GLuint _textureIdfilm_aktiv;
GLuint _textureIdrecorded;
GLuint _textureIdfilm;
GLuint _textureIdmusic;
GLuint _textureIdtv;
GLuint _tvbar1_1;

GLuint newstuf_icon;                        // icon for new stuf in stream view
GLuint analog_clock_background;             // background for analog clock

GLuint _textureIdmusic_mask_anim[10];    // texture array to anim of music menu icon

fontctrl aktivfont;                                                             // font control (default aktiv font all over (if opencl))

unsigned int setupfontselectofset=0;                                            // valgte font i oversigt
unsigned int setupwlanselectofset=0;                                            // valgte wlan nr i oversigt

const int TEMA_ANTAL=10;                                                        // numbers of tema

// define for use before function is created
void *update_music_phread_loader();
void *update_spotify_phread_loader();
void *update_webserver_phread_loader();
void *update_spotifyonline_phread_loader();
void *webupdate_loader_spotify(void *data);
bool spotify_update_loaded_begin=false;

// hent mythtv version and return it

int hentmythtvver() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sqlselect[200];
    char txtversion[200];
//    int i;
    // mysql stuf
    strcpy(txtversion,"");
    sprintf(sqlselect,"select data from settings where value like 'DBSchemaVer'");
    // mysql stuf
    char *database = (char *) "mythconverg";
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(txtversion,row[0]);		// hent ver
      }
    }
    if (conn) mysql_close(conn);
    return(atoi(txtversion));
}


// ********************************************************** sound system error handler for fmod


#if defined USE_FMOD_MIXER
// fmod error handler
void ERRCHECK(FMOD_RESULT result,unsigned int songnr) {
  char file_path[1024];
  if (result != FMOD_OK) {
    fprintf(stderr,"FMOD error! (%d): %s on songnr %d \n", result, FMOD_ErrorString(result),songnr);
    if (vis_music_oversigt) {
      aktiv_playlist.m_play_playlist(file_path,songnr);
      fprintf(stderr,"File name %s\n",file_path);
    }
    vis_error_flag=result;
    vis_error = true;			// set vis error flag
    vis_error_songnr=songnr;		// gem fil navn som ikke kan spilles
    vis_error_timeout=ERROR_TIMEOUT;
    do_zoom_radio = false;		// close play info
    if ((result!=23) && (result!=37) && (result!=27) && (result!=20)) {
      fprintf(stderr,"FMOD ERROR %d %s ",result,FMOD_ErrorString(result));
//          exit(-1);
    }
  }
}

#endif




// ***********************************************************
//
// error handler sdl
//
// ***********************************************************


void ERRCHECK_SDL(char *text,unsigned int songnr) {
  char file_path[1024];
  if (vis_music_oversigt) {
    aktiv_playlist.m_play_playlist(file_path,songnr);
    write_logfile("File play error :");
    fprintf(stderr,"File name %s error : %s\n",file_path,text);
  }
  vis_error=1;			// vis error
  if (strcmp(text,"music parameter was NULL")==0) vis_error_flag=36;
  vis_error = true;			            // set vis error falg
  do_zoom_radio = false;            // close play info
  vis_error_songnr=songnr;          // gem fil navn som ikke kan spilles
  vis_error_timeout=ERROR_TIMEOUT;
}



// ****************************************************************************************
//
// config file paser
//
// enum def commands
//
// ****************************************************************************************


int parse_config(char *filename) {
    char buffer[513];
    FILE *fil;
    int n,nn;
    enum commands {setmysqlhost, setmysqluser, setmysqlpass, setsoundsystem, setsoundoutport, setscreensaver, setscreensavername,setscreensize, \
                   settema, setfont, setmouse, setuse3d, setland, sethostname, setdebugmode, setbackend, setscreenmode, setvideoplayer,setconfigdefaultmusicpath,setconfigdefaultmoviepath,setuvmetertype,setvolume,settvgraber,tvgraberupdate,tvguidercolor,tvguidefontsize,radiofontsize,musicfontsize,streamfontsize,moviefontsize,spotifydefaultdevice};
    int commandlength;
    char value[200];
    bool command = false;
    int command_nr;
    bool valueok;
    strcpy(configmythhost,"127.0.0.1");
    strcpy(configbackend,"mythtv");
    fil=fopen(filename,"r");
    if (fil) {
      while(!(feof(fil))) {
        fgets(buffer,512,fil);
        n=0;
        nn=0;
        command = false;
        valueok = false;
        // remove all spaces
        while((n<strlen(buffer)) && (*(buffer+n)==32)) n++;
        if (n!=strlen(buffer)) {
          // test for command
          if (buffer[n]!='#') {
            if (strncmp(buffer+n,"backend",6)==0) {
              command = true;
              command_nr=setbackend;
              commandlength=6;
            } else if (strncmp(buffer+n,"mythhost",7)==0)  {
              command = true;
              command_nr=sethostname;
              commandlength=7;
            } else if (strncmp(buffer+n,"mysqlhost",8)==0) {
              command = true;
              command_nr=setmysqlhost;
              commandlength=8;
            } else if (strncmp(buffer+n,"mysqluser",8)==0) {
              command = true;
              command_nr=setmysqluser;
              commandlength=8;
            } else if (strncmp(buffer+n,"mysqlpass",8)==0) {
              command = true;
              command_nr=setmysqlpass;
              commandlength=8;
            } else if (strncmp(buffer+n,"soundsystem",10)==0) {
              command = true;
              command_nr=setsoundsystem;
              commandlength=10;
            } else if (strncmp(buffer+n,"soundoutport",11)==0) {
              command = true;
              command_nr=setsoundoutport;
              commandlength=11;
            } else if (strncmp(buffer+n,"screensaver=",12)==0) {
              command = true;
              command_nr=setscreensaver;
              commandlength=10;
            } else if (strncmp(buffer+n,"screensavername",14)==0) {
              command = true;
              command_nr=setscreensavername;
              commandlength=14;
            } else if (strncmp(buffer+n,"screensize",9)==0) {
              command = true;
              command_nr=setscreensize;
              commandlength=9;
            } else if (strncmp(buffer+n,"tema",3)==0) {
              command = true;
              command_nr=settema;
              commandlength=3;
            } else if (strncmp(buffer+n,"font",3)==0) {
              command = true;
              command_nr=setfont;
              commandlength=3;
            } else if (strncmp(buffer+n,"mouse",4)==0) {
              command = true;
              command_nr=setmouse;
              commandlength=4;
            } else if (strncmp(buffer+n,"use3d",4)==0) {
              command = true;
              command_nr=setuse3d;
              commandlength=4;
            } else if (strncmp(buffer+n,"land",3)==0) {
              command = true;
              command_nr=setland;
              commandlength=3;
            } else if (strncmp(buffer+n,"fullscreen",9)==0) {
              command = true;
              command_nr=setscreenmode;
              commandlength=9;
            } else if (strncmp(buffer+n,"configdefaultmusicpath",21)==0) {
              command = true;
              command_nr=setconfigdefaultmusicpath;
              commandlength=21;
            } else if (strncmp(buffer+n,"configdefaultmoviepath",21)==0) {
              command = true;
              command_nr=setconfigdefaultmoviepath;
              commandlength=21;
            } else if (strncmp(buffer+n,"videoplayer",10)==0) {
              command = true;
              command_nr=setvideoplayer;
              commandlength=10;
            } else if (strncmp(buffer+n,"debug",4)==0) {
              command_nr=setdebugmode;
              command = true;
              commandlength=4;
              debugmode=atoi(value);		// set debug mode from config file
              showfps = true;
            } else  if (strncmp(buffer+n,"uvmetertype",10)==0) {
              command = true;
              command_nr=setuvmetertype;
              commandlength=10;
            } else if (strncmp(buffer+n,"defaultvolume",12)==0) {
              command = true;
              command_nr=setvolume;
              commandlength=12;
            } else if (strncmp(buffer+n,"tvgraberupdate",13)==0) {
              command = true;
              command_nr=tvgraberupdate;
              commandlength=13;
              // vi do not have tvgraberpath in config file
            } else if ((strncmp(buffer+n,"tvgraber",7)==0) && (strncmp(buffer+n,"tvgraberpath",11)!=0)) {
              command = true;
              command_nr=settvgraber;
              commandlength=7;
            } else if (strncmp(buffer+n,"tvguidercolor",12)==0) {
              command = true;
              command_nr=tvguidercolor;
              commandlength=12;
            } else if (strncmp(buffer+n,"tvguidefontsize",14)==0) {
              command = true;
              command_nr=tvguidefontsize;
              commandlength=14;
            } else if (strncmp(buffer+n,"radiofontsize",12)==0) {
              command = true;
              command_nr=radiofontsize;
              commandlength=12;
            } else if (strncmp(buffer+n,"musicfontsize",12)==0) {
              command = true;
              command_nr=musicfontsize;
              commandlength=12;
            } else if (strncmp(buffer+n,"streamfontsize",13)==0) {
              command = true;
              command_nr=streamfontsize;
              commandlength=13;
            } else if (strncmp(buffer+n,"moviefontsize",12)==0) {
              command = true;
              command_nr=moviefontsize;
              commandlength=12;
            } else if (strncmp(buffer+n,"spotifydefaultdevice",19)==0) {
              printf("Set command to set default spotify play device\n");
              command = true;
              command_nr=spotifydefaultdevice;
              commandlength=19;
            } else command = false;
          }
          strcpy(value,"");
          if (command) {
            while((n<strlen(buffer)) && (!(valueok))) {
              if ((buffer[n]!=10) && (buffer[n]!='=')) {
                if ((*(buffer+n)!='=') && (*(buffer+n)!=' ') && (*(buffer+n)!=10) && (*(buffer+n)!='\'') && (*(buffer+n)!=13)) {
                  valueok = true;
                  strcpy(value,buffer+n+commandlength+2);
                  nn=strlen(value);
                  if (nn>0) {
                      if (value[nn-1]=='\n') value[nn-1]=0;
                      else value[nn]=0;
                  }
                }
              }
              n++;
            }
          }
          if ((command) && (valueok)) {
            // set backupend system to XBMC or mythtv
            if (command_nr==setbackend) {
              strcpy(configbackend,value);
              fprintf(stderr,"*********** Set config mode MYTHTV/XBMC+KODI  ***********\n");
              fprintf(stderr,"Mode selected : %s\n",configbackend);
              fprintf(stderr,"*********************************************************\n");
            }
            // set tv graber
            else if (command_nr==settvgraber) {
              strcpy(configbackend_tvgraber,value);
              if (strcmp(configbackend_tvgraber,"tv_grab_na_dd")==0) aktiv_tv_graber.graberaktivnr=1;
              else if (strcmp(configbackend_tvgraber,"tv_grab_nl")==0) aktiv_tv_graber.graberaktivnr=2;
              else if (strcmp(configbackend_tvgraber,"tv_grab_es_laguiatv")==0) aktiv_tv_graber.graberaktivnr=3;
              else if (strcmp(configbackend_tvgraber,"tv_grab_il")==0) aktiv_tv_graber.graberaktivnr=4;
              else if (strcmp(configbackend_tvgraber,"tv_grab_na_tvmedia")==0) aktiv_tv_graber.graberaktivnr=5;
              else if (strcmp(configbackend_tvgraber,"tv_grab_dtv_la")==0) aktiv_tv_graber.graberaktivnr=6;
              else if (strcmp(configbackend_tvgraber,"tv_grab_fi")==0) aktiv_tv_graber.graberaktivnr=7;
              else if (strcmp(configbackend_tvgraber,"tv_grab_eu_dotmedia")==0) aktiv_tv_graber.graberaktivnr=8;
              else if (strcmp(configbackend_tvgraber,"tv_grab_se_swedb")==0) aktiv_tv_graber.graberaktivnr=9;
              else if (strcmp(configbackend_tvgraber,"tv_grab_pt_meo")==0) aktiv_tv_graber.graberaktivnr=10;
              else if (strcmp(configbackend_tvgraber,"tv_grab_fr")==0) aktiv_tv_graber.graberaktivnr=11;
              else if (strcmp(configbackend_tvgraber,"tv_grab_uk_bleb")==0) aktiv_tv_graber.graberaktivnr=12;
              else if (strcmp(configbackend_tvgraber,"tv_grab_huro")==0) aktiv_tv_graber.graberaktivnr=13;
              else if (strcmp(configbackend_tvgraber,"tv_grab_ch_search")==0) aktiv_tv_graber.graberaktivnr=14;
              else if (strcmp(configbackend_tvgraber,"tv_grab_it")==0) aktiv_tv_graber.graberaktivnr=15;
              else if (strcmp(configbackend_tvgraber,"tv_grab_is")==0) aktiv_tv_graber.graberaktivnr=16;
              else if (strcmp(configbackend_tvgraber,"tv_grab_fi_sv")==0) aktiv_tv_graber.graberaktivnr=17;
              else if (strcmp(configbackend_tvgraber,"tv_grab_na_dtv")==0) aktiv_tv_graber.graberaktivnr=18;
              else if (strcmp(configbackend_tvgraber,"tv_grab_tr")==0) aktiv_tv_graber.graberaktivnr=19;
              else if (strcmp(configbackend_tvgraber,"tv_grab_eu_egon")==0) aktiv_tv_graber.graberaktivnr=20;
              else if (strcmp(configbackend_tvgraber,"tv_grab_dk_dr")==0) aktiv_tv_graber.graberaktivnr=21;
              else if (strcmp(configbackend_tvgraber,"tv_grab_se_tvzon")==0) aktiv_tv_graber.graberaktivnr=22;
              else if (strcmp(configbackend_tvgraber,"tv_grab_ar")==0) aktiv_tv_graber.graberaktivnr=23;
              else if (strcmp(configbackend_tvgraber,"tv_grab_fr_kazer")==0) aktiv_tv_graber.graberaktivnr=24;
              else if (strcmp(configbackend_tvgraber,"tv_grab_uk_tvguide")==0) aktiv_tv_graber.graberaktivnr=25;
              else aktiv_tv_graber.graberaktivnr=0;

              fprintf(stderr,"**************** Set config xmltv graber ****************\n");
              fprintf(stderr,"Tv graber ....: %s\n",configbackend_tvgraber);
              fprintf(stderr,"*********************************************************\n");
            } else if (command_nr==tvgraberupdate) configtvguidelastupdate=atol(value);
            // set tvguide color or no color
            else if (command_nr==tvguidercolor) {
             if (strcmp(value,"yes")==0) aktiv_tv_oversigt.vistvguidecolors = true;
             else aktiv_tv_oversigt.vistvguidecolors = false;
            // set hostname
            } else if (command_nr==sethostname) strcpy(configmythhost,value);
            // mysql host
            else if (command_nr==setmysqlhost) strcpy(configmysqlhost,value);
            // mysql user
            else if (command_nr==setmysqluser) strcpy(configmysqluser,value);
            // mysql pass
            else if (command_nr==setmysqlpass) strcpy(configmysqlpass,value);
            // set sound system
            else if (command_nr==setsoundsystem) {
              strcpy(configmythsoundsystem,value);
              soundsystem=atoi(configmythsoundsystem);
            } else if (command_nr==setdebugmode) {
              if (strcmp(value,"0")!=0) {
                debugmode=atoi(value);
                fprintf(stderr,"Set debug mode %d  \n",debugmode);
              } else {
                debugmode=0;
                showfps = false;
              }
            }
            else if (command_nr==setvideoplayer) {
              if (strcmp(value,"")==0) strcpy(value,"default");                               // set default player (internal vlc)
              strcpy(configvideoplayer,value);
            }
            // sound port
            else if (command_nr==setsoundoutport) strcpy(configsoundoutport,value);
            // screen saver timeout
            else if (command_nr==setscreensaver) {
              strcpy(configscreensavertimeout,value);
              if (atoi(configscreensavertimeout)==0) strcpy(configscreensavertimeout,"30");
            }
            // screen saver
            else if (command_nr==setscreensavername) {
              strcpy(configaktivescreensavername,value);
              if (strncmp(configaktivescreensavername,"analog",6)==0) {
                  urtype=ANALOG;
              } else if (strncmp(configaktivescreensavername,"digital",6)==0) {
                  urtype=DIGITAL;
              } else if (strncmp(configaktivescreensavername,"3D2",3)==0) {
                  urtype=SAVER3D2;
              } else if (strncmp(configaktivescreensavername,"3D",2)==0) {
                  urtype=SAVER3D;
              } else if (strncmp(configaktivescreensavername,"PICTURE3D",9)==0) {
                  urtype=PICTURE3D;
              } else if (strncmp(configaktivescreensavername,"MUSICMETER",9)==0) {
                  urtype=MUSICMETER;
              } else urtype=ANALOG;
            }
            // screen size
            else if (command_nr==setscreensize) {
              screen_size=atoi(value);
              if (screen_size==0) screen_size=1;		// set min default screen size mode 1 = 1024/768
            }
            // select tema
            else if (command_nr==settema) {
              tema=atoi(value);
              if (tema==0) tema=6;
            }
            // set font name
            else if (command_nr==setfont) strcpy(configfontname,value);
            // show mouse (visible)
            else if (command_nr==setmouse) strcpy(configmouse,value);
            // set full screen off
            else if (command_nr==setscreenmode) {
              if (strcmp(value,"true")==0) full_screen = true; else full_screen = false;
            } else if (command_nr==setconfigdefaultmusicpath) {
              strcpy(configdefaultmusicpath,value);
              strcpy(configmusicpath,value);
            } else if (command_nr==setconfigdefaultmoviepath) {
              strcpy(configdefaultmoviepath,value);
              strcpy(configmoviepath,value);
            }
            // use 3d effect
            else if (command_nr==setuse3d) {
                if (strcmp(value,"yes")) {
                    use3deffect = true;
                    strcpy(configuse3deffect,"yes");
                } else {
                    use3deffect = false;
                    strcpy(configuse3deffect,"no");
                }
            } else if (command_nr==setland) {
              configland=1; // set default land code
            } else if (command_nr==setuvmetertype) {
              configuvmeter=atoi(value);
            } else if (command_nr==setvolume) {
              configsoundvolume=atof(value);                                    // set default volume under play
            } else if (command_nr==tvguidefontsize) {
              configdefaulttvguidefontsize=atof(value);                         // set tvguide font size
            } else if (command_nr==radiofontsize) {
               configdefaultradiofontsize=atof(value);                          // set radio font size
            } else if (command_nr==musicfontsize) {
                configdefaultmusicfontsize=atof(value);                         // set music font size
            } else if (command_nr==streamfontsize) {
              configdefaultstreamfontsize=atof(value);                          // set stream font size
            } else if (command_nr==moviefontsize) {
              configdefaultmoviefontsize=atof(value);                           // set movie font size
            } else if (command_nr==spotifydefaultdevice) {                      // do now work for now
              printf("Set default spotify play device to %s\n",value);
              #ifdef ENABLE_SPOTIFY
              strcpy(spotify_oversigt.active_default_play_device_name,value);   //
              #endif
            }
          }
        }
        strcpy(buffer,"");
      }
      fclose(fil);
    } else return(0);
    if (check_zerro_bytes_file(filename)>0) return(1); else return(0);
}


// ****************************************************************************************
//
// save config to file
//
// ****************************************************************************************

int save_config(char * filename) {
    bool error = false;
    char temp[80];
    FILE *file;
    if ((file = fopen(filename, "w"))) {
      fputs("backend=",file);
      fputs(configbackend,file);
      fputs("\n",file);
      fputs("mysqluser=",file);
      fputs(configmysqluser,file);
      fputs("\n",file);
      fputs("mysqlpass=",file);
      fputs(configmysqlpass,file);
      fputs("\n",file);
      fputs("mysqlhost=",file);
      fputs(configmysqlhost,file);
      fputs("\n",file);
      fputs("mythhost=",file);
      fputs(configmythhost,file);
      fputs("\n",file);
      fputs("soundsystem=",file);
      sprintf(temp,"%d",soundsystem);
      fputs(temp,file);
      fputs("\n",file);
      fputs("soundoutport=",file);
      fputs(configsoundoutport,file);
      fputs("\n",file);
      fputs("screensaver=",file);
      fputs(configscreensavertimeout,file);
      fputs("\n",file);
      fputs("screensavername=",file);
      if (urtype==DIGITAL) fputs("digital\n",file);
      else if (urtype==ANALOG) fputs("analog\n",file);
      else if (urtype==SAVER3D) fputs("3D\n",file);
      else if (urtype==SAVER3D2) fputs("3D2\n",file);
      else if (urtype==PICTURE3D) fputs("PICTURE3D\n",file);
      else if (urtype==MUSICMETER) fputs("MUSICMETER\n",file);
      else fputs("None\n",file);
      sprintf(temp,"screensize=%d\n",screen_size);
      fputs(temp,file);
      sprintf(temp,"tema=%d\n",tema);
      fputs(temp,file);
      sprintf(temp,"font=%s\n",configfontname);
      fputs(temp,file);
      sprintf(temp,"mouse=%s\n",configmouse);
      fputs(temp,file);
      sprintf(temp,"use3d=%s\n",configuse3deffect);
      fputs(temp,file);
      sprintf(temp,"land=%d\n",configland);
      fputs(temp,file);
      if (full_screen) sprintf(temp,"fullscreen=true\n"); else sprintf(temp,"fullscreen=false\n");
      fputs(temp,file);
      sprintf(temp,"debug=%d\n",debugmode);
      fputs(temp,file);
      sprintf(temp,"videoplayer=default\n");
      fputs(temp,file);
      sprintf(temp,"configdefaultmusicpath=%s\n",configdefaultmusicpath);
      fputs(temp,file);
      sprintf(temp,"configdefaultmoviepath=%s\n",configdefaultmoviepath);
      fputs(temp,file);
      sprintf(temp,"uvmetertype=%d\n",configuvmeter);                               // uv meter type
      fputs(temp,file);
      sprintf(temp,"defaultvolume=%2.2f\n",configsoundvolume);                      // sound volume
      fputs(temp,file);
      sprintf(temp,"tvgraber=%s\n",configbackend_tvgraber);                         // tv graber to use
      fputs(temp,file);
      sprintf(temp,"tvgraberupdate=%ld\n",configtvguidelastupdate);
      fputs(temp,file);
      sprintf(temp,"tvguidefontsize=%0.0f\n",configdefaulttvguidefontsize);
      fputs(temp,file);
      sprintf(temp,"radiofontsize=%0.0f\n",configdefaultradiofontsize);
      fputs(temp,file);
      sprintf(temp,"musicfontsize=%0.0f\n",configdefaultmusicfontsize);
      fputs(temp,file);
      sprintf(temp,"streamfontsize=%0.0f\n",configdefaultstreamfontsize);
      fputs(temp,file);
      sprintf(temp,"moviefontsize=%0.0f\n",configdefaultmoviefontsize);
      fputs(temp,file);
      //aktiv_tv_oversigt.vistvguidecolors=true;
      if (aktiv_tv_oversigt.vistvguidecolors) sprintf(temp,"tvguidercolor=yes\n");
      else sprintf(temp,"tvguidercolor=no\n");
      fputs(temp,file);
      #ifdef ENABLE_SPOTIFY
      sprintf(temp,"spotifydefaultdevice=%s\n",spotify_oversigt.get_device_name(spotify_oversigt.active_default_play_device));
      fputs(temp,file);
      #endif
      fclose(file);
    } else error = true;
    file = fopen("mythtv-controller.keys", "w");
    if (file) {
      fwrite(configkeyslayout,sizeof(configkeytype)*12,1,file);
      fclose(file);
    } else error = true;
    return(!(error));
}



// ****************************************************************************************
//
// load all config from config file
//
// ****************************************************************************************

void load_config(char * filename) {
    char sqlselect[512];
    char temptxt[512];
    char hostname[128];
    int i;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    FILE *file;
    struct hostent *remoteHost;
    struct in_addr **addr_list;
    gethostname(hostname,128);				                		// get this hosts name
    strcpy(confighostname,hostname);
    char *database = (char *) "mythconverg";			            // mythtv database name
    strcpy(configrecordpath,"");			                     		// default value (bliver fundet i mysql mythtv databasen)
    for(int i = 0; i < storagegroupantal; i++) {
      strcpy(configstoragerecord[i].path,"");
      strcpy(configstoragerecord[i].name,"");
    }
    strcpy(configdefaultplayer,"default");	                 	// default sound player (fmod)
    strcpy(configclosemythtvfrontend,"no");		                // close mythtv frontend
    strcpy(configscreensavertimeout,"30");	                 	// default screensaver timeout
    strcpy(configsoundoutport,"SPDIF");			                  // default sound interface
    strcpy(configdvale,"yes");
    strcpy(configuse3deffect,"yes");
    strcpy(configfontname,"FreeMono");
    strcpy(configvideoplayer,"default");
    strcpy(configdefaultmusicpath,"Music");                   // default start music dir
    strcpy(configdefaultmoviepath,"Movie");                   // default start music dir
    strcpy(configdefaultmoviepath,"Movie");                   // default start music dir
    strcpy(configbackend_tvgraber,"tv_grab_eu_dotmedia");      // default tv guide tv_grab_uk_tvguide
    strcpy(configbackend_tvgraberland,"");                    // default tv guide tv_grab_uk_tvguide other command
    configtvguidelastupdate=0;                                // default 0
    configsoundvolume=1.0f;
    configuvmeter=1;                                          // default uv meter type
    for(int t=0;t<12;t++) {
      strcpy(configkeyslayout[t].cmdname,"");
      configkeyslayout[t].scrnr=0;
    }
    // set default keys
    strcpy(configkeyslayout[0].cmdname,"spotify");
    // load/parse config file in to globals ver
    if (!(parse_config(filename))) {
      strcpy(configaktivescreensavername,"analog");				  // default analog clock
      urtype=2;								                              // default screen saver
      strcpy(configmysqluser,"mythtv");				           	  // default userid for mythtv
      strcpy(configmysqlpass,"password");				         	  // default password
      strcpy(configmysqlhost,"localhost");		      		    // localhost mysql server default
      strcpy(configmythhost,"localhost");				           	// localhost mythtv server default
      strcpy(configmythsoundsystem,"");			          		  //
      strcpy(configsoundoutport,"SPDIF");				         	  // spdif out default
      strcpy(configclosemythtvfrontend,"no");				     	  //
      strcpy(configfontname,"FreeMono");			           		//
      strcpy(configmouse,"1");						                  // enable mouse default
      FILE * file = fopen(filename, "w");                   // open file for write
      if (file) {
        fputs("mysqluser=mythtv\n",file);                  // write config info to config file
        fputs("mysqlpass=password\n",file);
        fputs("mysqlhost=localhost\n",file);
        fputs("mythhost=localhost\n",file);
        fputs("soundsystem=0\n",file);
        fputs("soundoutport=int\n",file);
        fputs("screensaver=60\n",file);
        strcpy(temptxt,"screensavername=");
        strcat(temptxt,configaktivescreensavername);
        strcat(temptxt,"\n");
        fputs(temptxt,file);
        fputs("screensize=3\n",file);
        fputs("tema=3\n",file);
        fputs("font=FreeMono\n",file);
        fputs("mouse=1\n",file);
        fputs("use3d=yes\n",file);
        fputs("land=1\n",file);
        fputs("debug=0\n",file);
        fputs("videoplayer=default\n",file);
        fputs("configdefaultmusicpath=Music\n",file);
        fputs("configdefaultmovie=Movies\n",file);
        fputs("uvmetertype=1\n",file);
        fputs("tvgraber=tv_grab_eu_dotmedia\n",file);
        fputs("tvgraberupdate=0\n",file);
        fputs("tvgrabercolor=yes\n",file);
        fputs("tvguidefontsize=18\n",file);
        fputs("radiofontsize=18\n",file);
        fputs("musicfontsize=18\n",file);
        fputs("streamfontsize=18\n",file);
        fputs("moviefontsize=18\n",file);
        fputs("spotifydefaultdevice=\n",file);
        fputs("tidaldefaultdevice=\n",file);
        fclose(file);
      } else {
        fprintf(stderr,"Config file not writeble ");
        fprintf(stderr,"%s \n",filename);
        fprintf(stderr,"check /etc/mythtv-controller.conf is writeble by user.");
        exit(0);
      }
    }
    remoteHost = gethostbyname(configmythhost);
    if (remoteHost) {
      addr_list = (struct in_addr **) remoteHost->h_addr_list;
      fprintf(stderr,"mediacenter server name is : %s\n", remoteHost->h_name);
      for(int i = 0; addr_list[i] != NULL; i++) {
        fprintf(stderr,"mediacenter server ip is  : %s\n", inet_ntoa(*addr_list[i]));
      }
      strcpy(confighostname,hostname);
      strcpy(configmysqlhost,remoteHost->h_name);
      strcpy(configmythhost,remoteHost->h_name);
      strcpy(configmysqlip,inet_ntoa(*addr_list[0]));			// ip adress on sql server
      remoteHost = gethostbyname(confighostname);
      if (remoteHost) {
        addr_list = (struct in_addr **) remoteHost->h_addr_list;
        fprintf(stderr,"Hostname : %s\n", remoteHost->h_name);
        for(int i = 0; addr_list[i] != NULL; i++) {
          fprintf(stderr,"Ip is  : %s\n", inet_ntoa(*addr_list[i]));
        }
        strcpy(confighostip,inet_ntoa(*addr_list[0]));
      } else strcpy(confighostip,"127.0.0.1");
    } else {
      fprintf(stderr,"Error recolving hostname.\n");
    }
    strcpy(sqlselect,"SELECT data from settings where value like 'MusicLocation' and hostname like '");
    strcat(sqlselect,configmysqlhost);
    strcat(sqlselect,"' ");
    fprintf(stderr,"start reading database setup.....\n");
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(configmusicpath,row[0]);
        // write to debug log
        strcpy(debuglogdata,"Fundet music config directory ");
        strcat(debuglogdata,row[0]);
        write_logfile((char *) debuglogdata);
      }
      //
      // hvis der ikke er fundet et dir denne mysql server med rigtigt hostname
      // load default
      //
      if (strcmp(configmusicpath,"")==0) {
        strcpy(sqlselect,"SELECT data from settings where value like 'MusicLocation'");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            strcpy(configmusicpath,row[0]);
            // write to debug log
            strcpy(debuglogdata,"Search on 'MusicLocation' give config dir ");
            strcat(debuglogdata,row[0]);
            write_logfile((char *) debuglogdata);
          }
        }
      }
      if ((strlen(configmusicpath)>0) && (configmusicpath[strlen(configmusicpath)-1]!='/')) strcat(configmusicpath,"/");             // add last '/' if not exist
      strcpy(sqlselect,"SELECT data from settings where value like 'VideoStartupDir' and hostname like '");
      strcat(sqlselect,configmysqlhost);
      strcat(sqlselect,"' ");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          strcpy(configmoviepath,row[0]);
        }
      } else {
        fprintf(stderr,"No access to mysql database for mythtv... \nCan not read config infomations from mythtv settings.\n");
        exit(-1);
      }
      //
      // hvis der ikke er fundet et dir denne mysql server med rigtigt hostname
      // load default
      //
      if (strcmp(configmoviepath,"")==0) {
        strcpy(sqlselect,"SELECT data from settings where value like 'VideoStartupDir'");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            strcpy(configmoviepath,row[0]);
            // write to debug log
            strcpy(debuglogdata,"Search on 'VideoStartupDir' give config dir ");
            strcat(debuglogdata,row[0]);
            write_logfile((char *) debuglogdata);

          }
        }
      }
      if ((strlen(configmoviepath)>0) && (configmoviepath[strlen(configmoviepath)-1]!='/')) strcat(configmoviepath,"/");
      strcpy(sqlselect,"SELECT data from settings where value like 'GalleryDir' and hostname like '");
      strcat(sqlselect,configmysqlhost);
      strcat(sqlselect,"' ");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          strcpy(configpicturepath,row[0]);
        }
      } else {
        fprintf(stderr,"No access to mysql database... Can not read GalleryDir table.\n");
        exit(-1);
      }
      if (strlen(configpicturepath)>0) strcat(configpicturepath,"/mythc-gallery/");
      // find storagegroup dirs and load them
      strcpy(sqlselect,"SELECT dirname,groupname from storagegroup where hostname like '");
      strcat(sqlselect,configmysqlhost);
      strcat(sqlselect,"' and groupname like 'Default' order by id");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        i=0;
        while ((((row = mysql_fetch_row(res)) != NULL)) && (i<storagegroupantal)) {
          if (i==0) strcpy(configrecordpath,row[0]);					                         // store fist found default path
          strcpy(configstoragerecord[i].path,row[0]);
          strcpy(configstoragerecord[i].name,row[1]);
          if ((strlen(configstoragerecord[i].path)>0) && (configstoragerecord[i].path[strlen(configstoragerecord[i].path)-1]!='/')) strcat(configstoragerecord[i].path,"/");             // add last '/' if not exist
          i++;
        }
        if ((strlen(configrecordpath)>0) && (configrecordpath[strlen(configrecordpath)-1]!='/')) strcat(configrecordpath,"/");             // add last '/' if not exist
      } else {
          fprintf(stderr,"No storagegroup table or access to mysql database... Can not read storagegroup infomations from mythtv.\n");
      }

      // er der ingen storagegroup defined. Load old type from settings table
      if (strcmp(configrecordpath,"")==0) {
        strcpy(sqlselect,"SELECT data from settings where value like 'RecordFilePrefix' and hostname like '");
        strcat(sqlselect,configmysqlhost);
        strcat(sqlselect,"' ");
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            strcpy(configrecordpath,row[0]);
            if ((strlen(configrecordpath)>0) && (configrecordpath[strlen(configrecordpath)-1]!='/')) strcat(configrecordpath,"/");             // add last '/' if not exist
          }
        } else {
          fprintf(stderr,"No access to settings table searching for RecordFilePrefix in mysql database... Can not read config infomations from mythtv.\n");
          exit(-1);
        }
        if (conn) mysql_close(conn);
      }
    } else {
      strcpy(configmusicpath,"");
      fprintf(stderr,"No access to mysql database... Can not read config infomations.\n\nUse setup (F1) to config mythtv sql access.\n");
    }
    // read key file setup
    if ((file = fopen("mythtv-controller.keys", "r"))) {
      if (!(feof(file))) {
        fread(configkeyslayout,sizeof(configkeytype)*12,1,file);
        fclose(file);
      }
    } else {
      file = fopen("mythtv-controller.keys", "w");
      if (file) {
        fwrite(configkeyslayout,sizeof(configkeytype)*12,1,file);
        fclose(file);
      } else fprintf(stderr,"Disk write error, saving mythtv-controller.keys\n");
    }
    #ifdef ENABLE_SPOTIFY
    // get/set default play device on spotify
    if (strcmp(spotify_oversigt.active_default_play_device_name,"")!=0) {
      sprintf(sqlselect,"SELECT device_name,intnr from spotify_device where device_name like '%s'",spotify_oversigt.active_default_play_device_name);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        spotify_oversigt.active_default_play_device=atoi(row[1]);
        spotify_oversigt.active_spotify_device=atoi(row[1]);
        fprintf(stderr,"Set Default spotify device as %s device # %d\n",spotify_oversigt.active_default_play_device_name,spotify_oversigt.active_default_play_device);
      } else {
        fprintf(stderr,"No default spotify device found \n");
        spotify_oversigt.active_default_play_device=-1;
        spotify_oversigt.active_default_play_device=-1;
      }
    }
    #endif
    if (conn) mysql_close(conn);
}


// ****************************************************************************************
//
// find sangs id og opdateret antal af afspilinger samt afspilings dato
//
// ****************************************************************************************

int update_afspillinger_music_song(char *filename)

{
    char sqlselect[1024];
    char songname[1024];
    char *pathpointer;
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    MYSQL *conn;
    MYSQL_RES *res;
    //MYSQL_ROW row;
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    pathpointer=strrchr(filename,'/');
    if (pathpointer) {
      strcpy(songname,pathpointer+1);		// hentfilename efter path (sidste '/')
    } else {
      strcpy(songname,filename);		//
    }
    sprintf(sqlselect,"update music_songs set music_songs.lastplay=NOW(), music_songs.numplays=music_songs.numplays+1 where music_songs.filename like '%s' limit 1",songname);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    mysql_close(conn);
    return(1);
}




// ****************************************************************************************
//
// parameret til mysql er dirid = directory_id i database
// retunere path og parent_id som bliver udfyldt fra mysql databasen
//
// ****************************************************************************************

void hent_dir_id_info(char *path,char *parent_id,char *dirid) {
    // mysql stuf
    char database[256];
    char sqlselect[256];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    strcpy(sqlselect,"select parent_id,path from music_directories where directory_id=");
    strcat(sqlselect,dirid);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(parent_id,row[0]);
        strcpy(path,row[1]);
      }
    }
    mysql_close(conn);
}


// ****************************************************************************************
//
// music play list loader fra mythtv
//
// load playlist til oversigt ask_open_dir_play
//
// ****************************************************************************************

int hent_mythtv_playlist(int playlistnr) {
    GLuint texture; //The id of the texture
    char sqlselect[1024];
    char tmptxt[512];
    char tmptxt1[512];
    char tmptxt2[512];
    char tmptxt3[512];
//    char husk_tmptxt3[512];
    char parent_id[512];
    char songid[20];
    char artistid[20];
    char albumname[40];
    char songname[80];
    char artistname[80];
    char songlength[20];
//    GLuint textureId;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1;
    MYSQL_ROW row,row1;
    int songnr;
    int error=0;
    bool finish=0;
    // mysql stuf
    char database[255];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    // write to debug log
    sprintf(debuglogdata,"Loading info about playlist nr: %d ",playlistnr);
    write_logfile((char *) debuglogdata);
    songnr=1;
    aktiv_playlist.clean_playlist();		// clear old playlist
    conn=mysql_init(NULL);
    int songintnr;				// sang nr som skal i playliste
    long songantal=0;				// antal sange i array i database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    while(!(finish)) {
      // select sange fra myhthtv playlist
      sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,' ',%d),' ',-1) as songs,songcount FROM music_playlist where playlist_id=%d",songnr,playlistnr);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          songintnr=atoi(row[0]);
          songantal=atol(row[1]);
          fprintf(stderr,"Song antal fundet = %ld \n",songantal);
          // find cd cover samt sange info i mythtv music database
          sprintf(sqlselect,"select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where song_id=%d and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id",songintnr);
          mysql_query(conn,sqlselect);
          res1 = mysql_store_result(conn);
          if ((res1) && (songantal>0)) {
            while ((row1 = mysql_fetch_row(res1)) != NULL) {
              strcpy(songid,row1[0]);
              strcpy(artistid,row1[5]);
              strcpy(albumname,row1[3]);
              strcpy(songname,row1[4]);
              strcpy(artistname,row1[6]);
              strcpy(songlength,row1[7]);

              // write to debug log
              sprintf(debuglogdata,"Fundet sang song_id=%s artist id=%s filename=%40s",songid,row1[5],row1[1]);
              write_logfile((char *) debuglogdata);

              strcpy(tmptxt,configmusicpath);		                           // start path
              sprintf(tmptxt2,"%s",row1[2]);			                         // hent dir id
              hent_dir_id_info(tmptxt1,parent_id,tmptxt2);		             // hent path af tmptxt2 som er = dir_id
              strcat(tmptxt,tmptxt1);				                               // add path
              strcat(tmptxt,"/");
              strcpy(tmptxt3,tmptxt);			                                 // er = path
              strcat(tmptxt3,"mythcFront.jpg");		                         // add filename til cover
              strcat(tmptxt,row1[1]);				                               // add filename til sang
              strcpy(tmptxt,row1[1]);				                               // add filename til sang
              if (file_exists(tmptxt3)) {
                texture=loadTexture(tmptxt3);				                       // load texture
              } else {
                fprintf(stderr," Error loading texture file : %s \n",tmptxt3);
                texture=0;
              }
            }
            aktiv_playlist.m_add_playlist(tmptxt,songid,artistid,albumname,songname,artistname,songlength,0,texture);	// add (gem) info i playlist
          } else {
            finish = true;
            error=1;
          }
        }
        songnr++;
      }
      if ((res==0) || (songnr==songantal)) finish = true;
      if (songantal==1) finish = true;
    }
    mysql_close(conn);
    if ((finish) && (error==0)) return(songantal); else return(0);
}






// ****************************************************************************************
//
// MUSIC stuf
// Can have memory error
// load dir icons after update.
//
// ****************************************************************************************

/*
void opdatere_music_oversigt_icons() {
  unsigned int i;
  char tmpfilename[200];
  for(i=0;i<MUSIC_OVERSIGT_TYPE_SIZE;i++) {
    musicoversigt[i].textureId=0;
  }
  i=0;
  while (i<MUSIC_OVERSIGT_TYPE_SIZE) {
    strncpy(tmpfilename,musicoversigt[i].album_coverfile,200);
    if ((strcmp(tmpfilename,"")!=0) && (file_exists(tmpfilename))) {
      // load covers file into opengl as textures (png/jpg)
      musicoversigt[i].textureId = loadTexture((char *) tmpfilename);
    } else {
      musicoversigt[i].textureId=0;
    }
    i++;
  }
}
*/

// ****************************************************************************************
//
// hent antal af songs fra mythtv playlist database og fyld music play array
//
// ****************************************************************************************

unsigned int hent_antal_dir_songs_playlist(int playlistnr) {
    char sqlselect[512];
    long i=0;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1;
    MYSQL_ROW row;
    // mysql stuf
    char database[256];
    bool finish = false;
    char songid[20];
    char artistid[20];
    char albumname[40];
    char songname[80];
    char artistname[80];
    char songlength[20];
    int songintnr;				// sang nr som skal i playliste
    long songantal=0;				// antal sange i array i database
    int songnr=1;
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    if (debugmode & 2) {
      fprintf(stderr,"Henter info om playlistnr = %d \n",playlistnr);
      sprintf(debuglogdata,"Henter info om playlistnr = %d ",playlistnr);
      write_logfile((char *) debuglogdata);
    }
    dirmusic.emtydirmusic();
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    while(!(finish)) {
      sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,',',%d),',',-1) as songs,songcount FROM music_playlist where playlist_id=%d",songnr,playlistnr);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          //                if (debugmode & 2) printf("Fundet recnr (songnr) %s \n",row[0]);
          songintnr=atoi(row[0]);
          songantal=atoi(row[1]);
          // find cd cover samt sange info i mythtv music database
          sprintf(sqlselect,"select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where song_id=%d and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id order by name",songintnr);
          mysql_query(conn,sqlselect);
          res1 = mysql_store_result(conn);
          //              if (debugmode & 2) printf("Hentet music nummer = %i ,add song %s to liste\n",i,songname);
          if (res1) {
            while ((row = mysql_fetch_row(res1)) != NULL) {
              strcpy(songid,row[0]);
              strcpy(artistid,row[5]);
              strcpy(albumname,row[3]);
              strcpy(songname,row[4]);
              strcpy(artistname,row[6]);
              strcpy(songlength,row[7]);
              dirmusic.pushsong(songname,artistid);
              i++;
            }
          }
        } // end while
      } // endif
      songnr++;
      if (i==songantal) finish = true;
  } // endwhile
  mysql_close(conn);
  return(songnr);		// antal sange fundet i dir id
}



// ****************************************************************************************
//
// hent antal af songs i et dir (dirid) og fyld music play array fra dir path
//
// ****************************************************************************************

unsigned int hent_antal_dir_songs(int dirid) {
    char tmpfilename[200];
    char sqlselect[512];
    char tmptxt[200];
    unsigned int i,ii;
    GLuint textureId;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    // write debug log
    sprintf(debuglogdata,"Hent info om directory_id = %d ",dirid);
    write_logfile((char *) debuglogdata);
    dirmusic.emtydirmusic();
    strcpy(sqlselect,"SELECT song_id,name,artist_id FROM music_songs where directory_id=");
    sprintf(tmptxt,"%d order by name limit %d",dirid,dirliste_size);
    strcat(sqlselect,tmptxt);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    i=0;
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dirmusic.pushsong(row[1],row[2]);
        i++;
      }
    }
    // hent aktiv dir id info
    strcpy(sqlselect,"SELECT path FROM music_directories where directory_id=");
    sprintf(tmptxt,"%d limit 1",dirid);
    strcat(sqlselect,tmptxt);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        if (global_use_internal_music_loader_system) strcpy(tmpfilename,configdefaultmusicpath); else strcpy(tmpfilename,configmusicpath);
        sprintf(tmptxt,"%s",row[0]);
        strcat(tmpfilename,tmptxt);
        strcat(tmpfilename,"/mythcFront.jpg");
        if (file_exists(tmpfilename)) {
          dirmusic.textureId=loadTexture((char *) tmpfilename);
        } else {
          dirmusic.textureId=0;
        }
      }
    }
    // hent dirs info til visning samt covers data i 3d format
    strcpy(sqlselect,"SELECT directory_id,path FROM music_directories where parent_id=");
    sprintf(tmptxt,"%d limit 100",dirid);
    strcat(sqlselect,tmptxt);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    ii=0;
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dirmusic.pushdir(row[1],row[0]);
        if (global_use_internal_music_loader_system) strcpy(tmpfilename,configdefaultmusicpath); else strcpy(tmpfilename,configmusicpath);
        sprintf(tmptxt,"%s",row[1]);
        strcat(tmpfilename,tmptxt);
        strcat(tmpfilename,"/mythcFront.jpg");
        if (file_exists(tmpfilename)) {
          textureId = loadTexture((char *) tmpfilename);		// load texture to opengl
        } else {
          //                printf("Music cover file load error \n");
          textureId=0;
        }
        dirmusic.settexture(textureId,ii);				// set directory texture
        ii++;
      }
    }
    mysql_close(conn);
    return(i);		// antal sange fundet i dir id
}



// ****************************************************************************************
//
// init lirc
// remove controler
//
// ****************************************************************************************

int initlirc() {
  // LIRC SETUP
  int flags;
  sock=lirc_init((char *) "mythtv-controller",1);                  // print error to stderr
  if (sock!=-1) {
    if (lirc_readconfig("~/.config/lirc/mythtv-controller.lircrc",&lircconfig,NULL)!=0) {
      // write debug log status for remote controller is loaded
      write_logfile((char *) "No lirc mythtv-controller config file found.");
      lirc_deinit();
      sock=-1;                                              				// lirc error code
    } else {
      // fjern vente tid fra lirc getchar
      fcntl(sock,F_SETOWN,getpid());
      flags=fcntl(sock,F_GETFL,0);
      if(flags!=-1) {
        fcntl(sock,F_SETFL,flags|O_NONBLOCK);
      }
      fprintf(stderr,"Lirc mythtv-controller info loaded.\n");
    }
    fprintf(stderr,"Remote control init done ok (config file found at %s.\n","~/.config/lirc/mythtv-controller.lircrc");
    return(sock);
  } else {
    // no lirc support error
    fprintf(stderr,"Remote control not found...\n");
    fprintf(stderr,"No lirc. (No remote control support)\n");
  }
  return(-1);
}



// ****************************************************************************************
//
// Load/init ttf fonts list
//
// ****************************************************************************************

int init_ttf_fonts() {
    // uni font config *****************************************************************************************
    ctx = glcGenContext();
    glcContext(ctx);
    // *********************************************************************************************************
    static GLint glc_font_id;
    GLint count,ii;
    // Get the number of entries in the catalog list
    /* Get a unique font ID. */
    glc_font_id = glcGenFontID();
    // add dir for fonts
    glcAppendCatalog("/usr/share/fonts/truetype");
    myFont = glcGenFontID();
    glcNewFontFromFamily(myFont, configfontname);                                       // Droid Serif,UbuntumFreeMono
    glcFontFace(myFont, "Bold");
    glcFont(myFont);
    aktivfont.updatefontlist();                                                          // update font list
    /* Draw letters as filled polygons. */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Get the number of entries in the catalog list
    count = glcGeti(GLC_CATALOG_COUNT);
    // Print the path to the catalog
    for (ii = 0; ii < count; ii++) {
      fprintf(stderr,"Font found in directory %s\n", (char *) glcGetListc(GLC_CATALOG_LIST, ii));
    }
    // note FROM quesoglc-doc-0.7.0 DOC.
    // If the rendering style of the text is not GLC_BITMAP, then you should use glTranslate() and
    // glScale() instead of glRasterPos() and glcScale() (p. 64).
    glcDisable(GLC_GL_OBJECTS);
    //glcRenderStyle(GLC_LINE);                 // lines
    //glcRenderStyle(GLC_TEXTURE);
    glcRenderStyle(GLC_TRIANGLE);               // filled                       // NORMAL used in mythtv-controller
    //glcRenderStyle(GLC_BITMAP);
    glcStringType(GLC_UTF8_QSO);
    //glcEnable(GLC_HINTING_QSO);
    return(1);
}


// ****************************************************************************************
//
// save command to be exectued to script exec
//
// ****************************************************************************************

int saveexitcommand(configkeytype command) {
  FILE *file;
  file=fopen("mythtv-controller.cmd","w");
  if (file) {
    fputs(command.cmdname,file);
    fputs(";",file);
    fprintf(file,"%d",command.scrnr);
    fputs("\n",file);
    fclose(file);
    return(1);
  } else return(0);
}


// ****************************************************************************************
//
// do shell command from functions key pressed
//
// ****************************************************************************************

void doexitcommand() {
  char command[1024];
  FILE *file;
  bool ok=false;
  file=fopen("mythtv-controller.cmd","r");
  if (file) {
    fgets(command,1023,file);
    fclose(file);
    if (command!=NULL) ok=true;
    fprintf(stderr,"Command to run %s \n",command);
  }
  if (ok) system(command);
}

// ****************************************************************************************
//
// find sangs id til playlist opdatering/restore
// returnere songid fra music_songstablen
//
// ****************************************************************************************

long find_music_song_placering(char *filename,char *songname) {
    char sqlselect[1024];
    char *database = (char *) "mythconverg";
    long recnr=0;
    bool fundet = false;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    sprintf(sqlselect,"select song_id from music_songs where filename like '%s' and name like '%s'",filename,songname);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        recnr=atol(row[0]);
        fundet = true;						// sang fundet i music_songs
      }
    }
    mysql_close(conn);
    return(recnr);
}

// ****************************************************************************************
//
// restore database til mythtv music playlist datbase
//
// ****************************************************************************************

unsigned int do_playlist_restore_playlist() {
//    long playlistnr;
    long songplacering;
    char sqlselect[8192];
    long i;
    long songlength;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1,*res2;
    MYSQL_ROW row,row1;
    // mysql stuf
    const char *database = (char *) "mythconverg";
    bool fundet;
    char playlistname[512];
    fprintf(stderr,"Restore music from playlist backup \n");
    i=0;
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);

    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,"select * from music_songs_tmp");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(playlistname,row[1]);				// hent playlist name
        fundet = false;
        while(!(fundet)) {
          sprintf(sqlselect,"select * from music_playlist where playlist_name like '%s'",playlistname);
          mysql_query(conn,"set NAMES 'utf8'");
          res1 = mysql_store_result(conn);
          mysql_query(conn,sqlselect);
          res1 = mysql_store_result(conn);
          if (res1) {
            while ((row1 = mysql_fetch_row(res1)) != NULL) {
              if (fundet==false) {
                fprintf(stderr,"Found playlist %s updating ",playlistname);
                sprintf(sqlselect,"update music_playlists set music_playlist.playlist_songs='', music_playlist.length=0, music_playlist.songcount=0 where playlist_id=%s",row1[0]);
                mysql_query(conn,"set NAMES 'utf8'");
                res2 = mysql_store_result(conn);
                mysql_query(conn,sqlselect);
                res2 = mysql_store_result(conn);
                // missing Check done
              }
              fundet = true;
              // find song in music database
              songplacering=find_music_song_placering(row[1],row[2]);				// get song recnr
              songlength=atol(row[8]);							// get song length
              // if song exist update playlist
              if ( songplacering > 0 ) {
                fprintf(stderr,".");
                sprintf(sqlselect,"update music_playlists set music_playlist.playlist_songs=concat(playlist_songs,',%ld'), music_playlist.songcount=music_playlist.songcount+1, music_playlist.length=music_playlist.length+%s where music_playlist.playlist_id=%ld",songplacering,row1[0],songlength);
                mysql_query(conn,"set NAMES 'utf8'");
                res2 = mysql_store_result(conn);
                mysql_query(conn,sqlselect);
                res2 = mysql_store_result(conn);
              } // endif
            } // endwhile
          } // endif
          fprintf(stderr,"\n");
          if (!( fundet )) {
            // else create new playlist
            fprintf(stderr,"Create new playlist %s......\n",playlistname);
            songplacering=find_music_song_placering(row[1],row[2]);
            sprintf(sqlselect,"insert into music_playlists values (0,'%s','%ld','','%s',1,'')",playlistname,songplacering,row[8]);
            mysql_query(conn,"set NAMES 'utf8'");
            res2 = mysql_store_result(conn);
            mysql_query(conn,sqlselect);
            res2 = mysql_store_result(conn);
          }
        } // endwhile
      } // endwhile
      // delete tmp tabel again
      mysql_query(conn,"set NAMES 'utf8'");
      res2 = mysql_store_result(conn);
      mysql_query(conn,"drop table music_songs_tmp");
      res2 = mysql_store_result(conn);
    } // endif
    printf("Finish\n");
    mysql_close(conn);
    return(1);		// exit all ok
}


// ****************************************************************************************
//
// lav en backup af sange i playlister
// til en table ved navn music_songs_tmp
//
// ****************************************************************************************

unsigned int do_playlist_backup_playlist() {
    int playlistnr;
    char sqlselect[8192];
    long i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1,*res2,*res3;
    MYSQL_ROW row,row3;
    // mysql stuf
    const char *database = (char *) "mythconverg";
    bool finish = false;
    char playlistname[256];
//    GLuint textureId;
    int songintnr;				// sang nr som skal i playliste
    long songantal=0;				// antal sange i array i database
    int songnr=1;
    if (debugmode & 2) fprintf(stderr,"Gemmer sange fra playlister \n");
    i=0;
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    // drop old table
    mysql_query(conn,"drop table music_songs_tmp");
    res2 = mysql_store_result(conn);
    // Create temp table to backup of songs in playlist
    strcpy(sqlselect,"create table music_songs_tmp(playlist_id int not null AUTO_INCREMENT,PRIMARY KEY(playlist_id), playlist_name varchar(255), filename text, song_name varchar(255), artist_name varchar(255), album_name varchar(255), genre_name varchar(255), year int, length int,numplays int, rating int, lastplayed datetime,date_entered datetime, date_modified datetime, format varchar(4), size int, descrition varchar(255), comment varchar(255), disc_count int, disc_number int, track_count int, start_time int, stop_stime int,eq_preset varchar(255), relative_volume int, sample_rate int, bitrate int, bpm int,  directory_name varchar(255))");
    mysql_query(conn,sqlselect);
    res2 = mysql_store_result(conn);
    if (!( res2 )) {
        fprintf(stderr,"NO create temp database\n ");
        return(0);
    }
    // Hent playlistes med sange tilknyttet elementer i playlist_songs array
    strcpy(sqlselect,"select playlist_id,playlist_name from music_playlist where playlist_songs!=''");
    mysql_query(conn,sqlselect);
    res3 = mysql_store_result(conn);
    if ( res3 ) {
      while ((row3 = mysql_fetch_row(res3)) != NULL) {
        playlistnr=atol(row3[0]);							// hent playlist nr
        strcpy(playlistname,row3[1]);						// hent playlistname
        finish = false;
        fprintf(stderr,"Save music info from playlist name:%s \n",playlistname);
        i=0;
        while(!(finish)) {
          sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,',',%d),',',-1) as songs,songcount FROM music_playlist where playlist_id=%d",songnr,playlistnr);
          mysql_query(conn,"set NAMES 'utf8'");
          res = mysql_store_result(conn);
          mysql_query(conn,sqlselect);
          res = mysql_store_result(conn);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              songintnr=atoi(row[0]);
              songantal=atoi(row[1]);					// hent antal sange i playlist
              // find cd cover samt sange info i mythtv music database om denne aktive sang
              sprintf(sqlselect,"select song_id, filename, music_albums.album_name, name, music_artists.artist_name, music_genres.genre, length, numplays, rating, lastplay, date_entered, date_modified, music_directories.path,music_songs.year  from music_songs,music_artists,music_albums,music_genres,music_directories where song_id=%d and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id and music_songs.genre_id=music_genres.genre_id and music_songs.directory_id=music_directories.directory_id",songintnr);
              mysql_query(conn,sqlselect);
              res1 = mysql_store_result(conn);
              // write debug log
              sprintf(debuglogdata,"Playlist %s Hentet music nummer = %ld af %ld",playlistname,i,songantal);
              write_logfile((char *) debuglogdata);
              if (res1) {
                while ((row = mysql_fetch_row(res1)) != NULL) {
                  sprintf(sqlselect,"insert music_songs_tmp values (0,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%d','%s','%s','%d','%d','%d','%d','%d','%s','%d','%d','%d','%d','%s')",
                  playlistname,row[1],row[3],row[4],row[2],row[5],row[13],row[6],row[7],row[8],row[11],row[12],row[13],row[14],0,"","", 0, 0, 0, 0, 0,"",0,0,0,0,row[12]);
                  mysql_query(conn,sqlselect);
                  res2 = mysql_store_result(conn);
                  //printf("sql %s\n",sqlselect);
                }
              }
              songnr++;
            } // endwhile
          } // endif
          i++;
          if (i>songantal) finish = true;
        } // endwhile
      } // endwhile
    } // endif
    mysql_close(conn);
    return(songnr);		// antal sange fundet i dir id
}


// ****************************************************************************************
//
// Bruges af ur pause display
//
// ****************************************************************************************

void myglprint24(char *string) {
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}

// ****************************************************************************************
//
// show background picture
//
// ****************************************************************************************

void show_background() {
  // make background
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  // glBlendFunc(GL_ONE, GL_ONE);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
  if ( screen_size != 4 ) {
    if (vis_music_oversigt) glBindTexture(GL_TEXTURE_2D, _textureIdback_music);					// background picture
    else if (do_show_setup) glBindTexture(GL_TEXTURE_2D, _textureIdback_setup);
    else if (vis_radio_oversigt) glBindTexture(GL_TEXTURE_2D, _textureIdback_music);
    else glBindTexture(GL_TEXTURE_2D, _textureIdback_other);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 1280.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1920.0, 1280, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1920.0, 0.0, 0.0);
    glEnd();
  }
  glPopMatrix();
}




// ****************************************************************************************
//
// clock stuf
//
// ****************************************************************************************

const float clockR=80.0f;

void newLine(float rStart, float rEnd, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  glVertex2f((clockR*rStart*c)+(screenx/2),(clockR*rStart*s)+(screeny/2));
  glVertex2f((clockR*rEnd*c)+(screenx/2),(clockR*rEnd*s)+(screeny/2));
}

// ****************************************************************************************
//
// startup parameters
// we have to start spotify to enable log from web
//
// ****************************************************************************************

static bool do_update_xmltv_show = false;                           // Show update xml tv
static bool do_update_rss_show  = false;                            //
static bool do_update_rss       = false;                            // Show update rss
static bool do_update_music     = false;                            // Show update music Library
// do stuff
static bool do_update_xmltv     = false;                            // Start update
static bool do_update_music_now = false;                            // start the process to update music db from global dir
static bool do_update_moviedb   = false;                            // set true to start thread on update movie db
static bool do_update_spotify   = true;                             // set true to start thread on update spotify + run web server



// ****************************************************************************************
//
// Main diskplay
//
// ****************************************************************************************



void display() {
    static picture_saver *psaver=NULL;

    // used by xmltv updater func
    static bool startwebbrowser=true;                                           // start web browser to enable login to spotify
    static bool getstarttidintvguidefromarray = true;
    static time_t today=0;
    static time_t lasttoday=0;
    static bool firsttime_rssupdate = false;                                    // only used first time
    static int starttimer=0;                                                    // show logo timeout
    bool do_play_music_aktiv_nr_select_array[1000];                             // array til at fortælle om sange i playlist askopendir er aktiv
    char temptxt[200];
    char temprgtxt[2000];
    int i;
    struct tm *timeinfo;
    float mgrader,tgrader;
    static time_t rawtime,rawtime1=0;
    int min,tim;
    static bool remove_log_file = true;
    int sounderrflag;                           // husk last sound system status
    int numtags, numtagsupdated, count;         // bliver brugt til at vise stream tags
    GLuint textureId;
    static int show_newmovietimeout=120*10;
    bool radio_pictureloaded;
    unsigned int lenbytes, kbps;
    unsigned int playtime,playtime_min,playtime_sec,playtime_hour;
    unsigned int playtime_songlength,playtime_length_min,playtime_length_sec;
    // radio
    unsigned int radio_playtime,radio_playtime_min,radio_playtime_sec,radio_playtime_hour;
    unsigned int radio_playtime_songlength,radio_playtime_length_min,radio_playtime_length_sec;
    float fontsize=4.2f;                        // font size in show radio info
    /* These are to calculate our fps */
    // int fps;
    static GLint T0     = 0;
    static GLint Frames = 0;
    //static bool firsttime_xmltvupdate=true;     // global for now
    int no_open_dir=0;
    char temptxt1[200];
    int xof=10;
    int yof=10;
    int	buttonsize=200;
    int ofset;
    int dirmusiclistemax;
    bool aktiv;
    char *pos;                          // temp pointer
    char strhour[20];
    char strmin[20];
    int sampleSize = 64;
    float *spec;
    float *spec2;
    float *specLeft, *specRight;
    bool startup = true;
    // uv color table
    static int tmpcounter=0;
    // fade colors (over time) for clock
    const float analogclock_color_table[]={1.0,1.0,1.0, \
                                     0.8,0.8,0.8, \
                                     0.6,0.6,0.6, \
                                     0.4,0.4,0.4, \
                                     0.2,0.2,0.2, \
                                     0.2,0.2,0.2, \
                                     0.4,0.4,0.4, \
                                     0.6,0.6,0.6, \
                                     0.8,0.8,0.8, \
                                     0.9,0.9,0.9, \
                                     1.0,1.0,1.0};
    const float uvcolortable[]={0.0,0.8,0.8, \
                          0.2,0.8,0.8, \
                          0.3,0.7,0.7, \
                          0.3,0.7,0.7, \
                          0.4,0.6,0.6, \
                          0.4,0.6,0.6, \
                          0.5,0.5,0.5, \
                          0.5,0.5,0.5, \
                          0.6,0.4,0.4, \
                          0.6,0.4,0.4, \
                          0.7,0.3,0.3, \
                          0.7,0.3,0.3, \
                          0.9,0.0,0.0, \
                          0.9,0.0,0.0};
    // uv color table 2
    const float uvcolortable2[]={0.8,0.8,0.8, \
                          0.8,0.8,0.8, \
                          0.7,0.7,0.7, \
                          0.7,0.7,0.7, \
                          0.6,0.6,0.6, \
                          0.6,0.6,0.6, \
                          0.5,0.5,0.5, \
                          0.5,0.5,0.5, \
                          0.4,0.4,0.4, \
                          0.4,0.4,0.4, \
                          0.3,0.3,0.3, \
                          0.3,0.3,0.3, \
                          0.1,0.1,0.1, \
                          0.0,0.0,0.0};
    // uv color table 2
    float uvcolortable1[]={0.8,0.8,0.8, \
                          0.8,0.4,0.8, \
                          0.7,0.4,0.7, \
                          0.7,0.4,0.7, \
                          0.6,0.4,0.6, \
                          0.6,0.4,0.6, \
                          0.5,0.4,0.0, \
                          0.5,0.4,0.0, \
                          0.4,0.4,0.4, \
                          0.4,0.4,0.4, \
                          0.3,0.3,0.3, \
                          0.3,0.3,0.3, \
                          0.8,0.1,0.1, \
                          0.8,0.1,0.1};



    clock_t start;

    static int barantal=45;
    struct timeb tb;
    struct tm* t;
    float clockVol=1000.0f, angle1min = M_PI / 30.0f,  minStart=4.9f,minEnd=5.0f, stepStart=4.8f,stepEnd=5.0f;
    static float angleHour = 0,angleMin  = 0,angleSec  = 0;
    static float last_angleSec=0.0f;
    bool clock_udpate;
    static int lastsec=0;
    int winsizx=1920;
    int winsizy=1080;
    static bool firsttime = true;                                                 // gen first time clock
    static bool clock_gluptimetime = false;
    static GLuint index;
    static int lastohur;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static int xrand=0;
    static int yrand=0;
    static int do_we_play_check=0;
    //glLoadIdentity();
    rawtime=time(NULL);                                 // hent now time
    int savertimeout=atoi(configscreensavertimeout);
    if ((rawtime1==0) || (saver_irq)) {                 // ur timer
      rawtime1=rawtime+(60*savertimeout);             // x minuter hentet i config
      visur = false;                                    // if (debug) printf("Start screen saver timer.\n");
      saver_irq = false;                          // start screen saver
    }
    timeinfo = localtime(&rawtime);
    strftime(strhour,20,"%I",timeinfo);
    strftime(strmin,20,"%M",timeinfo);
    min=atoi(strmin);
    tim=atoi(strhour);
    // update clock
    if (rawtime>rawtime1) {
      visur = true;
    }
    // make xmltv update
    today=time(NULL);
    t=localtime(&today);                                                        // local time
    ftime(&tb);
    clock_udpate = false;
    // used by analog clock
    if (lastsec>=20) {
      clock_udpate = true;
      lastsec=0;
    }
    lastsec++;
    if (clock_udpate) {
      angleSec=(float)(t->tm_sec+ (float)tb.millitm/1000.0f)/30.0f * M_PI;
      angleMin = (float)(t->tm_min)/30.0f * M_PI + angleSec/60.0f;
      angleHour = (float)(t->tm_hour > 12 ? t->tm_hour-12 : t->tm_hour)/6.0f * M_PI+angleMin/12.0f;
    }
    clock_udpate = false;
    // gvguide update
    // update interval
    // set in main.h
    if (((lasttoday+(doxmltvupdateinterval)<today) && (do_update_xmltv==false)) || (firsttime_xmltvupdate)) {
      write_logfile((char *) "start timer xmltvguide update process.");
      lasttoday = today;                                                          // rember last update
      do_update_xmltv = true;                                                     // do update tvguide
      do_update_xmltv_show = true;                                                // show we are updating
      firsttime_xmltvupdate=false;                                                // only used first time
    }
    // rss update
    // update interval
    // set in main.h
    if (((lasttoday+(dorssupdateinterval)<today) && (do_update_rss==false)) || (firsttime_rssupdate)) {
      write_logfile((char *) "start timer rss update process.");
      lasttoday = today;                                                          // rember last update
      do_update_rss = true;                                                       // do update rss
      do_update_rss_show = true;                                                  // show we are updating rss
      firsttime_rssupdate=false;                                                // only used first time
    }
    //
    // do the update from spotify
    //
    #ifdef ENABLE_SPOTIFY
    if ((do_update_spotify_playlist) && ((do_update_rss==false) && (do_update_xmltv==false))) {
      write_logfile((char *) "Start spotify update thread");
      update_spotifyonline_phread_loader();                                     // start thread loader
      do_update_spotify_playlist=false;
    }
    #endif
    glPushMatrix();
    // background picture
    if ((!(visur)) && (_textureIdback_music) && (_textureIdback_main) && (!(vis_radio_oversigt)) && (!(vis_stream_oversigt)) && (!(vis_spotify_oversigt)) && (!(vis_tv_oversigt))) show_background();
    //visur=1;
    if (visur) {
      glPushMatrix();
      switch (urtype) {
        case DIGITAL:
            // time
            glPushMatrix();
            glDisable(GL_TEXTURE_2D);
            if (tmpcounter>200) {
              tmpcounter = 0;
              xrand = (rand()/10240)/300;
              yrand = (rand()/10240)/480;
            }
            tmpcounter++;
            strftime(temptxt, 20, "%H:%M", timeinfo);
            glTranslatef(xrand+70, yrand, 0.0f);
            glRasterPos2f(0.0f, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glScalef(70.0, 70.0, 1.0);
            glcRenderString(temptxt);
            glPopMatrix();
            // date
            glPushMatrix();
            glDisable(GL_TEXTURE_2D);
            strftime(temptxt, 80, "%d %B %Y", timeinfo);
            //glTranslatef(((orgwinsizex/2)-(strlen(temptxt)*8))-xrand, ((orgwinsizey/2)-80)-yrand, 0.0f);
            glTranslatef(xrand, yrand-(strlen(temptxt)*5), 0.0f);
            glRasterPos2f(0.0f, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glScalef(40.0, 40.0, 1.0);
            glcRenderString(temptxt);
            glPopMatrix();
            break;
        case ANALOG:
        case MUSICMETER:
            // create one display list
            if (((snd==0) && (visur==MUSICMETER)) || (urtype==ANALOG)) {
              if (firsttime) {
                index = glGenLists(1);
                lastohur=t->tm_hour;
              }
              if (t->tm_hour>lastohur) {
                clock_gluptimetime = true;
                lastohur = t->tm_hour;
                // delete old display list and create new
                glDeleteLists(index,1);
                index = glGenLists(1);
              }
              //glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
              // create display list index
              if ((firsttime) || (clock_gluptimetime)) {
                firsttime = false;
                clock_gluptimetime = false;
                // compile the display list, store a triangle in it
                glNewList(index, GL_COMPILE);
                // background
                glPushMatrix();
                glEnable(GL_TEXTURE_2D);
                glTranslatef(0.0f, 0.0f, 0.0f);
                // switch color over night
                switch(t->tm_hour) {
                  case 20:glColor4f(analogclock_color_table[0],analogclock_color_table[1],analogclock_color_table[2],1.0f);
                          break;
                  case 21:glColor4f(analogclock_color_table[3],analogclock_color_table[4],analogclock_color_table[5],1.0f);
                          break;
                  case 22:glColor4f(analogclock_color_table[6],analogclock_color_table[7],analogclock_color_table[8],1.0f);
                          break;
                  case 23:glColor4f(analogclock_color_table[9],analogclock_color_table[10],analogclock_color_table[11],1.0f);
                          break;
                  case 24:glColor4f(analogclock_color_table[12],analogclock_color_table[13],analogclock_color_table[14],1.0f);
                          break;
                  case 01:glColor4f(analogclock_color_table[15],analogclock_color_table[16],analogclock_color_table[17],1.0f);
                          break;
                  case 02:glColor4f(analogclock_color_table[18],analogclock_color_table[19],analogclock_color_table[20],1.0f);
                          break;
                  case 03:glColor4f(analogclock_color_table[21],analogclock_color_table[22],analogclock_color_table[23],1.0f);
                          break;
                  case 04:glColor4f(analogclock_color_table[24],analogclock_color_table[25],analogclock_color_table[26],1.0f);
                          break;
                  case 05:glColor4f(analogclock_color_table[27],analogclock_color_table[28],analogclock_color_table[29],1.0f);
                          break;
                  case 06:glColor4f(analogclock_color_table[30],analogclock_color_table[31],analogclock_color_table[32],1.0f);
                          break;
                  default:
                          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                          break;
                }
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glBindTexture(GL_TEXTURE_2D,analog_clock_background);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
                glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
                glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
                glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
                glEnd();
                glPopMatrix();
                // end display list
                glEndList();
              } else {
                // Call display list to show object
                // show clock static background
                glCallList(index);
              }
              // make the analog clock
              glTranslatef(0.0f, 0.0f, 0.0f);
              glDisable(GL_TEXTURE_2D);
              glLineWidth(5.0f);
              glBegin(GL_LINES);
              for(i=0; i<60; i++) {
                if(i%5) {                                                         // normal minute
                  if(i%5 == 1) glColor3f(1.0f, 1.0f, 1.0f);
                  newLine(minStart, minEnd, i*angle1min);
                } else {
                  glColor3f(1.0f, 0.0f, 0.0f);
                  newLine(stepStart, stepEnd, i*angle1min);
                }
              }
              glEnd();
              glLineWidth(5.0f);
              glBegin(GL_LINES);
              newLine(-0.2f, 3.5f, -angleHour+M_PI/2);                       // timer
              newLine(-0.2f, 4.5f, -angleMin+M_PI/2);                        // min
              glEnd();
              glLineWidth(3.0f);
              glColor3f(1.0f, 1.0f, 1.0f);
              //glColor3f(0.0f, 0.0f, 1.0f);
              glBegin(GL_LINES);
              newLine(-0.2f, 4.7f, -angleSec+M_PI/2);                        // sec
              glEnd();
            }
            break;
        case SAVER3D:
            //reset pos
/*
            glLoadIdentity();
            glTranslatef(0.0f, 0.0f, -7.0f);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
*/
            //glTranslatef(0.0f, 0.0f, -7.0f);
            //glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            // music gfx loaded
            // if not load before screen saver
            /*
            if (!(music_oversigt_loaded)) {
                music_oversigt_loaded=true;
                load_music_covergfx(musicoversigt);
                mybox.settexture(musicoversigt);
            }
            */
            if (mybox.get_loaded_status()==false) {
              // if not loaded load
              if (mybox.loadboxpictures()==false) {
                printf("Error loading textures\n");
                write_logfile("Error loading textures.");
              }
            }
            _angle++;
            glPushMatrix();
            start = clock();
            mybox.show_music_3d(_angle,screensaverbox,screensaverbox,screensaverbox1);
            if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
            glPopMatrix();
            break;
        case SAVER3D2:
            // reset pos
            //glLoadIdentity();
            //glTranslatef(0.0f, 0.0f, -2.0f);
            //glEnable(GL_DEPTH_TEST);
            //glDisable(GL_BLEND);
            start = clock();
            _angle++;
            //mybox.settexture(musicoversigt);
            mybox.show_music_3d_2(_angle,screensaverbox);	//_textureId19
            if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
            break;
        case PICTURE3D:
            // picture screen saver
            start = clock();
            if (psaver==NULL) psaver=new picture_saver();
            if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
            //glLoadIdentity();
            glPushMatrix();
            psaver->show_pictures();
            glPopMatrix();
            break;
      }
      glPopMatrix();
    }
    //
    // vis spectium in screen saver
    //
    if ((visur) && (urtype==MUSICMETER)) {
      int uvypos = 0;
      float xxofset;
      float xpos = 0.0f;
      float ypos = 0.0f;
      float siz_x = 22.0f;                    // size 16
      float siz_y = 6.0f;                     // size 8
      if (snd) {
        glPushMatrix();
//        glTranslatef(100.0f, 100.0f, 0.0f);
        glTranslatef(orgwinsizex/2,orgwinsizey/2,0.0f);
        glRotatef(0,0.0f,1.0f,0.0f);
        static float rr=0.0f;
        uvypos =0;
        winsizx = 16;
        winsizy = 16;
        xpos = (-16)*16;
        ypos = 0.0f;
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D,texturedot);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glRotatef(0.0f,0.0f,0.0f,0.0f);
        float high;
        xxofset = 40.0f;                            // start ofset
        // create the bars
        for(int xp=0;xp<barantal;xp++) {
          xpos = (-siz_x)*xxofset;
          ypos = (-400)+((siz_y*2)+2.0);
          high = sqrt(spectrum[xp]*8)*2;
          for(int yp=0;yp<high;yp++) {
            // front
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos)  , siz_y+(ypos) , 0.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos)  ,-siz_y+(ypos) , 0.0f); // 4
            // left
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 32.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f); // 4
            // right
            glTexCoord2f(0, 0); glVertex3f((siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos) , siz_y+(ypos) , 32.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f); // 4
            // back
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f);
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 32.0f);
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos)  , siz_y+(ypos) , 32.0f);
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos)  ,-siz_y+(ypos) , 32.0f);
            glEnd();
            ypos += (siz_y*2)+2.0;
          }
          xxofset = xxofset-1.8f;    // mellem rum mellem hver søjle
        }
        //
        // show max value
        //
        xxofset = 40.0f;                                                        // start ofset
        glBindTexture(GL_TEXTURE_2D,texturedot1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor3f(1.0f, 1.0f, 1.0f);
        for(int xp=0;xp<barantal;xp++) {
          high = sqrt(uvmax_values[xp]*8)*2;
          //printf("xp =%2d high = %0.3f \n",xp,high*2);
          xpos = (-siz_x)*xxofset;
          ypos = (-388)+((siz_y*(high*2))+2.0);
          for(int yp=0;yp<1;yp++) {
            // front
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos)  , siz_y+(ypos) , 0.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos)  ,-siz_y+(ypos) , 0.0f); // 4
            glEnd();
            ypos += (siz_y*2)+2.0;
          }
          xxofset = xxofset-1.8f;    // mellem rum mellem hver søjle
        }
        //
        // Mirror
        //
        glColor3f(0.4f, 0.4f, 0.4f);
        glBindTexture(GL_TEXTURE_2D,texturedot);
        //glBindTexture(GL_TEXTURE_2D,_textureuv1);
        xxofset = 40.0f;                            // start ofset
        for(int xp=0;xp<barantal;xp++) {
          xpos = (-siz_x)*xxofset;
          ypos = (-432)+((siz_y*4)+2.0);
          high = sqrt(spectrum[xp]*4);
          for(int yp=0;yp<high/2;yp++) {
            // front
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos)  , siz_y+(ypos) , 0.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos)  ,-siz_y+(ypos) , 0.0f); // 4
            // left
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 32.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f); // 4
            // right
            glTexCoord2f(0, 0); glVertex3f((siz_x)+(xpos) ,-siz_y+(ypos) , 0.0f); // 1
            glTexCoord2f(0, 1); glVertex3f((siz_x)+(xpos) , siz_y+(ypos) , 0.0f); // 2
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos) , siz_y+(ypos) , 32.0f); // 3
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f); // 4
            // back
            glTexCoord2f(0, 0); glVertex3f((-siz_x)+(xpos) ,-siz_y+(ypos) , 32.0f);
            glTexCoord2f(0, 1); glVertex3f((-siz_x)+(xpos) , siz_y+(ypos) , 32.0f);
            glTexCoord2f(1, 1); glVertex3f((siz_x)+(xpos)  , siz_y+(ypos) , 32.0f);
            glTexCoord2f(1, 0); glVertex3f((siz_x)+(xpos)  ,-siz_y+(ypos) , 32.0f);
            glEnd();
            ypos -= (siz_y*2)+2.0;
          }
          xxofset = xxofset-1.8f;    // mellem rum mellem hver søjle
        }
        glPopMatrix();
      }
    }
    // end spectium
    if ((visur) && (sleeper)) {
      sleeper = 0;
      FILE *sfile;
      sleep_ok = 1;
      sfile=fopen("mythtv-controller.lock","w");
      if (sfile) {
        fputs("Lockfile",sfile);
        fclose(sfile);
        remove_log_file = true;                                               // set flag to delete log file
      }
    }
    if ((visur==0) && (sleep_ok==0) && (remove_log_file)) {
      fprintf(stderr,"remove logfile\n");
      remove("mythtv-controller.lock");       // fjern lock file
      sleeper = 1;
      sleep_ok = 0;
      rawtime1 = 0;                             // reset timer så den henter ti
      remove_log_file = false;                  // clear remove lock file flag
    }
    int iconsizex = 200;
    int iconspacey = 210;
    if ((screen_size == 3) || (screen_size == 4)) {
      iconsizex = 192;                            // icon size
      iconspacey = 192;
    }
    // vis menu **********************************************************************
    // main menu
    if ((!(visur)) && (!(vis_tv_oversigt)) && (starttimer == 0)) {
        //
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        // tv icon or play info icon
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        // play info icon
        if ((vis_music_oversigt) || (vis_radio_oversigt) || (vis_film_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt) || (vis_tidal_oversigt)) {
          glBindTexture(GL_TEXTURE_2D, _textureIdplayinfo);                         // default show music/radio/film/stream/spotify play info
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(27);                                                           // Info icon nr 27
        } else {                                                                    // else default tv
          glBindTexture(GL_TEXTURE_2D, _textureIdtv);		                         		// Tv texture icon
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glLoadName(1);                                                            // Tv guide icon nr 1
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*1) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*1)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*1)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*1) , 0.0);
        glEnd();
        // movie stuf
        if ((vis_film_oversigt) || (vis_stream_oversigt)) {
          glBindTexture(GL_TEXTURE_2D,_textureIdfilm_aktiv);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(3); 			                                                  // film icon name 3
        } else {
          if ((vis_music_oversigt) || (vis_radio_oversigt)) {
            glBindTexture(GL_TEXTURE_2D,_textureIdmusic_aktiv);                 //
          } else {
            glBindTexture(GL_TEXTURE_2D, _textureIdmusic);                      // music icon
          }
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(2); 			// Overwrite the first name in the buffer
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*2) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*2)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*2)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*2) , 0.0);
        glEnd();
        //film icon or pil up
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt) || (vis_tidal_oversigt)) {
          glBindTexture(GL_TEXTURE_2D, _textureIdpup);				// ved music filn radio stream  vis up icon
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(23); 			// Overwrite the first name in the buffer
        } else {
          if ((vis_film_oversigt) || (vis_stream_oversigt)) {
            glBindTexture(GL_TEXTURE_2D,_textureIdfilm_aktiv);
          } else {
            glBindTexture(GL_TEXTURE_2D, _textureIdfilm);				// default film
          }
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(3); 			// Overwrite the first name in the buffer
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*3) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*3)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*3)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*3) , 0.0);
        glEnd();
        // recorded icon
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt)) {
          glBindTexture(GL_TEXTURE_2D,_textureIdpdown);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(24); 			                                                // load film icon name
        } else {
          if (vis_recorded_oversigt) {
            glBindTexture(GL_TEXTURE_2D,_textureIdrecorded_aktiv);
          } else {
            glBindTexture(GL_TEXTURE_2D, _textureIdrecorded);                              // music icon
          }
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glColor4f(1.0f, 1.0f, 1.0f,1.0f);
          glLoadName(4);                                                        //
        }
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*4) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*4)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*4)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*4) , 0.0);
        glEnd();
        //
        // show reset movie search oversigt
        //
        if ((vis_film_oversigt) && (film_oversigt.get_search_view())) {
          glBindTexture(GL_TEXTURE_2D, _textureIdreset_search);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(28);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-1050 , 0.0);
          glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-1050+iconsizex , 0.0);
          glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-1050+iconsizex , 0.0);
          glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-1050 , 0.0);
          glEnd();
        }
        if (vis_uv_meter==false) {
          if ((!(vis_music_oversigt)) && (!(vis_film_oversigt))  && (!(vis_recorded_oversigt)) &&  (!(vis_stream_oversigt)) && (!(vis_radio_oversigt)) && (!(vis_spotify_oversigt)) && (!(show_status_update))) {
            // setup icon
            if (do_show_setup) glBindTexture(GL_TEXTURE_2D, _texturesetupmenu_select);
            else glBindTexture(GL_TEXTURE_2D, _texturesetupmenu);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(5);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-1050 , 0.0);
            glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-1050+iconsizex , 0.0);
            glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-1050+iconsizex , 0.0);
            glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-1050 , 0.0);
            glEnd();
          }
        }
        // spotify online search button
        if ((vis_spotify_oversigt) && (firsttimespotifyupdate==false)) {
          if (strcmp(keybuffer,"")!=0) glBindTexture(GL_TEXTURE_2D, spotify_search_back); else glBindTexture(GL_TEXTURE_2D, spotify_search);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(5);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-1050 , 0.0);
          glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-1050+iconsizex , 0.0);
          glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-1050+iconsizex , 0.0);
          glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-1050 , 0.0);
          glEnd();
        }
        // exit button
        if ((!(vis_music_oversigt)) && (!(vis_tv_oversigt)) && (!(vis_film_oversigt)) && (!(vis_stream_oversigt)) && (!(vis_spotify_oversigt)) && (!(vis_tidal_oversigt)) && (!(vis_radio_oversigt)) && (!((do_show_spotify_search_oversigt)))) {
          glBindTexture(GL_TEXTURE_2D, _textureexit);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(6);                                                        // exit button nr 6
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( 0 ,  orgwinsizey-(iconspacey/3) , 0.0);
          glTexCoord2f(0, 1); glVertex3f( 0,   orgwinsizey-(iconspacey/3)+(iconsizex/3) , 0.0);
          glTexCoord2f(1, 1); glVertex3f( 0+(iconsizex/3),orgwinsizey-(iconspacey/3)+(iconsizex/3) , 0.0);
          glTexCoord2f(1, 0); glVertex3f( 0+(iconsizex/3),orgwinsizey-(iconspacey/3) , 0.0);
          glEnd();
        }
        glPopMatrix();
    }
    // radio stuf / music / spotyfi / tidal selector
    if ((vis_radio_or_music_oversigt) && (!(visur))) {				//
      // Show tidal and spotify icons
      #ifndef ENABLE_TIDAL
      // img radio button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, radiobutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(80);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, (orgwinsizey-400)+0, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, (orgwinsizey-400)+200, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+500, (orgwinsizey-400)+200, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+500, (orgwinsizey-400)+0, 0.0);
      glEnd();
      glPopMatrix();
      // img music button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, musicbutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(81);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, ((orgwinsizey/2)-125)+0, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, ((orgwinsizey/2)-125)+200, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+500, ((orgwinsizey/2)-125)+200, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+500, ((orgwinsizey/2)-125)+0, 0.0);
      glEnd();
      glPopMatrix();
      // img spotify button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, spotifybutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(82);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, ((orgwinsizey/3)-200)+0, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, ((orgwinsizey/3)-200)+200, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+500, ((orgwinsizey/3)-200)+200, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+500, ((orgwinsizey/3)-200)+0, 0.0);
      glEnd();
      glPopMatrix();
      #else
      // if Tidal defined (supported)
      // img radio button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, radiobutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(80);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+0-200, (orgwinsizey-400)+0-75, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+0-200, (orgwinsizey-400)+200-75, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+500-200, (orgwinsizey-400)+200-75, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+500-200, (orgwinsizey-400)+0-75, 0.0);
      glEnd();
      glPopMatrix();

      // img music button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, musicbutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(81);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+0-200, ((orgwinsizey/2)-125)+0-75, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+0-200, ((orgwinsizey/2)-125)+200-75, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+500-200, ((orgwinsizey/2)-125)+200-75, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+500-200, ((orgwinsizey/2)-125)+0-75, 0.0);
      glEnd();
      glPopMatrix();

      // img spotify button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, spotifybutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(82);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+0+400, (orgwinsizey-400)+0-75, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+0+400, (orgwinsizey-400)+200-75, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+500+400, (orgwinsizey-400)+200-75, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+500+400, (orgwinsizey-400)+0-75, 0.0);
      glEnd();

      glPopMatrix();
      // img tidal button
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, tidalbutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(83);
      glBegin(GL_QUADS);

      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+0+400, ((orgwinsizey/2)-125)+0-75, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+0+400, ((orgwinsizey/2)-125)+200-75, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+500+400, ((orgwinsizey/2)-125)+200-75, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+500+400, ((orgwinsizey/2)-125)+0-75, 0.0);
      glEnd();
      glPopMatrix();
      #endif
    }

    //if (vis_stream_oversigt) printf("_sangley=%d stream_key_selected=%d stream_select_iconnr=%d  antal %d \n",_sangley,stream_key_selected,stream_select_iconnr,streamoversigt.streamantal());

    // stream and movie stuf
    if ((vis_stream_or_movie_oversigt) && (!(visur))) {
      // img
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, streambutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(80);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/2)+0, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/2)+200, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+500, (orgwinsizey/2)+200, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+500, (orgwinsizey/2)+0, 0.0);
      glEnd();
      glPopMatrix();
      // img
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D, moviebutton);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glLoadName(81);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/4)+0, 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3)+0, (orgwinsizey/4)+200, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+500, (orgwinsizey/4)+200, 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+500, (orgwinsizey/4)+0, 0.0);
      glEnd();
      glPopMatrix();
    }
    //
    if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_recorded_oversigt) || (vis_tv_oversigt) || (vis_radio_or_music_oversigt) || (vis_stream_or_movie_oversigt)) {
      show_newmovietimeout = 0;
      vis_nyefilm_oversigt = false;
      if ((vis_tv_oversigt) && (getstarttidintvguidefromarray == true)) {
        fprintf(stderr,"fundet start record in tv guide nr %d \n",aktiv_tv_oversigt.find_start_pointinarray(0));
        tvsubvalgtrecordnr = aktiv_tv_oversigt.find_start_pointinarray(0)-1;
        tvvalgtrecordnr = 0;
        getstarttidintvguidefromarray = false;
      }
    }
    // show oversigt over nye film inden timeout
    if ((vis_nyefilm_oversigt) && (do_show_setup == false)) {
      if (show_newmovietimeout == 0) vis_nyefilm_oversigt = false;
      if (fknapnr == 0) show_newmovietimeout--;
      film_oversigt.show_minifilm_oversigt(0,0);
    }
    // search movies
    if (vis_film_oversigt) {
      if (keybufferindex>0) {
        keybufferopenwin=true;					// yes open filename window
        if ((keybufferopenwin) && (hent_film_search)) {				// vi har søgt og skal reset view ofset til 0 = start i 3d visning.
          hent_film_search=false;
          if (debugmode & 8) fprintf(stderr,"Search movie string: %s \n ",keybuffer);
          // start search for movie title. And fill movie view from db
          if (strcmp(keybuffer,"")!=0) {
            //film_oversigt.opdatere_search_film_oversigt(keybuffer);
            film_oversigt.opdatere_film_oversigt(keybuffer);
            film_oversigt.set_search_view(true);
          }
          keybuffer[0] = 0;
          keybufferindex = 0;
          film_select_iconnr = 0;
          film_key_selected = 0;
        }
      }
    }
    // search radio station buffer search
    if ((vis_radio_oversigt) && (!(visur)))  {
      if (keybufferindex>0) {						// er der kommet noget i keyboard buffer
        keybufferopenwin = true;					// yes open filename window
        // hent søgte sange oversigt
        if ((keybufferopenwin) && (hent_radio_search)) {				// vi har søgt og skal reset view ofset til 0 = start i 3d visning.
          hent_radio_search = false;
          // write debug log
          sprintf(debuglogdata,"Search radio string: %s ",keybuffer);
          write_logfile((char *) debuglogdata);
          radiooversigt.clean_radio_oversigt();			// clean old liste
          radiooversigt.opdatere_radio_oversigt(keybuffer);	// load new
          radiooversigt.load_radio_stations_gfx();
          keybuffer[0] = 0;
          keybufferindex = 0;
          _angley = 0.0f;
          rknapnr = 0;
          radio_key_selected = 1;   						                                // reset cursor position
          do_play_radio = false;							                                  // Do not play station
        }
      }
    }
    // search func for music after enter is pressed
    if ((vis_music_oversigt) && (!(visur)) && (!(ask_save_playlist)))  {
      if (keybufferindex>0) {						// er der kommet noget i keyboard buffer
        keybufferopenwin=true;					// yes open filename window
        // hent søgte sange oversigt
        if ((keybufferopenwin) && (hent_music_search)) {				                // vi har søgt og skal reset view ofset til 0 = start i 3d visning.
          hent_music_search=false;
          if (findtype==0) {
            // new ver
           musicoversigt.opdatere_music_oversigt_searchtxt(keybuffer , 0);
            // old ver
            //opdatere_music_oversigt_searchtxt( musicoversigt , keybuffer , 0 );	// find det som der søges kunster
         } else {
            // new ver
           musicoversigt.opdatere_music_oversigt_searchtxt(keybuffer , 0);
            // old ver
            //opdatere_music_oversigt_searchtxt( musicoversigt , keybuffer , 1 );  // find det som der søges efter sange navn
          }

          // new ver
         musicoversigt.opdatere_music_oversigt_icons(); 					            // load gfx icons

          // old ver
          //opdatere_music_oversigt_icons(); 					                            // load gfx icons
          keybuffer[0] = 0;
          keybufferindex = 0;
          _angley = 0.0f;
          do_music_icon_anim_icon_ofset = -1;
          music_icon_anim_icon_ofset = 0;						// vis music oversigt fra start
          music_icon_anim_icon_ofsety = 0;						//
          mknapnr = 0;
          music_key_selected = 1;                    // reset cursor position
          ask_open_dir_or_play_aopen = 1;		//
          ask_open_dir_or_play = 0;
        }
      }
    }
    #ifdef ENABLE_SPOTIFY
    // spotify do the search after enter is pressed
    if ((vis_spotify_oversigt) && (!(visur)) && (do_show_spotify_search_oversigt==false)) {
      if ((keybufferopenwin) && (hent_spotify_search)) {
        hent_spotify_search=false;
        spotify_selected_startofset=0;                                          // icon show start ofset
        spotifyknapnr=1;                                                        // reset pos
        spotify_oversigt_loaded_begin=true;
        if (keybufferindex>0) {		                                       				// er der kommet noget i keyboard buffer
          if (spotify_oversigt.search_playlist_song==0)                         //
           spotify_oversigt.opdatere_spotify_oversigt_searchtxt(keybuffer,0);	  // find det som der søges playlist navn
          else
           spotify_oversigt.opdatere_spotify_oversigt_searchtxt(keybuffer,1);   // find det som der søges efter sange navn
          //spotify_oversigt.load_spotify_iconoversigt();
          keybuffer[0] = 0;
          keybufferindex = 0;
        }
        spotify_oversigt_loaded_begin=false;
      }
    }
    #endif
    //
    // auto search spotify online
    //
    const int start_search_action_waittime=4;
    struct tm* t1;
    static time_t lasttime1=0;
    static time_t nowdate1;
    time(&nowdate1);
    if (do_show_spotify_search_oversigt) {
      if ((search_spotify_string_changed) && (difftime(nowdate1, lasttime1)>start_search_action_waittime)) {
        time(&lasttime1);
        hent_spotify_search_online=true;
        search_spotify_string_changed=false;
      }
      if (hent_spotify_search_online) {
        hent_spotify_search_online=false;
        if (keybufferindex>=2) {
          // start the search online
          do_hent_spotify_search_online=true;
          //keybufferindex=1;
        }
      }
    }
    start = clock();
    if (!(visur)) {
      // music view
      if (vis_music_oversigt) {

        // New ver
       musicoversigt.show_music_oversigt(_textureId_dir,_textureIdback,_textureId28,_mangley,music_key_selected);

        // old ver
        //show_music_oversigt(musicoversigt,_textureId_dir,_textureIdback,_textureId28,0,_mangley,music_key_selected);

        if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
      } else if (vis_film_oversigt) {
        glPushMatrix();
        //aktivfont.selectfont("DejaVu Sans");
        film_oversigt.show_film_oversigt(_fangley,fknapnr);
        glPopMatrix();
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
      } else if (vis_stream_oversigt) {
        glPushMatrix();
        streamoversigt.show_stream_oversigt(onlinestream, onlinestream_empty,onlinestream_empty1 ,_sangley,stream_key_selected);
        glPopMatrix();
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
      } else if (vis_radio_oversigt) {
        radio_pictureloaded=radiooversigt.show_radio_oversigt( _textureId_dir , 0 , _textureIdback , _textureId28 , _rangley);
        // show radio options menu
        if ((show_radio_options) && (!(visur))) {
          radiooversigt.show_radio_options();
        }
      }
      #ifdef ENABLE_SPOTIFY
      if (vis_spotify_oversigt) {
        if (do_show_spotify_search_oversigt==false) {
          spotify_oversigt.show_spotify_oversigt( _textureId_dir , _textureId_song , _textureIdback , _textureIdback , spotify_selected_startofset , spotifyknapnr );
        } else {
          spotify_oversigt.show_spotify_search_oversigt( onlineradio , _textureId_song , _textureId_dir , _textureIdback , spotify_selected_startofset , spotifyknapnr ,keybuffer);
        }
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
        if (strcmp(spotify_oversigt.spotify_get_token(),"")==0) {
          if (startwebbrowser) {
            // start webbroser to login on spotify
            system("firefox localhost:8000");
            startwebbrowser=false;
          }
        }
        // 7 ms is my timer
        // select play device
        if (do_select_device_to_play) {
          spotify_oversigt.select_device_to_play();
        }
      }
      #endif
      if (vis_tidal_oversigt) {
        // show Tidal overview
        #ifdef ENABLE_TIDAL
        if (tidal_oversigt) tidal_oversigt->show_tidal_oversigt( _textureId_dir , _textureId_song , _textureIdback , _textureIdback , spotify_selected_startofset , spotifyknapnr );
        #endif
      } else if (vis_tv_oversigt) {
        // show tv guide
        // take time on it
        aktiv_tv_oversigt.show_fasttv_oversigt( tvvalgtrecordnr , tvsubvalgtrecordnr , do_update_xmltv_show );
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000 ) << " ms" << endl;
        //
        // show tv program info about selected program in tv guide
        //
        if ((do_zoom_tvprg_aktiv_nr)>0) {
          glPushMatrix();
          // show info om program selected
          //aktivfont.selectfont("FreeMono");
          //aktivfont.selectfont(configfontname);
          aktiv_tv_oversigt.showandsetprginfo( tvvalgtrecordnr , tvsubvalgtrecordnr );
          glPopMatrix();
          //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
        }
        // show record program menu
      } else if (vis_recorded_oversigt) {
        recordoversigt.show_recorded_oversigt1(0,0);
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
      }
      // show record tv programs in tv guide
      if ((vis_tvrec_list) && (do_show_setup)) {
        // show tv record list
        glPushMatrix();
        //aktiv_crecordlist.showtvreclist();
        write_logfile((char *) "show tv record wiew.");
        glPopMatrix();
        //if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
      }
    }

    if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;

    #ifdef ENABLE_SPOTIFY
    //firsttimespotifyupdate=true;
    // show firsttime spotify update windows request
    if ((!(visur)) && (firsttimespotifyupdate) && (vis_spotify_oversigt) && (strcmp(spotify_oversigt.spotify_get_token(),"")!=0)) {
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D,_texturemusicplayer);                           // _texturemovieinfobox
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      glTranslatef(550, 350, 0.0f);
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 400.0, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(640.0, 400.0, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(640.0, 0.0, 0.0);
      glEnd();
      glPopMatrix();
      aktivfont.selectfont((char *) "URW Bookman L");
      // print text in window
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(590, 370+300, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(20.5, 20.5, 1.0);
      // 0 = uk 1 = dk 2 =  France 3 = tysk 4 = Arabi
      glcRenderString(spotify_firsttime_line1[configland]);
      glPopMatrix();
      // print text in window
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(590, 370+275, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString(spotify_firsttime_line2[configland]);
      glPopMatrix();
      // print text in window
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(590, 370+250, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString(spotify_firsttime_line3[configland]);
      glPopMatrix();
      // print text in window
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(590, 370+220, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString(spotify_firsttime_line4[configland]);
      glPopMatrix();
      // print text in window
      static time_t today1=0;
      static time_t lasttime=0;
      static bool switch_text=false;
      today1=time(NULL);
      if ((lasttime==0) || (lasttime+1<=today1)) {
        lasttime=today1;
        switch_text=!switch_text;
      }
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(780, 370+70, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(30.5, 30.5, 1.0);                    // danish charset ttf
      if ((spotify_oversigt.get_spotify_update_flag()) && (switch_text)) {
        glcRenderString(spotify_firsttime_pleasewait[configland]);
      }
      glPopMatrix();
      aktivfont.selectfont(configfontname);
    }
    #endif
    // show search box and text for radio and music and movie and spotify offline search NOT spotify online search
    if ((vis_radio_oversigt) || (vis_music_oversigt) || (vis_film_oversigt) || ((vis_spotify_oversigt) && (do_show_spotify_search_oversigt==false))) {
      if ((vis_spotify_oversigt) && (strcmp(keybuffer,"")!=0)) keybufferopenwin=true;
      if ((keybufferopenwin) && (strcmp(keybuffer,"")!=0)) {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        if (vis_radio_oversigt) glBindTexture(GL_TEXTURE_2D,_textureIdradiosearch);
        else if (vis_music_oversigt) glBindTexture(GL_TEXTURE_2D,_textureIdmusicsearch);
        else if (vis_film_oversigt) glBindTexture(GL_TEXTURE_2D,_textureIdmoviesearch);
        #ifdef ENABLE_SPOTIFY
        if (vis_spotify_oversigt) {
          if (spotify_oversigt.search_playlist_song==0) glBindTexture(GL_TEXTURE_2D,_textureIdmusicsearch1);
          else glBindTexture(GL_TEXTURE_2D,_textureIdmusicsearch);
        }
        #endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(50, 50, 0.0f);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 100.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(640.0, 100.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(640.0, 0.0, 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef(80, 70, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        glcRenderString(keybuffer);
        glPopMatrix();
      }
    }
    // show volume value
    if (show_volume_info) {
      vis_volume_timeout--;
      if (vis_volume_timeout<=0) show_volume_info = false;
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glColor4f(1.0f,1.0f,1.0f,0.2f);
      glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D, texturedot);                                 // volume icon
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTranslatef(10, 19, 0.0f);
      int aa = 0;
      for(i=0;i<(configsoundvolume*10);i++) {
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0, 0); glVertex3f( 10.0+aa, 0.0, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 10.0+aa, 40.0, 0.0);
        glTexCoord2f(1, 1); glVertex3f( 40.0+aa, 40.0, 0.0);
        glTexCoord2f(1, 0); glVertex3f( 40.0+aa, 0.0, 0.0);
        glEnd(); //End quadrilateral coordinates
        aa+=40;
      }
      glPopMatrix();
    }
    // vis_error=true;
    // vis_error_timeout=10;
    // show music loader/player (sdl_mixer/fmod) error in program
    if ((vis_error) && (vis_error_timeout)) {
      vis_error_timeout--;
      // background
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glColor4f(1.0f,1.0f,1.0f,0.2f);
      glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D, _errorbox);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTranslatef(550, 19, 0.0f);                                          // orgwinsizey
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 10.0, 0.0, 0.0);
      glTexCoord2f(0, 1); glVertex3f( 10.0, 50.0, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 640.0, 50.0, 0.0);
      glTexCoord2f(1, 0); glVertex3f( 640.0, 0.0, 0.0);
      glEnd();
      glPopMatrix();
      if (film_oversigt.film_is_playing) {
        strcpy(temptxt1,"Error playing movie.");
      } else {
        strcpy(temptxt1,"Fault playing.");
        switch(vis_error_flag) {
            case 18:
              strcpy(temptxt1,"Music/Radio station not online.");
              break;
            case 20:
              strcpy(temptxt1,"Couldn't perform seek. Limitation of the medium.");
              break;
            case 25:
              strcpy(temptxt1,"Error 25.");
              break;
            case 27:
              strcpy(temptxt1,"Error 27.");
              break;
            case 38:
              strcpy(temptxt1,"Music/Radio station not online.");
              break;
            case 36:
              strcpy(temptxt1,"Error 36 - An invalid object handle was used.");
              break;
            case 37:
              strcpy(temptxt1,"Error 37.");
              break;
            case 50:
              strcpy(temptxt1,"Error 50 - Couldn't connect to the specified host.");
              break;
            case 51:
              strcpy(temptxt1,"Error 51.");
              break;
            default:
              #if defined USE_FMOD_MIXER
              sprintf(temptxt1,music_songloaderror[configland],vis_error_flag,(char *) FMOD_ErrorString((FMOD_RESULT) vis_error_flag));
              #endif
              break;
        }
      }
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(580, 30, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString(temptxt1);
      glPopMatrix();
    }
    //
    // skal vi til at spørge ask open dir or play (ask about play)
    //
    if ((vis_music_oversigt) && (!(visur)) && (ask_open_dir_or_play) && (mknapnr>0)) {
      do_swing_music_cover = false;
      if (do_swing_music_cover==false) {
        xof = 500;
        yof = 200;
        buttonsize = 200;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor3f(1.0f, 1.0f, 1.0f);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, _textureId9_askbox);						// texture9
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS); // draw ask box
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+800, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+800, yof+800 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+800,yof , 0.0);
        glEnd();
        glPopMatrix();
        // ***************************************************************** play icon
        xof=550;
        yof=250;
        buttonsize=100;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _textureIdplayicon);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(20);                      				  // play icon nr
        glBegin(GL_QUADS);
        // play icon
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************ Open/or close window again
        xof = 650;
        yof = 250;
        buttonsize = 100;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        if (dirmusic.numbersindirlist()>0) {
          glBindTexture(GL_TEXTURE_2D, _textureopen);
        } else {
          //glBindTexture(GL_TEXTURE_2D, _textureopen);                // _textureclose);
          glBindTexture(GL_TEXTURE_2D, _textureclose);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(21);                        // Overwrite the first name in the buffer
        glBegin(GL_QUADS);
        // play icon
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
        glEnd();
        glPopMatrix();
        // swap ************************************************************** icon swap
        xof = 750;
        yof = 250;
        buttonsize = 100;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _textureswap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(22);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
        glEnd();
        glPopMatrix();
        // draw cd cover
        xof = 750;
        yof = 250;
        no_open_dir = 0;
        buttonsize = 300;
        if (dirmusic.numbersindirlist()==0) {						// er der nogle dirs
          glPushMatrix();
          glColor4f(0.8f, 0.8f, 0.8f,1.0f);
          glRotatef(0.0f, 0.0f, 0.5f, 0.1f);
          glBlendFunc(GL_ONE, GL_ONE);
          glBindTexture(GL_TEXTURE_2D, _textureId9_2);                	// box no cd cover
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); 							//	Begin quadrilateral coordinates
          // 										draw front box
          glTexCoord2f(0, 0); glVertex3f(200+ xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f(200+ xof,yof+buttonsize, 0.0);
          glTexCoord2f(1, 1); glVertex3f(200+ xof+buttonsize, yof+buttonsize , 0.0);
          glTexCoord2f(1, 0); glVertex3f(200+ xof+buttonsize,yof , 0.0);
          glEnd();
          // ****************************************************************
          // draw cd cover
          no_open_dir = 1;
          glColor4f(0.8f, 0.8f, 0.8f,1.0f);
          glRotatef(0.0f, 0.0f, 0.5f, 0.1f);
          glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
          if (dirmusic.textureId) {
            glBindTexture(GL_TEXTURE_2D, dirmusic.textureId);		// cover
          } else {
            glBindTexture(GL_TEXTURE_2D, _texture_nocdcover);                	// box no cd cover
          }
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); 							//	Begin quadrilateral coordinates
          glTexCoord2f(0, 0); glVertex3f(200+ xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f(200+ xof,yof+buttonsize, 0.0);
          glTexCoord2f(1, 1); glVertex3f(200+ xof+buttonsize, yof+buttonsize , 0.0);
          glTexCoord2f(1, 0); glVertex3f(200+ xof+buttonsize,yof , 0.0);
          glEnd(); //End quadrilateral coordinates
          glPopMatrix();
        }
        // ****************************************************************************************************
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(560.0f, 950.0f , 0.0f);							// pos
        glRasterPos2f(0.0f, 0.0f);
        // print dirid navn
        strcpy(temptxt1,musicoversigt.get_album_name(mknapnr-1));						// hent albun navn
        if (strcmp(temptxt1,"")==0) strcpy(temptxt1,music_noartistfound[configland]);
        // check for maxlength
        if (strlen(temptxt1)>24) {
          strcpy(temptxt1,"..");
          //strcat(temptxt1,musicoversigt[mknapnr-1].album_name+(strlen(musicoversigt[mknapnr-1].album_name)-24));
          strcat(temptxt1,musicoversigt.get_album_name(mknapnr-1)+(strlen(musicoversigt.get_album_name(mknapnr-1))-24));

        }
        sprintf(temptxt,music_nomberofsongs[configland],dirmusic.numbersinlist(),temptxt1);
        glScalef(20.5, 20.5, 1.0);                                                    // danish charset ttf
        glcRenderString(temptxt);			                                                // show numbers of songs in list
        glPopMatrix();
        i = 0;
        if (dirmusic.numbersindirlist()==0) dirmusiclistemax=16; else dirmusiclistemax=8;		// hvis ingen dir mere plads til flere sange i sangliste
        //
        // show cd song list så man kan vælge
        //
        //aktivfont.selectfont("courier 10 Pitch");
        while (((unsigned int) i<(unsigned int) dirmusic.numbersinlist()) && ((unsigned int) i<(unsigned int) dirmusiclistemax)) {	// er der nogle sange navne som skal vises
          ofset = 18*i;
          dirmusic.popsong(temptxt,&aktiv,i+do_show_play_open_select_line_ofset);				// hent sang info
          pos=strrchr(temptxt,'/');
          if (pos>0) strcpy(temptxt,pos+1);
          pos=strrchr(temptxt,'.');
          if (pos>0) temptxt[pos-temptxt]=0;
          if (i<12) temptxt[54]=0; else temptxt[35]=0;
          sprintf(temptxt1,"%-45s",temptxt);
          temptxt1[45]='\0';
          if (i==do_show_play_open_select_line) glColor4f(textcolor[0],textcolor[1],textcolor[2],1.0f);
           else glColor4f(selecttextcolor[0],selecttextcolor[1],selecttextcolor[2],1.0f);
          glPushMatrix();
          glTranslatef(560.0f, 850.0f -ofset, 0.0f);
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          //aktivfont.selectfont("Courier 10 Pitch");
          glcRenderString(temptxt1);
          i++;
          glTranslatef(5.0f, 0.0f, 0.0f);
          if (aktiv==true) glcRenderString("[X]");
            else glcRenderString("[ ]");
          glPopMatrix();
        }
      }
    }
    //
    // ask save playlist name
    //
    if (vis_music_oversigt) {
      if (ask_save_playlist) {
        xof = 500;
        yof = 600;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, _texturesaveplaylist);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS); // draw ask box
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+50, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+600, yof+50 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+600,yof , 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(xof+20.0f,yof+10+5, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        //aktivfont.selectfont((char *) "Courier 10 Pitch");
        glcRenderString("Playlist name :");
        glcRenderString(keybuffer);
        glPopMatrix();
        glPushMatrix();
        showcoursornow(330,460+5,strlen(keybuffer));
        glPopMatrix();
      }
    }
    // save playlist to file
    if (save_ask_save_playlist) {
     musicoversigt.save_music_oversigt_playlists(playlistfilename);
      save_ask_save_playlist=false;
      ask_save_playlist=false;
      // reset keyboard buffer
      strcpy(keybuffer,"");
      strcpy(playlistfilename,"");
      keybufferindex=0;
    }

    //
    // spotify ask play or open playlist
    //
    #ifdef ENABLE_SPOTIFY
    if ((vis_spotify_oversigt) && (!(visur)) && (ask_open_dir_or_play_spotify) && (spotifyknapnr>0)) {
      xof = 550;
      yof = 500;
      buttonsize = 200;
      // ***************************************************************** play icon
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
      glBindTexture(GL_TEXTURE_2D, spotify_askplay);						// texture9
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(20);                      				  // play icon nr
      glBegin(GL_QUADS); // draw ask box
      glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
      glTexCoord2f(0, 1); glVertex3f( xof,yof+328-80, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+651-80, yof+328-80 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+651-80,yof , 0.0);
      glEnd();
      glPopMatrix();
      // if playlist or artist you can open it else only show play
      if ((spotify_oversigt.get_spotify_type(spotifyknapnr)==0) || (spotify_oversigt.get_spotify_type(spotifyknapnr)==2)) {
        // ***************************************************************** open icon
        xof=550;
        yof=200;
        buttonsize=100;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, spotify_askopen);						// texture9
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(21);                      				  // play icon nr
        glBegin(GL_QUADS); // draw ask box
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+328-80, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+651-80, yof+328-80 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+651-80,yof , 0.0);
        glEnd();
        glPopMatrix();
      }
    }
    #endif
    //
    // tidal ask play or open playlist
    //
    #ifdef ENABLE_TIDAL
    if ((vis_tidal_oversigt) && (!(visur)) && (ask_open_dir_or_play_tidal) && (tidalknapnr>0)) {
      xof = 550;
      yof = 500;
      buttonsize = 200;
      // ***************************************************************** play icon
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
      glBindTexture(GL_TEXTURE_2D, spotify_askplay);						// texture9
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(20);                      				  // play icon nr
      glBegin(GL_QUADS); // draw ask box
      glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
      glTexCoord2f(0, 1); glVertex3f( xof,yof+328-80, 0.0);
      glTexCoord2f(1, 1); glVertex3f( xof+651-80, yof+328-80 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( xof+651-80,yof , 0.0);
      glEnd();
      glPopMatrix();
      // if playlist or artist you can open it else only show play
      if ((tidal_oversigt->get_tidal_type(tidalknapnr)==0) || (tidal_oversigt->get_tidal_type(tidalknapnr)==2)) {
        // ***************************************************************** open icon
        xof=550;
        yof=200;
        buttonsize=100;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, spotify_askopen);						// texture9
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(21);                      				  // play icon nr
        glBegin(GL_QUADS); // draw ask box
        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+328-80, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+651-80, yof+328-80 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+651-80,yof , 0.0);
        glEnd();
        glPopMatrix();
      }
    }
    #endif


    // start play radio station
    // and stop old player of playing
    if (vis_radio_oversigt) {
      if ((do_play_radio) && (rknapnr>0) && (rknapnr<=radiooversigt.radioantal())) {
        // do we play now ?
        // def code for FMOD and SDL MIXER
        #if defined USE_FMOD_MIXER
        if (snd) {
          // yes stop play
          // stop old playing
          sound->release();                                                                       // stop last playing song
          dsp = 0;                                                                                  // reset uv
          ERRCHECK(result,0);
          snd = 0;                                // set play new flag
        }
        #endif
        #if defined USE_SDL_MIXER
        if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
        sdlmusicplayer = NULL;
        snd = 0;                                // set play new flag
        #endif
        write_logfile("Stop music player.");
        if (snd==0) {
          snd = 1;
          sprintf(debuglogdata,"Start play radio station nr %d url %s ",rknapnr-1,radiooversigt.get_stream_url(rknapnr-1));
          write_logfile((char *) debuglogdata);
          if (snd == 0) {
              #if defined USE_FMOD_MIXER
              result = sndsystem->init(32, FMOD_INIT_NORMAL, 0);
              ERRCHECK(result,0);
              #endif
          }
          #if defined USE_SDL_MIXER
          if (sdlmusicplayer==0) {
              fprintf(stderr,"Setup SDL_MIXER soundsystem\n");
              // load support for the OGG and MOD sample/music formats
              int flags=MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC;
              SDL_Init(SDL_INIT_AUDIO);

              /* This is where we open up our audio device.  Mix_OpenAudio takes
              as its parameters the audio format we'd /like/ to have. */
              if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
                  fprintf(stderr,"Unable to open audio!\n");
                  exit(1);
              }
              /* If we actually care about what we got, we can ask here.
              In this program we don't, but I'm showing the function call here anyway in case we'd want to know later. */
              Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
              sdlmusic=Mix_Init(flags);
              if(sdlmusic&flags != flags) {
                  fprintf(stderr,"Mix_Init: Failed to init required ogg,mp3,flac support!\n");
                  fprintf(stderr,"Mix_Init: %s\n", Mix_GetError());
                  // handle error
              }
          }
          #endif
          if (strcmp(radiooversigt.get_stream_url(rknapnr-1),"")!=0) {
            strcpy(aktivplay_music_path,radiooversigt.get_stream_url(rknapnr-1));
            // write debug log
            //sprintf(debuglogdata,"play radio path = %s ",aktivplay_music_path);
            //write_logfile(debuglogdata);
            // fmod player
            #if defined USE_FMOD_MIXER
            // set big sound buffer to stop lag
            result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);
            result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
            ERRCHECK(result,rknapnr);
            if ((result==FMOD_OK) && (openstate!=FMOD_OPENSTATE_CONNECTING)) {
              if (sound) result = sndsystem->playSound(sound,NULL, false, &channel);
              //ERRCHECK(result,do_play_music_aktiv_table_nr);
              if (sndsystem) channel->setVolume(configsoundvolume);
              radiooversigt.set_radio_popular(rknapnr-1);				// set afspillings antal
              radiooversigt.set_radio_online(rknapnr-1,true);				// station virker fint ok status igen
              radiooversigt.set_radio_intonline(rknapnr-1);
            } else {
              radiooversigt.set_radio_online(rknapnr-1,false);			// set radio til ofline (vis som rød)
              radiooversigt.set_radio_aktiv(rknapnr-1,false);			//
            }
            if (result==FMOD_OK) do_zoom_radio=true;
            #endif
            // play online radio by sdl
            #if defined USE_SDL_MIXER
            //if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
            sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
            Mix_PlayMusic(sdlmusicplayer, 0);
            if (!(sdlmusicplayer)) {
              fprintf(stderr,"Error load music. %s\n",aktivplay_music_path);
              ERRCHECK_SDL(Mix_GetError(),rknapnr);
            }
            
            if (sdlmusicplayer) {
              radiooversigt.set_radio_popular(rknapnr-1);                             // set afspillings antal
              radiooversigt.set_radio_online(rknapnr-1,true);                         // station virker fint ok status igen
              radiooversigt.set_radio_intonline(rknapnr-1);
            } else {
              radiooversigt.set_radio_online(rknapnr-1,false);	// close if down
              radiooversigt.set_radio_aktiv(rknapnr-1,false);			//
            }
            if (sdlmusicplayer) do_zoom_radio=true;
            #endif
          }
          aktiv_radio_station = rknapnr-1;		                                    // Husk aktiv radio station så vi kan vi den senere
          radio_key_selected = rknapnr;		                                        // husk den valgte radio station
          rknapnr = 0;
          mknapnr = 0;		                                                  		// reset select fire again
          do_play_radio = false;
        }
      }
      // check om vi spiller radio
      #if defined USE_FMOD_MIXER
      if ((channel) && (sound) && (snd)) {
        //result=sound->getOpenState(&openstate,&percent,&starving,false);
        sounderrflag=result;							// gem load flag info
        ERRCHECK(result,rknapnr);
        if (result==FMOD_OK) {
          result = sound->getNumTags(&numtags, &numtagsupdated);
          ERRCHECK(result,0);
          if (numtags) {
            for(count=0; count < numtags; count++) {
              FMOD_TAG tag;
              if (sound->getTag(0,count,&tag)!=FMOD_OK) {
                  ERRCHECK(result,0);
                  break;
              } else if (tag.datatype==FMOD_TAGDATATYPE_STRING) {
                if (strcmp(tag.name,"TITLE")==0) strcpy(aktivsongname,(char *) tag.data); else strcpy(aktivsongname,"");
                if (strcmp(tag.name,"ARTIST")==0) strcpy(aktivartistname,(char *) tag.data);
                if (strcmp(tag.name,"icy-br")==0) {
                  // get and save sound bit rate from stream tag
                  if (strncmp((char *) tag.data,"160",3)) radiooversigt.set_kbps(aktiv_radio_station,160);
                  else if (strncmp((char *) tag.data,"128",3)) radiooversigt.set_kbps(aktiv_radio_station,128);
                  else if (strncmp((char *) tag.data,"112",3)) radiooversigt.set_kbps(aktiv_radio_station,112);
                  else if (strncmp((char *) tag.data,"192",3)) radiooversigt.set_kbps(aktiv_radio_station,192);
                  else if (strncmp((char *) tag.data,"64",2))  radiooversigt.set_kbps(aktiv_radio_station,64);
                  else if (strncmp((char *) tag.data,"32",2))  radiooversigt.set_kbps(aktiv_radio_station,32);
                  else if (strncmp((char *) tag.data,"8",2))   radiooversigt.set_kbps(aktiv_radio_station,8);
                  else if (strncmp((char *) tag.data,"320",3)) radiooversigt.set_kbps(aktiv_radio_station,320);
                  else radiooversigt.set_kbps(aktiv_radio_station,atoi((char *) tag.data));
                }
                //printf("%s=%s string length= (%d bytes) \n",tag.name, (char *) tag.data, tag.datalen);
              } else if ((FMOD_TAGTYPE) tag.datatype==FMOD_TAGTYPE_SHOUTCAST) {
                fprintf(stderr,"FMOD_TAGTYPE_SHOUTCAST\n");
                //printf("%s = <binary> (%d bytes)\n", tag.name, tag.datalen);
              } else if ((FMOD_TAGTYPE) tag.datatype==FMOD_TAGTYPE_ID3V1) {
                fprintf(stderr,"FMOD_TAGTYPE_ID3V1\n");
              }
            }
          }
        }
      }
      // create radio station online check tread
      #endif
      #if defined USE_SDL_MIXER
      #endif
      if (!(check_radio_thread)) {
        check_radio_thread=true;
        pthread_t loaderthread;           // check radio status thread
        int rc=pthread_create(&loaderthread,NULL,radio_check_statusloader,NULL);
        if (rc) {
          fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
        }
      }
    }
    // stop music
    // if playing
    if (vis_stream_oversigt) {
      if ((do_play_stream) && (sknapnr>0) && (sknapnr<=streamoversigt.streamantal())) {
        #if defined USE_FMOD_MIXER
        if (snd) {
         // stop old playing
         sound->release();                                                                       // stop last playing song
         ERRCHECK(result,0);
         snd = 0;
        }
        #endif
        #if defined USE_SDL_MIXER
        Mix_FreeMusic(sdlmusicplayer);
        sdlmusicplayer=NULL;
        #endif
      }
    }
    #if defined USE_FMOD_MIXER
    if ((snd) && (sound) && (channel)) {
      //result = sound->getOpenState(&openstate,&percent,&starving,false);
      ERRCHECK(result,rknapnr);
      channel->setMute(starving);
      ERRCHECK(result,rknapnr);
      if (openstate==FMOD_OPENSTATE_CONNECTING) strcpy(aktivsongstatus,radio_connecting[configland]);
      else if ((openstate==FMOD_OPENSTATE_BUFFERING) || (starving)) sprintf(aktivsongstatus,radio_buffing[configland],percent);
      else strcpy(aktivsongstatus,radio_playing[configland]);
    } else if (sound) strcpy(aktivsongstatus,radio_connecting[configland]); else strcpy(aktivsongstatus,"            ");
    #endif
    #if defined USE_SDL_MIXER
    strcpy(aktivsongstatus,"Playing");
    #endif
    // alt music er her under player
    if (do_play_music_cover) {
      // play list
      if (do_find_playlist) {
        do_find_playlist=false;                                     // sluk func igen
        do_play_music_cover=0;
        if (((do_zoom_music_cover==false) || (do_play_music_aktiv_play==0)) && (mknapnr!=0)) {
          // playliste funktion set start play
          if (debugmode & 2) fprintf(stderr,"Type af sange nr %d som skal loades %d\n ",mknapnr-1,musicoversigt.get_album_type(mknapnr-1));
          if (musicoversigt.get_album_type(mknapnr-1)==-1) {
            if (debugmode & 2) fprintf(stderr,"Loading song from mythtv playlist: %4d %d\n",do_play_music_aktiv_nr,mknapnr);
            if (hent_mythtv_playlist(musicoversigt.get_directory_id(mknapnr-1))==0) {		// tilføj musik valgte til playliste + load af covers
              fprintf(stderr,"**** PLAYLIST LOAD ERROR **** No songs. mythtv playlist id =%d\n",musicoversigt.get_directory_id(mknapnr-1));
              exit(2);
            }
            do_play_music_aktiv_table_nr = 1;						// sæt start play aktiv nr
          // normal cd cover browser funktion set start play
          } else if (musicoversigt.get_album_type(mknapnr-1)==0) {
            if (debugmode & 2) fprintf(stderr,"Loading songs from id:%4d \n",do_play_music_aktiv_nr);
             // reset valgt liste
            bool eraktiv;
            for(int t=0;t<dirmusic.numbersinlist();t++) {
              dirmusic.popsong(temptxt1,&eraktiv,t);
              do_play_music_aktiv_nr_select_array[t]=eraktiv;
            }
            get_music_pick_playlist(do_play_music_aktiv_nr,do_play_music_aktiv_nr_select_array);	// tilføj musik valgt til aktiv play liste + load af cover
            if (do_play_music_aktiv_table_nr==0) {
              do_play_music_aktiv_table_nr = 1;			// sæt start play aktiv nr
            }
            if (debugmode & 2) fprintf(stderr,"Set aktiv playlist nr to start ved nr 1\n");
          }
          if (do_play_music_aktiv_table_nr>0) {
            #if defined USE_FMOD_MIXER
            if (snd==0) {
                // aktiv_playlist = class to control music to play
                // (indenholder array til playliste samt hvor mange der er i playliste)
               aktiv_playlist.m_play_playlist(aktivplay_music_path,0);			// hent første sang
               if (strcmp(configsoundoutport,"STREAM")!=0) {
                 result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                 ERRCHECK(result,do_play_music_aktiv_table_nr);
               } else {
                 sprintf(aktivplay_music_path,"http://%s/mythweb/music/stream?i=%d",configmysqlhost,aktiv_playlist.get_songid(do_play_music_aktiv_table_nr-1));
                 result = sndsystem->createSound(aktivplay_music_path,FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                 ERRCHECK(result,do_play_music_aktiv_table_nr);
               }
               if (result==FMOD_OK) {
                 result = sndsystem->playSound( sound,NULL,false, &channel);
                 ERRCHECK(result,do_play_music_aktiv_table_nr);
                 if (sndsystem) channel->setVolume(configsoundvolume);
               }
               do_stop_music_all=false;						// fjern stop musik bremse
               if (result==FMOD_OK) snd=1;
               dsp=0;                             // reset uv to start
            }
            #endif
            #if defined USE_SDL_MIXER
            aktiv_playlist.m_play_playlist(aktivplay_music_path,0);                  // hent første sang
            sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
            Mix_PlayMusic(sdlmusicplayer, 0);
            do_stop_music_all=false;                                         // fjern stop musik bremse
            do_zoom_music_cover=true;
            if (sdlmusicplayer==NULL) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
            #endif
          } else {
            fprintf(stderr,"No aktiv play liste\n");
          }
          do_play_music_aktiv=false;
          do_zoom_music_cover=true;
          mknapnr=0;					// reset music knapnr (så vi kan modtage knapnr data igen)
          if (snd) {
              ;
          } else {
              fprintf(stderr,"ERROR Loading song.....: '%s' \n\n",aktivplay_music_path);
              fprintf(stderr,"Please check music path is readable '%s' \n",configmusicpath);
          }
        }
      }
    }
    // ******************************************************************************************************************
    // ******************************************************************************************************************
    // ******************************************************************************************************************
    // show we player music
    if ((vis_music_oversigt) && (!(visur))) {
      // spiller vi en sang vis status info i 3d   (do_play_music_aktiv=1 hvis der er status vindow
      if (do_zoom_music_cover) {
        //  printf("numbers of songs = %d aktiv song =%d in array  play position %d sec   \n",aktiv_playlist.numbers_in_playlist(),do_play_music_aktiv_nr,(snd->getPlayPosition())/1000);
        // show background mask
        int buttonsize = 800;
        int buttonsizey = 500;
        yof = 200;
        // background
        glPushMatrix();
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        if (do_stop_music_all) {						// SKAL checkes om gfx er ok
            glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);
        } else {
            glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);			//  _textureId1
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor4f(1.0f, 1.0f, 1.0f,1.00f);					// lav alpha blending. 80%
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4), yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),yof+buttonsizey, 0.0);
        glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+buttonsize, yof+buttonsizey , 0.0);
        glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+buttonsize,yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************ play
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _texturemplay);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(8);                        // play button
        glBegin(GL_QUADS);
        // last
        glTexCoord2f(0, 0); glVertex3f(20+ (orgwinsizex/4), 20+yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f(20+ (orgwinsizex/4),20+yof+100, 0.0);
        glTexCoord2f(1, 1); glVertex3f(20+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(20+ (orgwinsizex/4)+100,20+yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************ last
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _texturemlast2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(6);                                              // last button
        glBegin(GL_QUADS);
        // last
        glTexCoord2f(0, 0); glVertex3f(120+ (orgwinsizex/4),20+yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f(120+ (orgwinsizex/4),20+yof+100, 0.0);
        glTexCoord2f(1, 1); glVertex3f(120+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(120+ (orgwinsizex/4)+100,20+yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************ next
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _texturemnext);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(7);                     						   // next button
        glBegin(GL_QUADS);
        // last
        glTexCoord2f(0, 0); glVertex3f(220+ (orgwinsizex/4),20+yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f(220+ (orgwinsizex/4),20+yof+100, 0.0);
        glTexCoord2f(1, 1); glVertex3f(220+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(220+ (orgwinsizex/4)+100,20+yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************stop
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _texturemstop);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(9);                                                          // Stop button
        glBegin(GL_QUADS);
        // last
        glTexCoord2f(0, 0); glVertex3f(320+ (orgwinsizex/4),20+yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f(320+ (orgwinsizex/4),20+yof+100, 0.0);
        glTexCoord2f(1, 1); glVertex3f(320+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(320+ (orgwinsizex/4)+100,20+yof , 0.0);
        glEnd();
        glPopMatrix();
        // ************************************************************* draw cover
        // Draw music cover
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        textureId=aktiv_playlist.get_textureid(do_play_music_aktiv_table_nr-1);		// get cd texture opengl id
        if (textureId==0) textureId=_texture_nocdcover;		                       				// hvis ingen texture (music cover) set default (box2.bmp) / use default if no cover
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(9);                                                            // Set button id
        glBegin(GL_QUADS);
        // last
        glTexCoord2f(0, 0); glVertex3f(560+ (orgwinsizex/4),40+yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f(560+ (orgwinsizex/4),40+yof+200, 0.0);
        glTexCoord2f(1, 1); glVertex3f(560+ (orgwinsizex/4)+200, 40+yof+200 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(560+ (orgwinsizex/4)+200,40+yof , 0.0);
        glEnd();
        glPopMatrix();
        // show artist
        glDisable(GL_TEXTURE_2D);
        glPushMatrix();
        glColor3f(0.5f, 0.5f, 0.5f);
        glTranslatef(520.0f, 650.0f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);
        glcRenderString(music_artist[configland]);
        glPopMatrix();
        // show artist name
        glPushMatrix();
        glColor3f(0.6f, 0.6f, 0.6f);
        glTranslatef(700.0f, 650.0f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        aktiv_playlist.get_artistname(temptxt,do_play_music_aktiv_table_nr-1);
        temptxt[40]=0;
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);
        glcRenderString(temptxt);
        glPopMatrix();
        // show cd album name
        glPushMatrix();
        glTranslatef(520.0f, 630.0f, 0.0f);
        glColor3f(0.6f, 0.6f, 0.6f);
        strcpy(temptxt,music_album[configland]);                // music album text
        temptxt[50]=0;
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);
        glcRenderString(temptxt);
        glPopMatrix();
        // show album name
        glPushMatrix();
        glTranslatef(700.0f, 630.0f, 0.0f);
        glColor3f(0.6f, 0.6f, 0.6f);
        glRasterPos2f(0.0f, 0.0f);
        aktiv_playlist.get_albumname(temptxt,do_play_music_aktiv_table_nr-1);
        temptxt[40]=0;
        glScalef(20.5, 20.5, 1.0);
        glcRenderString(temptxt);
        glPopMatrix();
        // show song name
        glPushMatrix();
        glColor3f(0.6f, 0.6f, 0.6f);
        glTranslatef(520.0f, 610.0f, 0.0f);
        strcpy(temptxt,music_songname[configland]);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);
        glcRenderString(temptxt);
        glPopMatrix();
        char *pos;
        // show artist name
        glPushMatrix();
        glTranslatef(700.0f, 610.0f, 0.0f);
        // hent song name
        aktiv_playlist.get_songname(temptxt,do_play_music_aktiv_table_nr-1);
        pos=strrchr(temptxt,'/');
        if (pos>0) {
          strcpy(temptxt1,pos+1);
          strcpy(temptxt,temptxt1);
        }
        pos=strrchr(temptxt,'.');
        if (pos>0) {
          temptxt[pos-temptxt]='\0';
        }
        temptxt[40]=0;
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);
        glColor3f(0.6f, 0.6f, 0.6f);
        glcRenderString(temptxt);
        glPopMatrix();
        if (!(do_stop_music_all)) {
          // play position
          unsigned int ms = 0;
          float frequency;
          vis_error=false;
          if (vis_error==false)  {
            #if defined USE_FMOD_MIXER
            result=channel->getPosition(&ms, FMOD_TIMEUNIT_MS);		// get fmod audio info
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
              ERRCHECK(result,do_play_music_aktiv_table_nr);
            }
            // get play length new version
            result=sound->getLength(&playtime_songlength,FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
              ERRCHECK(result,do_play_music_aktiv_table_nr);
            }
            result=sound->getLength(&lenbytes,FMOD_TIMEUNIT_RAWBYTES);
            if (result!=FMOD_OK) {
              ERRCHECK(result,do_play_music_aktiv_table_nr);
            }
            #endif
            #if defined USE_SDL_MIXER
            ms = 0;
            playtime_songlength = 20000;
            lenbytes = 200;
            frequency = audio_rate;		// show rate
            //Mix_QuerySpec(int *frequency, Uint16 *format, int *channels);
            #endif
            // do the calc
            #if defined USE_FMOD_MIXER
            if ((playtime_songlength>0) && (result==FMOD_OK)) {
              kbps = (lenbytes/(playtime_songlength/1000)*8)/1000;			// calc bit rate
            } else {
              kbps=0;
            }
            if (result==FMOD_OK) {
              playtime_songlength = playtime_songlength/1000;
              playtime = ms/1000;
            } else {
              playtime_songlength = 0;
              playtime = ms/1000;
            }
            #endif
          } else if (vis_error) {
            kbps = 0;
            ms = 0;
            playtime_songlength=0;
            playtime = ms/1000;
          }
          glPushMatrix();
          glColor3f(0.6f, 0.6f, 0.6f);
          playtime_hour = (playtime/60)/60;
          playtime_min = (playtime/60);
          playtime_sec = playtime-(playtime_min*60);
          playtime_min = playtime_min-(playtime_hour*60);
          // old metode.
          playtime_length_min = playtime_songlength/60;
          playtime_length_sec = playtime_songlength-(playtime_length_min*60);
          // configland = lande kode
          strcpy(temptxt,music_play_time[configland]);
          temptxt[40] = 0;
          glTranslatef(520.0f, 580.0f, 0.0f);
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(temptxt);
          glPopMatrix();
          glPushMatrix();
          // show artist name
          sprintf(temptxt,"%02d:%02d/%02d:%02d ",playtime_min,playtime_sec,playtime_length_min,playtime_length_sec);
          temptxt[40]=0;
          glTranslatef(700.0f, 580.0f, 0.0f);
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(temptxt);
          glPopMatrix();
          #if defined USE_FMOD_MIXER
          channel->getFrequency(&frequency);
          #endif
          glPushMatrix();
          glColor3f(0.6f, 0.6f, 0.6f);
          glTranslatef(520.0f, 560.0f, 0.0f);
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(music_samplerate[configland]);
          glPopMatrix();
          glPushMatrix();
          glTranslatef(700.0f, 560.0f, 0.0f);
          // show artist name
          sprintf(temptxt,"%5.0f/%d Kbits.",frequency,kbps);
          temptxt[40]=0;
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(temptxt);
          glPopMatrix();
        }
        // play list status
        glPushMatrix();
        glColor3f(0.6f, 0.6f, 0.6f);
        glTranslatef(520.0f, 540.0f, 0.0f);
        temptxt[50]=0;
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        glcRenderString(music_playsong[configland]);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(700.0f, 540.0f, 0.0f);
        // show artist name
        // format as %d/%d in playlist
        sprintf(temptxt,music_numberinplaylist[configland],do_play_music_aktiv_table_nr,aktiv_playlist.numbers_in_playlist());
        temptxt[40]=0;
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        glcRenderString(temptxt);
        glPopMatrix();
        // show next song in status window
        glColor3f(0.6f, 0.6f, 0.6f);
        if ((do_play_music_aktiv_table_nr)<aktiv_playlist.numbers_in_playlist()) {
          strcpy(temptxt,music_nextsong[configland]);
          temptxt[52]=0;
          glPushMatrix();
          glTranslatef(520.0f, 520.0f, 0.0f);
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(temptxt);
          glPopMatrix();
          glPushMatrix();
          glTranslatef(700.0f, 520.0f, 0.0f);
          aktiv_playlist.get_songname(temptxt,do_play_music_aktiv_table_nr);
          pos=strrchr(temptxt,'/');
          if (pos>0) {
            strcpy(temptxt1,pos+1);
            strcpy(temptxt,temptxt1);
          }
          pos=strrchr(temptxt,'.');
          if (pos>0) temptxt[pos-temptxt]='\0';
          temptxt[40]=0;
          glRasterPos2f(0.0f, 0.0f);
          glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
          glcRenderString(temptxt);
          glPopMatrix();
        }
      }
    }
    //
    // *************** Spotify show play stuf **********************************************************
    //
    #ifdef ENABLE_SPOTIFY
    if ((vis_spotify_oversigt) && (do_zoom_spotify_cover) && (!(visur))) {
      do_we_play_check=0;
      if (do_we_play_check==0) {
        spotify_oversigt.spotify_do_we_play();
      }
      do_we_play_check++;
      // check again ?
      if (do_we_play_check>50) do_we_play_check=0;
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      // window texture
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4) ,  300 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4),400+300, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+640,400+300 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+640,300, 0.0);
      glEnd();
      // spotify play info icon
      glEnable(GL_BLEND);
      if (spotify_oversigt.get_texture(spotifyknapnr))
        if (spotifyknapnr-1>0) glBindTexture(GL_TEXTURE_2D,spotify_oversigt.get_texture(spotifyknapnr-1));                        // get playlist conver icon
      else
        glBindTexture(GL_TEXTURE_2D,spotify_ecover);                                                                              // else default icon

      if (spotify_oversigt.aktiv_song_spotify_icon) glBindTexture(GL_TEXTURE_2D,spotify_oversigt.aktiv_song_spotify_icon);        // set active icon

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+395 ,   370 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+395,200+370, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+395+200,200+370 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+395+200,370, 0.0);
      glEnd();
      // backward button
      glEnable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D,_texturemlast);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(10);                        // 10 = forward(10)
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+50 ,  320 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+50,100+320, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+50+100,100+320 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+50+100,320, 0.0);
      glEnd();
      // stop button
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D,_texturemstop);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(9);                        // 9 = stop
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+150 ,  320 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+150,100+320, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+150+100,100+320 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+150+100,320, 0.0);
      glEnd();
      // forward button
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D,_texturemnext);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(11);                        // 10 = forward(10)
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+250 ,  320 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+250,100+320, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+250+100,100+320 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+250+100,320, 0.0);
      glEnd();

      // play list name or artist name
      // show playlist or artist name
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 640.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);
      if (spotify_oversigt.get_spotify_type(spotifyknapnr)==0) {
        strcpy(temptxt1,"playlist  ");
        glcRenderString(temptxt1);
      } else {
        strcpy(temptxt,"Artist    ");
        glcRenderString(temptxt);
      }
      glPopMatrix();


      int textofset=140;

      // show value
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f+textofset, 640.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);
      if (spotify_oversigt.get_spotify_type(spotifyknapnr)==0) {
        strcpy(temptxt1,spotify_oversigt.spotify_playlistname);
        *(temptxt1+46)=0;
        glcRenderString(temptxt1);
      } else {
        strcpy(temptxt,spotify_oversigt.get_spotify_artistname(spotifyknapnr));
        glcRenderString(temptxt);
      }
      glPopMatrix();

      // show songname
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 620.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString("Songname ");
      glPopMatrix();

      // show songname value
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f+textofset, 620.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      sprintf(temptxt1,"%s",(char *) spotify_oversigt.spotify_aktiv_song_name());
      temptxt1[40]=0;
      glcRenderString(temptxt1);
      glPopMatrix();

      // show artist
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 600.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      if (spotify_oversigt.get_spotify_type(spotifyknapnr)==0) {
        glcRenderString("Artist    ");
      } else {
        glcRenderString("Album     ");
      }
      glPopMatrix();

      // show artist value
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f+textofset, 600.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      if (spotify_oversigt.get_spotify_type(spotifyknapnr)==0) {
        sprintf(temptxt1,"%s",(char *) spotify_oversigt.spotify_aktiv_artist_name());
        glcRenderString(temptxt1);
      } else {
        //glcRenderString("Album");
        //sprintf(temptxt1,"%s",(char *) );
        //glcRenderString(temptxt1);
      }
      glPopMatrix();

      // player device
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 580.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);
      // show active play device
      glcRenderString("Player    ");
      glPopMatrix();

      // player device value
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f+textofset, 580.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);
      // show active play device
      if (spotify_oversigt.active_spotify_device>-1) {
        glcRenderString(spotify_oversigt.get_active_spotify_device_name());
      }
      glPopMatrix();

      // player play status background
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBindTexture(GL_TEXTURE_2D,0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor4f(0.7f, 0.41f, 1.0f, 0.6f);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+170 ,        555-20 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+170 ,     19+555-20 , 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+170+200,  19+555-20 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+170+200,     555-20 , 0.0);
      glEnd();
      glPopMatrix();
      // show playtime as gfx box
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 540.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      sprintf(temptxt1,"playtime  ");
      glcRenderString(temptxt1);
      glPopMatrix();
      glPushMatrix();
      glColor3f(1.0f, 1.0f, 1.0f);
      int statuswxpos = 432;
      int statuswypos = 557-20;
      float y=spotify_oversigt.spotify_aktiv_song_msplay()/1000;
      float ll=spotify_oversigt.spotify_aktiv_song_mslength()/1000;
      int xxx;
      if ((y>0) && (ll>0)) {
        xxx = ((y/ll)*16);
      } else xxx=0;
      for(int x=0;x<xxx;x++) {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(statuswxpos+222+(x*12), statuswypos , 0.0);
        glTexCoord2f(0, 1); glVertex3f(statuswxpos+222+(x*12), statuswypos+(15), 0.0);
        glTexCoord2f(1, 1); glVertex3f(statuswxpos+222+(10)+(x*12), statuswypos+(15) , 0.0);
        glTexCoord2f(1, 0); glVertex3f(statuswxpos+222+(10)+(x*12), statuswypos , 0.0);
        glEnd();
      }
      glPopMatrix();

      // updated date on spotify
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f, 560.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      glcRenderString("Release   ");
      glPopMatrix();

      // updated date on spotify value
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(520.0f+textofset, 560.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
      sprintf(temptxt1,"%s",spotify_oversigt.spotify_aktiv_song_release_date());
      glcRenderString(temptxt1);
      glPopMatrix();


    }
    #endif
    //
    // *************** Stream stuf *******************************************************************************
    // show stream player control
    // hvis show_stream_oversigt or zoom
    if (!(visur)) {
      if ((vis_stream_oversigt) && (do_zoom_stream_cover)) {
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        // window texture
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4) ,  300 , 0.0);
        glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4),400+300, 0.0);
        glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+640,400+300 , 0.0);
        glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+640,300, 0.0);
        glEnd();
        // play button
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        // show play pause icon
        if (streamoversigt.stream_is_pause) glBindTexture(GL_TEXTURE_2D,_texturemplay);
        else glBindTexture(GL_TEXTURE_2D,_texturempause);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (streamoversigt.stream_is_playing) glLoadName(12);                        // 12 = pause
        else glLoadName(8);                                                          // 8 = play
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+50 ,  320 , 0.0);
        glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+50,100+320, 0.0);
        glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+50+100,100+320 , 0.0);
        glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+50+100,320, 0.0);
        glEnd();
        // stop button
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,_texturemstop);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(9);                        // 9 = stop
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+150 ,  320 , 0.0);
        glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+150,100+320, 0.0);
        glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+150+100,100+320 , 0.0);
        glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+150+100,320, 0.0);
        glEnd();
        // backward button
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,_texturemlast);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(10);                        // 10 = forward(10)
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+250 ,  320 , 0.0);
        glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+250,100+320, 0.0);
        glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+250+100,100+320 , 0.0);
        glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+250+100,320, 0.0);
        glEnd();
        // forward button
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,_texturemnext);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(11);                        // 10 = forward(10)
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+350 ,  320 , 0.0);
        glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+350,100+320, 0.0);
        glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+350+100,100+320 , 0.0);
        glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+350+100,320, 0.0);
        glEnd();
        glPushMatrix();
        glTranslatef((orgwinsizex/4)+20,(orgwinsizey/2)+96, 0.0f);
        glScalef(20,20, 1.0);                    // danish charset ttf
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glDisable(GL_TEXTURE_2D);
        sprintf(temptxt,"Name      %-20s",stream_playing_name);
        temptxt[26]=0;
        glcRenderString(temptxt);
        glPopMatrix();
        // play position
        if (streamoversigt.stream_is_playing) playtime=streamoversigt.getstream_pos()*1000;
        else playtime=0;
        playtime_hour=(playtime/60)/60;
        playtime_min=(playtime/60);
        playtime_sec=(int) playtime % 60;
        glPushMatrix();
        glColor3f(0.6f, 0.6f, 0.6f);
        // show artist name
        if (streamoversigt.stream_is_playing) sprintf(temptxt,"Playing    %02d:%02d:%02d ",playtime_hour,playtime_min,playtime_sec);
        else sprintf(temptxt,"                                        ");
        temptxt[40]=0;
        glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+48+20, 0);
        //glTranslatef((orgwinsizex/4)+20,(orgwinsizey/2)+96, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        glcRenderString(temptxt);
        glPopMatrix();
        glPushMatrix();
        glTranslatef((orgwinsizex/4)+20,(orgwinsizey/2)+0, 0);
        glScalef(20,20, 1.0);                    // danish charset ttf
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glDisable(GL_TEXTURE_2D);
        sprintf(temptxt,"%-30s",stream_playing_desc);
        temptxt[30]='\0';
        glcRenderString(temptxt);
        glPopMatrix();
        // get stream texture
        if (stream_playing_icon) {
          glEnable(GL_TEXTURE_2D);
          glEnable(GL_BLEND);
          glBindTexture(GL_TEXTURE_2D,stream_playing_icon);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+400 ,  480 , 0.0);
          glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+400 ,  200+480, 0.0);
          glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+400+200 , 200+480 , 0.0);
          glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+400+200 , 480, 0.0);
          glEnd();
        }
      }
    }
    //
    // *************** RADIO stuf *******************************************************************************
    // show radio player
    if (!(visur)) {
      if (vis_radio_oversigt) {
        // show playing radio station
        if ((snd) && (do_zoom_radio)) {
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);
            // window texture
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4) ,  300 , 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4),400+300, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+640,400+300 , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+640,300, 0.0);
            glEnd();
            // play button
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D,_texturemplay);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(8);                        // 8 = play
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+50 ,  320 , 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+50,100+320, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+50+100,100+320 , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+50+100,320, 0.0);
            glEnd();
            // stop button
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D,_texturemstop);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(9);                        // 9 = stop
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+150 ,  320 , 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+150,100+320, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+150+100,100+320 , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+150+100,320, 0.0);
            glEnd();
            // ff button
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D,_texturemlast);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(8);                        // 8 = play
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+250 ,  320 , 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+250,100+320, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+250+100,100+320 , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+250+100,320, 0.0);
            glEnd();
            // back button
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D,_texturemnext);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(8);                        // 8 = play
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+350 ,  320 , 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+350,100+320, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+350+100,100+320 , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+350+100,320, 0.0);
            glEnd();
            // get radio station texture
            textureId=radiooversigt.get_texture(aktiv_radio_station);                           // get radio texture opengl id
            if (textureId) {
              // radio icon big size
              glEnable(GL_TEXTURE_2D);
              glEnable(GL_BLEND);
              // if (textureId==0) textureId=onlineradio;                                            // hvis ingen texture (music cover) set default
              glBindTexture(GL_TEXTURE_2D,textureId);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glBegin(GL_QUADS);
              glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+400 ,  480 , 0.0);
              glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+400 ,  200+480, 0.0);
              glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+400+200 , 200+480 , 0.0);
              glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+400+200 , 480, 0.0);
              glEnd();
            }
            if (snd) {
              // play position
              unsigned int ms = 0;
              float frequency;
              #if defined USE_FMOD_MIXER
              result=channel->getPosition(&ms, FMOD_TIMEUNIT_MS);		// get fmod audio info
              if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
                ERRCHECK(result,0);
              }
              // get play length new version
              result=sound->getLength(&radio_playtime_songlength,FMOD_TIMEUNIT_MS);
              if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
                ERRCHECK(result,do_play_music_aktiv_table_nr);
              }
              result=sound->getLength(&lenbytes,FMOD_TIMEUNIT_RAWBYTES);
              if (result!=FMOD_OK) {
                ERRCHECK(result,0);
              }
              // do the calc
              if (result==FMOD_OK) {
                //playtime_songlength=playtime_songlength/1000;
                radio_playtime=ms/1000;
              } else {
                radio_playtime_songlength=0;
                radio_playtime=0;
              }
              #endif
              #if defined USE_SDL_MIXER
              #endif
              glPushMatrix();
              glDisable(GL_TEXTURE_2D);
              // show song name
              sprintf(temptxt,"Song Name ");
              temptxt[22]=0;
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+120, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+120, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+120, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+200, 0);
                        break;
                default:glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+120, 0);
              }
              glScalef(20,20, 1.0);                    // danish charset ttf
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glcRenderString(temptxt);
              glPopMatrix();
              //
              glPushMatrix();
              glDisable(GL_TEXTURE_2D);
              // show song name
              sprintf(temptxt," %-30s",aktivsongname);
              temptxt[22]=0;
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+120, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+120, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+120, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+200, 0);
                        break;
                default:glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+120, 0);
              }
              glScalef(20,20, 1.0);                    // danish charset ttf
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              // show station name
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+80, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+80, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+80, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+160, 0);
                        break;
                default:glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+80, 0);
              }
              sprintf(temptxt,"Station ");
              temptxt[39]=0;
              glScalef(20,20, 1.0);
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              // show station name
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+80, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+80, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+80, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+160, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+80, 0);
              }
              sprintf(temptxt," %s",radiooversigt.get_station_name(aktiv_radio_station));
              temptxt[39]=0;
              glScalef(20,20, 1.0);
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              // play time
              radio_playtime_hour=(radio_playtime/60)/60;
              radio_playtime_min=(radio_playtime/60);
              radio_playtime_sec=radio_playtime-(radio_playtime_min*60);
              radio_playtime_min=radio_playtime_min-(radio_playtime_hour*60);
              if (radio_playtime_min>60) radio_playtime_min=0;
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+60, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+60, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+60, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+140, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+60, 0);
              }
              sprintf(temptxt,"%s",music_timename[1]);       // 1 = danish
              temptxt[40]=0;
              glTranslatef(1, 1, 0);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glcRenderString(temptxt);                // write text
              glPopMatrix();
              glPushMatrix();
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+60, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+60, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+60, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+140, 0);
                        break;
                default:  glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+60, 0);
              }
              glTranslatef(1, 1, 0);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              sprintf(temptxt," %02d:%02d:%02d ",radio_playtime_hour,radio_playtime_min,radio_playtime_sec);
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              // speed info
              frequency=192;
              sprintf(temptxt,"Bitrate ");
              temptxt[40]=0;
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+40, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+40, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+40, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+120, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+40, 0);
              }
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              // speed info
              frequency=192;
              sprintf(temptxt," %3.0f Kbits",frequency);
              temptxt[40]=0;
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+40, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+40, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+40, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+120, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+40, 0);
              }
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+20, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+20, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+20, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+100, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+20, (orgwinsizey/2)+20, 0);
              }
              // song status / loading
              sprintf(temptxt,"Status ");
              temptxt[40]=0;
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glRasterPos2f(120.f, 100.f);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glcRenderString(temptxt);
              glPopMatrix();
              glPushMatrix();
              switch (screen_size) {
                case 1: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+20, 0);
                        break;
                case 2: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+20, 0);
                        break;
                case 3: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+20, 0);
                        break;
                case 4: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+100, 0);
                        break;
                default: glTranslatef((orgwinsizex/4)+140, (orgwinsizey/2)+20, 0);
              }
              // song status / loading
              sprintf(temptxt," %-20s",aktivsongstatus);
              temptxt[40]=0;
              glColor4f(1.0f,1.0f,1.0f,1.0f);
              glRasterPos2f(120.f, 100.f);
              glScalef(20,20, 1.0);                    // danish charset ttf
              glcRenderString(temptxt);
              glPopMatrix();
            }
          }
        }
        // Show setup stuf windows
        if (do_show_setup) {
          // reset color to nomal after uv
          glColor4f(1.0,1.0,1.0,1.0);
          show_setup_interface();                                             // show setup interface
          if (do_show_setup_sound) show_setup_sound();                        // sound device
          if (do_show_setup_screen) show_setup_screen();                      //
          if (do_show_videoplayer) show_setup_video();                        //
          if (do_show_setup_sql) show_setup_sql();                            //
          if (do_show_setup_tema) show_setup_tema();                          // select tema
          if (do_show_setup_network) {                                        //
            show_setup_network();
            if (show_wlan_select) {
              show_wlan_networks((int) setupwlanselectofset);                         // show wlan list in opengl
            }
          }
          if (do_show_setup_font) show_setup_font(setupfontselectofset);      // show setup font
          if (do_show_setup_keys) show_setup_keys();                          // Function keys
          if (do_show_tvgraber) show_setup_tv_graber(tvchannel_startofset);   // tv graber
          if (do_show_setup_rss) show_setup_rss(configrss_ofset);             // podcast rss feeds source
          #ifdef ENABLE_SPOTIFY
          if (do_show_setup_spotify) spotify_oversigt.show_setup_spotify();   // spotify
          #endif
        }
        if (vis_tv_oversigt) {
          // F1 in tv_guide view
          if (do_show_tvgraber) show_setup_tv_graber(tvchannel_startofset);   // tv graber
        }
        glPopMatrix();
    }
    // end radio stuf
    // create uv meter
    if ((snd) && (show_uv)) vis_uv_meter=true;
    //
    // calc spectrum
    // from fmod
    #if defined USE_FMOD_MIXER
    if (snd) {
      // getSpectrum() performs the frequency analysis, see explanation below
      sampleSize = 1024;                // nr of samples default 64
      // uv works only on fmod for now
      FMOD_DSP_PARAMETER_FFT *fft = 0;
      int chan;
      static bool build_frequencyOctaves=false;
      if (build_frequencyOctaves==false) {
        build_frequencyOctaves = true;
        for(int zz=0;zz<sampleSize;zz++) {
          spectrum[zz] = 0.0f;
          spectrum_left[zz] = 0.0f;                                             // used for spectium
          spectrum_right[zz] = 0.0f;                                            // used for spectium
          uvmax_values[zz] = 0.0f;                                              // used for spectium
        }
        for (int i=0;i<13;i++) frequencyOctaves[i]=(int) (44100/2)/(float) pow(2,12-i);
      }
      FMOD::ChannelGroup *mastergroup;
      if (!(dsp)) {
        sndsystem->getMasterChannelGroup(&mastergroup);
        sndsystem->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
        dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE,FMOD_DSP_FFT_WINDOW_TRIANGLE);
        dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, sampleSize);
        mastergroup->addDSP(0, dsp);
        //channel->addDSP(FMOD_DSP_PARAMETER_DATA_TYPE_FFT, dsp);
        dsp->setActive(true);
      }
      result=dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);
      if (result!=FMOD_OK) fprintf(stderr,"Error DSP %s\n",FMOD_ErrorString(result));
      int length = fft->length/2;
      int numChannels = fft->numchannels;
      if (fft) {
        // new ver 4
        for (int i=0; i<fft->length; i++) {
          float spectum_value=(fft->spectrum[0][i]*40)+(fft->spectrum[1][i]*40);
          spectrum[i] = spectum_value;
          spectrum_left[i] = spectum_value;
          spectrum_right[i] = spectum_value;
          if (spectum_value > uvmax_values[i]) uvmax_values[i] = spectum_value;
          else if (uvmax_values[i] > 0.0f) uvmax_values[i] = uvmax_values[i] - 1.00f;
          else uvmax_values[i] = spectum_value;
        }
      }
    }
    #endif

    //
    // show uv metter in music player
    //
    if (((snd) && (visur==false) && (vis_uv_meter) && (configuvmeter) && ((radio_pictureloaded) && (vis_radio_oversigt))) || (vis_music_oversigt) || (vis_radio_or_music_oversigt)) {
      // draw uv meter
      int high = 2;
      int qq = 1;
      int uvypos = 0;
      if (((configuvmeter==1) || (configuvmeter==3)) && (screen_size!=4)) {
        glPushMatrix();
        winsizx = 16;
        winsizy = 16;
        int xpos = 1350;
        int ypos = 10;
        // uv
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D,texturedot);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        for(qq=0;qq<16;qq++) {
          ypos = 10;
          high = sqrt(spectrum[qq]*2);
          high += 1;
          if (high>14) high=14;
          for(i=0;i<high;i++) {
            // uv
            switch(i) {
              case 0: glColor4f(uvcolortable2[0],uvcolortable2[1],uvcolortable2[2],1.0);
                break;
                case 1: glColor4f(uvcolortable2[3],uvcolortable2[4],uvcolortable2[5],1.0);
                break;
                case 2: glColor4f(uvcolortable2[6],uvcolortable2[7],uvcolortable2[8],1.0);
                break;
                case 3: glColor4f(uvcolortable2[9],uvcolortable2[10],uvcolortable2[11],1.0);
                break;
                case 4: glColor4f(uvcolortable2[12],uvcolortable2[13],uvcolortable2[14],1.0);
                break;
                case 5: glColor4f(uvcolortable2[15],uvcolortable2[16],uvcolortable2[17],1.0);
                break;
                case 6: glColor4f(uvcolortable2[18],uvcolortable2[19],uvcolortable2[20],1.0);
                break;
                case 7: glColor4f(uvcolortable2[21],uvcolortable2[22],uvcolortable2[23],1.0);
                break;
                case 8: glColor4f(uvcolortable2[24],uvcolortable2[25],uvcolortable2[26],1.0);
                break;
                case 9: glColor4f(uvcolortable2[27],uvcolortable2[28],uvcolortable2[29],1.0);
                break;
                case 10:glColor4f(uvcolortable2[30],uvcolortable2[31],uvcolortable2[32],1.0);
                break;
                case 11:glColor4f(uvcolortable2[33],uvcolortable2[34],uvcolortable2[35],1.0);
                break;
                case 12:glColor4f(uvcolortable2[36],uvcolortable2[37],uvcolortable2[38],1.0);
                break;
                case 13:glColor4f(uvcolortable2[39],uvcolortable2[40],uvcolortable2[41],1.0);
                break;
                case 14:glColor4f(uvcolortable2[42],uvcolortable2[43],uvcolortable2[44],1.0);
                break;
                default:glColor4f(uvcolortable2[0],uvcolortable2[1],uvcolortable2[2],1.0);
                break;
            }
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos,ypos , 0.0);
            glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos,ypos+winsizy , 0.0);
            glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos+winsizx,ypos+winsizy , 0.0);
            glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos+winsizx,ypos , 0.0);
            glEnd();
            ypos = ypos + 16;
          }
          uvypos += 14;
        }
        //
        uvypos=0;
        float siz_x = 6.0f;
        float siz_y = 4.0f;
        xpos = 1350;
        ypos = 10;
        for(qq=0;qq<16;qq++) {
          high = sqrt(uvmax_values[qq]*18);
          ypos = 10+(siz_y*high);
          for(i=0;i<1;i++) {
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D,texturedot1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos,ypos , 0.0);
            glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos,ypos+siz_y , 0.0);
            glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos+winsizx,ypos+siz_y , 0.0);
            glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+uvypos+winsizx,ypos , 0.0);
            glEnd();
          }
          uvypos += 14;
        }
        glPopMatrix();
      } else if ((configuvmeter==2) && (screen_size!=4)) {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D,texturedot);                              // texturedot
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int uvypos = 0;
        int uvyypos = 0;
        int high;
        int qq;
        // Draw uv lines
        for(qq=0;qq<32;qq++) {
          uvypos = 0;
          uvyypos = 0;
          high=sqrt(spectrum_left[qq]*2);
          if (high>7) high = 6;
          // draw 1 bar
          for(i=0;i<high;i+=1) {
            switch(i) {
                case 1:
                case 2:
                case 3:
                case 4:
                        glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                        //glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
                        break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                      glColor4f(uvcolortable1[1],uvcolortable1[37],uvcolortable1[1],1.0);
                      //glBindTexture(GL_TEXTURE_2D,_textureuv1_top);         //texturedot)
                      break;
                default:
                      glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                      //glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
                      break;
            }
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+1250 +(qq*6),  120+4 +uvypos, 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+1250 +(qq*6),  120+14+4+uvypos, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+1250+5 +(qq*6),  120+14+4+uvypos , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+1250+5 +(qq*6),  120+4+uvypos, 0.0);
            glEnd();
            uvypos+=16;
          }

          high = sqrt(spectrum_right[qq]*2);
          if (high > 7) high = 6;
          for(i=0;i<high;i+=1) {
            switch(i) {
                case 1:
                case 2:
                case 3:
                case 4:
                        glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                        glBindTexture(GL_TEXTURE_2D,texturedot1);         //texturedot);
                        break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                        glColor4f(uvcolortable1[1],uvcolortable1[37],uvcolortable1[1],1.0);
                        glBindTexture(GL_TEXTURE_2D,texturedot1);         //texturedot)
                        break;
                default:
                        glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                        glBindTexture(GL_TEXTURE_2D,texturedot1);         //texturedot);
                        break;
            }
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+1250 +(qq*6),  120+4 -uvyypos, 0.0);
            glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+1250 +(qq*6),  120+14+4-uvyypos, 0.0);
            glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+1250+5 +(qq*6),  120+14+4-uvyypos , 0.0);
            glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+1250+5 +(qq*6),  120+4-uvyypos, 0.0);
            glEnd();
            uvyypos+=16;
          }
        }
        // done uv stuf
        glPopMatrix();
      }
      // do clean up after uv meters
    }
    // load new team gfx files from config
    if (do_save_config) {
      do_save_config = false;
      if (save_config((char *) "/etc/mythtv-controller.conf")==0) {
        fprintf(stderr,"Error saving config file mythtv-controller.conf\n");
      } else fprintf(stderr,"Saving config ok.\n");
      // crash crash
      //rssstreamoversigt.save_rss_data();                                        // save rss data in db
      // load all new textures
      // free all loaded menu + icon gfx
      //freegfx();                                                                // free gfx loaded
      //loadgfx();                                                                // reload all menu + icon gfx
    }
    //
    // show update if rss podcast
    // update rss db
    //
    if (do_save_setup_rss) {
      if (debugmode) fprintf(stderr,"Saving rssdb to mysql\n");
      write_logfile("Saving rssdb to mysql.");
      rssstreamoversigt.save_rss_data();                                        // save rss data in db
      streamoversigt.loadrssfile(1);                                            // download/update rss files (1=force all)
      do_save_setup_rss=false;
    }
    // do start movie player
    if ((startmovie) && (do_zoom_film_cover)) {
      // non default player
      if (strcmp("default",configdefaultplayer)!=0)  {
        // write debug log
        sprintf(debuglogdata,"Start movie nr %d Player is vlc path :%s ",fknapnr,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());
        write_logfile((char *) debuglogdata);
        strcpy(systemcommand,"/bin/sh /usr/bin/startmovie.sh ");
        strcat(systemcommand,"'");
        strcat(systemcommand,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());      // old strcat(systemcommand,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmfilename());
        strcat(systemcommand,"'");
        system(systemcommand);
      } else {
        // default
        // start internal player (vlc)
        sprintf(debuglogdata,"Start play use default player film nr: %d name: %s ",fknapnr,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());
        // write to log file
        write_logfile((char *) debuglogdata);
        // if we play music/stream (radio) stop that before play movie stream (vlc)
        // stop music if play before start movie
        // write to log
        write_logfile((char *) "Stop playing music/radio.");
        #if defined USE_FMOD_MIXER
        if ((sound) && (snd)) {
          // stop sound playing
          result=channel->stop();                         // stop fmod player
          // release sound system again
          result=sound->release();
          // mp uv meters
          dsp=0;
        }
        #endif
        #if defined USE_SDL_MIXER
        Mix_PauseMusic();
        Mix_FreeMusic(sdlmusicplayer);                  // stop SDL player
        sdlmusicplayer=NULL;
        #endif
        // no play sound flag
        #if defined USE_FMOD_MIXER
        snd = 0;
        sound = 0;
        #endif
        // clean music playlist
        aktiv_playlist.clean_playlist();                // clean play list (reset) play list
        do_play_music_aktiv_table_nr=1;			// reset play start nr
        // write to log
        write_logfile((char *) "Stop playing media/wideo if any");
        if (film_oversigt.film_is_playing) {
          //write to debug log
          write_logfile((char *) "Stop playing last movie before start new");
          // stop playing (active movie)
          film_oversigt.softstopmovie();
        }
        // start movie
        if (film_oversigt.playmovie(fknapnr-1)==0) {
          vis_error = true;
          vis_error_timeout = 60;
        }
      }
      startmovie = false;                   // start kun 1 instans
    }
    if (stopmovie) {
      // write debug log
      write_logfile((char *) "Stop movie");
      //sleep(10); // play
      if (strcmp("default",configdefaultplayer)!=0) {
        // close non default player
        // neeed to be coded
      } else {
        // close default player (vlc plugin)
        film_oversigt.stopmovie();
      }
      //stop do it again next loop
      stopmovie = false;
    }
    // start play stream
    // still use the old system call bach file
    if (startstream) {
      if ((do_play_stream) && (stream_playnr==0)) {
        do_zoom_stream_cover=true;
        stream_playnr=sknapnr;                                                  // rember the stream we play
        strcpy(stream_playing_name,streamoversigt.get_stream_name(stream_playnr-1));
        strcpy(stream_playing_desc,streamoversigt.get_stream_desc(stream_playnr-1));
        stream_playing_icon=streamoversigt.get_texture(stream_playnr-1);
      }
      if (strcmp("default",configdefaultplayer)!=0)  {
        fprintf(stderr,"Start stream nr %d Player is firefox \n",sknapnr);
        strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
        strcat(systemcommand,"'");
        if (sknapnr>0) {
          if (strncmp(streamoversigt.get_stream_url(sknapnr),"mythflash:",10)==0) {
            strcat(systemcommand,"http://");
            strcat(systemcommand,streamoversigt.get_stream_url(sknapnr)+10);
          } else strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1));
          strcat(systemcommand,"' &");
          if (system(systemcommand)!=0) {
            vis_error=true;
            vis_error_timeout=60;
          }
        }
      } else {
        // start play stream or show rss page
        // write to log file
        sprintf(debuglogdata,"Stream to play %s ",streamoversigt.get_stream_url(sknapnr-1));
        write_logfile((char *) debuglogdata);
        sprintf(debuglogdata,"Start stream. Player is internal ");
        write_logfile((char *) debuglogdata);
        // stop playing stream
        if (streamoversigt.stream_is_playing) {
          streamoversigt.stopstream();
        }
        if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"https://www.youtube.com/watch?v=",32)==0) {
          // play by firefox
          if (sknapnr>0) {
            strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
            strcat(systemcommand,"'");
            char *p=strstr(streamoversigt.get_stream_url(sknapnr-1),"watch?v=");
            if (p) {
              strcpy(temprgtxt,"https://www.youtube.com/embed/");
              strcat(temprgtxt,p+8);                                            // add/get video id
              strcat(temprgtxt,"?autoplay=1");
            } else strcpy(temprgtxt,streamoversigt.get_stream_url(sknapnr-1));
            strcat(systemcommand,temprgtxt);
            strcat(systemcommand,"' &");
            if (system(systemcommand)!=0) {
              vis_error=true;
              vis_error_timeout=60;
            }
          }
          // update db
          streamoversigt.update_rss_nr_of_view(streamoversigt.get_stream_url(sknapnr-1));
          // set played flag
          streamoversigt.set_rss_new(sknapnr-1,false);
        } else {
          // update db
          streamoversigt.update_rss_nr_of_view(streamoversigt.get_stream_url(sknapnr-1));
          // set played flag
          streamoversigt.set_rss_new(sknapnr-1,false);
          // start playing stream by libvlc
          streamoversigt.playstream_url(streamoversigt.get_stream_url(sknapnr-1));
        }
      }
      // reset play function to new select
      startstream = false;                      // start kun 1 instans
      do_play_stream = false;                   //
      stream_playnr = sknapnr;                  //
    }
    // stop stream if play
    //
    if (do_stop_stream) {
      if (stopstream) {
        stopstream = false;                     // start kun 1 instans
        stream_playnr = 0;
        strcpy(stream_playing_name,"");
        strcpy(stream_playing_desc,"");
        stream_playing_icon = 0;
        if (streamoversigt.stream_is_playing) {
          // write debug log
          sprintf(debuglogdata,"Stop playing stream");
          write_logfile((char *) debuglogdata);
          // stop playing (active movie)
          //film_oversigt.softstopmovie();
        }
      }
    }
    // set play on pause rss stream
    if (do_pause_stream) {
      streamoversigt.pausestream(1);
      do_pause_stream = false;
    }
    // play recorded program
    if (do_play_recorded_aktiv_nr) {
      // write debug log
      sprintf(debuglogdata,"Start playing recorded program");
      write_logfile((char *) debuglogdata);

      if (strcmp("default",configdefaultplayer)!=0) {
        strcpy(systemcommand,"./startrecorded.sh ");
        recordoversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
        //strcat(systemcommand,configrecordpath);
        strcat(systemcommand,temptxt);
        if (debugmode & 64) {
          sprintf(debuglogdata,"Start command :%s ",systemcommand);
          write_logfile((char *) debuglogdata);
        }
        system(systemcommand);
        do_play_recorded_aktiv_nr=0;                                                                // start kun 1 player
      } else {
        // write debug log
        write_logfile((char *) "Start default playing recorded program");
        strcpy(systemcommand,"./startrecorded.sh ");
        recordoversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
        //strcat(systemcommand,configrecordpath);
        strcat(systemcommand,temptxt);
        // write debug log
        sprintf(debuglogdata,"Start command :%s ",systemcommand);
        write_logfile((char *) debuglogdata);
        system(systemcommand);
        do_play_recorded_aktiv_nr=0;                                                                // start kun 1 player
      }
    }
    //
    // show movie info
    //
    if (((vis_nyefilm_oversigt) || (vis_film_oversigt)) && (do_zoom_film_cover) && (fknapnr>0) && (!(visur))) {
      do_zoom_film_aktiv_nr=fknapnr-1;
      if ((file_exists(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmfcoverfile())) && (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfronttextureid()==0)) {
        film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].loadfronttextureidfile();
      }
      if ((file_exists(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmbcoverfile())) && (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getbacktextureid()==0)) {
        film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].loadbacktextureidfile();
      }
      // draw window
      glPushMatrix();
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glTranslatef(400,400,0);
      glEnable(GL_TEXTURE_2D);
      //glBlendFunc(GL_DST_COLOR, GL_ZERO);
      glEnable(GL_BLEND);
      //glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, _texturemovieinfobox);        //
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 0, 0 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( 0, 0+550, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 0+800, 0+550 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( 0+800, 0 , 0.0);
      glEnd();
      glPopMatrix();
      // show play movie icon
      glPushMatrix();
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glTranslatef(400+20,400+20,0);
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, _texturemplay);        //
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(25);                                                             // icon nr for play movie
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 0, 0 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( 0, 0+65+10, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 0+80+10, 0+65+10 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( 0+80+10, 0 , 0.0);
      glEnd();
      glPopMatrix();
      // show stop movie icon
      glPushMatrix();
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glTranslatef(490+20,400+20,0);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, _texturemstop);        //
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(25);                                                             // icon nr for stop movie
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 0, 0 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( 0, 0+65+10, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 0+80+10, 0+65+10 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( 0+80+10, 0 , 0.0);
      glEnd();
      glPopMatrix();
      glDisable(GL_BLEND);
      // show movie dvd cover
      glPushMatrix();
      glBindTexture(GL_TEXTURE_2D,_defaultdvdcover);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glTranslatef(420,600,0);
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_DST_COLOR, GL_ZERO);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 0, 100 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( 0, 0+320, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 0+220, 0+320 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( 0+220, 100 , 0.0);
      glEnd();
      glPopMatrix();
      //
      // show movie info
      // show movie icon over dvd cover
      //
      textureId=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfronttextureid();
      if (textureId==0) textureId=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].gettextureid();
      if (textureId) {
        glPushMatrix();
        if (textureId==0) textureId=_defaultdvdcover;                               // hvis ingen dvdcover findes
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        glTranslatef(420,600,0);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( 0+30, 100 +5, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 0+30, 0+320-5, 0.0);
        glTexCoord2f(1, 1); glVertex3f( 0+220-3, 0+320-5 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( 0+220-3, 100+5 , 0.0);
        glEnd();
        glPopMatrix();
      }
      // text genre
      glDisable(GL_TEXTURE_2D);
      glPushMatrix();
      strcpy(temptxt,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].genre);
      temptxt[41]=0;
      glTranslatef(670,800+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      glcRenderString(movie_genre[configland]);
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie title
      glPushMatrix();
      glTranslatef(670,760+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      strcpy(temptxt,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle());
      if (strlen(temptxt)>41) {
        temptxt[42]=0;
      }
      glcRenderString(movie_title[configland]);
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie length
      glPushMatrix();
      glTranslatef(670,740+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      sprintf(temptxt,"%d min.",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmlength());
      temptxt[23]=0;
      glcRenderString(movie_length[configland]);
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie year
      glPushMatrix();
      glTranslatef(670,720+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmaar());
      temptxt[23]=0;
      glcRenderString(movie_year[configland]);
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie rating on imdb
      glPushMatrix();
      glTranslatef(670,700+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      if (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating())
        sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating());
      else
        strcpy(temptxt,"None");
      temptxt[23]=0;
      glcRenderString(movie_rating[configland]);
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie imdb nr
      glPushMatrix();
      glTranslatef(670,680+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      strcpy(temptxt,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmimdbnummer());
      if (strcmp(temptxt,"")!=0) sprintf(temptxt,"%s ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmimdbnummer()); else strcpy(temptxt,"None");
      temptxt[23]=0;
      glcRenderString("Imdb");
      glcRenderString(" ");
      glcRenderString(temptxt);
      glPopMatrix();
      // show movie land
      glPushMatrix();
      glTranslatef(670,660+100,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      glcRenderString(movie_cast[configland]);
      int ll=0;
      float xof=0;
      float yof=0;
      while((strcmp(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].cast[ll],"")!=0) && (ll<5)) {
        sprintf(temptxt,"%s",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].cast[ll]);
        ll++;
        xof+=strlen(temptxt)/4;
        if (xof>2) {
          xof=0;
          yof-=0.4;
        }
      }
      glPopMatrix();
      // show movie descrition
      glPushMatrix();
      glTranslatef(430,560+90,0);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(20.0, 20.0, 1.0);
      glcRenderString(movie_description[configland]);
      glPopMatrix();
      //
      glPushMatrix();
      int sted=0;
      float linof=0.0f;
      while((sted<(int) strlen(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].film_subtitle)) && (linof>-60.0f)) {
        strncpy(temptxt,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].film_subtitle+sted,45);
        temptxt[45]='\0';
        glPushMatrix();
        glTranslatef(430,(500+90)-linof,0);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(20.0, 20.0, 1.0);
        glcRenderString(temptxt);
        glPopMatrix();
        sted+=45;
        linof-=20.f;
      }
      glPopMatrix();
    }


    //
    // show status of all the thread loaders
    //
    #ifdef ENABLE_SPOTIFY
    if ((strcmp(music_db_update_loader,"")>0) || ((radio_oversigt_loaded_begin==true) && (radio_oversigt_loaded_done==false) && (vis_radio_oversigt)) || (spotify_oversigt.get_spotify_update_flag()) || (do_update_spotify_playlist) || (do_update_rss_show) || (movie_oversigt_gfx_loading) && (movie_oversigt_loaded_nr<film_oversigt.film_antal())) {
      show_status_update=true;
      // show loader status
      int statuswxpos = 1470;
      int statuswypos = 75;
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(statuswxpos+200, statuswypos , 0.0);
      glTexCoord2f(0, 1); glVertex3f(statuswxpos+200, statuswypos+130, 0.0);
      glTexCoord2f(1, 1); glVertex3f(statuswxpos+200+250, statuswypos+130 , 0.0);
      glTexCoord2f(1, 0); glVertex3f(statuswxpos+200+250, statuswypos , 0.0);
      glEnd();
      float y = 0;
      int xx = 0;
      int valgtnr = 0;
      if ((radio_oversigt_loaded_nr<radiooversigt.radioantal()) && (radio_oversigt_loaded_done==false)) {
        y = (float) radio_oversigt_loaded_nr/radiooversigt.radioantal();
        xx = (float) y*18;
        if (y>0.0f) valgtnr = 1;
      }
      // show music loader status
      if (valgtnr==0) {
        if (music_oversigt_loaded_nr<7344) {
          y = (float) music_oversigt_loaded_nr/7344;
          xx = (float) y*18;
          if (y>0.0f) valgtnr = 2;
        }
      }

      // spotify
      if (valgtnr==0) {
        if (spotify_oversigt_loaded_begin) {
          y = (float) spotify_oversigt.antal_spotify_streams();
          xx = (float) y*6;
          if (y>0.0f) valgtnr=6;
        }
      }

      // show stream loader status
      if (valgtnr==0) {
        if (streamoversigt.streams_rss_loaded()) {
          //y = (float) streamoversigt.streams_rss_loaded()/streamoversigt.antal_rss_streams();
          y = (float) streamoversigt.streams_rss_loaded()/88;                       // 88
          //xx = (float) y*18;
          xx = (float) y*6;
          if (y>0.0f) valgtnr=3;
        }
      }

      // show movie loader status
      if (valgtnr==0) {
        y = (float) movie_oversigt_loaded_nr/film_oversigt.get_film_antal();
        xx = (float) y*17;
        if (y > 0.0f) valgtnr = 4;
      } else y=0;

      // show spotify loader status
      if (spotify_oversigt.get_spotify_update_flag()) {
        y = (float) spotify_oversigt.loaded_antal/spotify_oversigt.spotify_playlist_antal;                                          // spotify_oversigt_loaded/spotify_oversigt.antal_spotify_streams();
        xx = (float) y*17;
        valgtnr=6;
      } else y=0;

      if (valgtnr==0) xx=0;
      for(int x=0;x<xx;x++) {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(statuswxpos+222+(x*12), 125 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(statuswxpos+222+(x*12), 125+(25), 0.0);
        glTexCoord2f(1, 1); glVertex3f(statuswxpos+222+(10)+(x*12), 125+(25) , 0.0);
        glTexCoord2f(1, 0); glVertex3f(statuswxpos+222+(10)+(x*12), 125 , 0.0);
        glEnd();
      }

      //static int spotify_oversigt_loaded=0;
      //spotify_oversigt_loaded++;

      glDisable(GL_TEXTURE_2D);
      glTranslatef(statuswxpos+220+20,95,0);
      glScalef(24.0, 24.0, 1.0);
      glColor3f(0.6f, 0.6f, 0.6f);
      switch(valgtnr) {
        case 1: glcRenderString("Update Radio");
                break;
        case 2: glcRenderString(" Update Music");
                break;
        case 3: glcRenderString("Update Stream");
                break;
        case 4: glcRenderString(" Update Movie");
                break;
        case 5: glcRenderString("     Other");
                break;
        case 6: glcRenderString("Update Spotify");
                break;
        default: glcRenderString("    Other");
      }
      glPopMatrix();
    } else show_status_update=false;
    #endif

    // show pfs
    // debug mode 1
    if ((showfps) && (debugmode & 1)) {
      glPushMatrix();
      // Gather our frames per second
      Frames++;
      GLuint t = SDL_GetTicks();
      GLfloat fps;
      GLfloat seconds = (t - T0) / 1000.0;
      fps = Frames / seconds;
      T0 = t;
      Frames = 0;
      sprintf(temptxt,"FPS: %-4.0f", fps);
      glTranslatef(1, 1, 0);
      glScalef(20,20, 1.0);                    // danish charset ttf
      glDisable(GL_TEXTURE_2D);
      glcRenderString(temptxt);
      glPopMatrix();
    }
    //  STOP ALL sound
    if (do_stop_music) {				// pressed stop music
      do_stop_music = false;
      do_stop_music_all = true;
      #if defined USE_FMOD_MIXER
      if ((sound) && (snd)) result=sound->release();			// stop all music if user press show playlist stop button
      ERRCHECK(result,do_play_music_aktiv_table_nr);
      #endif
      #if defined USE_SDL_MIXER
      if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);	// stop music and free music
      #endif
      snd = 0;
      do_zoom_music_cover = false;
      aktiv_playlist.clean_playlist();                    // clean play list (reset) play list
      do_play_music_aktiv_table_nr = 1;             			// reset play start nr
    }
    // stop radio
    if (do_stop_radio) {
      #if defined USE_FMOD_MIXER
      result = sound->release();
      ERRCHECK(result,0);
      #endif
      #if defined USE_SDL_MIXER
      if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
      #endif
      snd = 0;
      do_stop_radio = false;
      do_stop_music_all = true;
    }
    // is sound system working
    #if defined USE_FMOD_MIXER
    if ((channel) && (!(do_stop_music_all))) {
    #endif
    #if defined USE_SDL_MIXER
    if (!(do_stop_music_all)) {
    #endif
        // vent på sang er færdig
        //saver_irq=true;						// stop screen saver at starte timeout når vi spiller music
        #if defined USE_FMOD_MIXER
        result = channel->isPlaying(&playing);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
            ERRCHECK(result,do_play_music_aktiv_table_nr);
        }
        #endif
        #if defined USE_SDL_MIXER
        playing = Mix_Playing(0);
        #endif
        if (playing==false) {		//snd->isFinished()
            // hent next aktiv song
            update_afspillinger_music_song(aktivplay_music_path);				// Set aktive sang antal played +1 og set dagsdato til lastplayed mysql felt
            if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {			// er der flere sange i playliste
              do_play_music_aktiv_table_nr++;								// auto next song
              aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);	// hent ny aktiv sang aktivplay_music_path=PATH
              #if defined USE_FMOD_MIXER
              // fmod
              sound->release();           // stop last playing song
              ERRCHECK(result,do_play_music_aktiv_table_nr);
              if (debugmode & 2) fprintf(stderr,"Auto1 Next song %s \n",aktivplay_music_path);
              // start load song to play buffer
              if (strcmp(configsoundoutport,"STREAM")!=0) {
                result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                ERRCHECK(result,do_play_music_aktiv_table_nr);
              } else {
                sprintf(aktivplay_music_path,"%s/mythweb/music/stream?i=%d",configmysqlhost,aktiv_playlist.get_songid(do_play_music_aktiv_table_nr-1));
                result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                ERRCHECK(result,do_play_music_aktiv_table_nr);
              }
              if (result==0) {
                // start play song
                result = sndsystem->playSound(sound,NULL,false, &channel);
                ERRCHECK(result,do_play_music_aktiv_table_nr);
                if (sndsystem) channel->setVolume(configsoundvolume);
                dsp = 0;
              } else {
                // play next song error (set skip song flag) jump to next song
                if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {
                  do_play_music_aktiv_table_nr++;
                  do_shift_song = true;
                } else {
                  do_stop_music_all = true;            // stop play music
                }
              }
              #endif
              #if defined USE_SDL_MIXER
              // aktivplay_music_path = next song to play
              if (debugmode & 2) fprintf(stderr,"Auto1 Next song %s \n",aktivplay_music_path);
              sdlmusicplayer = Mix_LoadMUS(aktivplay_music_path);
              Mix_PlayMusic(sdlmusicplayer, 0);
              if (!(sdlmusicplayer)) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
              #endif
              do_zoom_music_cover_remove_timeout=showtimeout;			              // set close info window timeout
              do_zoom_music_cover = true;				                                  // show music cover info til timeout showtimeout
            } else {				                                    		// else slet playliste (reset player)
              do_play_music_aktiv_table_nr = 1;
              #if defined USE_FMOD_MIXER
              result = sound->release();          		                            // stop last played sound on soundsystem fmod
              ERRCHECK(result,do_play_music_aktiv_table_nr);
              #endif
              #if defined USE_SDL_MIXER
              Mix_FreeMusic(sdlmusicplayer);
              sdlmusicplayer = NULL;
              #endif
              // write debug log
              write_logfile((char *) "Stop player and clear playlist");
              do_stop_music_all = true;				                                 // stop all music
              snd = 0;	                                             					 // clear music pointer for irrsound
              do_zoom_music_cover = false;			                                 // remove play info window
              aktiv_playlist.clean_playlist();		                             // clean play list (reset) play list
              // do_play_music_aktiv_table_nr=1;
            }
        }
        if (do_shift_song) {
          aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);			// hent aktive sang i playliste
          #if defined USE_FMOD_MIXER
          sound->release();							// stop last playing song
          if (strcmp(configsoundoutport,"STREAM")!=0) {
            result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
            ERRCHECK(result,do_play_music_aktiv_table_nr-1);
          } else {
            sprintf(aktivplay_music_path,"http://%s/mythweb/music/stream?i=%d",configmysqlhost,aktiv_playlist.get_songid(do_play_music_aktiv_table_nr-1));
            result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
            ERRCHECK(result,do_play_music_aktiv_table_nr);
          }
          if (result==0) {
            result = sndsystem->playSound( sound,NULL,false, &channel);
            ERRCHECK(result,do_play_music_aktiv_table_nr-1);
            if (sndsystem) channel->setVolume(configsoundvolume);
            dsp=0;
          } else {
            // play next song error (set skip song flag) jump to next song
            if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {
              do_play_music_aktiv_table_nr++;
              do_shift_song=true;
            } else {
              do_stop_music_all=true;            // stop play music
            }
          }
          if (result==0) do_shift_song=false;
          #endif
          #if defined USE_SDL_MIXER
          if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
          sdlmusicplayer = NULL;
          sdlmusicplayer = Mix_LoadMUS(aktivplay_music_path);
          Mix_PlayMusic(sdlmusicplayer, 0);
          if (sdlmusicplayer==NULL) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
          #endif
          // write debug log
          sprintf(debuglogdata,"User Next song %s ",aktivplay_music_path);
          write_logfile((char *) debuglogdata);
        }
    } else if (vis_music_oversigt) {
      // press play on music
      if (do_shift_song) {
        aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);			// hent første sang ,0
        // write debug log
        sprintf(debuglogdata,"Auto2 Next song %s ",aktivplay_music_path);
        write_logfile((char *) debuglogdata);
        #if defined USE_FMOD_MIXER
        if (strcmp(aktivplay_music_path,"")) sound->release();          								// stop last playing song
        ERRCHECK(result,do_play_music_aktiv_table_nr);
        if (strcmp(configsoundoutport,"STREAM")!=0) {
          result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
          ERRCHECK(result,do_play_music_aktiv_table_nr);
        } else {
          sprintf(aktivplay_music_path,"http://%s/mythweb/music/stream?i=%d",configmysqlhost,aktiv_playlist.get_songid(do_play_music_aktiv_table_nr-1));
          result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
          ERRCHECK(result,do_play_music_aktiv_table_nr);
        }
        if (result==0) {
          result = sndsystem->playSound(sound, NULL,false, &channel);
          ERRCHECK(result,do_play_music_aktiv_table_nr);
          if (sndsystem) channel->setVolume(configsoundvolume);
          dsp=0;
        } else {
          // play next song error (set skip song flag) jump to next song
          if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {
            do_play_music_aktiv_table_nr++;
            do_shift_song=true;
          } else {
            do_stop_music_all=true;		// stop play music
          }
        }
        if (result==0) {
          do_stop_music_all=false;
          do_shift_song=false;							// vi har skiftet sang set finish flag
        }
        #endif
        #if defined USE_SDL_MIXER
        // skift sang
        // stop last played
        if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
        sdlmusicplayer=NULL;
        // load new played
        sdlmusicplayer = Mix_LoadMUS(aktivplay_music_path);
        Mix_PlayMusic(sdlmusicplayer, 0);
        if (sdlmusicplayer==NULL) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
        #endif
      }
    }
    if (do_update_xmltv) {
      // call update xmltv multi phread
      fprintf(stderr,"Start phread xmltv.\n");
      update_xmltv_phread_loader();
      do_update_xmltv=false;
    }
    if (do_update_rss) {
      // call update xmltv multi phread
      fprintf(stderr,"Start phread podcast.\n");
      update_rss_phread_loader();
      do_update_rss = false;
    }
    if (do_update_music) {
      fprintf(stderr,"Start phread music.\n");
      update_music_phread_loader();
      do_update_music = false;
    }
    if (do_update_spotify) {
      fprintf(stderr,"Start phread spotify and web server.\n");
      update_spotify_phread_loader();                                           // update spotify view (load it)
      update_webserver_phread_loader();                                         //
      do_update_spotify = false;
    }
    //  don't wait!
    //  start processing buffered OpenGL routines
    //
    glFlush();
    //glfwPollEvents(); // do not exist in linux
    glutSwapBuffers();


}
// end display()


// ****************************************************************************************
//
// used by mouse stuf
// to check mouse buttons
// names is the array of gfx on screen as buttons
//
// ****************************************************************************************

int list_hits(GLint hits, GLuint *names,int x,int y) {
    int i = hits;			// numbers of hits
    bool fundet = false;
    int returnfunc = 0;
    /* For each hit in the buffer are allocated 4 bytes:
      1. Number of hits selected (always one, beacuse when we draw each object we use glLoadName, so we replace the prevous name in the stack)
      2. Min Z  3. Max Z  4. Name of the hit (glLoadName)
    */
    do {
      // setup menu
      if ((fundet==false) && (do_show_setup)) {
        // test for setup menu sound
        if (((GLubyte) names[i*4+3]==30) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
          do_show_setup_sound = true;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_screen = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          do_show_setup_rss = false;
          do_show_setup_spotify = false;
          fundet = true;
        }
        // test screen setup
        if (((GLubyte) names[i*4+3]==31)  && (do_show_setup_sql==false) && (do_show_setup_sound==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = true;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for gfx opløsning
        if (((GLubyte) names[i*4+3]==32) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_font = false;
          do_show_setup_tema = true;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for tema setup
        if (((GLubyte) names[i*4+3]==33)  && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
          do_show_setup_sound = false;
          do_show_setup_screen = true;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for sql setup/info
        if (((GLubyte) names[i*4+3]==34) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = true;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for ttffont setup/info
        if (((GLubyte) names[i*4+3]==35) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_setup_font = true;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for F keys setup/info
        if (((GLubyte) names[i*4+3]==36) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_videoplayer = false;
          do_show_setup_keys = true;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for setupclose
        if (((GLubyte) names[i*4+3]==37) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_setup = false;
          do_show_tvgraber = false;
          fundet = true;
          do_save_config = true;             // save setup now
        }
        if (((GLubyte) names[i*4+3]==38) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
          do_show_videoplayer = true;
          do_show_setup_sound = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_screen = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // do_show_tvgraber
        if (((GLubyte) names[i*4+3]==39) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
          do_show_setup_sound = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_screen = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = true;
          fundet = true;
        }
        // test for close windows again icon for all other windows in setup glLoadName(40)
        //
        if (((GLubyte) names[i*4+3]==40) && ((do_show_setup_sound) || (do_show_setup_screen) || (do_show_setup_sql) || (do_show_setup_network) || (do_show_setup_tema) || (do_show_setup_font) || (do_show_setup_keys) || (do_show_videoplayer) || (do_show_tvgraber))) {
          do_show_setup_sound = false;
          do_show_setup_screen = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_tema = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_setup_rss = false;
          do_show_setup_spotify = false;
          if (do_show_setup_font) {
            if (debugmode) fprintf(stderr,"Set aktiv font to '%s' \n",aktivfont.typeinfo[setupfontselectofset].fontname);
            strcpy(configfontname,aktivfont.typeinfo[setupfontselectofset].fontname);
            aktivfont.selectfont(aktivfont.typeinfo[setupfontselectofset].fontname);
            do_show_setup_font = false;
          }
          if (do_show_tvgraber) {
            // hent/update tv guide from db
            // efter den er saved i db fra setup tvguide function som saver data.
            aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
            do_show_tvgraber = false;
          }
          fundet = true;
        }
        // test for tema setup/info
        if (((GLubyte) names[i*4+3]==41) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_font==false) && (do_show_setup_tema))  {
          // next tema
          tema++;
          if (tema>TEMA_ANTAL) tema = 1;
          fundet = true;
        }
        // test for rss setup
        if (((GLubyte) names[i*4+3]==42) && (do_show_setup_sql==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
          // close  all show setup windows
          do_show_setup_sound = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_screen = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          do_show_rss = false;
          do_show_setup_rss = true;
          do_show_setup_spotify = false;
          fundet = true;
        }

        // test for spotify setup
        if (((GLubyte) names[i*4+3]==43) && (do_show_setup_sql==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_rss==false)) {
          // close  all show setup windows
          do_show_setup_sound = false;
          do_show_setup_sql = false;
          do_show_setup_network = false;
          do_show_setup_screen = false;
          do_show_setup_tema = false;
          do_show_setup_font = false;
          do_show_setup_keys = false;
          do_show_videoplayer = false;
          do_show_tvgraber = false;
          do_show_rss = false;
          do_show_setup_rss = false;
          do_show_setup_spotify = true;
          fundet = true;
        }
        //
        if ((GLubyte) names[i*4+3]==45) {
          fundet = true;
          fprintf(stderr,"45 Button pressed \n");
        }
      }
      //
      // main menu
      //
      if ((fundet==false) && (do_show_setup==false)) {
        // test for menu select tv
        if ((GLubyte) names[i*4+3]==1) {
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt =! vis_tv_oversigt;
          vis_recorded_oversigt = false;
          vis_radio_or_music_oversigt = false;
          vis_stream_oversigt = false;
          vis_spotify_oversigt=false;
          vis_stream_or_movie_oversigt = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for menu select music
        if ((GLubyte) names[i*4+3]==2) {
          vis_radio_or_music_oversigt=!vis_radio_or_music_oversigt;
          //vis_radio_oversigt=!vis_radio_oversigt;
          //vis_music_oversigt=!vis_music_oversigt;
          vis_radio_oversigt = false;
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt = false;
          vis_recorded_oversigt = false;
          vis_stream_oversigt = false;
          vis_spotify_oversigt=false;
          vis_stream_or_movie_oversigt = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for menu select film/streams
        if ((GLubyte) names[i*4+3]==3) {
          vis_stream_or_movie_oversigt =! vis_stream_or_movie_oversigt;
          // close all other setup windows
          vis_radio_oversigt = false;
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt = false;
          vis_recorded_oversigt = false;
          vis_stream_oversigt = false;
          vis_spotify_oversigt=false;
          vis_radio_or_music_oversigt = false;
          do_show_tvgraber = false;
          fundet = true;
        }
        // test for menu select recorded
        if ((GLubyte) names[i*4+3]==4) {
          // close all other setup windows
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt = false;
          vis_radio_or_music_oversigt = false;
          vis_spotify_oversigt=false;
          vis_recorded_oversigt =! vis_recorded_oversigt;
          vis_stream_oversigt = false;
          vis_stream_or_movie_oversigt = false;
          do_show_tvgraber = false;
          fundet = true;
        }
      }
      // setup menu i main
      if ((!(vis_radio_oversigt)) && (!(vis_music_oversigt)) && (!(vis_recorded_oversigt)) && (!(vis_spotify_oversigt)) && (!(vis_tv_oversigt)) && (!(vis_stream_oversigt))) {
        // test for menu select setup
        if ((GLubyte) names[i*4+3]==5) {
          // close all other setup windows
          do_show_setup =! do_show_setup;
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt = false;
          vis_stream_oversigt = false;
          vis_spotify_oversigt=false;
          vis_stream_or_movie_oversigt = false;
          vis_radio_or_music_oversigt = false;
          do_show_tvgraber = false;
          do_show_setup_rss = false;
          do_show_setup_spotify = false;
          if (do_show_setup) write_logfile((char *) "Show setup menu."); else write_logfile((char *) "close setup menu.");
          fundet = true;
        }
        // test for exit selected                                               // exit program
        if ((GLubyte) names[i*4+3]==6) {
          // close all other setup windows
          vis_music_oversigt = false;
          vis_film_oversigt = false;
          vis_tv_oversigt =! vis_tv_oversigt;
          vis_recorded_oversigt = false;
          vis_radio_or_music_oversigt = false;
          vis_stream_oversigt = false;
          vis_spotify_oversigt=false;
          vis_stream_or_movie_oversigt = false;
          do_show_tvgraber = false;
          fundet = true;
          remove("mythtv-controller.lock");
          runwebserver=false;
          order_channel_list();                                               // order tv channel list
          save_channel_list();                                                //
          txmltvgraber_createconfig();                                        // create tv grabber config
          write_logfile((char *) "Exit program.");
          exit(0);                                                            // exit
        }
      }

      // start/stop spotify online search view
      #ifdef ENABLE_SPOTIFY
      if (vis_spotify_oversigt) {
        if ((GLubyte) names[i*4+3]==5) {
          strcpy(keybuffer,"");                                                 // reset text buffer
          keybufferindex=0;                                                     //
          spotify_selected_startofset=0;
          ask_open_dir_or_play_spotify = false;
          strcpy(spotify_oversigt.overview_show_band_name,"");
          if (do_show_spotify_search_oversigt==true) {
            do_show_spotify_search_oversigt=false;
            spotify_oversigt_loaded_begin=true;                                 //
            spotify_oversigt.opdatere_spotify_oversigt(0);                      // update view from root
            spotify_oversigt.set_search_loaded();                               // triger icon loader
            //spotify_oversigt.load_spotify_iconoversigt();                     // update icons
            spotify_oversigt_loaded_begin=false;                                //
          } else {
            do_show_spotify_search_oversigt=true;
          }
          write_logfile((char *) "Spotify search.");
        }
      }
      #endif
      //
      // stream control
      //
      if (vis_stream_oversigt) {
        if (!(fundet)) {
          // we have a select mouse/touch element dirid
          if ((GLubyte) names[i*4+3]==23) {
            if (debugmode & 4) fprintf(stderr,"scroll down\n");
            returnfunc = 1;
            fundet = true;
          }
          if ((GLubyte) names[i*4+3]==24) {
            if (debugmode & 4) fprintf(stderr,"scroll up\n");
            returnfunc = 2;
            fundet = true;
          }
          if ((GLubyte) names[i*4+3]==27) {
            //fprintf(stderr,"Close stream info\n");
            returnfunc = 2;
            do_zoom_stream_cover = false;
            do_stop_stream = true;                                            // flag to stop play
            stopstream = true;                                                // flag to stop play
            do_play_stream = false;                                           // we are not play a
            write_logfile((char *) "Close stream info.");
            fundet = true;
          }
        }
      }
      //
      // film
      //
      if (vis_film_oversigt) {
        if (!(fundet)) {
          // we have a select mouse/touch element dirid
          // scroll down
          if ((GLubyte) names[i*4+3]==23) {
            if (debugmode & 8) fprintf(stderr,"scroll down\n");
            returnfunc = 1;
            fundet = true;
          }
          // scroll up
          if ((GLubyte) names[i*4+3]==24) {
            if (debugmode & 8) fprintf(stderr,"scroll up\n");
            returnfunc = 2;
            fundet = true;
          }
          // close window
          if ((GLubyte) names[i*4+3]==27) {
            returnfunc = 2;
            do_zoom_film_cover = false;
            write_logfile((char *) "Close movie info.");
            fundet = true;
          }
          // reset movie search view
          // show all movies again
          if ((GLubyte) names[i*4+3]==28) {
            film_oversigt.opdatere_film_oversigt((char *) "%");
            //  remove search flag again and show all movies
            film_oversigt.set_search_view(false);
          }
        }
      }
      //
      // music
      //
      if (vis_music_oversigt) {
        if (!(fundet)) {		// hvis vi ikke har en aaben dirid så er det muligt at vælge dirid
          // we have a select mouse/touch element dirid
          // scroll down
          if ((GLubyte) names[i*4+3]==23) {
            if (debugmode & 2) fprintf(stderr,"scroll down\n");
            returnfunc = 1;
            fundet = true;
          }
          // scroll up
          if ((GLubyte) names[i*4+3]==24) {
            if (debugmode & 2) fprintf(stderr,"scroll up\n");
            returnfunc = 2;
            fundet = true;
          }
          if ((GLubyte) names[i*4+3]==27) {
            do_zoom_music_cover =! do_zoom_music_cover;
            ask_open_dir_or_play = false;
            if (do_zoom_music_cover) write_logfile((char *) "Show music info."); else write_logfile((char *) "Close music info.");
            fundet = true;
          }
        }
        // Bruges til mus/touch skærm
        if ((!(fundet)) && (!(do_zoom_music_cover)) && (!(ask_open_dir_or_play))) {		// hvis vi ikke har en aaben dirid så er det muligt at vælge dirid
          // we have a select mouse/touch element dirid
          if ((GLuint) names[i*4+3]>=100) {
            mknapnr=(GLuint) names[i*4+3]-99;				// hent music knap nr
            if (debugmode & 2) fprintf(stderr,"music selected=%u  \n",mknapnr);
            fundet = true;
          }
          // husk last
          if (mknapnr!=0) swknapnr=mknapnr;                                     // swknapnr
          // mknapnr=mknapnr+(music_icon_anim_icon_ofsety*4);
          mknapnr = mknapnr+(_mangley/41)*8;
        }
        //
        // hvis vis ask_open_dir_or_play window
        //
        if ((!(fundet)) && (ask_open_dir_or_play) && (!(do_zoom_music_cover))) {
          // play button
          if ((GLubyte) names[i*4+3]==20) {
            ask_open_dir_or_play = false;                                         // flag luk vindue igen
            do_play_music_cover = 1;                                              // der er trykket på cover play det
            do_zoom_music_cover = false;                                          // ja den skal spilles lav zoom cover info window
            do_find_playlist = true;                                              // find de sange som skal indsættes til playlist (og load playlist andet sted)
            fundet = true;
          }
          // open music dir or close window
          if ((GLubyte) names[i*4+3]==21) {
            // pressed close
            if (debugmode & 2) fprintf(stderr,"Close window again\n");
            ask_open_dir_or_play = false;				// flag luk vindue igen
            do_zoom_music_cover = false;
            mknapnr = 0;
            fundet = true;
          }
          // swap selected
          if ((GLubyte) names[i*4+3]==22) {
            // pressed Swap
            if (debugmode & 2) fprintf(stderr,"Swap button pressed\n");
            for(int x=0;x<dirmusic.numbersinlist();x++) {
              dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(x)),x);
            }
            fundet = true;
          }
        }
        // hvis show player
        // use as controller
        if ((!(fundet)) && (do_zoom_music_cover)) {
          if ((GLubyte) names[i*4+3]==5) {
            // if touch/mouse click on window then close windows again
            do_zoom_music_cover = false;
            ask_open_dir_or_play = false;				// flag luk vindue igen
            fundet = true;
          }
          // last song
          if ((GLubyte) names[i*4+3]==6) {
            if ((do_play_music_aktiv_table_nr>1) && (do_shift_song==false)) {
              do_play_music_aktiv_table_nr--;                                   // skift aktiv sang
              do_shift_song = true;                                             // sæt flag til skift
            }
            fundet = true;
          }

          // next song
          if ((GLubyte) names[i*4+3]==7) {
            if ((do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) && (do_shift_song==false)) {
              do_play_music_aktiv_table_nr++;
              do_shift_song = true;
            }
            fundet = true;
          }
          // stop song
          if ((GLubyte) names[i*4+3]==9) {
            do_stop_music = 1;
            fundet = true;
          }
          // play song
          if ((GLubyte) names[i*4+3]==8) {
            do_stop_music = 0;
            do_shift_song = true;
            if (do_play_music_aktiv_table_nr>=aktiv_playlist.numbers_in_playlist()) {
                if (aktiv_playlist.numbers_in_playlist()==0) {
                    do_stop_music_all = true;                                   // stop play music
                    do_shift_song = false;
                } else {
                    do_play_music_aktiv_table_nr = 1;
                }
            }
            fundet = true;
            if (debugmode & 2) fprintf(stderr,"Start play \n");
            // write to debug log
            write_logfile((char *) "Start play");
          }
        }
      }
      //
      // spotify stuf offline search (only in local db)
      //
      #ifdef ENABLE_SPOTIFY
      if (vis_spotify_oversigt) {
        if (do_show_spotify_search_oversigt==false) {
          if ((!(do_show_setup_spotify))  && (!(fundet))) {
            if ((GLuint) names[i*4+3]>=100) {
              spotifyknapnr = (GLuint) names[i*4+3]-99;				                  // hent spotify knap nr
              spotify_select_iconnr=spotifyknapnr;
              fundet = true;                                                    //
              do_zoom_spotify_cover = false;                                    // close player status to ask about play other selected playlist/song
              if (spotify_oversigt.type==0) {
                ask_open_dir_or_play_spotify=true;
              } else if (spotify_oversigt.type==1) {
                ask_open_dir_or_play_spotify=true;
              }
            }
            // works ok
            // back icon to main playlist overview
            // do update from root
            if ((spotifyknapnr==1) && (spotify_oversigt.show_search_result)) {
              if ((spotify_oversigt.type==0) || (spotify_oversigt.type==1)) {
                // update
                spotify_selected_startofset=0;                                  // default selected in view
                spotify_oversigt.opdatere_spotify_oversigt(0);                  // update view
                //spotify_oversigt.load_spotify_iconoversigt();                   // load icons
                spotify_oversigt.set_search_loaded();                           // triger icon loader
                ask_open_dir_or_play_spotify = false;                           // close windows again
                fundet = true;
              }
            }
            // play playlist icon select (20) type 0
            if (((GLubyte) names[i*4+3]==20) && (spotify_oversigt.type==0)) {
              // write to debug log
              sprintf(debuglogdata,"play spotify playlist.");
              write_logfile((char *) debuglogdata);
              do_select_device_to_play=true;
              returnfunc = 4;
              fundet = true;
            }
            // play song icon select (20) type 1
            if (((GLubyte) names[i*4+3]==20) && (spotify_oversigt.type==1)) {
              write_logfile((char *) "play spotify song.");
              do_select_device_to_play=true;
              returnfunc = 5;
              fundet = true;
            }
            // open
            if ((GLubyte) names[i*4+3]==21) {
              write_logfile((char *) "open spotify playlist");
              returnfunc = 3;
              fundet = true;
            }
            // Stop play
            if ((GLubyte) names[i*4+3]==9) {
              write_logfile((char *) "(Spotify) Stop play");
              returnfunc = 5;                                                       //
              fundet = true;
            }
            // Next
            if ((GLubyte) names[i*4+3]==11) {
              write_logfile((char *) "(Spotify) Next song");
              returnfunc = 6;                                                       //
              fundet = true;
            }
            // last
            if ((GLubyte) names[i*4+3]==10) {
              write_logfile((char *) "(Spotify) last song");
              returnfunc = 7;                                                       //
              fundet = true;
            }
            // scroll up
            if ((GLubyte) names[i*4+3]==23) {
              // write debug log
              sprintf(debuglogdata,"scroll down spotify_selected_startofset = %d ",spotify_selected_startofset);
              write_logfile((char *) debuglogdata);
              if (spotify_selected_startofset+40<spotify_oversigt.streamantal()) {
                spotify_selected_startofset+=8;
                spotify_selected_startofset+=8;
                returnfunc = 2;
                fundet = true;
              }
            }
            // scroll down
            if ((GLubyte) names[i*4+3]==24) {
              // write debug log
              sprintf(debuglogdata,"scroll up spotify_selected_startofset = %d",spotify_selected_startofset);
              write_logfile((char *) debuglogdata);
              if ((spotify_selected_startofset+8)>8) spotify_selected_startofset-=8;
              if (spotify_selected_startofset<0) spotify_selected_startofset=0;
              returnfunc = 1;
              fundet = true;
            }
            // show close spotify info (27 need to move) 27 now is global exit
            if ((GLubyte) names[i*4+3]==27) {
              // write debug log
              write_logfile((char *) "Show/close spotify info\n");
              if (ask_open_dir_or_play_spotify==false) do_zoom_spotify_cover =! do_zoom_spotify_cover;
              if (ask_open_dir_or_play_spotify) ask_open_dir_or_play_spotify=false;
              fundet = true;
            }
          }
        }
        // online spotify stuf
        if (do_show_spotify_search_oversigt==true) {
          if ((!(do_show_setup_spotify))  && (!(fundet))) {
            if ((GLuint) names[i*4+3]>=100) {
              spotifyknapnr = (GLuint) names[i*4+3]-99;				                    // hent spotify knap nr
              spotify_select_iconnr=spotifyknapnr;
              fundet = true;
              do_zoom_spotify_cover = false;                                      // close player status to ask about play other selected playlist/song
              if (spotify_oversigt.type==0) {                                     // playlist type
                ask_open_dir_or_play_spotify=true;
              } else if (spotify_oversigt.type==1) {                              // song type
                ask_open_dir_or_play_spotify=true;
              } else if (spotify_oversigt.type==2) {                              // artist type
                ask_open_dir_or_play_spotify=true;
              }
            }
            if ((GLuint) names[i*4+3]==27) {
              spotifyknapnr=(GLuint) names[i*4+3];
              if (debugmode & 8) fprintf(stderr,"Show/close spotify info\n");
              if (ask_open_dir_or_play_spotify==false) do_zoom_spotify_cover =! do_zoom_spotify_cover;
              if (ask_open_dir_or_play_spotify) ask_open_dir_or_play_spotify=false;
              fundet = true;
            }
          }
          // write debug log
          #ifdef ENABLE_SPOTIFY
          sprintf(debuglogdata,"spotifyknapnr %d type=%d fundet %d",spotifyknapnr,spotify_oversigt.get_spotify_type(spotifyknapnr));
          write_logfile((char *) debuglogdata);
          #endif

          // back button

          // works ok
          // back icon to main playlist overview
          // do update from root
          if ((spotifyknapnr==1) && (spotify_oversigt.show_search_result)) {
            // update
            spotify_selected_startofset=0;
            spotify_oversigt.opdatere_spotify_oversigt(0);                      // update view
            //spotify_oversigt.load_spotify_iconoversigt();                       // load icons
            spotify_oversigt.set_search_loaded();                           // triger icon loader
            ask_open_dir_or_play_spotify = false;
            fundet = true;
          }

          /*
          if (( fundet == false) && ( spotifyknapnr == 1 ) && ( ask_open_dir_or_play_spotify == false ) && (strcmp(spotify_oversigt.get_spotify_name(spotifyknapnr-1),"Back") == 0)) {
            // update
            if (spotify_oversigt.type==0) {
              spotify_selected_startofset=0;
              spotify_oversigt.opdatere_spotify_oversigt(0);
              //spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,3); //type 3 = tracks ()
              spotify_oversigt.load_spotify_iconoversigt();
              ask_open_dir_or_play_spotify = false;
              fundet = true;
            }
            if (spotify_oversigt.type==1) {
              // update
              spotify_selected_startofset=0;
              spotify_oversigt.opdatere_spotify_oversigt(0);
              //spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,3); //type 3 = tracks ()
              spotify_oversigt.load_spotify_iconoversigt();
              ask_open_dir_or_play_spotify = false;
              fundet = true;
            }
            if (spotify_oversigt.type==2) {
              // update
              spotify_selected_startofset=0;
              spotify_oversigt.opdatere_spotify_oversigt(0);
              //spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,3); //type 3 = tracks ()
              spotify_oversigt.load_spotify_iconoversigt();
              ask_open_dir_or_play_spotify = false;
              fundet = true;
            }
            if (spotify_oversigt.get_spotify_type(spotifyknapnr)==3) {
              // update
              spotify_selected_startofset=0;
              spotify_oversigt.opdatere_spotify_oversigt(0);
              //spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,3); //type 3 = tracks ()
              spotify_oversigt.load_spotify_iconoversigt();
              ask_open_dir_or_play_spotify = false;
              fundet = true;
            }
          }
          */

          // play playlist icon select (20) type 0
          if (((GLubyte) names[i*4+3]==20) && (spotify_oversigt.type==0)) {
            fprintf(stderr,"play spotify playlist. type 0\n");
            write_logfile((char *) "play spotify playlist.");
            do_select_device_to_play=true;
            returnfunc = 4;
            fundet = true;
          }
          // play song icon select (20) type 1
          if (((GLubyte) names[i*4+3]==20) && (spotify_oversigt.type==1)) {
            fprintf(stderr,"play spotify song. type 1\n");
            write_logfile((char *) "play spotify song.");
            do_select_device_to_play=true;
            returnfunc = 5;
            fundet = true;
          }
          // play song icon select (20) type 2
          if (((GLubyte) names[i*4+3]==20) && (spotify_oversigt.type==2)) {
            fprintf(stderr,"play spotify artist. type 2\n");
            write_logfile((char *) "play spotify artist.");
            do_select_device_to_play=true;
            returnfunc = 5;
            fundet = true;
          }
          // open 6
          if ((GLubyte) names[i*4+3]==21) {
            fprintf(stderr,"open spotify artist type 2\n");
            returnfunc = 6;
            fundet = true;
          }
          // Stop play
          if ((GLubyte) names[i*4+3]==9) {
            fprintf(stderr,"(Spotify) Stop play\n");
            write_logfile((char *) "Stop Spotify play.");
            returnfunc = 5;                                                       //
            fundet = true;
          }
          // next song
          if ((GLubyte) names[i*4+3]==11) {
            fprintf(stderr,"(Spotify) Next song\n");
            write_logfile((char *) "Next Spotify song.");
            returnfunc = 7;
            fundet = true;
          }
          // last song
          if ((GLubyte) names[i*4+3]==12) {
            fprintf(stderr,"(Spotify) last song\n");
            write_logfile((char *) "Last Spotify song.");
            returnfunc = 7;                                                       //
            fundet = true;
          }
        }
      }
      #endif
      //
      // spotify stuf
      // set default device to play on
      //
      #ifdef ENABLE_SPOTIFY
      if ((do_show_setup_spotify)  && (!(fundet))) {
        // select default play device
        if ((GLubyte) names[i*4+3]==10) {
          fprintf(stderr,"selected first device\n");
          spotify_oversigt.set_default_device_to_play(0);
          write_logfile((char *) "selected 1 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==11) {
          fprintf(stderr,"selected 2 device\n");
          spotify_oversigt.set_default_device_to_play(1);
          write_logfile((char *) "selected 2 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==12) {
          fprintf(stderr,"selected 3 device\n");
          spotify_oversigt.set_default_device_to_play(2);
          write_logfile((char *) "selected 3 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==13) {
          fprintf(stderr,"selected 4 device\n");
          spotify_oversigt.set_default_device_to_play(3);
          write_logfile((char *) "selected 4 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==14) {
          fprintf(stderr,"selected 5 device\n");
          spotify_oversigt.set_default_device_to_play(4);
          write_logfile((char *) "selected 5 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==15) {
          fprintf(stderr,"selected 6 device\n");
          spotify_oversigt.set_default_device_to_play(5);
          write_logfile((char *) "selected 6 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==16) {
          fprintf(stderr,"selected 7 device\n");
          spotify_oversigt.set_default_device_to_play(6);
          write_logfile((char *) "selected 7 device.");
          returnfunc = 0;
          fundet = true;
        }
        if ((GLubyte) names[i*4+3]==17) {
          fprintf(stderr,"selected 8 device\n");
          spotify_oversigt.set_default_device_to_play(7);
          write_logfile((char *) "selected 8 device.");
          returnfunc = 0;
          fundet = true;
        }
      }
      #endif
      // vælg skal der spilles music eller radio
      if ((vis_radio_or_music_oversigt) && (!(fundet))) {
        // Radio
        if ((GLubyte) names[i*4+3]==80) {
          fundet = true;
          vis_radio_oversigt = true;
          vis_music_oversigt = false;
          vis_spotify_oversigt = false;
          vis_radio_or_music_oversigt=false;
        }
        // Music
        if ((GLubyte) names[i*4+3]==81) {
          fundet = true;
          vis_music_oversigt = true;
          vis_radio_oversigt = false;
          vis_spotify_oversigt = false;
          vis_radio_or_music_oversigt = false;
        } // spotify
        if ((GLubyte) names[i*4+3]==82) {
          fundet = true;
          vis_music_oversigt = false;
          vis_radio_oversigt = false;
          vis_spotify_oversigt = true;                                          // show spotify overview
          vis_radio_or_music_oversigt = false;
        }
        #if defined(ENABLE_TIDAL)
        // tidal
        if ((GLubyte) names[i*4+3]==83) {
          fundet = true;
          vis_music_oversigt = false;
          vis_radio_oversigt = false;
          vis_spotify_oversigt = false;
          vis_tidal_oversigt = true;                                          // show tidal overview
          vis_radio_or_music_oversigt = false;
        }
        #endif
      }
      #if defined(ENABLE_TIDAL)
      // kun til mus/touch skærm (tidal oversigt)
      if ((vis_tidal_oversigt)  && (!(fundet))) {
        if ((GLubyte) names[i*4+3]==23) {
          if (debugmode & 8) fprintf(stderr,"scroll down\n");
          returnfunc = 1;
          fundet = true;
        }
        // scroll up
        if ((GLubyte) names[i*4+3]==24) {
          if (debugmode & 8) fprintf(stderr,"scroll up\n");
          returnfunc = 2;
          fundet = true;
        }
        // show close radio info (27 need to move) 27 now is global exit
        if ((GLubyte) names[i*4+3]==27) {
          if (debugmode & 8) fprintf(stderr,"Show/close tidal info\n");
          do_zoom_tidal =! do_zoom_tidal;
          fundet = true;
        }
      }
      #endif

      // kun til mus/touch skærm (spotify oversigt)
      // luk show play radio
      // scroll down
      if ((vis_radio_oversigt)  && (!(fundet))) {
        if ((GLubyte) names[i*4+3]==23) {
          if (debugmode & 8) fprintf(stderr,"scroll down\n");
          returnfunc = 1;
          fundet = true;
        }
        // scroll up
        if ((GLubyte) names[i*4+3]==24) {
          if (debugmode & 8) fprintf(stderr,"scroll up\n");
          returnfunc = 2;
          fundet = true;
        }
        // show close radio info (27 need to move) 27 now is global exit
        if ((GLubyte) names[i*4+3]==27) {
          if (debugmode & 8) fprintf(stderr,"Show/close radio info\n");
          do_zoom_radio =! do_zoom_radio;
          fundet = true;
        }
      }
      if ((vis_radio_oversigt) && (show_radio_options==false)) {
        // Bruges vist kun til mus/touch skærm (radio stationer)
        if (!(fundet)) {		// hvis ingen valgt
          // we have a select mouse/touch element dirid
          if ((GLuint) names[i*4+3]>=100) {
            rknapnr = (GLuint) names[i*4+3]-99;				// hent music knap nr
            // write debug log
            sprintf(debuglogdata,"radio station selected=%d glID=%u ",rknapnr,names[i*4+3]-99);
            write_logfile((char *) debuglogdata);
            fundet = true;
          }
          // husk last
          if (rknapnr!=0) swknapnr=rknapnr;
          rknapnr = rknapnr+(_mangley/41)*8;
        }
        // stop radio player if vis_radio_oversigt
        if (!(fundet)) {
          // tryk stop radio
          if ((GLubyte) names[i*4+3]==9) {
            // write debug log
            write_logfile((char *) "stop radio");
            do_stop_radio = 1;
            do_play_radio = false;			// no playing
            fundet = true;
            rknapnr = 0;
          }
        }
      } // radio overview
      // vælg skal der spilles film eller stream
      if ((vis_stream_or_movie_oversigt) && (!(fundet))) {
        // stream
        if ((GLubyte) names[i*4+3]==80) {
          fundet = true;
          vis_stream_oversigt=true;
          vis_stream_or_movie_oversigt=false;
        }
        // stream
        if ((GLubyte) names[i*4+3]==81) {
          fundet = true;
          vis_film_oversigt = true;
          vis_stream_or_movie_oversigt = false;
        }
      }
      //
      if ((vis_stream_or_movie_oversigt) && (!(fundet))) {
        if ((GLubyte) names[i*4+3]==3) {
          fundet = true;
          vis_stream_oversigt = false;
          vis_stream_or_movie_oversigt = false;
        }
      }
      // stream oversigt
      if ((vis_stream_oversigt) && (!(fundet))) {
        if ((GLuint) names[i*4+3]>=100) {
          sknapnr=(GLuint) names[i*4+3]-99;				// hent stream knap nr
          // write debug log
          sprintf(debuglogdata,"stream selected=%u",sknapnr);
          write_logfile((char *) debuglogdata);
          fundet = true;
        }
        // close open stream or movie
        if ((GLubyte) names[i*4+3]==3) {
          fundet = true;
          vis_stream_or_movie_oversigt = false;
          vis_stream_oversigt = false;
        }
        // play button pressed
        if (((GLubyte) names[i*4+3]==8) && (do_zoom_stream_cover) && (fundet==false)) {
          // start play
          //do_zoom_stream_cover=!do_zoom_stream_cover;
          fundet = true;
          if (sknapnr>0) {
            do_play_stream=1;						// select button do play
            if (debugmode & 4) fprintf(stderr,"Set do_play_stream flag %d \n",sknapnr);
          }
        }
        // stop button
        if (((GLubyte) names[i*4+3]==9) && (do_zoom_stream_cover) && (fundet==false)) {
          fundet = true;
          if (streamoversigt.stream_is_playing) {
            do_zoom_stream_cover = false;
            do_stop_stream = true;                                            // flag to stop play
            stopstream = true;                                                // flag to stop play
            do_play_stream = false;                                           // we are not play any more
            stream_jump = false;                                              // we can not jump in stream any more
            streamoversigt.stopstream();
          }
        }
        // pause play button pressed
        // pause play
        if (((GLubyte) names[i*4+3]==12) && (do_zoom_stream_cover) && (fundet==false)) {
          fundet = true;
          if (do_pause_stream) do_pause_stream=false; else do_pause_stream=true;
          // write debug log
          write_logfile((char *) "Set/reset player pause.");
        }
        // jump forward button stream
        if (((GLubyte) names[i*4+3]==11) && (do_zoom_stream_cover) && (fundet==false)) {
          fundet = true;
          stream_jump=true;
          if (streamoversigt.stream_is_playing) streamoversigt.jump_position(10.0f);
        }
        // jump backward button stream
        if (((GLubyte) names[i*4+3]==10) && (do_zoom_stream_cover) && (fundet==false)) {
          fundet = true;
          stream_jump = true;
          if (streamoversigt.stream_is_playing) streamoversigt.jump_position(-10.0f);
        }
      }
      // film oversigt
      if ((vis_film_oversigt) && (!(fundet))) {
        if ((GLubyte) names[i*4+3]==25) {
          // write debug log
          write_logfile((char *) "Start movie player.");
          fundet = true;
          startmovie = true;
        }
        // stop play movie
        if ((GLubyte) names[i*4+3]==26) {
          // write debug log
          write_logfile((char *) "Stop movie.");
          fundet = true;
          // stop movie playing
          stopmovie = true;
        }
        // we have a select mouse/touch
        if ((!(fundet)) && ((GLuint) names[i*4+3]>=100)) {
          fknapnr=(GLuint) names[i*4+3]-99;			                                // get movie id
          fprintf(stderr,"Film selected=%d\n",fknapnr);                                 //
          // write debug log
          sprintf(debuglogdata,"Film selected=%d",fknapnr);                                 //
          write_logfile((char *) debuglogdata);
          fundet = true;
        }
      }
      // vis ny film oversigt
      if ((vis_nyefilm_oversigt) && (!(fundet))) {
        if ((GLubyte) names[i*4+3]==25) {
          // write debug log
          write_logfile((char *) "Start movie player.");
          fundet = true;
          startmovie = true;
        }
        if ((GLubyte) names[i*4+3]==26) {
          // write debug log
          write_logfile((char *) "Stop movie player.");
          fundet = true;
          stopmovie = true;
        }
        if ((!(fundet)) && ((GLuint) names[i*4+3]>=100)) {
          fknapnr=(GLuint) names[i*4+3]-99;                       // hent filmknap nr
          fprintf(stderr,"Film new selected=%d\n",fknapnr);
          fundet = true;
        }
      }
      // vis tv overview
      if (vis_tv_oversigt) {
        if ((do_show_tvgraber==false) && (!(fundet))) {
          // close view
          if ((GLubyte) names[i*4+3]==27) {
            // write debug log
            write_logfile((char *) "Close tv overview.");
            vis_tv_oversigt = false;
            fundet = true;
          }
          // show recorded programs
          if (((GLubyte) names[i*4+3]==28) && (!(fundet))) {
            write_logfile((char *) "Close recorded overview.");
            vis_tv_oversigt = false;
            fundet = true;
          }
          // close record
          if (((GLubyte) names[i*4+3]==29) && (!(fundet))) {
            if (debugmode & 64) fprintf(stderr,"close start record tv program.\n");
            ask_tv_record = true;
            do_zoom_tvprg_aktiv_nr = 0;
            //                vis_tv_oversigt=false;
            fundet = true;
          }
          //
          // hvis vi viser tv guide og der ikke er valgt vis old rec/vis optager liste
          //
          if ((!(vis_old_recorded)) && (!(vis_tvrec_list))) {
            // er der trykket på et tv program
            if ((!(fundet)) && ((GLubyte) names[i*4+3]>=100) && ((GLubyte) names[i*4+3]<=1000)) {
              tvknapnr=(GLuint) names[i*4+3]-100;        					                // hent tv knap nr
              if (debugmode & 256) fprintf(stderr,"tvknapnr %d.\n",tvknapnr);
              fundet = true;
            }
          }
          // show old recordings
          if ((!(fundet)) && (!(vis_tvrec_list)) && ((GLubyte) names[i*4+3]==44)) {
            fprintf(stderr,"Show old recordings \n");
            vis_old_recorded =! vis_old_recorded;							// SKAL fixes
            fundet = true;
          }
          // show new recordings
          if ((!(fundet)) && (!(vis_old_recorded)) && ((GLubyte) names[i*4+3]==45)) {
            fprintf(stderr,"Show new recordings \n");
            vis_tvrec_list =! vis_tvrec_list;
            fundet = true;
          }
          // er vi igang med at spørge om vi skal optage programmet
          if (ask_tv_record) {
            if (((GLubyte) names[i*4+3]==40) && (!(fundet))) {
              // write debug log
              write_logfile((char *) "Close ask do we record tv program window again.");
              ask_tv_record = false;
              fundet = true;
              returnfunc = 3;
              do_zoom_tvprg_aktiv_nr = 0;
            }
            if (((GLubyte) names[i*4+3]==41) && (!(fundet))) {
              // write debug log
              write_logfile((char *) "Set program to record.");
              ask_tv_record = false;
              fundet = true;
              returnfunc = 3;
              do_zoom_tvprg_aktiv_nr = 0;
              // set start record tv prgoram
              //aktiv_tv_oversigt.gettvprogramrecinfo(tvvalgtrecordnr,tvsubvalgtrecordnr,prgtitle,prgstarttid,prgendtid);
              aktiv_tv_oversigt.tvprgrecord_addrec(tvvalgtrecordnr,tvsubvalgtrecordnr);					// put tv prgoram into table record in mythtv backend (to set mythtv to record the program)
              // opdatere tv guide med nyt info
              aktiv_tv_oversigt.set_program_torecord(tvvalgtrecordnr,tvsubvalgtrecordnr);       // set record flag to show in tv_guide
            }
          }
        } else if ((show_setup_tv_graber) && (!(fundet))) {
          if ((GLubyte) names[i*4+3]==40) {
            write_logfile((char *) "Close tv graber config overview.");
            do_show_tvgraber = false;

            // close tv_graber cofig (mouse)

            // kill running graber
            killrunninggraber();
            // clear old tvguide in db
            aktiv_tv_oversigt.cleartvguide();                             // clear old db
            aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
            // hent/update tv guide from db
            aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
            order_channel_list();                                         // ordre struct
            // save chennel list info to internal datafile
            save_channel_list();                                          // save to db file
            // set update flag in display() func
            firsttime_xmltvupdate = true;                                 // if true reset xml config file
            // close tv graber windows again
            do_show_tvgraber=false;
            do_show_setup=false;

            fundet = true;
          }
        }
      }

      if (!(ask_tv_record)) {
        // show old recorded and close
        if ((!(fundet)) && (vis_old_recorded)) {
          if ((GLubyte) names[i*4+3]==40) {
            vis_old_recorded =! vis_old_recorded;
            fundet = 1;
            returnfunc = 3;
          }
        }
        // show active tv rec list and close
        if ((!(fundet)) && (vis_tvrec_list)) {
          if ((GLubyte) names[i*4+3]==40) {
            vis_tvrec_list =! vis_tvrec_list;
            fundet = 1;
          }
        }
      }
      i--;
    } while ((i>=0) && (!(fundet)));
    /*
    if (debugmode & 1) fprintf(stderr,"%d hits:\n", hits);
    if (hits) {
      if (debugmode & 1) {
        for(i=0;i<hits;i++) {
          fprintf(stderr,"nr %2d stacknr: %d Icon nr on stack %d \n",i, (GLuint)names[i * 4], (GLuint)names[i * 4 + 3]);
        }
      }
    }
    */
    return(returnfunc);
}

// ****************************************************************************************
//
// get selected icon
//
// ****************************************************************************************

int gl_select(int x,int y) {
    GLuint buff[64] = {0};		// info buffer
    GLint hits, view[4];
    //    int id;
    glSelectBuffer(64,buff);	                                                 	// This choose the buffer where store the values for the selection data
    glGetIntegerv(GL_VIEWPORT, view);	                                          // This retrieve info about the viewport
    glRenderMode(GL_SELECT);	                                                 	// Switching in selecton mode
    glInitNames();			                                                        // clear name stack
    glPushName(0);	                                                         		// fill stack with one element (or glloadname give error)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, y, 1.0, 1.0, view);	                                      // restrict the draw to an area around the cursor
    glOrtho(0.0f, (float) orgwinsizex, 0.0f, (float) orgwinsizey, -0.0f,100.0f);
    glMatrixMode(GL_MODELVIEW);		// Draw the objects onto the screen
    glutSwapBuffers();			// draw only the names in the stack, and fill the array
    display();
    glMatrixMode(GL_PROJECTION);		// Do you remeber? We do pushMatrix in PROJECTION mode
    glPopMatrix();
    hits = glRenderMode(GL_RENDER);	// get number of objects drawed in that area
    // and return to render mode
    return(list_hits(hits, buff,x,y));	// return 1 = scroll up 2 = scroll down 0 = no scroll
}

// ****************************************************************************************
//
// ************* mouse handler ********************************************************************************************
//
// ****************************************************************************************

void handleMouse(int button,int state,int mousex,int mousey) {
    //static char huskname[1024];
    char tmp[80];
    char temptxt[200];
    int retfunc=0;
    int spotify_player_start_status;
    int numbers_cd_covers_on_line;
    int numbers_film_covers_on_line;
    int numbers_radio_covers_on_line;
    int numbers_stream_covers_on_line;
    numbers_cd_covers_on_line=8;        // 9
    numbers_film_covers_on_line=9;
    numbers_radio_covers_on_line=8;
    numbers_stream_covers_on_line=9;
    strcpy(temptxt,"");
    saver_irq=true;					// stop screen saver
    if (visur==false) {
      switch(button) {                                                            //
          case GLUT_LEFT_BUTTON:
              if (state==GLUT_UP) {
                retfunc=gl_select(mousex,screeny-mousey);	// hent den som er trykket på
                // nu er mknapnr/fknapnr/rknapnr/spotifyknapnr=den som er trykket på bliver sat i gl_select
                // retfunc er !=0 hvis der er trykket på en knap up/down
                // give error
                if (debugmode & 2) {
                  if ((vis_music_oversigt) && (vis_tv_oversigt==false) && (vis_stream_oversigt==false) && (vis_radio_oversigt==false)) {
                    fprintf(stderr,"mknapnr = %d type = %d \n",mknapnr-1,musicoversigt.get_album_type(mknapnr-1));
                  }
                }
                if (vis_tv_oversigt) {
                  if (debugmode & 8) fprintf(stderr,"tvknapnr = %d\n",tvknapnr-1);
                }
                if (vis_spotify_oversigt) {
                  if (debugmode & 8) fprintf(stderr,"spotifyknapnr = %d\n",spotifyknapnr-1);
                }
                #ifdef ENABLE_TIDAL
                if (vis_tidal_oversigt) {
                  if (debugmode & 8) fprintf(stderr,"tidalknapnr = %d\n",tidalknapnr-1);
                }
                #endif
                if (vis_stream_oversigt) {
                  if (debugmode & 4) fprintf(stderr,"sknapnr = %d\n",sknapnr-1);
                }
                if (vis_tv_oversigt) {
                  if (debugmode & 8) fprintf(stderr,"tv prg knapnr = %d\n",mknapnr-1);
                }
                // any music buttons active
                if ((mknapnr>0) && (vis_music_oversigt)) {
                  if ((retfunc==0) && ((mknapnr-1==0) || (musicoversigt.get_directory_id(mknapnr-1)!=0)) && (!(do_zoom_music_cover))) {
                    if (musicoversigt.get_album_type(mknapnr-1)==0) {
                      if (debugmode & 2) fprintf(stderr,"Normal dir id load.\n");
                      do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1); 	// set det aktiv dir id
                      antal_songs=hent_antal_dir_songs(musicoversigt.get_directory_id(mknapnr-1));    // loader antal dir/song i dir id
                      if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d name %s \n",antal_songs,musicoversigt.get_album_name(mknapnr-1));
                      if ((antal_songs==0) || (musicoversigt.get_directory_id(mknapnr-1)==0)) {
                        ask_open_dir_or_play_aopen = true;
                      } else {
                        ask_open_dir_or_play_aopen = false;
                      }
                    } else {
                      // here
                      if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                      // playlist loader
                      do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                      if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                      if (do_play_music_aktiv_nr>0) {
                        antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                      } else antal_songs=0;
                      // write debug log
                      sprintf(debuglogdata,"Found numbers of songs:%2d",antal_songs);
                      write_logfile((char *) debuglogdata);
                      if (antal_songs==0) {
                        ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                      } else {
                        ask_open_dir_or_play_aopen = false;
                      }
                    }
                    if (do_play_music_aktiv_nr) {						// er der et dirid/playlistid
                      ask_open_dir_or_play = true;						// yes ask om de skal spilles
                    } else ask_open_dir_or_play = true;
                  }
                }
                // stream stuf open stram and show rss feeds
                if ((vis_stream_oversigt) && (retfunc==0) && (stopstream==false) && (do_zoom_stream_cover==false)) {
                  if (sknapnr>0) {
                    do_play_stream = 1;						// select button do open or play
                    if (debugmode & 4) fprintf(stderr,"Set do_play_stream flag %d \n",sknapnr);
                  }
                }
                // radio stuf
                if ((vis_radio_oversigt) && (retfunc==0)) {
                  if (rknapnr>0) {
                    do_play_radio = 1;						// select button do play
                    if (debugmode) fprintf(stderr,"Set do_play_radio flag rknapnr=%d \n",rknapnr);
                  }
                }
                #ifdef ENABLE_SPOTIFY
                if (vis_spotify_oversigt) {
                  // normal spotify stuf
                  if (do_show_spotify_search_oversigt==false) {
                    // spotify stuf
                    // play
                    if (retfunc==3) {
                      if (spotifyknapnr>0) {
                        do_play_spotify=1;
                        do_open_spotifyplaylist=0;
                        fprintf(stderr,"Set do_play_spotify flag spotifyknapnr=%d \n",spotifyknapnr);
                      }
                    }
                    // stop play
                    if (retfunc==2) {
                      if (spotifyknapnr==9) {
                        do_play_spotify=0;
                        do_open_spotifyplaylist=0;
                        // write debug log
                        write_logfile((char *) "Set stop play spotify flag");
                      }
                    }
                    // next play
                    if (retfunc==6) {
                      do_play_spotify=0;
                      do_open_spotifyplaylist=1;
                      // write debug log
                      write_logfile((char *) "Set next play spotify flag");
                      spotify_oversigt.spotify_next_play();
                    }
                    // last play
                    if (retfunc==7) {
                      do_play_spotify=0;
                      do_open_spotifyplaylist=1;
                      // write debug log
                      write_logfile((char *) "Set last play spotify flag");
                      spotify_oversigt.spotify_last_play();
                    }
                  }
                  // online spotify stuf
                  if (do_show_spotify_search_oversigt==true) {
                    // spotify stuf
                    if (retfunc==4) {
                      if (spotifyknapnr>0) {
                        do_play_spotify=1;
                        do_open_spotifyplaylist=0;
                        fprintf(stderr,"Set do_play_spotify flag spotifyknapnr=%d \n",spotifyknapnr);
                      }
                    }
                    // open
                    if (retfunc==6) {
                      if (spotifyknapnr>0) {
                        do_play_spotify=1;
                        do_open_spotifyplaylist=0;
                        fprintf(stderr,"Open spotify flag spotifyknapnr=%d \n",spotifyknapnr);
                      }
                    }
                    // stop play online
                    if (retfunc==5) {
                      if (spotifyknapnr==9) {
                        do_play_spotify=0;
                        do_open_spotifyplaylist=0;
                        fprintf(stderr,"Set stop play spotify flag\n");
                      }
                    }
                    // next play  online
                    if (retfunc==7) {
                      do_play_spotify=0;
                      do_open_spotifyplaylist=1;
                      fprintf(stderr,"Set next play spotify flag\n");
                      spotify_oversigt.spotify_next_play();
                    }
                    // last play online
                    if (retfunc==8) {
                      do_play_spotify=0;
                      do_open_spotifyplaylist=1;
                      fprintf(stderr,"Set last play spotify flag\n");
                      spotify_oversigt.spotify_last_play();
                    }
                  }
                }
                #endif
                // ved vis film oversigt
                if ((vis_film_oversigt) & (retfunc==0)) {
                  do_zoom_film_cover = true;
                  do_zoom_film_aktiv_nr = fknapnr;
                  do_swing_movie_cover = 1;
                }
                // ved vis tv oversigt
                if ((vis_tv_oversigt) && (retfunc==0)) {
                  /*
                  ask_tv_record=true;
                  do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
                  tvsubvalgtrecordnr=tvknapnr;
                  */
                  ask_tv_record=true;
                  do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
                  tvvalgtrecordnr=(tvknapnr / 100);
                  tvsubvalgtrecordnr=tvknapnr-(tvvalgtrecordnr*100);
                  if (debugmode & 256) fprintf(stderr,"tv prg selected in array is = %d  \n",tvknapnr);
                  if (debugmode & 256) fprintf(stderr,"tv kanal %d prgnr %d  \n",tvvalgtrecordnr,tvsubvalgtrecordnr);
                }
                // fra start film nyt fast show
                if ((vis_nyefilm_oversigt) && (retfunc==0)) {
                  do_zoom_film_cover = true;
                  do_zoom_film_aktiv_nr = fknapnr;
                  do_swing_movie_cover = 1;
                }
                // set flag to update
                if ((do_show_tvgraber) && (retfunc==0)) {
                  fprintf(stderr,"* Delete old tvguide *\n");
                  unlink("~/tvguide_channels.dat");
                  fprintf(stderr,"* Update new tvguide *\n");
                  hent_tv_channels=false;
                }
              }
              break;
          case GLUT_RIGHT_BUTTON:
              if (vis_music_oversigt) {
                if ((ask_open_dir_or_play) && (state==GLUT_UP)) {
                  ask_open_dir_or_play=false;
                  mknapnr=0;
                } else if ((ask_open_dir_or_play==false) && (do_play_music_aktiv_table_nr>0) && (state==0) && (button==2)) {
                  do_zoom_music_cover=!do_zoom_music_cover;
                  if (ask_open_dir_or_play) ask_open_dir_or_play=false;
                }
              }
              // close/show net radio play status window flag
              if ((vis_radio_oversigt) && (state==GLUT_UP)) {
                do_zoom_radio=!do_zoom_radio;
              }
              // close ask film again
              if ((vis_film_oversigt) && (state==GLUT_UP)) {
                if (do_zoom_film_cover) do_zoom_film_cover=false; else if (!(do_zoom_film_cover)) do_zoom_film_cover=true;
              }
              // close ask tv again
              if ((vis_tv_oversigt) && (state==GLUT_UP)) {
                ask_tv_record=true;
                do_zoom_tvprg_aktiv_nr=0;
              }
              // close vis stream play
              if ((vis_stream_oversigt) && (state==GLUT_UP)) {
                if (debugmode & 4) fprintf(stderr,"sknapnr %d stream_playnr %d \n",sknapnr,stream_playnr);                                      // show last selected no stream is playing (stream_playnr=0)
                if ((sknapnr>0) && (stream_playnr>0)) do_zoom_stream_cover=!do_zoom_stream_cover;
              }
              // close info (do not show new movies)
              if ((vis_nyefilm_oversigt) && (state==GLUT_UP)) {
                vis_nyefilm_oversigt=!vis_nyefilm_oversigt;
              }
              if (vis_spotify_oversigt) {
                // close ask play window again no selected icon right click
                if ((ask_open_dir_or_play_spotify) && (state==GLUT_UP)) {
                  ask_open_dir_or_play_spotify=false;
                  do_zoom_spotify_cover=false;
                } else if ((ask_open_dir_or_play_spotify==false) && (state==GLUT_UP)) {
                  // show what we play or not show what we play
                  do_zoom_spotify_cover=!do_zoom_spotify_cover;                                             // close/open window
                  do_select_device_to_play=false;                                                           // close window
                }
              }
              break;
      }
      if (vis_music_oversigt) {
        // auto opendir
        if ((ask_open_dir_or_play_aopen) && (retfunc==0)) {
          ask_open_dir_or_play=false;
          ask_open_dir_or_play_aopen=false;
          if (musicoversigt.get_album_type(mknapnr-1)==-1) {
            // write debug log
            sprintf(debuglogdata,"Open/read playlist id %d ",musicoversigt.get_directory_id(mknapnr-1));
            write_logfile((char *) debuglogdata);
            sprintf(debuglogdata,"Opdatere musicarray henter playlist oversigt");
            // write debug log
            write_logfile((char *) debuglogdata);

            // hent playlist oversigt
            // new ver
           musicoversigt.opdatere_music_oversigt_playlists();	// hent list over mythtv playlistes

            // old ver
            //opdatere_music_oversigt_playlists(musicoversigt);	// hent list over mythtv playlistes
          } else {
            // write debug log
            sprintf(debuglogdata,"Opdatere musicarray Henter oversigt dir id = %d ",musicoversigt.get_directory_id(mknapnr-1));
            write_logfile((char *) debuglogdata);
            // opdate fra mythtv-backend if avable

            // New ver
            if (musicoversigt.opdatere_music_oversigt(musicoversigt.get_directory_id(mknapnr-1))>0) {
             musicoversigt.opdatere_music_oversigt_icons();                                  // load icons
            } else {
              // opdatere music oversigt fra internpath
              fprintf(stderr,"nr %d path=%s\n",mknapnr-1,musicoversigt.get_album_path(mknapnr-1));

              // New ver
              if (musicoversigt.opdatere_music_oversigt_nodb()==0) {
                // no update posible
                fprintf(stderr,"No Music loaded/found by internal loader.\n");
                write_logfile("No Music loaded/found by internal loader.");
              }
              /*
              // old ver
              if (opdatere_music_oversigt_nodb(musicoversigt[mknapnr].album_path,musicoversigt)==0) {
                // no update posible
                fprintf(stderr,"No Music loaded/found by internal loader.\n");
                write_logfile("No Music loaded/found by internal loader.");
              }
              */
            }
          }
          /// reset mouse/key pos in vis_music_oversigt
          mknapnr = 0;
          music_key_selected = 1;
          music_select_iconnr = 0;
          music_icon_anim_icon_ofset = 0;
          music_icon_anim_icon_ofsety = 0;
          _mangley= 0.0f;
        } else {
            //if (debugmode & 2) printf("der scrolles _mangley =%d \n",(_mangley/41)*5);
            //do_find_playlist=true;
            //do_play_music_cover=true;
            // playlist oversigt fil loader
            /*
            if ((musicoversigt[mknapnr-1].oversigttype==-1) && (musicoversigt[mknapnr-1].directory_id!=0)) {	// der er et playlistid til mysql database
                // user open button pressed in ask_open_dir_or_play window
            ask_open_dir_or_play=false;                                                     // luk vindue igen
                if (debugmode & 2) printf("Opdatere musicarray oversigt Henter dir id = %d \n",musicoversigt[mknapnr-1].directory_id);
                opdatere_music_oversigt(musicoversigt,musicoversigt[mknapnr-1].directory_id);               // ************************
                opdatere_music_oversigt_icons();                                                            // opdatere icons visning
                printf("Loaded numbers of covers: %d \n",musicoversigt_antal);
                mknapnr=0;
                music_key_selected=1;
                music_icon_anim_icon_ofset=0;
                music_icon_anim_icon_ofsety=0;
                _mangley=0.0f;
            }
            */
        }
        // do scroll up/down view mouse or screen button
        if  (!(ask_open_dir_or_play)) {
          // scroll down
          //printf("music_icon_anim_icon_ofsety= %d  divide = %d  \n",music_icon_anim_icon_ofsety,musicoversigt_antal/numbers_cd_covers_on_line);
          if (((retfunc==2) || (button==4)) && ((_mangley/41.0f)+2<(int) (musicoversigt_antal/numbers_cd_covers_on_line)) && (music_icon_anim_icon_ofset==0)) { // scroll button
            do_music_icon_anim_icon_ofset=1;			             	// direction -1 = up 1 = down
            _mangley+=(41.0f);				                      		// scroll window down one icon
            music_select_iconnr+=numbers_cd_covers_on_line;			// add to next line
          }
          // scroll up
          if (((retfunc==1) || (button==3)) && (_mangley>0)) {
            do_music_icon_anim_icon_ofset=-1;			              // direction -1 = up 1 = down
            _mangley-=(41.0f);			                       			// scroll window up
            music_select_iconnr-=numbers_cd_covers_on_line;			// add to next line
          }
        }
      }

      //printf("spotify playlist nr %d type = %d \n ",spotifyknapnr,spotify_oversigt.get_spotify_type(spotifyknapnr));
      #ifdef ENABLE_SPOTIFY
      if ((vis_spotify_oversigt) && (do_show_spotify_search_oversigt==false)) {
        // scroll spotify up/down
        // and open / play / stop / next / last play control // open playlist
        if (!(do_zoom_spotify_cover)) {
          if ((retfunc == 2 ) || (button == 4 )) {
            if (spotify_selected_startofset+40<spotify_oversigt.streamantal()) spotify_selected_startofset+=8;
            button=0;
          }
          // scroll up
          if ((retfunc == 1 ) || (button == 3 )) {
            if ((spotify_selected_startofset+8)>8) spotify_selected_startofset-=8;
            if (spotify_selected_startofset<0) spotify_selected_startofset=0;
            button=0;
          }
        }
        if (do_zoom_spotify_cover) {
          // pause/stop spotify music
          if (( retfunc == 5 ) || (button==3)) {
            if (do_play_spotify_cover==false) spotify_oversigt.spotify_resume_play();
            spotify_oversigt.spotify_pause_play();                              // spotify stop play
            do_zoom_spotify_cover=false;
            ask_open_dir_or_play_spotify=false;
            do_play_spotify_cover=false;
            do_select_device_to_play=false;                                     // stop show device
            spotifyknapnr=0;
            spotify_selected_startofset=0;                                      //
          }
        } else {
          // open spotify playlist
          if ((( retfunc == 3 ) || (button==3)) && (spotifyknapnr>0)) {
            ask_open_dir_or_play_spotify=false;
            // write debug log
            sprintf(debuglogdata,"Open spotify playliste %s ", spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1));
            write_logfile((char *) debuglogdata);
            // opdate view from intnr id.
            spotify_oversigt.opdatere_spotify_oversigt(spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1));         // update view
            spotify_oversigt.load_spotify_iconoversigt();                                                                 // load icons
            //spotify_oversigt.set_search_loaded();                           // triger icon loader
            spotifyknapnr=0;                                                    // reset select
            spotify_selected_startofset=0;                                      //
            strcpy(spotify_oversigt.overview_show_band_name,"");
          }
          // play spotify playlist or song
          if (((retfunc==4) || (retfunc==5)) && (spotifyknapnr>0)) {
            switch(spotify_oversigt.get_spotify_type(spotifyknapnr-1)) {
              case 0: fprintf(stderr,"play nr %d spotify playliste id %s named %s \n",spotifyknapnr-1, spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1),spotify_oversigt.get_spotify_name(spotifyknapnr-1));
                      break;
              case 1: fprintf(stderr,"play nr %d spotify song id %s named %s \n", spotifyknapnr-1, spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1),spotify_oversigt.get_spotify_name(spotifyknapnr-1));
                      break;
              default: fprintf(stderr,"Error in type. Type found %d  \n",spotify_oversigt.get_spotify_type(spotifyknapnr-1));
                      break;

            }
            //
            ask_open_dir_or_play_spotify=false;                                                               // close widow
            if (strcmp(spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1),"")!=0) {
              // try load and start playing playlist
              if (spotify_oversigt.get_spotify_type(spotifyknapnr-1)==0) {
                spotify_player_start_status = spotify_oversigt.spotify_play_now_playlist( spotify_oversigt.get_spotify_playlistid( spotifyknapnr-1 ), 1);
              }
              // try load and  play song
              if (spotify_oversigt.get_spotify_type(spotifyknapnr-1)==1) {
                spotify_player_start_status = spotify_oversigt.spotify_play_now_song( spotify_oversigt.get_spotify_playlistid( spotifyknapnr-1 ), 1);
              }
              //do_select_device_to_play=false;
              if (spotify_player_start_status==0) fprintf(stderr,"spotify start play return ok.\n");
                else fprintf(stderr,"spotify start play return value %d \n ",spotify_player_start_status);
              if (spotify_player_start_status == 0) {
                if (spotify_oversigt.get_spotify_type(spotifyknapnr-1)==0) strcpy(spotify_oversigt.spotify_playlistname,spotify_oversigt.get_spotify_name(spotifyknapnr-1));
                else strcpy(spotify_oversigt.spotify_playlistname,"None");
                do_play_spotify_cover=true;
                do_zoom_spotify_cover=true;                                       // show we play
              } else {
                printf("Error start plying spotify song \n");
                do_play_spotify_cover=false;
                do_zoom_spotify_cover=false;                                       // show we play
                // error start playing
              }
            } else {
              printf("Error playid i smissing.\n");
            }
          }
        }
      }
      // and open / play / stop / next / last play control / open playlist / scroll
      if ((vis_spotify_oversigt) && (do_show_spotify_search_oversigt==true)) {
        if (!(do_zoom_spotify_cover)) {
          // scroll down
          if ((retfunc==2) || (button==4)) {
            if (spotify_selected_startofset+40<spotify_oversigt.streamantal()) spotify_selected_startofset+=8;
            button=0;
          }
          // scroll up
          if ((retfunc==1) || (button==3)) {
            if ((spotify_selected_startofset+8)>8) spotify_selected_startofset-=8;
            if (spotify_selected_startofset<0) spotify_selected_startofset=0;
            button=0;
          }
          // back
          if (((spotifyknapnr-1)==0) && (strcmp(spotify_oversigt.get_spotify_name(spotifyknapnr-1),"Back")==0)) {
            if ( debugmode & 4 ) fprintf(stderr,"Back button from search \n");
            spotify_oversigt.clean_spotify_oversigt();
            //printf("huskname %s \n",huskname  );
            spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,0); //type 3 = tracks ()
            //spotify_oversigt.load_spotify_iconoversigt();                       // load icons
            spotify_oversigt.set_search_loaded();                           // triger icon loader
            //spotify_oversigt.opdatere_spotify_oversigt(0);                  // reset spotify overview
            spotifyknapnr=0;                                                  // reset selected
            spotify_selected_startofset=0;
            strcpy(spotify_oversigt.overview_show_band_name,"");
          }
        }
        if (do_zoom_spotify_cover) {
          // pause/stop spotify music
          if ((retfunc==5) || (button==3)) {
            #ifdef ENABLE_SPOTIFY
            if (do_play_spotify_cover==false) spotify_oversigt.spotify_resume_play();
            spotify_oversigt.spotify_pause_play();                              // spotify stop play
            do_zoom_spotify_cover=false;
            ask_open_dir_or_play_spotify=false;
            do_play_spotify_cover=false;
            do_select_device_to_play=false;                                     // stop show device
            spotifyknapnr=0;
            #endif
          }
        } else {
          // open spotify artist
          if (((retfunc == 6 ) || (button == 3 )) && (spotifyknapnr>0)) {
            ask_open_dir_or_play_spotify=false;
            // write debug log
            sprintf(debuglogdata,"Open spotify artist %s type %d ", spotify_oversigt.get_spotify_name(spotifyknapnr-1),spotify_oversigt.type);
            write_logfile((char *) debuglogdata);
            // update from web
            // clear old first
            //static char huskname[1024];
            strcpy(huskname,spotify_oversigt.get_spotify_name(spotifyknapnr-1));
            strcpy(spotify_oversigt.overview_show_band_name,huskname);
            printf("Loading view......\n");
            // Ingen icons bliver loaded da det er url som staar i gfxlink og er IKKE downloaded
            spotify_oversigt.clean_spotify_oversigt();                                              //
            if (huskname) spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(huskname,3);  //type 3 = tracks ()
            else spotify_oversigt.opdatere_spotify_oversigt(0);
            //spotify_oversigt.set_search_loaded();                           // triger icon loader
            //spotify_oversigt.load_spotify_iconoversigt();                                           // load icons
            spotify_oversigt.set_search_loaded();
            printf("Done Loading view......\n");
            // reset select in spotify view
            spotifyknapnr=0;                                                  // reset selected
            spotify_selected_startofset=0;
          }
          // spotify play artist/or playlist / or cd
          if ((((retfunc==4) || (retfunc==5)) || (button==3)) && (spotifyknapnr>0)) {
            ask_open_dir_or_play_spotify=false;                                                               // close widow again
            if (strcmp(spotify_oversigt.get_spotify_name(spotifyknapnr-1),"")!=0) {
              switch (spotify_oversigt.get_spotify_type(spotifyknapnr-1)) {
                case 0: fprintf(stderr,"button nr %d play Spotify playlist %s type = %d\n",spotifyknapnr-1,spotify_oversigt.get_spotify_name(spotifyknapnr-1),spotify_oversigt.get_spotify_type(spotifyknapnr-1));
                        spotify_player_start_status = spotify_oversigt.spotify_play_now_playlist( spotify_oversigt.get_spotify_playlistid( spotifyknapnr-1 ), 1);
                        break;
                case 1: fprintf(stderr,"button nr %d play Spotify artist song %s type = %d\n",spotifyknapnr-1,spotify_oversigt.get_spotify_name(spotifyknapnr-2),spotify_oversigt.get_spotify_type(spotifyknapnr-2));
                        spotify_player_start_status = spotify_oversigt.spotify_play_now_song( spotify_oversigt.get_spotify_playlistid( spotifyknapnr-2 ), 1);
                        break;
                case 2: fprintf(stderr,"button nr %d play Spotify artist name %s type = %d\n",spotifyknapnr-1,spotify_oversigt.get_spotify_name(spotifyknapnr-1),spotify_oversigt.get_spotify_type(spotifyknapnr-1));
                        spotify_player_start_status = spotify_oversigt.spotify_play_now_artist( spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1) , 1);
                        break;
                case 3: fprintf(stderr,"button nr %d play Spotify album %s type = %d\n",spotifyknapnr-1,spotify_oversigt.get_spotify_name(spotifyknapnr-1),spotify_oversigt.get_spotify_type(spotifyknapnr-1));
                        // do not play the right album
                        spotify_player_start_status = spotify_oversigt.spotify_play_now_album( spotify_oversigt.get_spotify_playlistid(spotifyknapnr-1) , 1);
                        break;
              }
              if (spotify_player_start_status==0) {
                switch (spotify_oversigt.get_spotify_type(spotifyknapnr-1)) {
                  case 0: fprintf(stderr,"spotify start play playlist return ok.\n");
                          break;
                  case 1: fprintf(stderr,"spotify start play song return ok.\n");
                          break;
                  case 2: fprintf(stderr,"spotify start play artist return ok.\n");
                          break;
                  case 3: fprintf(stderr,"spotify start play cd return ok.\n");
                          break;
                }
              } else fprintf(stderr,"spotify start play artist return value %d \n ",spotify_player_start_status);
              if (spotify_player_start_status == 0) {
                do_play_spotify_cover=true;
                do_zoom_spotify_cover=true;                                       // show we play
              }
            }
          }
        }
      }
      #endif
      //
      // scroll tv guide up/down
      //
      if (vis_tv_oversigt) {
        // scroll tv guide down
        if ((retfunc==2) || (button==4)) { // scroll button
          if (aktiv_tv_oversigt.vistvguidekl<24*2) aktiv_tv_oversigt.vistvguidekl++;
        }
        // scroll up
        if ((retfunc==1) || (button==3)) {
          if (aktiv_tv_oversigt.vistvguidekl>0) aktiv_tv_oversigt.vistvguidekl--; else aktiv_tv_oversigt.vistvguidekl=24;
        }
      }
      // scroll film up/down
      // scroll down
      if (vis_film_oversigt) {
        if (((button==4) || (retfunc==2)) && ((unsigned int) film_select_iconnr+9<film_oversigt.film_antal()-1)) {
          do_movie_icon_anim_icon_ofset=1;
          switch(screen_size) {
              case 1: _fangley+=(41.0f);					// 51 scroll 1 line
                      break;
              case 2: _fangley+=(41.0f);
                      break;
              case 3: _fangley+=(41.0f);					// scroll 2 lines
                      break;
              case 4: _fangley+=(41.0f);
                      break;
          }
          film_select_iconnr+=numbers_film_covers_on_line;			// move to next line
        }
        // scroll up
        if (((button==3) || (retfunc==1)) && (film_select_iconnr-4>0)) {
          do_movie_icon_anim_icon_ofset=-1;
          switch(screen_size) {
              case 1: _fangley-=(41.0f);					// 51 scroll 1 line
                      break;
              case 2: _fangley-=(41.0f);					// 49
                      break;
              case 3: _fangley-=(41.0f);					// 46 scroll 2 lines
                      break;
              case 4: _fangley-=(41.0f);					// 44
                      break;
          }
          film_select_iconnr-=numbers_film_covers_on_line;			// move to last line
          if (film_select_iconnr<0) film_select_iconnr=0;       //
        }
      }
      // scroll radio up/down
      if (vis_radio_oversigt) {
        // scroll down
        //printf("radio rangley= %d   \n",_rangley);
        if (((retfunc==2) || (button==4)) && ((_rangley/41.0f)+4<(int) (radiooversigt_antal/numbers_radio_covers_on_line))) { // scroll button
          //do_music_icon_anim_icon_ofset=1;				// direction -1 = up 1 = down
          _rangley+=(41.0f);						// scroll window down one icon
          radio_select_iconnr+=numbers_radio_covers_on_line;			// add to next line
        }
        // scroll up
        if (((retfunc==1) || (button==3)) && (_rangley>0)) {
          //do_music_icon_anim_icon_ofset=-1;				// direction -1 = up 1 = down
          _rangley-=(41.0f);						// scroll window up
          radio_select_iconnr-=numbers_radio_covers_on_line;			// add to next line
        }
      }
      // update or scroll stream up/down/play
      if ((vis_stream_oversigt) && (stream_jump==false)) {
        if ((retfunc==0) && (sknapnr>0) && (do_play_stream)) {
          if (debugmode) fprintf(stderr,"sknapnr %d  path_antal=%d type %d stream antal = %d \n",sknapnr-1,streamoversigt.get_stream_groupantal(sknapnr-1),streamoversigt.type,streamoversigt.streamantal());
          if (streamoversigt.type==0) {
            strncpy(temptxt,streamoversigt.get_stream_name(sknapnr-1),200);
            streamoversigt.clean_stream_oversigt();
            fprintf(stderr,"stream nr %d name %s \n ",sknapnr-1,temptxt);
            streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");
            do_play_stream=false;
          } else if ((sknapnr-1>0) && (streamoversigt.type!=1)) {
            // update
            streamoversigt.clean_stream_oversigt();
            streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr-1),streamoversigt.get_stream_path(sknapnr-1));
            do_play_stream=false;
          } else {
            // back button
            if ((sknapnr-1)==0) {
              if (streamoversigt.type==2) {
                // one level up
                streamoversigt.clean_stream_oversigt();
                streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr-1),(char *)"");
                do_play_stream=false;
              } else {
                // jump to top
                streamoversigt.clean_stream_oversigt();
                streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                do_play_stream=false;
              }
            }
          }
          // play stream
          if ((sknapnr-1>=0) && (do_play_stream)) {
            if ((streamoversigt.get_stream_url(sknapnr-1)) && (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash",9)==0)) {
              startstream=true;
              do_zoom_stream=true;                    // set show player
            } else {
              if (streamoversigt.get_stream_url(sknapnr-1)) startstream=true;
              do_zoom_stream=true;                    // set show player
            }
            fprintf(stderr,"Set show playing stream\n ");
          }
          //sknapnr=0;
          stream_key_selected = 1;
          stream_select_iconnr = 0;
          _sangley = 0.0f;
        }
        if (((retfunc==2) || (button==4)) && ((_sangley/41.0f)+4<(int) (streamoversigt.streamantal()/numbers_stream_covers_on_line))) { // scroll button
          //do_music_icon_anim_icon_ofset=1;				                // direction -1 = up 1 = down
          _sangley+=(41.0f);						                            // scroll window down one icon
          stream_select_iconnr+=numbers_stream_covers_on_line;			// add to next line
        }
        // scroll up
        if (((retfunc==1) || (button==3)) && (_sangley>0)) {
          //do_music_icon_anim_icon_ofset=-1;				                // direction -1 = up 1 = down
          _sangley-=(41.0f);						                            // scroll window up
          stream_select_iconnr-=numbers_stream_covers_on_line;			// add to next line
        }
      }
    }
}




// ****************************************************************************************
//
// handle keys
// like functions keys
//
// ****************************************************************************************

void handlespeckeypress(int key,int x,int y) {
    float MOVIE_CS;
    float MUSIC_CS;
    float RADIO_CS;
    unsigned int mnumbersoficonline;
    int numbers_cd_covers_on_line=8;
    int fnumbersoficonline;
    int rnumbersoficonline;
    int snumbersoficonline;
    saver_irq=true;                                     // stop screen saver
    mnumbersoficonline=8;		// antal i music oversigt
    fnumbersoficonline=8;	  // antal i film oversigt
    rnumbersoficonline=8;   // antal i radio oversigt
    snumbersoficonline=8;   // antal i stream/spotify oversigt
    MOVIE_CS=46.0f;					// movie dvd cover side
    MOVIE_CS=46.0f;					// movie dvd cover side
    MOVIE_CS=46.0f;					// movie dvd cover side
    MUSIC_CS=41.0;					// music cd cover side
    RADIO_CS=41.0;					// radio cd cover side
    switch(key) {
        // F1 setup menu
        case 1: if (vis_tv_oversigt) {
                  do_show_tvgraber=!do_show_tvgraber;
                  if (do_show_tvgraber) write_logfile((char *) "Show TV channel Setup menu."); else write_logfile((char *) "Hide TV channel Setup menu.");
                  // update the tv overview
                  if (do_show_tvgraber==false) {
                    //order_channel_list_in_tvguide_db();
                    //aktiv_tv_oversigt.set_channel_state(channel_list);                      // update channel struct
                    //aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                    //update_xmltv_phread_loader();                   // start thred update flag in main loop
                    // F1 key
                    killrunninggraber();
                    // clear old tvguide in db
                    aktiv_tv_oversigt.cleartvguide();
                    aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
                    order_channel_list();                                         // ordre struct
                    save_channel_list();                                          // save to db file
                    // hent/update tv guide from db
                    aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
                    // set update flag in display() func
                    firsttime_xmltvupdate = true;                                 // if true reset xml config file
                  }
                } else if (vis_music_oversigt) {
                  if (findtype==0) findtype=1;
                  else if (findtype==1) findtype=0;
                } else if ((!(vis_radio_oversigt)) && (!(vis_radio_or_music_oversigt))) {
                  if (do_show_setup) do_save_config = true;		                   // set save config file flag
                  do_save_setup_rss = true;                                      // save rss setup
                  vis_radio_oversigt = false;
                  vis_tv_oversigt = false;
                  vis_film_oversigt = false;
                  vis_music_oversigt = false;
                  vis_recorded_oversigt = false;
                  vis_stream_oversigt = false;
                  vis_radio_or_music_oversigt = false;
                  vis_stream_or_movie_oversigt = false;
                  do_show_setup =! do_show_setup;
                }
                break;
        case 2:
                // F2 exit app key
                fprintf(stderr,"Close down now exit(2)\n");
                remove("mythtv-controller.lock");
                exit(2);
                break;
        case 3: // F3 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[0].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[0].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[0].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[0]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 4: // F4 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[1].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[1].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[1].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[1]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 5: // F5 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[2].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[2].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[2].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[2]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 6: // F6 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[3].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[3].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[3].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[3]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 7: // F7 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[4].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[4].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[4].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[4]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 8: // F8 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[5].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[5].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[5].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[5]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 9: // F9 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[6].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[6].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[6].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[6]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 10: // F10 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[7].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[7].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[7].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[7]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 11: // F11 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[8].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[8].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[8].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[8]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 12: // F12 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[9].cmdname,"playlistbackup")==0) {
                  do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[9].cmdname,"playlistrestore")==0) {
                  do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[9].cmdname,"")!=0) {
                  saveexitcommand(configkeyslayout[9]);
                  doexitcommand();
                  //exit(100);
                }
                break;
        case 100:  // left key
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                  if (music_key_selected>1) {
                    music_key_selected--;
                    music_select_iconnr--;
                  } else {
                    if ((music_select_iconnr>0) && (_mangley>0)) {
                       _mangley-=MUSIC_CS;
                       music_key_selected+=mnumbersoficonline-1;  // den viste på skærm af 1 til 20
                       music_select_iconnr--;                  	// den rigtige valgte af 1 til cd antal
                    }
                  }
                }
                // move left
                if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify))) {
                  if (spotifyknapnr>1) {
                    if (spotifyknapnr==1) {
                      if (spotify_selected_startofset>0) {
                        spotify_selected_startofset-=8;
                        spotifyknapnr+=7;
                      }
                    }
                    spotifyknapnr--;
                    spotify_key_selected--;
                    spotify_select_iconnr--;
                  } else {
                    if (spotify_selected_startofset>0) {
                      spotify_selected_startofset-=8;
                      spotifyknapnr+=7;
                    }
                  }
                }
                // film
                if (vis_film_oversigt) {
                  if (film_key_selected>1) {
                    film_key_selected--;
                    film_select_iconnr--;
                  } else {
                    if ((film_select_iconnr>0) && (_fangley>0)) {
                      _fangley-=MOVIE_CS;
                      film_key_selected+=fnumbersoficonline-1;	// den viste på skærm af 1 til 20
                      film_select_iconnr--;			// den rigtige valgte af 1 til cd antal
                    }
                  }
                }
                if (vis_recorded_oversigt) {
                  if (visvalgtnrtype==1) visvalgtnrtype=2;
                  else if (visvalgtnrtype==2) visvalgtnrtype=1;
                }
                if (vis_radio_oversigt) {
                  if (radio_key_selected>1) {
                    radio_key_selected--;
                    radio_select_iconnr--;
                  } else {
                    if ((radio_select_iconnr>0) && (_rangley>0)) {
                      _rangley-=RADIO_CS;
                      radio_key_selected+=rnumbersoficonline-1;	// den viste på skærm af 1 til 20
                      radio_select_iconnr--;			// den rigtige valgte af 1 til cd antal
                    }
                  }
                }
                if (vis_stream_oversigt) {
                  if (stream_key_selected>1) {
                    stream_key_selected--;
//                    stream_select_iconnr--;
                  } else {
                    if ((stream_select_iconnr>0) && (_sangley>0)) {
                      _sangley-=RADIO_CS;
                      stream_key_selected+=snumbersoficonline-1;	// den viste på skærm af 1 til 20
//                      stream_select_iconnr--;			                // den rigtige valgte af 1 til cd antal
                    }
                  }
                  if (stream_select_iconnr>0) stream_select_iconnr--;
                }
                // left key
                // if indside tv overoview
                // normal tv overview select last channel
                if (vis_tv_oversigt) {
                  // Normal view
                  if (!(do_show_tvgraber)) {
                    if ((tvvisvalgtnrtype==1) && (tvvalgtrecordnr>0)) {
                      tvvalgtrecordnr--;
                      tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr+1,tvsubvalgtrecordnr));
                    }
                    // Show tv graber setup view over tv_oversigt
                  } else if (do_show_tvgraber) {
                    // first reset all other
                    for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                    channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=false;
                  }
                }
                if ((do_show_setup) && (do_show_tvgraber)) {
                  // first reset all other
                  for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                  channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=false;
                }
                break;
        case 102: // key right
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr<musicoversigt_antal)) {
                  if ((music_key_selected % (mnumbersoficonline*4)==0) || ((music_select_iconnr==((mnumbersoficonline*4)-1)) && (music_key_selected % mnumbersoficonline==0))) {
                    _spangley+=MUSIC_CS;
                    music_key_selected-=mnumbersoficonline;			// den viste på skærm af 1 til 20
                    music_select_iconnr++;	                 		// den rigtige valgte af 1 til cd antal
                  } else {
                    music_select_iconnr++;			                // den rigtige valgte af 1 til cd antal
                  }
                  music_key_selected++;
                }
                // spotify normal
                // key right
                #ifdef ENABLE_SPOTIFY
                if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify))) {
                  if (do_show_spotify_search_oversigt==false) {
                    if ((spotifyknapnr+spotify_selected_startofset)<spotify_oversigt.streamantal()+1) {
                      if (spotifyknapnr+1>40) {
                        spotify_selected_startofset+=8;
                        spotifyknapnr-=(spotify_selected_startofset-1);
                      } else {
                        spotifyknapnr++;
                        spotify_key_selected++;
                        spotify_select_iconnr++;
                      }
                    }
                  }
                  // spotify online search
                  // key right
                  if (do_show_spotify_search_oversigt) {
                    if ((spotifyknapnr+spotify_selected_startofset)<spotify_oversigt.streamantal()+1) {
                      if (spotifyknapnr+1>32) {
                        spotify_selected_startofset+=8;
                        spotifyknapnr-=(spotify_selected_startofset-1);
                      } else {
                        spotifyknapnr++;
                        spotify_key_selected++;
                        spotify_select_iconnr++;
                      }
                    }
                  }
                }
                #endif
                //film_select_iconnr+film_key_selected
                // key right
                if ((vis_film_oversigt) && ((int unsigned) (film_select_iconnr)<film_oversigt.film_antal()-1)) {
                  if ((film_key_selected % (mnumbersoficonline*3)==0) || ((film_select_iconnr==14) && (film_key_selected % mnumbersoficonline==0))) {
                    _fangley+=MOVIE_CS;
                    film_key_selected-=mnumbersoficonline;	// den viste på skærm af 1 til 20
                    film_select_iconnr++;			// den rigtige valgte af 1 til film antal
                  } else {
                    film_select_iconnr++;			// den rigtige valgte af 1 til film antal
                  }
                  film_key_selected++;
                }
                if (vis_recorded_oversigt) {
                  if (visvalgtnrtype==1) visvalgtnrtype=2;
                  else if (visvalgtnrtype==2) visvalgtnrtype=1;
                }
                // radio
                // key right
                if ((vis_radio_oversigt)  && (radio_select_iconnr<radiooversigt_antal)) {
                  if ((radio_key_selected % (rnumbersoficonline*3)==0) || ((radio_select_iconnr==19) && (radio_key_selected % rnumbersoficonline==0))) {
                    _rangley+=RADIO_CS;
                    radio_key_selected-=rnumbersoficonline;	// den viste på skærm af 1 til 20
                    radio_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                  } else {
                    radio_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                  }
                  radio_key_selected++;
                }
                // Podcast
                // key right
                if ((vis_stream_oversigt)  && (stream_select_iconnr<streamoversigt.streamantal()-1)) {
                  // er vi på første skærm ingen scroll
                  // så scroll
                  if ((stream_key_selected % (snumbersoficonline*6)==0) || ((stream_select_iconnr==19) && (stream_key_selected % snumbersoficonline==0))) {
                    // ikke max
                    if ((stream_select_iconnr+1)<streamoversigt.streamantal()) {
                      _sangley+=RADIO_CS;
                      stream_key_selected-=snumbersoficonline;	// den viste på skærm af 1 til 20
                      stream_select_iconnr++;			// den rigtige valgte af 1 til stream antal
                    }
                  } else {
                    if (stream_select_iconnr<streamoversigt.streamantal()) stream_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                  }
                  if ((stream_select_iconnr)<streamoversigt.streamantal()) stream_key_selected++;
                }
                // If indside tv overview
                // key right
                if (vis_tv_oversigt) {
                  // normal tv overview select next channel
                  if (do_show_tvgraber==false) {
                    if (tvvisvalgtnrtype==1) {
                      if (tvvalgtrecordnr<aktiv_tv_oversigt.tv_kanal_antal()-1) tvvalgtrecordnr++;
                      tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr-1,tvsubvalgtrecordnr));
                    }
                  } else {
                    // Show tv graber config over normal tv overview
                    // first reset all other
                    for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                    channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=true;
                    // set channel order nr
                    channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr=do_show_setup_select_linie+tvchannel_startofset;
                  }
                }
                // if indside a setup menu
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    // first reset all other
                    for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                    channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=true;
                    // set channel order nr
                    channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr=do_show_setup_select_linie+tvchannel_startofset;
                  }
                }
                break;
        case 103:  // key down
                // bruges af ask_open_dir_or_play
                if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                  if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                    if (do_show_play_open_select_line<15) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                  }
                }
                // hvis ikke ask_open_dir_or_play
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr+mnumbersoficonline<=musicoversigt_antal)) {
                  if ((unsigned int) music_key_selected>=((mnumbersoficonline*3)+1)) {
                    if (music_key_selected<(music_select_iconnr+mnumbersoficonline)) {
                      _mangley+=MUSIC_CS;								//scroll gfx down
                      music_select_iconnr+=mnumbersoficonline;
                      do_music_icon_anim_icon_ofset=1;                       // set scroll
                    }
                  } else {
                    music_key_selected+=mnumbersoficonline;
                    music_select_iconnr+=mnumbersoficonline;
                  }
                }
                // spotify stuf
                // old
                #ifdef ENABLE_SPOTIFY
                if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify))) {
                  // select device to play on
                  if (do_select_device_to_play) {
                    // select play device
                    if ((spotify_oversigt.active_spotify_device!=-1) && (spotify_oversigt.active_spotify_device<9)) {
                      if (strcmp(spotify_oversigt.get_device_name(spotify_oversigt.active_spotify_device+1),"")!=0) {
                        if (spotify_oversigt.active_spotify_device<9) spotify_oversigt.active_spotify_device+=1;
                      }
                    }
                  } else {
                    // move coursor
                    if ((spotifyknapnr+spotify_selected_startofset+snumbersoficonline)<spotify_oversigt.antal_spotify_streams()+1) {
                      if ((((spotifyknapnr+snumbersoficonline)>40) && (do_show_spotify_search_oversigt==false)) || (((spotifyknapnr+snumbersoficonline)>32) && (do_show_spotify_search_oversigt==true))) {
                        if ((spotifyknapnr+snumbersoficonline)<spotify_oversigt.antal_spotify_streams()) {
                          spotify_selected_startofset+=8;
                        } else {
                          if ((spotifyknapnr-1)<spotify_oversigt.antal_spotify_streams()) {
                            spotifyknapnr++;
                            spotify_key_selected+=1;
                            spotify_select_iconnr+=1;
                          }
                        }
                      } else {
                        spotifyknapnr+=snumbersoficonline;
                        spotify_key_selected+=snumbersoficonline;
                        spotify_select_iconnr+=snumbersoficonline;
                      }
                    }
                  }
                }
                #endif
                // movie
                // key down
                if ((vis_film_oversigt) && ((int) (film_select_iconnr+fnumbersoficonline)<(int) film_oversigt.film_antal()-1)) {
                  if (film_key_selected>=11) {
                    _fangley+=MOVIE_CS;
                    film_select_iconnr+=fnumbersoficonline;
                  } else {
                    film_key_selected+=fnumbersoficonline;
                    film_select_iconnr+=fnumbersoficonline;
                  }
                }
                // radio
                // key down
                if ((vis_radio_oversigt) && (show_radio_options==false) && ((radio_select_iconnr+rnumbersoficonline)<radiooversigt.radioantal()-1)) {
                  if (radio_key_selected>=20) {
                    _rangley+=RADIO_CS;
                    radio_select_iconnr+=rnumbersoficonline;
                  } else {
                    radio_key_selected+=rnumbersoficonline;
                    radio_select_iconnr+=rnumbersoficonline;
                  }
                }
                // show radio options
                // key down
                if ((vis_radio_oversigt) && (show_radio_options)) radiooversigt.nextradiooptselect();
                // stream
                // stream_select_iconnr = the real nr in the array
                // stream_key_selected = the number on the screen
                if ((vis_stream_oversigt) && (show_stream_options==false) && (stream_select_iconnr+snumbersoficonline<streamoversigt.streamantal())) {
                  if (stream_key_selected>=(snumbersoficonline*5)) {
                    if ((stream_key_selected+((_sangley/RADIO_CS)*snumbersoficonline))<streamoversigt.streamantal()) _sangley+=RADIO_CS;
                  } else {
                    if ((stream_select_iconnr+snumbersoficonline)<streamoversigt.streamantal()) stream_key_selected+=snumbersoficonline;
                  }
                  if (stream_select_iconnr>=0) {
                    if ((stream_select_iconnr+snumbersoficonline)<streamoversigt.streamantal()) stream_select_iconnr+=snumbersoficonline;
                  }
                }
                // recorded tv
                // key down
                if (vis_recorded_oversigt) {
                  if (visvalgtnrtype==1) {
                    if ((int) valgtrecordnr<(int) recordoversigt.top_antal()) {
                      valgtrecordnr++;
                      subvalgtrecordnr=0;
                    }
                  } else if (visvalgtnrtype==2) {
                    if ((int) subvalgtrecordnr<(int) recordoversigt.programs_type_antal(valgtrecordnr)-1) {
                      subvalgtrecordnr++;
                    }
                  }
                  reset_recorded_texture = true;                                //
                }
                // tv overview
                // if indside tv overoview
                // key down
                if (vis_tv_oversigt) {
                  if (!(do_show_tvgraber)) {
                    if (debugmode) fprintf(stderr,"prg antal in tv kanal %d \n ",aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr));
                    if (tvsubvalgtrecordnr+1<aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr)) {
                      tvsubvalgtrecordnr++;
                    }
                    // check hvor vi er
                    if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)>hourtounixtime(aktiv_tv_oversigt.vistvguidekl+3)) {
                      if (aktiv_tv_oversigt.vistvguidekl<24*2) aktiv_tv_oversigt.vistvguidekl++;
                    }
                  }
                  if (do_show_tvgraber) {
                    if ((do_show_setup_select_linie+tvchannel_startofset)>0) {
                      // if we wants to change order by the changeorderflag
                      if (channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr) {
                        //swap channels (key down)
                        channel_list_struct tempch;
                        tempch.selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
                        strcpy(tempch.id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id);
                        strcpy(tempch.name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name);
                        tempch.ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr;
                        tempch.changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected=tempch.selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id,tempch.id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name,tempch.name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr=tempch.ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr=tempch.changeordernr;
                      }
                    }
                    if ((do_show_setup_select_linie==13) && ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL)) tvchannel_startofset++; else if ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL) do_show_setup_select_linie++;
                  }
                }
                // if indside a setup menu
                // key down
                if (do_show_setup) {
                  // mythtv sql setup window
                  if (do_show_setup_sql) {
                     if (do_show_setup_select_linie<7) do_show_setup_select_linie++;
                  }
                  // sound setup window
                  if (do_show_setup_sound) {
                     if (do_show_setup_select_linie<2) do_show_setup_select_linie++;
                  }
                  // setup screen window (screem config)
                  if (do_show_setup_screen) {
                     if (do_show_setup_select_linie<5) do_show_setup_select_linie++;
                  }
                  // setup network window
                  if (do_show_setup_network) {
                     // if show normal network setup window
                     if (!(show_wlan_select)) {
                       if (do_show_setup_select_linie<4) do_show_setup_select_linie++;
                     } else {
                       // if show wlan window
                       if ((int) setupwlanselectofset+1<(int) wifinets.antal()) setupwlanselectofset++;
                     }
                  }
                  if (do_show_setup_font) {
                    if (setupfontselectofset<aktivfont.mastercount-1) setupfontselectofset++;
                  }
                  // setup keys window
                  if (do_show_setup_keys) {
                    if (do_show_setup_select_linie<17) do_show_setup_select_linie++;
                  }
                  // setup videoplayer window
                  if (do_show_videoplayer) {
                    if (do_show_setup_select_linie<4) do_show_setup_select_linie++;
                  }
                  // setup rss source window
                  if (do_show_setup_rss) {
                    if (do_show_setup_select_linie<35) do_show_setup_select_linie++;
                    else {
                      if (configrss_ofset<streamoversigt.antalstreams()) configrss_ofset++;
                    }
                    if (((do_show_setup_select_linie+configrss_ofset) % 2)==0) {
                      if ((realrssrecordnr)<43) realrssrecordnr++;
                    }
                  }
                  // setup rss source window
                  if (do_show_setup_spotify) {
                    if (do_show_setup_select_linie<1) do_show_setup_select_linie++;
                  }
                  // tv graber setup
                  if (do_show_tvgraber) {
                    if ((do_show_setup_select_linie+tvchannel_startofset)>0) {
                      // if we wants to change order by the changeorderflag
                      if (channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr) {
                        //swap channels (key down)
                        channel_list_struct tempch;
                        tempch.selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
                        strcpy(tempch.id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id);
                        strcpy(tempch.name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name);
                        tempch.ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr;
                        tempch.changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected=tempch.selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id,tempch.id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name,tempch.name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr=tempch.ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr=tempch.changeordernr;
                      }
                    }
                    if ((do_show_setup_select_linie==13) && ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL)) tvchannel_startofset++; else if ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL) do_show_setup_select_linie++;
                  }
                  keybuffer[0] = 0;
                  keybufferindex = 0;
                }
/*
                if (vis_tv_oversigt) {
                  if (do_show_tvgraber) {
                    if ((do_show_setup_select_linie+tvchannel_startofset)>0) {
                      // if we wants to change order by the changeorderflag
                      if (channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr) {
                        //swap channels (key down)
                        channel_list_struct tempch;
                        tempch.selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
                        strcpy(tempch.id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id);
                        strcpy(tempch.name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name);
                        tempch.ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr;
                        tempch.changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].id,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].name,channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].ordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].selected=tempch.selected;
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].id,tempch.id);
                        strcpy(channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].name,tempch.name);
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].ordernr=tempch.ordernr;
                        channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset+1].changeordernr=tempch.changeordernr;
                      }
                    }
                    if ((do_show_setup_select_linie==13) && ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL)) tvchannel_startofset++; else if ((tvchannel_startofset+do_show_setup_select_linie)<PRGLIST_ANTAL) do_show_setup_select_linie++;
                  }
                }
*/
                break;
        case 101: // up key
                // bruges af ask_open_dir_or_play
                if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                  if (do_show_play_open_select_line>0) do_show_play_open_select_line--; else
                    if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
                }
                // music stuf
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr>(mnumbersoficonline-1)) ) {
                    if ((_mangley>0) && ((unsigned int) music_key_selected<=mnumbersoficonline) && (music_select_iconnr>(mnumbersoficonline-1))) {
                      _mangley-=MUSIC_CS;
                      do_music_icon_anim_icon_ofset = -1;			// set scroll
                      music_select_iconnr -= mnumbersoficonline;
                    } else if ((music_select_iconnr-mnumbersoficonline)>0) {
                      music_select_iconnr -= mnumbersoficonline;
                    }
                    if (music_key_selected>(int ) mnumbersoficonline) music_key_selected-=mnumbersoficonline;
                }

                // spotify stuf
                if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify))) {
                  // select play device
                  if (do_select_device_to_play) {
                    #ifdef ENABLE_SPOTIFY
                    if (spotify_oversigt.active_spotify_device>0) spotify_oversigt.active_spotify_device-=1;
                    #endif
                  } else {
                    // move coursor
                    #ifdef ENABLE_SPOTIFY
                    if (((spotifyknapnr+spotify_selected_startofset)-snumbersoficonline)>=1) {
                      if (spotify_selected_startofset>0) {
                        if ((spotifyknapnr>=1) && (spotifyknapnr<=9)) {
                          spotify_selected_startofset-=8;
                        } else {
                          if (spotifyknapnr>1) {
                            spotifyknapnr-=snumbersoficonline;
                            spotify_key_selected-=snumbersoficonline;
                            spotify_select_iconnr-=snumbersoficonline;
                          }
                        }
                      } else {
                        if (spotifyknapnr>1) {
                          spotifyknapnr-=snumbersoficonline;
                          spotify_key_selected-=snumbersoficonline;
                          spotify_select_iconnr-=snumbersoficonline;
                        }
                      }
                    }
                    #endif
                  }
                }
                // movie stuf
                if (vis_film_oversigt) {
                  if ((vis_film_oversigt) && (film_select_iconnr>(fnumbersoficonline-1))) {
                    if ((film_key_selected<=fnumbersoficonline) && (film_select_iconnr>(fnumbersoficonline-1))) {
                      _fangley -= MOVIE_CS;
                      film_select_iconnr-=fnumbersoficonline;
                    } else film_select_iconnr-=fnumbersoficonline;
                    if (film_key_selected>fnumbersoficonline) film_key_selected-=fnumbersoficonline;
                  }
                }
                // radio
                if ((vis_radio_oversigt) && (show_radio_options==false)) {
                  if ((vis_radio_oversigt) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                    if ((_rangley>0) && (radio_key_selected<=fnumbersoficonline) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                      _rangley-=MOVIE_CS;
                      radio_select_iconnr-=rnumbersoficonline;
                    } else radio_select_iconnr-=rnumbersoficonline;
                    if (radio_key_selected>rnumbersoficonline) radio_key_selected-=rnumbersoficonline;
                  }
                }
                if ((vis_radio_oversigt) && (show_radio_options)) radiooversigt.lastradiooptselect();
                // stream stuf (podcast)
                if ((vis_stream_oversigt) && (show_stream_options==false)) {
                  //if ((vis_stream_oversigt) && (stream_select_iconnr>(snumbersoficonline-1))) {
                  if ((vis_stream_oversigt) && (stream_select_iconnr>(snumbersoficonline-1))) {
                    if ((_sangley>0) && (stream_key_selected<=snumbersoficonline) && (stream_select_iconnr>(snumbersoficonline-1))) {
                      _sangley-=MOVIE_CS;
                      stream_select_iconnr-=snumbersoficonline;
                    } else stream_select_iconnr-=snumbersoficonline;
                  } else {
                    if (stream_key_selected>snumbersoficonline) stream_key_selected-=snumbersoficonline;
                    else if (_sangley>0) _sangley-=MOVIE_CS;
                    if (snumbersoficonline<0) snumbersoficonline=0;
                    if (_sangley<0) _sangley=0;
                  }
                }
                //
                if (vis_recorded_oversigt) {
                  if ((visvalgtnrtype==1) && (valgtrecordnr>0)) {
                    valgtrecordnr--;
                    subvalgtrecordnr=0;
                  } else if ((visvalgtnrtype==2) && (subvalgtrecordnr>0)) {
                    subvalgtrecordnr--;	// bruges til visning af optaget programmer
                  }
                  reset_recorded_texture=true;		// load optaget programs texture gen by mythtv
                }
                // tv stuf up key
                // if indside tv overoview
                if (vis_tv_oversigt) {
                  // Normal tv overview
                  if (do_show_tvgraber==false) {
                    if (tvsubvalgtrecordnr>0) {
                      tvsubvalgtrecordnr--;
                      if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)<hourtounixtime(aktiv_tv_oversigt.vistvguidekl)) {
                        if (aktiv_tv_oversigt.vistvguidekl>0) aktiv_tv_oversigt.vistvguidekl--;
                      }
                    }
                    // Show tv graber setup view over normal tv overview
                  } else if (do_show_tvgraber==true) {
                    if (do_show_setup_select_linie>0) {
                      // controller scoll fuction in select program channel list
                      if ((tvchannel_startofset>0) && (do_show_setup_select_linie>13)) {
                        tvchannel_startofset--;
                      } else {
                        if ((tvchannel_startofset>0) && (do_show_setup_select_linie>1)) do_show_setup_select_linie--;
                        else if ((tvchannel_startofset>0) && (do_show_setup_select_linie==1)) tvchannel_startofset--;
                        else if ((tvchannel_startofset==0) && (do_show_setup_select_linie>0)) do_show_setup_select_linie--;
                      }
                    }
                    if (do_show_setup_select_linie<0) do_show_setup_select_linie=0;
                    if (tvchannel_startofset<0) tvchannel_startofset=0;
                    // move to other order
                    if (channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr) {
                      if (do_show_setup_select_linie+tvchannel_startofset>0) {
                        //swap channels
                        channel_list_struct tempch;
                        tempch.selected=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].selected;
                        strcpy(tempch.id,channel_list[(do_show_setup_select_linie)+tvchannel_startofset].id);
                        strcpy(tempch.name,channel_list[(do_show_setup_select_linie)+tvchannel_startofset].name);
                        tempch.ordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].ordernr;
                        tempch.changeordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr;
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset].selected=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].selected;
                        strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset].id,channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].id);
                        strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset].name,channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].name);
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset].ordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].ordernr;
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].changeordernr;
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].selected=tempch.selected;
                        strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].id,tempch.id);
                        strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].name,tempch.name);
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].ordernr=tempch.ordernr;
                        channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].changeordernr=tempch.changeordernr;
                      }
                    }
                  }
                }
                // if indside a setup menu
                if (do_show_setup) {
                  // sql setup
                  if (do_show_setup_sql) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                  // sound setup
                  if (do_show_setup_sound) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                  // screen setup
                  if (do_show_setup_screen) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                  // network setup
                  if (do_show_setup_network) {
                    // hvis vi ikke viser select wlan window hop med coursor i networksetup
                    if (!(show_wlan_select)) {
                      if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    } else {
                      if (setupwlanselectofset>0) setupwlanselectofset--;
                    }
                  }
                  // setup fonts
                  if (do_show_setup_font) {
                    if (setupfontselectofset>0) setupfontselectofset--;
                  }
                  // setup keys
                  if (do_show_setup_keys) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                  // setup video player
                  if (do_show_videoplayer) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                  // setup rss
                  if (do_show_setup_rss) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    else if (configrss_ofset>0) configrss_ofset--;
                    if ((((do_show_setup_select_linie+configrss_ofset) % 2)==0) && ((do_show_setup_select_linie+configrss_ofset)>0)) realrssrecordnr--;
                  }
                  if (do_show_setup_spotify) {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }

                  // config af xmltv graber
                  if (do_show_tvgraber) {
                    if (do_show_setup_select_linie>0) {
                      // controller scoll fuction in select program channel list
                      if ((tvchannel_startofset>0) && (do_show_setup_select_linie>13)) {
                        tvchannel_startofset--;
                      } else {
                        if ((tvchannel_startofset>0) && (do_show_setup_select_linie>1)) do_show_setup_select_linie--;
                        else if ((tvchannel_startofset>0) && (do_show_setup_select_linie==1)) tvchannel_startofset--;
                        else if ((tvchannel_startofset==0) && (do_show_setup_select_linie>0)) do_show_setup_select_linie--;
                      }
                    }
                    if (do_show_setup_select_linie<0) do_show_setup_select_linie=0;
                    if (tvchannel_startofset<0) tvchannel_startofset=0;
                  }
                  // move to other order
                  if (channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr) {
                    if (do_show_setup_select_linie+tvchannel_startofset>0) {
                      //swap channels
                      channel_list_struct tempch;
                      tempch.selected=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].selected;
                      strcpy(tempch.id,channel_list[(do_show_setup_select_linie)+tvchannel_startofset].id);
                      strcpy(tempch.name,channel_list[(do_show_setup_select_linie)+tvchannel_startofset].name);
                      tempch.ordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].ordernr;
                      tempch.changeordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr;
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset].selected=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].selected;
                      strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset].id,channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].id);
                      strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset].name,channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].name);
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset].ordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].ordernr;
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr=channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].changeordernr;
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].selected=tempch.selected;
                      strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].id,tempch.id);
                      strcpy(channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].name,tempch.name);
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].ordernr=tempch.ordernr;
                      channel_list[(do_show_setup_select_linie)+tvchannel_startofset-1].changeordernr=tempch.changeordernr;
                    }
                  }
                  keybuffer[0]=0;
                  keybufferindex=0;
                }
/*
                if (vis_tv_oversigt) {
                  // config af xmltv graber
                  if (do_show_tvgraber) {
                    if (do_show_setup_select_linie>0) {
                      // controller scoll fuction in select program channel list
                      if ((tvchannel_startofset>0) && (do_show_setup_select_linie>13)) {
                        tvchannel_startofset--;
                      } else {
                        if ((tvchannel_startofset>0) && (do_show_setup_select_linie>1)) do_show_setup_select_linie--;
                        else if ((tvchannel_startofset>0) && (do_show_setup_select_linie==1)) tvchannel_startofset--;
                        else if ((tvchannel_startofset==0) && (do_show_setup_select_linie>0)) do_show_setup_select_linie--;
                      }
                    }
                    if (do_show_setup_select_linie<0) do_show_setup_select_linie=0;
                    if (tvchannel_startofset<0) tvchannel_startofset=0;
                  }
                }
*/
                break;
        case GLUT_KEY_PAGE_UP:
                if ((vis_music_oversigt) && (music_select_iconnr>numbers_cd_covers_on_line)) {
                  do_music_icon_anim_icon_ofset=-1;			              // direction -1 = up 1 = down
                  _mangley-=(41.0f);			                       			// scroll window up
                  music_select_iconnr-=numbers_cd_covers_on_line;			// add to next line
                }
                #ifdef ENABLE_SPOTIFY
                if (vis_spotify_oversigt) {
                  spotify_oversigt.search_playlist_song=!spotify_oversigt.search_playlist_song;
                }
                #endif
                // if indside tv overoview
                if (vis_tv_oversigt) {
                  aktiv_tv_oversigt.changetime(60);
                  if (aktiv_tv_oversigt.vistvguidekl<24) {
                    // hent ur
                    if (aktiv_tv_oversigt.vistvguidekl==0) {
                      time_t rawtime;
                      struct tm *timelist;
                      time(&rawtime);
                      timelist=localtime(&rawtime);
                      aktiv_tv_oversigt.vistvguidekl=timelist->tm_hour;
                    }
                    aktiv_tv_oversigt.vistvguidekl++;
                  }
                  //aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                  ask_tv_record=false;
                  tvknapnr=0;
                  do_zoom_tvprg_aktiv_nr=0;					                          // slet valget
                }
                // podcast view
                if (vis_stream_oversigt) {
                  if (streamoversigt.streamantal()>0) {
                    stream_key_selected-=4*snumbersoficonline;
                    //stream_key_selected+=streamoversigt.streamantal() % 8;       //
                    if (stream_key_selected<0) stream_key_selected=0;
                    //stream_key_selected=5*snumbersoficonline;
                    stream_select_iconnr=0;
                    _sangley-=4*RADIO_CS;
                    if (_sangley<0) _sangley=0;
                  }
                }
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    if (tvchannel_startofset>1) tvchannel_startofset-=12;
                    else if ((do_show_setup_select_linie)>0) do_show_setup_select_linie-=12;
                    if (do_show_setup_select_linie<3) do_show_setup_select_linie=3;
                    if (tvchannel_startofset<0) tvchannel_startofset=0;
                  }
                }
                break;
        case GLUT_KEY_PAGE_DOWN:
                if (vis_music_oversigt) {
                  do_music_icon_anim_icon_ofset=1;			             	// direction -1 = up 1 = down
                  _mangley+=(41.0f);				                      		// scroll window down one icon
                  music_select_iconnr+=numbers_cd_covers_on_line;			// add to next line
                }
                #ifdef ENABLE_SPOTIFY
                if (vis_spotify_oversigt) {
                  spotify_oversigt.search_playlist_song=!spotify_oversigt.search_playlist_song;
                }
                #endif
                if (vis_stream_oversigt) {
                  if (streamoversigt.streamantal()>0) {
                    stream_key_selected=4*snumbersoficonline;
                    //stream_key_selected+=streamoversigt.streamantal() % 8;       //
                    //stream_key_selected=5*snumbersoficonline;
                    stream_select_iconnr=0;
                    _sangley=4*RADIO_CS;
                  }
                }
                // if indside tv overoview
                if ((vis_tv_oversigt) && ((aktiv_tv_oversigt.vistvguidekl>1) || (aktiv_tv_oversigt.vistvguidekl==0))) {
                    aktiv_tv_oversigt.changetime(-(60));
                    if (aktiv_tv_oversigt.vistvguidekl==0) {
                      time_t rawtime;
                      struct tm *timelist;
                      time(&rawtime);
                      timelist=localtime(&rawtime);
                      aktiv_tv_oversigt.vistvguidekl=timelist->tm_hour;
                    }
                    if (aktiv_tv_oversigt.vistvguidekl>0) aktiv_tv_oversigt.vistvguidekl--;
                    //aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                    ask_tv_record=false;
                    tvknapnr=0;
                    do_zoom_tvprg_aktiv_nr=0;			                          		// slet valget
                }
                // if indside a setup menu
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    if (tvchannel_startofset>0) tvchannel_startofset+=12;
                    else if ((do_show_setup_select_linie)>0) do_show_setup_select_linie+=12;
                  }
                }
                break;
        case GLUT_KEY_HOME:
                // reset music oversigt
                if (vis_music_oversigt) {
                  _mangley = 0;     	                      							  // scroll start ofset reset to start
                  music_select_iconnr = 1;                                  // first icon in view left top conner
                  do_music_icon_anim_icon_ofset = 1;                        // set scroll
                  music_key_selected = 1;
                }
                if (vis_stream_oversigt) {
                  _sangley=0;     	                      							  // scroll start ofset reset to start
                  stream_key_selected=1;
                }
                // if indside tv overview reset show time to now (localtime)
                if (vis_tv_oversigt) {
                    // reset tvgide time to now
                    aktiv_tv_oversigt.reset_tvguide_time();
                    if (ask_tv_record) {
                      ask_tv_record = false;
                      tvknapnr = 0;
                      do_zoom_tvprg_aktiv_nr = 0;			                          		// slet valget
                    }
                }
                // radio
                if ((vis_radio_oversigt) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                  if ((_rangley>0) && (radio_key_selected<=fnumbersoficonline) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                    _rangley-=0;
                    radio_select_iconnr = 1;
                  }
                  _rangley = MOVIE_CS;
                  radio_key_selected = 1;
                  radio_select_iconnr = 1;
                }
                // if indside a setup menu
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    // select line 0
                    tvchannel_startofset = 0;
                    do_show_setup_select_linie = 0;
                  }
                  // rss setup
                  if (show_setup_rss) {
                    do_show_setup_select_linie = 0;
                    configrss_ofset = 0;
                  }
                }
                break;
        case GLUT_KEY_END:
                // music oversigt
                if (vis_music_oversigt) {
                  _mangley = 0;     	                      							  // scroll start ofset reset to start
                  music_select_iconnr = 1;                                  // first icon in view left top conner
                  do_music_icon_anim_icon_ofset = 1;                        // set scroll
                  music_key_selected = 1;
                }
                // jump to end on list (podcast)
                if (vis_stream_oversigt) {
                  stream_key_selected=((streamoversigt.streamantal()/snumbersoficonline)/2)*snumbersoficonline;
                  stream_key_selected+=streamoversigt.streamantal() % 8;       //
                  //stream_key_selected=5*snumbersoficonline;
                  stream_select_iconnr=0;
                  _sangley=((streamoversigt.streamantal()/snumbersoficonline)/2)*RADIO_CS;
                }
                // if indside tv overoview
                if (vis_tv_oversigt) {
                  aktiv_tv_oversigt.changetime((60*60*24));
                  aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                }
                // if indside a setup menu
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    // select lasy line
                  }
                  if (show_setup_rss) {
                    // jump to button of text
                    if (streamoversigt.antalstreams()>17) do_show_setup_select_linie=34; else do_show_setup_select_linie=0;
                    configrss_ofset=0;
                    for(int i=configrss_ofset;i<3000-1;i++) {
                      if (streamoversigt.get_stream_name(configrss_ofset)) {
                        configrss_ofset++;
                      }
                    }
                    if (configrss_ofset>8) configrss_ofset-=8;
                  }
                }
                break;
        case 114:
        case 115:
                printf("CTRL key pressed \n");
                break;
    }
    if (debugmode) {
      if (vis_radio_oversigt) fprintf(stderr,"Radio_key_selected = %d  radio_select_iconnr = %d \n ",radio_key_selected,radio_select_iconnr);
      if (vis_music_oversigt) fprintf(stderr,"Music_key_selected = %d  music_select_iconnr = %d musicoversigt_antal= %d \n ",music_key_selected,music_select_iconnr,musicoversigt_antal);
      if (vis_film_oversigt) fprintf(stderr,"film_key_selected = %d  film_select_iconnr = %d filmoversigt_antal=%d \n ",film_key_selected,film_select_iconnr,film_oversigt.film_antal());
      if (do_show_tvgraber) fprintf(stderr,"line %2d of %2d ofset = %d \n",do_show_setup_select_linie,PRGLIST_ANTAL,tvchannel_startofset);
      if (vis_tv_oversigt) fprintf(stderr,"tvvalgtrecordnr %2d tvsubvalgtrecordnr %2d antal kanler %2d kl %2d \n",tvvalgtrecordnr,tvsubvalgtrecordnr,aktiv_tv_oversigt.tv_kanal_antal(),aktiv_tv_oversigt.vistvguidekl);
      if (show_setup_rss) fprintf(stderr,"Antal %d realrssrecordnr %d \n ",streamoversigt.antalstreams(),realrssrecordnr);
      #ifdef ENABLE_SPOTIFY
      if (vis_spotify_oversigt) fprintf(stderr,"Spotify_key_selected = %d  spotify_select_iconnr = %d spotifyoversigt_antal= \n ",spotify_key_selected,spotify_select_iconnr);
      #endif
      if (vis_stream_oversigt) fprintf(stderr,"antal %d selected stream_select_iconnr %d stream_key_selected %d \n",streamoversigt.streamantal(),stream_select_iconnr ,stream_key_selected);
    }
}



// ****************************************************************************************
//
// Keyboard handler
//
// ****************************************************************************************

void handleKeypress(unsigned char key, int x, int y) {
    const char optionmenukey='O';
    char id[80];		// bruges af wlan setup
    char tmptxt[1024];
    char temptxt[200];
    saver_irq=true;                                     // stop screen saver
    char path[1024];
    stream_loadergfx_started_break=true;		// break tread stream gfx loader
    if (key=='+') {
      #ifdef ENABLE_SPOTIFY
      if (!(vis_spotify_oversigt)) {
        if ((configsoundvolume+0.05)<1.0f) configsoundvolume+=0.05f;
        #if defined USE_FMOD_MIXER
        if (sndsystem) channel->setVolume(configsoundvolume);
        #endif
        save_config((char *) "/etc/mythtv-controller.conf");
        show_volume_info=true;					// show volume info window
        vis_volume_timeout=80;
      }
      #endif
    }
    if (key=='-') {                               // volume down
      if (!(vis_spotify_oversigt)) {
        if ((configsoundvolume-0.05)>0) configsoundvolume-=0.05f;
        #if defined USE_FMOD_MIXER
        if (sndsystem) channel->setVolume(configsoundvolume);
        #endif
        save_config((char *) "/etc/mythtv-controller.conf");
        show_volume_info=true;					// show volume info window
        vis_volume_timeout=80;
      }
    }
    if (((key!=27) && (key!='*') && (key!=13) && (key!='+') && (key!='-') && (key!='S') && ((key!='U') && (vis_music_oversigt)) && ((vis_music_oversigt) || ((vis_radio_oversigt) && (key!=optionmenukey)) || (do_show_setup))) || (((do_show_tvgraber) || (do_show_setup_rss) || (do_show_setup) || ((vis_film_oversigt) && (key!=13))) && (key!=27)) || ((vis_spotify_oversigt) && (key!=13) && (key!='*')) && (key!=27)) {
      // rss setup windows is open
      if (do_show_setup_rss) {
        switch(do_show_setup_select_linie) {
          case 0: if (rssstreamoversigt.get_stream_name(0+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(0+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 1: if (rssstreamoversigt.get_stream_url(0+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(0+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 2: if (rssstreamoversigt.get_stream_name(1+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(1+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 3: if (rssstreamoversigt.get_stream_url(1+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(1+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 4: if (rssstreamoversigt.get_stream_name(2+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(2+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 5: if (rssstreamoversigt.get_stream_url(2+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(2+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 6: if (rssstreamoversigt.get_stream_name(3+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(3+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 7: if (rssstreamoversigt.get_stream_url(3+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(3+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 8: if (rssstreamoversigt.get_stream_name(4+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(4+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 9: if (rssstreamoversigt.get_stream_url(4+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(4+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 10:if (rssstreamoversigt.get_stream_name(5+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(5+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 11:if (rssstreamoversigt.get_stream_url(5+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(5+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 12:if (rssstreamoversigt.get_stream_name(6+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(6+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 13:if (rssstreamoversigt.get_stream_url(6+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(6+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 14:if (rssstreamoversigt.get_stream_name(7+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(7+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 15:if (rssstreamoversigt.get_stream_url(7+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(7+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 16:if (rssstreamoversigt.get_stream_name(8+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(8+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 17:if (rssstreamoversigt.get_stream_url(8+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(8+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 18:if (rssstreamoversigt.get_stream_name(9+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(9+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 19:if (rssstreamoversigt.get_stream_url(9+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(9+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 20:if (rssstreamoversigt.get_stream_name(10+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(10+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 21:if (rssstreamoversigt.get_stream_url(10+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(10+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 22:if (rssstreamoversigt.get_stream_name(11+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(11+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 23:if (rssstreamoversigt.get_stream_url(11+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(11+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 24:if (rssstreamoversigt.get_stream_name(12+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(12+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 25:if (rssstreamoversigt.get_stream_url(12+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(12+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 26:if (rssstreamoversigt.get_stream_name(13+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(13+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 27:if (rssstreamoversigt.get_stream_url(13+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(13+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 28:if (rssstreamoversigt.get_stream_name(14+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(14+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 29:if (rssstreamoversigt.get_stream_url(14+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(14+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 30:if (rssstreamoversigt.get_stream_name(15+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(15+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 31:if (rssstreamoversigt.get_stream_url(15+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(15+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 32:if (rssstreamoversigt.get_stream_name(16+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(16+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 33:if (rssstreamoversigt.get_stream_url(16+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(16+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 34:if (rssstreamoversigt.get_stream_name(17+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(17+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 35:if (rssstreamoversigt.get_stream_url(17+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(17+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 36:if (rssstreamoversigt.get_stream_name(18+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_name(18+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
          case 37:if (rssstreamoversigt.get_stream_url(18+configrss_ofset)) strcpy(keybuffer,rssstreamoversigt.get_stream_url(18+configrss_ofset)); else strcpy(keybuffer,"");
                  keybufferindex=strlen(keybuffer);
                  break;
        }
      }
      #ifdef ENABLE_SPOTIFY
      if (do_show_setup_spotify) {
        switch (do_show_setup_select_linie) {
          case 0: strcpy(keybuffer,spotify_oversigt.spotify_client_id);
                  keybufferindex=strlen(keybuffer);
                  break;
          case 1: strcpy(keybuffer,spotify_oversigt.spotify_secret_id);
                  keybufferindex=strlen(keybuffer);
                  break;
        }
      }
      #endif
      if (do_show_setup_screen) {
        switch (do_show_setup_select_linie) {
          case 0: break;
          case 1: break;
          case 2: strcpy(keybuffer,configscreensavertimeout);
                  break;
          default:
                  break;
        }
      }
      // gem key pressed in buffer
      if (keybufferindex<80) {
        if (key==8) {						// back space
          if (keybufferindex>0) {
            keybufferindex--;
            keybuffer[keybufferindex]=0;
          }
        } else {
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
              if (key==32) {
                dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
              }
          }
          //hvis vi ikke gør andre ting
          if ((ask_save_playlist==false) || (save_ask_save_playlist==false)) {
            // søg sang/artist navn
            if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';       // else input key text in buffer
              }
            }
          }
          // show/select device to play on
          #ifdef ENABLE_SPOTIFY
          if ((vis_spotify_oversigt) && (keybufferindex==0)) {
            if (key=='D') {
              do_select_device_to_play=true;
            }
          }
          if ((firsttimespotifyupdate==false) && (strcmp(spotify_oversigt.spotify_get_token(),"")!=0)) {
            // søg efter spotify not online fill buffer from keyboard
            if ((vis_spotify_oversigt) && (!(do_show_spotify_search_oversigt))) {
              if ((do_select_device_to_play==false) && (do_zoom_spotify_cover==false)) {
                //do_zoom_spotify_cover=!do_zoom_spotify_cover;                                             // close/open window
                if ((key!=13) && (key!='*')) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';       // else input key text in buffer
                  if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
                }
              }
            }
            // do show search spodify oversigt online
            if ((vis_spotify_oversigt) && (do_show_spotify_search_oversigt)) {
              if ((key!=13) && (key!='*') &&  (keybufferindex<search_string_max_length)) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';       // else input key text in buffer
                if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
                search_spotify_string_changed=true;
              }
            }
          }
          #endif
          // søg efter radio station navn fill buffer from keyboard
          if ((vis_radio_oversigt) && (!(show_radio_options))) {
            if (key!=13) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';       // else input key text in buffer
              if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
            }
          }
          // is ask for playlist file name use keybuffer to get filename
          if (ask_save_playlist) {
            if (key!=13) {
              if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
              strcpy(playlistfilename,keybuffer);
              playlistfilename[keybufferindex]='\0';       // else input key text in buffer
            }
          }
          // is ask movie totle fill buffer from keyboard
          if (vis_film_oversigt) {
            if (key!=13) {
              if ((key>31) && (key<127)) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';       // else input key text in buffer
              }
              if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
            }
          }

          // vis tvoversigt and tv graber setup
          if ((vis_tv_oversigt) && (do_show_tvgraber)) {
            // in setup menu
            // show_setup_tv_graber = true
            if ((key==32) && (do_show_setup_select_linie==0)) {
              if (aktiv_tv_graber.graberaktivnr<aktiv_tv_graber.graberantal+1) aktiv_tv_graber.graberaktivnr++; else aktiv_tv_graber.graberaktivnr=0;
              // husk last selected
              strcpy(configbackend_tvgraber_old,configbackend_tvgraber);
              strcpy(configbackend_tvgraber,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
              // set load flag to show_setup_tv_graber() func not good way to do it global var
              // and delete old db file to get the graber to update it
            }
            if (debugmode) fprintf(stderr,"do_show_setup_select_linie %d tvchannel_startofset %d \n",do_show_setup_select_linie,tvchannel_startofset);
            if (do_show_setup_select_linie>=1) {
              // set tvguide channel activate or inactive
              channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=!channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
            }
          }


          // setup window
          if (do_show_setup) {
            if (do_show_setup_sound) {
                if (do_show_setup_select_linie==0) {
                  if (key==32) {
                    if (soundsystem<numbersofsoundsystems-1) soundsystem++; else soundsystem=0;
                    strcpy(keybuffer,avalible_device[soundsystem]);
                    strcpy(configmythsoundsystem,avalible_device[soundsystem]);
                  } else strcpy(keybuffer,avalible_device[soundsystem]);
                } else if (do_show_setup_select_linie==1) {
                  if (key==32) {
                    if (strcmp(keybuffer,"HDMI")==0) strcpy(keybuffer,"SPDIF");
                    else if (strcmp(keybuffer,"SPDIF")==0) strcpy(keybuffer,"STREAM");
                    else if (strcmp(keybuffer,"STREAM")==0) strcpy(keybuffer,"HDMI");
                    else strcpy(keybuffer,"STREAM");
                  } else strcpy(keybuffer,"DEFAULT");
                } else if (do_show_setup_select_linie==2) {
                  if (key==32) {
                    if (strcmp(keybuffer,"FMOD")==0) strcpy(keybuffer,"SDL");
                    else if (strcmp(keybuffer,"SDL")==0) strcpy(keybuffer,"FMOD");
                    else strcpy(keybuffer,"FMOD");
                  } else strcpy(keybuffer,"FMOD");
                } else {
                  if (key!=13) {
                    keybuffer[keybufferindex]=key;
                    keybufferindex++;
                    keybuffer[keybufferindex]='\0';	// else input key text in buffer
                  }
                }
            } else if (do_show_setup_screen) {
              if (do_show_setup_select_linie==0) {
                if (key==32) {		// space key
                  if (screen_size==1) {
                    strcpy(keybuffer,"1280 x 1024 (720p)");
                    screen_size=2;
                  } else if (screen_size==2) {
                    strcpy(keybuffer,"1920 x 1080 (1080p)");
                    screen_size=3;
                  } else if (screen_size==3) {
                    strcpy(keybuffer,"1360 x 768         ");
                    screen_size=4;
                  } else if (screen_size==4) {
                    strcpy(keybuffer,"1024 x 768 (720p)  ");
                    screen_size=1;
                  }
                }
              // screen saver
              } else if (do_show_setup_select_linie==1) {
                if (key==32) {		// space key
                  if (strncmp(keybuffer,"analog",7)==0) {
                    strcpy(keybuffer,"digital");
                    urtype=DIGITAL;
                  } else if (strcmp(keybuffer,"digital")==0) {
                    strcpy(keybuffer,"3D");
                    urtype=SAVER3D;
                  } else if (strcmp(keybuffer,"3D")==0) {
                    strcpy(keybuffer,"3D2");
                    urtype=SAVER3D2;
                  } else if (strcmp(keybuffer,"3D2")==0) {
                    strcpy(keybuffer,"PICTURE3D");
                    urtype=PICTURE3D;
                  } else if (strcmp(keybuffer,"PICTURE3D")==0) {
                    strcpy(keybuffer,"None");
                    urtype=0;
                  } else if (strcmp(keybuffer,"None")==0) {
                    strcpy(keybuffer,"analog");
                    urtype=ANALOG;
                  } else strcpy(keybuffer,"analog");
                }
                // screen saver timeout
              } else if (do_show_setup_select_linie==2) {
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                } // use3d
              } else if (do_show_setup_select_linie==3) {
                if (key==32) {
                  if (use3deffect==true) {
                    use3deffect=false;
                    strcpy(configuse3deffect,"no");
                    strcpy(keybuffer,"no");
                  } else if (use3deffect==false) {
                    strcpy(configuse3deffect,"yes");
                    strcpy(keybuffer,"yes");
                    use3deffect=true;
                  }
                } else strcpy(keybuffer,configuse3deffect);
              // language
              } else if (do_show_setup_select_linie==4) {
                if (key==32) {
                  if (configland<(configlandantal-1)) configland++;
                  else configland=0;
                }
                strcpy(keybuffer,configlandsprog[configland]);
              } else if (do_show_setup_select_linie==5) {
                if (key==32) full_screen=!full_screen;
                if (full_screen) strcpy(keybuffer,"yes"); else strcpy(keybuffer,"no");
              }
            } else if (do_show_setup_network) {
              // hostname
              if (do_show_setup_select_linie==0) {
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                }
                // network link type
              } else if (do_show_setup_select_linie==1) {
                if (key==32) {				// space key
                  if (strcmp(keybuffer,"DHCP")==0) strcpy(keybuffer,"MANUAL");
                  else if (strcmp(keybuffer,"MANUAL")==0) strcpy(keybuffer,"DHCP");
                }
              } else if (do_show_setup_select_linie==2) {
                // mulighed for at manuelt indtaste wlan essid to use
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                }
              } else if (do_show_setup_select_linie==3) {
                // set show wlan network select
                if (key==32) {
                  show_wlan_select=!show_wlan_select;
                }
              } else if (do_show_setup_select_linie==4) {
                // set show wlan network password
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                }
              }
              // sql setup
            } else if (do_show_setup_sql) {
              if (do_show_setup_select_linie==0) {
                if (key==32) {
                  if (strcmp(keybuffer,"xbmc")==0) strcpy(keybuffer,"mythtv");
                  else if (strcmp(keybuffer,"mythtv")==0) strcpy(keybuffer,"xbmc");
                }
              } else {
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                }
              }
            } else if (do_show_setup_tema) {
              if (do_show_setup_select_linie==0) {
                if (key==32) {
                  tema++;
                  if (tema>TEMA_ANTAL) tema=1;
                }
              }
            } else if (do_show_setup_keys) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
              }
              // rss editor
            } else if (do_show_setup_rss) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
              }
            } else if (do_show_setup_spotify) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
              }
            } else if (do_show_videoplayer) {
              // video player setting
              if (do_show_setup_select_linie==0) {
                if (key!=13) {
                  keybuffer[keybufferindex] = key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                  //if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
                }
              }
              // video player screen mode
              if (do_show_setup_select_linie==1) {
                if (key==32) {
                  if (configdefaultplayer_screenmode<3) configdefaultplayer_screenmode++;
                  else configdefaultplayer_screenmode = 0;
                }
              }
              // select debug mode to show in console
              if (do_show_setup_select_linie==2) {
                if (key==32) {
                  if (debugmode>1) debugmode=debugmode*2; else debugmode++;
                  if (debugmode>512) debugmode = 0;
                }
              }
              // uv meter mode
              if (do_show_setup_select_linie==3) {
                if (key==32) {
                  configuvmeter=+1;
                  if (configuvmeter>2) configuvmeter = 0;
                }
                if (configuvmeter==0) strcpy(keybuffer,"none");
                if (configuvmeter==1) strcpy(keybuffer,"Simple");
                if (configuvmeter==2) strcpy(keybuffer,"Dual");
                sprintf(keybuffer,"%d",configuvmeter);
              }
            } else if (do_show_tvgraber) {
              // in setup menu
              // show_setup_tv_graber = true
              if ((key==32) && (do_show_setup_select_linie==0)) {
                if (aktiv_tv_graber.graberaktivnr<aktiv_tv_graber.graberantal+1) aktiv_tv_graber.graberaktivnr++; else aktiv_tv_graber.graberaktivnr=0;
                // husk last selected
                strcpy(configbackend_tvgraber_old,configbackend_tvgraber);
                strcpy(configbackend_tvgraber,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
                // set load flag to show_setup_tv_graber() func not good way to do it global var
                // and delete old db file to get the graber to update it
              }
              if (debugmode) fprintf(stderr,"do_show_setup_select_linie %d tvchannel_startofset %d \n",do_show_setup_select_linie,tvchannel_startofset);
              if (do_show_setup_select_linie>=1) {
                // set tvguide channel activate or inactive
                channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=!channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
              }
            }
          }
        }
      }
      // ved setup window
      if (do_show_setup) {
         if (do_show_setup_sound) {
             switch(do_show_setup_select_linie) {
                 case 0:
                  strcpy(configmythsoundsystem,keybuffer);
                  break;
                 case 1:
                  strcpy(configsoundoutport,keybuffer);
                  break;
                 case 2:
                  //#if defined USE_FMOD_MIXER
                  //if strcmp(keybuffer,"SDL") soundsystem=1;
                  //if strcmp(keybuffer,"SDL") soundsystem=1;
                  break;
            }
         } else if (do_show_setup_screen) {
             switch(do_show_setup_select_linie) {
                 case 0:
                  if (screen_size==1) strcpy(keybuffer,"1024 x 768 ");
                  else if (screen_size==2) strcpy(keybuffer,"1280 x 1024");
                  else if (screen_size==3) strcpy(keybuffer,"1920 x 1080");
                  else if (screen_size==4) strcpy(keybuffer,"1360 x 768 ");
                  break;
                 case 1:
                  strcpy(configaktivescreensavername,keybuffer);
                  break;
                 case 2:
                  strcpy(configscreensavertimeout,keybuffer);
                  break;
                 case 3:
                  //strcpy(configuse3deffect,keybuffer);
                  break;
                 case 4:
                   //strcpy(configuse3deffect,keybuffer);
                  break;
                 case 5:
                  if (key==32) full_screen=!full_screen;
            }
         } else if (do_show_setup_network) {
             switch(do_show_setup_select_linie) {
                 case 0:
                  strcpy(confighostname,keybuffer);
                  break;
                 case 1:
                  strcpy(confighostip,keybuffer);
                  break;
                 case 2:
                  strcpy(confighostwlanname,keybuffer);
                  break;
                 case 3:
                  // bruges ikke (er en knap (space bar))
                  break;
                 case 4:
                  wifinets.put_wifipass(setupwlanselectofset,keybuffer);
                  //strcpy(confighostip,keybuffer);
                  break;
            }
         } else if (do_show_setup_sql) {
             switch(do_show_setup_select_linie) {
                 case 0: strcpy(configbackend,keybuffer);
                         break;
                 case 1: strcpy(configmysqlhost,keybuffer);
                         configmythtvver=hentmythtvver();
                         break;
                 case 2: strcpy(configmysqluser,keybuffer);
                         configmythtvver=hentmythtvver();
                         break;
                 case 3: strcpy(configmysqlpass,keybuffer);
                         configmythtvver=hentmythtvver();
                         break;
                 case 4: strcpy(configmusicpath,keybuffer);
                         break;
                 case 5: strcpy(configpicturepath,keybuffer);
                         break;
                 case 6: strcpy(configmoviepath,keybuffer);
                         break;
                 case 7: strcpy(configrecordpath,keybuffer);
                         break;
            }
         } else if (do_show_setup_rss) {
            // update records
            switch(do_show_setup_select_linie) {
              case 0: rssstreamoversigt.set_stream_name(0+configrss_ofset,keybuffer);
                      break;
              case 1: rssstreamoversigt.set_stream_url(0+configrss_ofset,keybuffer);
                      break;
              case 2: rssstreamoversigt.set_stream_name(1+configrss_ofset,keybuffer);
                      break;
              case 3: rssstreamoversigt.set_stream_url(1+configrss_ofset,keybuffer);
                      break;
              case 4: rssstreamoversigt.set_stream_name(2+configrss_ofset,keybuffer);
                      break;
              case 5: rssstreamoversigt.set_stream_url(2+configrss_ofset,keybuffer);
                      break;
              case 6: rssstreamoversigt.set_stream_name(3+configrss_ofset,keybuffer);
                      break;
              case 7: rssstreamoversigt.set_stream_url(3+configrss_ofset,keybuffer);
                      break;
              case 8: rssstreamoversigt.set_stream_name(4+configrss_ofset,keybuffer);
                      break;
              case 9: rssstreamoversigt.set_stream_url(4+configrss_ofset,keybuffer);
                      break;
              case 10:rssstreamoversigt.set_stream_name(5+configrss_ofset,keybuffer);
                      break;
              case 11:rssstreamoversigt.set_stream_url(5+configrss_ofset,keybuffer);
                      break;
              case 12:rssstreamoversigt.set_stream_name(6+configrss_ofset,keybuffer);
                      break;
              case 13:rssstreamoversigt.set_stream_url(6+configrss_ofset,keybuffer);
                      break;
              case 14:rssstreamoversigt.set_stream_name(7+configrss_ofset,keybuffer);
                      break;
              case 15:rssstreamoversigt.set_stream_url(7+configrss_ofset,keybuffer);
                      break;
              case 16:rssstreamoversigt.set_stream_name(8+configrss_ofset,keybuffer);
                      break;
              case 17:rssstreamoversigt.set_stream_url(8+configrss_ofset,keybuffer);
                      break;
              case 18: rssstreamoversigt.set_stream_name(9+configrss_ofset,keybuffer);
                      break;
              case 19: rssstreamoversigt.set_stream_url(9+configrss_ofset,keybuffer);
                      break;
              case 20: rssstreamoversigt.set_stream_name(10+configrss_ofset,keybuffer);
                      break;
              case 21: rssstreamoversigt.set_stream_url(10+configrss_ofset,keybuffer);
                      break;
              case 22: rssstreamoversigt.set_stream_name(11+configrss_ofset,keybuffer);
                      break;
              case 23: rssstreamoversigt.set_stream_url(11+configrss_ofset,keybuffer);
                      break;
              case 24: rssstreamoversigt.set_stream_name(12+configrss_ofset,keybuffer);
                      break;
              case 25: rssstreamoversigt.set_stream_url(12+configrss_ofset,keybuffer);
                      break;
              case 26: rssstreamoversigt.set_stream_name(13+configrss_ofset,keybuffer);
                      break;
              case 27: rssstreamoversigt.set_stream_url(13+configrss_ofset,keybuffer);
                      break;
              case 28:rssstreamoversigt.set_stream_name(14+configrss_ofset,keybuffer);
                      break;
              case 29:rssstreamoversigt.set_stream_url(14+configrss_ofset,keybuffer);
                      break;
              case 30:rssstreamoversigt.set_stream_name(15+configrss_ofset,keybuffer);
                      break;
              case 31:rssstreamoversigt.set_stream_url(15+configrss_ofset,keybuffer);
                      break;
              case 32:rssstreamoversigt.set_stream_name(16+configrss_ofset,keybuffer);
                      break;
              case 33:rssstreamoversigt.set_stream_url(16+configrss_ofset,keybuffer);
                      break;
              case 34:rssstreamoversigt.set_stream_name(17+configrss_ofset,keybuffer);
                      break;
              case 35:rssstreamoversigt.set_stream_url(17+configrss_ofset,keybuffer);
                      break;
              case 36:rssstreamoversigt.set_stream_name(18+configrss_ofset,keybuffer);
                      break;
              case 37:rssstreamoversigt.set_stream_url(18+configrss_ofset,keybuffer);
                      break;
             }
         }
         #ifdef ENABLE_SPOTIFY
         if (do_show_setup_spotify) {
             switch(do_show_setup_select_linie) {
               case 0: strcpy(spotify_oversigt.spotify_client_id,keybuffer);
                       break;
               case 1: strcpy(spotify_oversigt.spotify_secret_id,keybuffer);
                       break;
             }
         }
         #endif
         if (do_show_setup_keys) {
             switch(do_show_setup_select_linie) {
                 case 0: strcpy(configkeyslayout[0].cmdname,keybuffer);
                         break;
                 case 1: configkeyslayout[0].scrnr=atoi(keybuffer);
                         break;
                 case 2: strcpy(configkeyslayout[1].cmdname,keybuffer);
                         break;
                 case 3: configkeyslayout[1].scrnr=atoi(keybuffer);
                         break;
                 case 4: strcpy(configkeyslayout[2].cmdname,keybuffer);
                         break;
                 case 5: configkeyslayout[2].scrnr=atoi(keybuffer);
                         break;
                 case 6: strcpy(configkeyslayout[3].cmdname,keybuffer);
                         break;
                 case 7: configkeyslayout[3].scrnr=atoi(keybuffer);
                         break;
                 case 8: strcpy(configkeyslayout[4].cmdname,keybuffer);
                         break;
                 case 9: configkeyslayout[4].scrnr=atoi(keybuffer);
                         break;
                 case 10: strcpy(configkeyslayout[5].cmdname,keybuffer);
                         break;
                 case 11: configkeyslayout[5].scrnr=atoi(keybuffer);
                          break;
                 case 12: strcpy(configkeyslayout[6].cmdname,keybuffer);
                          break;
                 case 13: configkeyslayout[6].scrnr=atoi(keybuffer);
                          break;
                 case 14: strcpy(configkeyslayout[7].cmdname,keybuffer);
                          break;
                 case 15: configkeyslayout[7].scrnr=atoi(keybuffer);
                          break;
                 case 16: strcpy(configkeyslayout[8].cmdname,keybuffer);
                          break;
                 case 17: configkeyslayout[8].scrnr=atoi(keybuffer);
                          break;
                 case 18: strcpy(configkeyslayout[9].cmdname,keybuffer);
                          break;
                 case 19: configkeyslayout[9].scrnr=atoi(keybuffer);
                          break;
             }
         } else if (do_show_videoplayer) {
            switch(do_show_setup_select_linie) {
                case 0: strcpy(configdefaultplayer,keybuffer);
                        break;
                case 1: strcpy(configvideoplayer,keybuffer);
                        break;
                case 2: debugmode=atoi(keybuffer);                   // set debugmode
                        break;
                case 3: if ((keybuffer,"none")==0) {
                          configuvmeter=0;
                        } else if ((keybuffer,"single")==0) {
                          configuvmeter=1;
                        } else if ((keybuffer,"dual")==0) {
                          configuvmeter=2;
                        } else configuvmeter=1;
                        break;
            }
         } else if (do_show_tvgraber) {
           switch(do_show_setup_select_linie) {
              case 0: break;
              case 1: if (strcmp(configbackend_tvgraber,"Other")==0) strcpy(configbackend_tvgraberland,keybuffer);
                      else {
                        fprintf(stderr,"Select tv channels\n");
                      }
                      break;
              case 2: break;
              default: break;
           }
         }
      }
    // end if **( start if) ****************************************
    // All other keys that + - * S U ESC
    } else {
        switch(key) {
            case 27:
              if (ask_save_playlist) {
                save_ask_save_playlist=false;
                ask_save_playlist=false;
              }
              // close setup windows again or close proram window.
            if (do_show_setup) {
                if (do_show_tvgraber) {
                  // make new tv overview
                  // kill running graber
                  // NOT ESC key
                  killrunninggraber();
                  // clear old tvguide in db
                  aktiv_tv_oversigt.cleartvguide();                             // clear old db
                  aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
                  // hent/update tv guide from db
                  aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
                  order_channel_list();                                         // ordre struct
                  // save chennel list info to internal datafile
                  save_channel_list();                                          // save to db file
                  // set update flag in display() func
                  firsttime_xmltvupdate = true;                                 // if true reset xml config file
                  // close tv graber windows again
                  do_show_tvgraber=false;
                  do_show_setup=false;
                  key=0;
                } else if (do_show_videoplayer) {
                  do_show_videoplayer=false;
                  key=0;
                } else if (do_show_setup_sql) {
                  do_show_setup_sql=false;
                  key=0;
                } else if (do_show_setup_font) {
                  do_show_setup_font=false;
                  key=0;
                } else if (do_show_setup_keys) {
                  do_show_setup_keys=false;
                  key=0;
                } else if (do_show_setup_tema) {
                  do_show_setup_tema=false;
                  key=0;
                } else if (do_show_setup_sound) {
                  do_show_setup_sound=false;
                  key=0;
                } else if (do_show_setup_screen) {
                  do_show_setup_screen=false;
                  key=0;
                } else if (do_show_setup_rss) {
                  // stop show setup of rss feeds
                  do_show_setup_rss=false;
                  key=0;
                } else if (do_show_setup_spotify) {
                  do_show_setup_spotify=false;
                  key=0;
                  // do_save_setup_rss=true;
                } else do_show_setup=false;
                key=0;
              }
              // vis tv overview and show tv_guide setup and press ESC key
              // called by esc  key
              if ((vis_tv_oversigt) && (do_show_setup==false) && (do_show_tvgraber)) {
                // make new tv overview
                // kill running graber
                // ESC KEY
                killrunninggraber();
                // clear old tvguide in db
                aktiv_tv_oversigt.cleartvguide();                             // clear old db
                aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
                // hent/update tv guide from db
                aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
                order_channel_list();                                         // ordre struct
                // save chennel list info to internal datafile
                save_channel_list();                                          // save to db file
                // set update flag in display() func
                firsttime_xmltvupdate = true;                                 // if true reset xml config file
                // close tv graber windows again
                do_show_tvgraber=false;
                key=0;
              } else if (vis_music_oversigt) {
                vis_music_oversigt=false;
                key=0;
              } else if (vis_radio_oversigt) {
                vis_radio_oversigt=false;
                key=0;
              } else if (vis_film_oversigt) {
                vis_film_oversigt=false;
                key=0;
              } else if (vis_stream_oversigt) {
                vis_stream_oversigt=false;
                key=0;
              } else if (vis_tv_oversigt) {
                vis_tv_oversigt=false;
                key=0;
              } else if (vis_recorded_oversigt) {
                vis_recorded_oversigt=false;
                key=0;
              } else if (vis_radio_or_music_oversigt) {
                vis_radio_or_music_oversigt=false;
                key=0;
              } else if (vis_stream_or_movie_oversigt) {
                vis_stream_or_movie_oversigt=false;
                key=0;
              } else if (vis_spotify_oversigt) {
                ask_save_playlist=false;
                hent_spotify_search=false;
                vis_spotify_oversigt=false;
                keybufferopenwin=false;
                //spotify_oversigt.opdatere_spotify_oversigt(0);                  // reset spotify overview
                //spotify_oversigt.load_spotify_iconoversigt();
                //spotify_oversigt.set_search_loaded();                           // triger icon loader
                key=0;
              } else if ((!(do_show_setup)) && (key==27)) {                       // exit program
                remove("mythtv-controller.lock");
                runwebserver=false;
                order_channel_list();
                save_channel_list();
                write_logfile((char *) "Exit program.");
                exit(0);                                                        //  exit program
              } else if ((vis_tv_oversigt) && (do_show_tvgraber)) {
                // Close tv_graber view from tv_oversigt
                do_show_tvgraber=false;
                key=0;
              } else key=0;
              break;
            case '*':
              // update spotify
              // or ask record tv program
              //if ((do_update_spotify_playlist==false) && (spotify_oversigt_loaded_begin==false)) do_update_spotify_playlist=true;       // set update flag
              #ifdef ENABLE_SPOTIFY
              if (do_show_spotify_search_oversigt==false) {
                if (do_update_spotify_playlist==false) do_update_spotify_playlist=true;       // set update flag til true og start background update
              } else if (vis_music_oversigt) do_zoom_music_cover=!do_zoom_music_cover;        // show/hide music info
              #endif
              if (vis_radio_oversigt) do_zoom_radio=!do_zoom_radio;                      // show/hide music info
              else if (vis_film_oversigt) do_zoom_film_cover=!do_zoom_film_cover;             // film info
              else if ((vis_stream_oversigt) && (sknapnr>0)) do_zoom_stream_cover=!do_zoom_stream_cover;  // stream info
              else if ((vis_tv_oversigt) && (do_zoom_tvprg_aktiv_nr>0)) {                     // tv oversigt zoom
                do_zoom_tvprg_aktiv_nr=0;
              } else if (vis_tv_oversigt) {
                // spørg kan/skal vi optage den ?
                ask_tv_record = true;
                tvknapnr=tvsubvalgtrecordnr;                                                   // set program nr
                do_zoom_tvprg_aktiv_nr=tvknapnr;					                                     // husk den valgte aktiv tv prg
              }
              #ifdef ENABLE_SPOTIFY
              if (vis_spotify_oversigt) {
                if (do_show_spotify_search_oversigt) {
                  spotify_oversigt.searchtype++;
                  if (spotify_oversigt.searchtype>3) spotify_oversigt.searchtype=0;
                }
              }
              #endif
              break;
            case optionmenukey:                                                       // default o
              if (vis_film_oversigt) {
                vis_movie_options=!vis_movie_options;
              } else if ((vis_tv_oversigt) && (!(vis_tvrec_list))) {
                vis_old_recorded=!vis_old_recorded;	                        	    // show old recorded programs
              } else if (vis_radio_oversigt) {
                show_radio_options=!show_radio_options;			                    	// show radio options
                if (do_zoom_radio) do_zoom_radio=false;
              }
              break;
            case 'g':
              if (vis_movie_options) {
                vis_movie_sort_option=1;
                vis_movie_options=false;                        // luk option window igen
              }
              break;
            case 'l':
              // load tv guide
              if (vis_tv_oversigt) {
                // load tv guide
                // write debug log
                write_logfile((char *) "Loading tvguidedb file.");
                aktiv_tv_oversigt.loadparsexmltvdb();
              }
              break;
            case 'p':
              if ((vis_tv_oversigt) && (!(vis_old_recorded))) {
                vis_tvrec_list=!vis_tvrec_list;
              } else if (!(vis_old_recorded)) {
                vis_tvrec_list=!vis_tvrec_list;
              }
              break;
            case 'r':
              // sort movies
              if (vis_movie_options) {
                vis_movie_sort_option=1;
                vis_movie_options=false;			// luk option window igen
              }
              // Ask record tv channel ?
              if ((vis_tv_oversigt) && (do_zoom_tvprg_aktiv_nr==0)) {                     // tv oversigt zoom
                // spørg kan/skal vi optage den ?
                ask_tv_record = true;
                tvknapnr=tvsubvalgtrecordnr;                                                   // set program nr
                do_zoom_tvprg_aktiv_nr=tvknapnr;					                                     // husk den valgte aktiv tv prg
              }
              break;
              // save playlist
            case 'S':
              // do save playlist
              if (vis_music_oversigt) {
                // save playlist
                fprintf(stderr,"Ask save playlist\n");
                ask_save_playlist = true;                                         // set save playlist flag
              }
              break;
            case 't':
              if (vis_movie_options) {
                vis_movie_sort_option=2;
                vis_movie_options=false;                        // luk option window igen
              }
              break;
            case 'y':
              /*
               if (do_show_spotify_search_oversigt==false) {
                 if (do_update_spotify_playlist==false) do_update_spotify_playlist=true;       // set update flag
               }
               */
//               if ((firsttimespotifyupdate==true) && (strcmp(spotify_oversigt.spotify_get_token(),"")!=0)) {
//                 do_update_spotify_playlist=true;             // start spotify update
//                 do_update_spotify = true;                                       // set flag to update spotify
//               }
              write_logfile((char *) "Key y Not in use.");
              break;
            case 'u':
              if ((vis_tv_oversigt) && (loading_tv_guide==false)) {
                // u key
                // Update tv guide
                write_logfile((char *) "Update tv guide.");
                // set flag for show update
                do_update_xmltv_show = true;
                loading_tv_guide = true;
                if (strcmp(configbackend,"mythtv")==0) {
                  update_xmltv_phread_loader();                   // start thred update flag in main loop
                } else if (strcmp(configbackend,"xbmc")==0) {
                  update_xmltv_phread_loader();
                }
                tv_guide_firsttime_update = true;                                 // set update flag
              }
              // Update rss stuf
              if (vis_stream_oversigt) {
                do_update_rss_show = true;                                     // set show update flag
                do_update_rss = true;                                          // set update flag
              }
              break;
            case 'U':
              // music
              if ((vis_music_oversigt) && (ask_open_dir_or_play==false)) {
                write_logfile((char *) "Update music db.");
                do_update_music = true;                                               // show update
                do_update_music_now = true;                                           // Set update flag
              }
              // spotify
              #ifdef ENABLE_SPOTIFY
              if (vis_spotify_oversigt) {
                write_logfile((char *) "Update Spotify.");
                do_update_spotify = true;                                       // set flag to update spotify
              }
              #endif

              if ((vis_film_oversigt) && (!(do_update_moviedb))) {
                do_update_moviedb = true;                                           // set update flag
                pthread_t loaderthread1;                                          // loader thread
                // start multi thread and update movie overview
                // movie loader
                // write debug log
                write_logfile((char *) "Update movie db.");
                if ((strncmp(configbackend,"xbmc",4)==0) || (strncmp(configbackend,"kodi",4)==0)) {
                  int rc1=pthread_create(&loaderthread1,NULL,xbmcdatainfoloader_movie,NULL);
                  if (rc1) {
                    fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
                    exit(-1);
                  }
                } else {
                  if (configmythtvver>=0) {
                    int rc1=pthread_create(&loaderthread1,NULL,datainfoloader_movie,NULL);
                    if (rc1) {
                      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
                      exit(-1);
                    }
                  }
                }
              }
              break;
            case 13:
              write_logfile((char *) "Start search....");
              if (debugmode) {
                if (vis_music_oversigt) {
                  if (ask_save_playlist) fprintf(stderr,"Save playlist key pressed, update music list.\n");
                  else fprintf(stderr,"Enter key pressed, update music list.\n");
                } else if (vis_radio_oversigt) fprintf(stderr,"Enter key pressed, play radio station.\n");
                else if (vis_stream_oversigt) fprintf(stderr,"Enter key pressed, update stream view.\n");
                else if (do_show_setup_network) fprintf(stderr,"Enter key pressed in set network\n");
                else if (vis_tv_oversigt) fprintf(stderr,"Enter key pressed in vis tv oversigt\n");
                else if (do_show_tvgraber) fprintf(stderr,"Enter key pressed in vis show tvgraber\n");
                else if (vis_spotify_oversigt) fprintf(stderr,"Enter key pressed in vis show spotify\n");
              }
              // set save flag of playlist
              if (ask_save_playlist) {
                save_ask_save_playlist = true;
              }
              if (vis_radio_oversigt) {
                rknapnr=0;
                hent_radio_search = true;			  	// start radio station search
                radio_key_selected=1;
                _rangley=0.0f;
              }
              if (vis_film_oversigt) {
                hent_film_search = true;			  	// start movie title search
                fknapnr=0;
              }
              // search podcast
              if (vis_stream_oversigt) {
                hent_stream_search = true;				// start stream station search
                sknapnr=stream_select_iconnr;   // selected
                stream_key_selected=1;
                stream_select_iconnr=0;         // tmp
                _sangley=0.0f;
                do_play_stream=0;
              }
              // search spotify
              #ifdef ENABLE_SPOTIFY
              if (vis_spotify_oversigt) {
                hent_spotify_search = true;				      // start stream station search
                spotifyknapnr=spotify_select_iconnr;   // selected
                spotify_key_selected=1;
                spotify_select_iconnr=0;
                //_sangley=0.0f;
                do_play_spotify=0;
                //spotifyknapnr = the one to play
              }
              #endif
              // start music search
              if ((vis_music_oversigt) && (keybufferopenwin)) {
                mknapnr=0;				                            	// reset mouse/key pos in vis_music_overs
                _mangley=0.0f;
                hent_music_search = true;                         // start music search (set flag)
                music_key_selected=1;                           // reset to first icon
                music_select_iconnr=0;                          // reset to first icon
                music_icon_anim_icon_ofset=0;                   // reset to first icon
                music_icon_anim_icon_ofsety=0;
              }
              if ((vis_music_oversigt) && (!(do_zoom_music_cover)) && ((ask_save_playlist==false))) {
                mknapnr=music_key_selected;	                     	// hent valget
                // normal dir
                if (musicoversigt.get_album_type(mknapnr-1)==0) {
                  if (debugmode & 2) fprintf(stderr,"Normal dir id load.\n");
                  if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                  do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                  if (debugmode & 2) fprintf(stderr,"dir id %d  ",do_play_music_aktiv_nr);
                  if (do_play_music_aktiv_nr>0) {
                    antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                  } else antal_songs=0;
                  // write debug log
                  sprintf(debuglogdata,"Found numbers of songs:%2d",antal_songs);
                  write_logfile((char *) debuglogdata);
                  if (antal_songs==0) {
                    ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                  } else {
                    ask_open_dir_or_play_aopen=false;
                  }
                  ask_open_dir_or_play = true;							// yes ask om vi skal spille den (play playlist)
                  //do_zoom_music_cover=true;
                } else {
                  // playlist dir
                  if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                  // playlist loader
                  do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                  if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                  if (do_play_music_aktiv_nr>0) {
//                          antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                  } else antal_songs=0;
                  // write debug log
                  sprintf(debuglogdata,"Found numbers of songs:%2d",antal_songs);
                  write_logfile((char *) debuglogdata);
                  if (antal_songs==0) {
                    ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                  } else {
                    ask_open_dir_or_play_aopen=false;
                  }
                  ask_open_dir_or_play = true;							// yes ask om vi skal spille den
                }
                /*
                  mknapnr=music_key_selected;					// hent valget
                  if (musicoversigt[mknapnr-1].oversigttype==0) {
                      // dirid som skal vises
                      do_play_music_aktiv_nr=musicoversigt[mknapnr-1].directory_id;			// set den aktive mappe dir id
                      // hent antal sange i dirid fra database og opdatere liste liste med sange samt loader cover
                      antal_songs=hent_antal_dir_songs(musicoversigt[mknapnr-1].directory_id);	// find antal dir/song i dir id
                      if (debugmode & 2) fprintf(stderr,"Antal sange fundet i dirid %d = %d \n",musicoversigt[mknapnr-1].directory_id,antal_songs);
                      if ((antal_songs==0) || (musicoversigt[mknapnr-1].directory_id==0)) {		// er der ingen sange i dir lav en auto open
                          ask_open_dir_or_play_aopen=1; 						// flag auto open
                      } else {
                          ask_open_dir_or_play_aopen=0;						// ingen auto open
                      }
                  } else {
                      do_play_music_aktiv_nr=musicoversigt[mknapnr-1].directory_id;
                      // hent antal sange i dirid fra database og opdatere liste liste med sange samt loader cover

                      if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);

                      if (do_play_music_aktiv_nr>0) {
                          antal_songs=hent_antal_dir_songs_playlist(mknapnr-1);

                          //                          antal_songs=0;
                      } else antal_songs=0;

                      if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d\n",antal_songs);
                      if (antal_songs==0) {		// er der ingen sange i dir lav en auto open
                          ask_open_dir_or_play_aopen=1; 						// flag auto open
                      } else {
                          ask_open_dir_or_play_aopen=0;						// ingen auto open
                      }
                  }

                  if ((do_play_music_aktiv_nr)) {							// er der en aktiv sang i mappen som skal spilles
                      do_swing_music_cover=0;
                      ask_open_dir_or_play=true;							// yes ask om vi skal spille den
                      do_swing_music_cover=true;
                  } else ask_open_dir_or_play=true;
//                      do_zoom_music_cover_remove_timeout=showtimeout;		// set show music info timeout
                */
              }
              // enter key pressed og ask open dir or play er åben så start play
              if ((vis_music_oversigt) && (ask_open_dir_or_play) && (ask_save_playlist==false)) {
                ask_open_dir_or_play=false;                 // flag luk vindue igen
                do_play_music_cover=1;                      // der er trykket på cover play det
                do_zoom_music_cover=false;                  // ja den skal spilles lav zoom cover info window
                do_find_playlist = true;                      // find de sange som skal indsættes til playlist (og load playlist andet sted)
              }
              if ((vis_radio_oversigt) && (show_radio_options==false)) {
                rknapnr=radio_select_iconnr;					// hent button
                fprintf(stderr,"Set do_play_radio flag rknapnr=%d \n",rknapnr);
                if (rknapnr>0) do_play_radio=1;					// start play
              }
              // opdatere radio oversigt igen efter vis radio options
              if ((vis_radio_oversigt) && (show_radio_options)) {
                // write debug log
                write_logfile((char *) "Update radio overview.");
                radiooversigt.clean_radio_oversigt();				// clean old liste
                radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
                radiooversigt.load_radio_stations_gfx();
                show_radio_options=false;
                _rangley=0;
              }
              if (do_show_setup) {
                if (do_show_setup_font) {
                  fprintf(stderr,"Set aktiv font to %s \n",aktivfont.typeinfo[setupfontselectofset].fontname);
                  strcpy(configfontname,aktivfont.typeinfo[setupfontselectofset].fontname);
                  aktivfont.selectfont(configfontname);
                }
              }
              if (vis_recorded_oversigt) {
                do_play_recorded_aktiv_nr=1;							// set play aktiv recorded program flag (bliver sat igang i draw)
              }
              // tv guide oversigt
              if (vis_tv_oversigt) {
                // hvis der trykkes enter på default ask_tv_record (yes)
                // blivere den sat til record mode (create mysql data in record table)
                fprintf(stderr,"Ask om vi skal optage program \n");
                if ((ask_tv_record) && (do_zoom_tvprg_aktiv_nr>0)) {
                  // do it
                  // set start record tv prgoram
                  //aktiv_tv_oversigt.gettvprogramrecinfo(tvvalgtrecordnr,tvsubvalgtrecordnr,prgtitle,prgstarttid,prgendtid);
                  aktiv_tv_oversigt.tvprgrecord_addrec(tvvalgtrecordnr,tvsubvalgtrecordnr);					// put tv prgoram into table record in mythtv backend (to set mythtv to record the program)
                  ask_tv_record=false;
                  // opdatere tv guide med nyt info
                  aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                } else {
                  // spørg kan/skal vi optage den ?
                  ask_tv_record = true;
                  tvknapnr=tvsubvalgtrecordnr;
                  do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
                }
              }
              // stream oversigt
              if ((vis_stream_oversigt) && (sknapnr>=0)){
                //if (debugmode) fprintf(stderr,"sknapnr %d  path_antal=%d type %d stream antal = %d \n",sknapnr,streamoversigt.get_stream_groupantal(sknapnr),streamoversigt.type,streamoversigt.streamantal());
                if (streamoversigt.type==0) {
                  strncpy(temptxt,streamoversigt.get_stream_name(sknapnr),200);
                  streamoversigt.clean_stream_oversigt();
                  if (debugmode & 4) fprintf(stderr,"stream nr %d name %s \n ",sknapnr,temptxt);
                  streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");
                  do_play_stream=false;
                } else if (streamoversigt.type==1) {
                  if (sknapnr>0) do_play_stream=1;						// select button do play
                  // do back
                  if (sknapnr==0) {
                    streamoversigt.clean_stream_oversigt();
                    streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                    //streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr),streamoversigt.get_stream_path(sknapnr));
                    do_play_stream=false;
                    stream_key_selected=1;
                    stream_select_iconnr=0;
                    _sangley=0.0f;
                  }
                } else {
                  // back button
                  if (debugmode & 4) fprintf(stderr,"stream nr %d \n ",sknapnr-1);
                  if ((sknapnr)==0) {
                    if (streamoversigt.type==2) {
                      // one level up
                      streamoversigt.clean_stream_oversigt();
                      streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr),(char *)"");
                      do_play_stream=false;
                      do_play_stream=false;
                      stream_key_selected=1;
                      stream_select_iconnr=0;
                      _sangley=0.0f;
                    } else {
                      // jump to top
                      streamoversigt.clean_stream_oversigt();
                      streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                      do_play_stream=false;
                      do_play_stream=false;
                      stream_key_selected=1;
                      stream_select_iconnr=0;
                      _sangley=0.0f;
                    }
                  }
                }
                // play stream
                if ((sknapnr>=0) && (do_play_stream)) {
                  if (strncmp(streamoversigt.get_stream_url(sknapnr),"mythflash",9)==0) {
                    startstream = true;
                  } else {
                    startstream = true;
                  }
                }
              }
              //
              // send spotify player to new device
              //
              #ifdef ENABLE_SPOTIFY
              if (vis_spotify_oversigt) {
                if (do_select_device_to_play) {
                  // select device to play on
                  // write debug log
                  write_logfile((char *) "Send play command to spotify device");
                  spotify_oversigt.spotify_play_now_playlist( spotify_oversigt.get_spotify_playlistid( (spotifyknapnr+spotify_selected_startofset)-1 ) ,1);
                  // close window again
                  do_select_device_to_play=false;
                }
                // search func
                if (!(do_select_device_to_play)) {
                  // write debug log
                  write_logfile((char *) "Start search spotify");
                }
              }
              #endif
              // enter pressed in setup window xmltv
              // select new tv guide provider
              if ((do_show_tvgraber) && (do_show_setup_select_linie==0)) {
                if (strcmp(configbackend_tvgraber_old,configbackend_tvgraber)!=0) {
                  // clean all tv guide data and reload
                  // remove config dat file
                  fprintf(stderr,"* Delete old tvguide *\n");
                  fprintf(stderr,"* Update new tvguide *\n");
                  getuserhomedir(path);
                  strcat(path,"/tvguide_channels.dat");
                  unlink(path);                                                 // delete file
                  hent_tv_channels=false;
                  // set update process
                  //do_update_xmltv=true;
                  strcpy(configbackend_tvgraber_old,configbackend_tvgraber);
                }
              }
              if (do_show_setup) {
              // ved (return) set wlan network and close show wlan select window
                if (show_wlan_select) {
                  show_wlan_select=false;
                  // set default wlan network to selected
                  write_logfile((char *) "Open wifi network scan");
                  wifinets.get_networkid(setupwlanselectofset,id);
                  strcpy(confighostwlanname,id);
                  sprintf(tmptxt,"sudo /sbin/iwconfig wlan0 essid %s",id);
                  fprintf(stderr,"Charge network by %s \n",tmptxt);
                  system(tmptxt);
                  sprintf(debuglogdata,"Set wifi to %s ",confighostwlanname);
                  write_logfile((char *) debuglogdata);
                }
                if (do_show_setup_network) {
                  if (do_show_setup_select_linie<4) do_show_setup_select_linie++;
                  fprintf(stderr,"next line %d \n",do_show_setup_select_linie);
                }
              }
              break;
        }
    }
}


// ****************************************************************************************
//
// in use
// lirc remove controller
//
// ****************************************************************************************

void update2(int value) {
  char cmd[200];
  int ret;
  int n;
  char *code=0;

  int numbers_film_covers_on_line;
  int numbers_cd_covers_on_line;
  int numbers_radio_covers_on_line;
  int numbers_stream_covers_on_line;
  const float MOVIE_CS=46.0f;					// movie dvd cover side
  const float MUSIC_CS=41.0;					// music cd cover side
  const float RADIO_CS=41.0;					// radio cd cover side
  numbers_cd_covers_on_line=8;        // 9
  numbers_film_covers_on_line=8;
  numbers_radio_covers_on_line=8;
  numbers_stream_covers_on_line=8;
  char temptxt[200];
  if ((sock!=0) && (sock!=-1)) {
    while ((ret=lirc_nextcode(&code))==0 && code!=NULL) {
      if (code==NULL) continue;
      if (lircconfig) {
        // sample lirc data
        // 0000000080010069 00 KEY_LEFT devinput-32
        // 000000008001006a 00 KEY_RIGHT devinput-32
        // 0000000080010201 00 KEY_NUMERIC_1 devinput-32
        // get command from string
        strcpy(cmd,code+20);
        n=strlen(cmd)-1;
        while((cmd[n]!=32) && (n>0)) {
          n--;
        }
        cmd[n]='\0';
        if (debugmode) fprintf(stderr,"Lirc say %s\n",cmd);
        // lirc vis_music_oversigt
        if (strcmp(cmd,"KEY_AUDIO")==0) {							// show music directoy
          vis_tv_oversigt=false;
          vis_film_oversigt=false;
          do_zoom_film_cover=false;
          do_zoom_stream_cover=false;
          vis_music_oversigt = true;                                    // show music oversigt
          vis_recorded_oversigt=false;
          vis_radio_oversigt=false;
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc vis_radio_oversigt
        if (strcmp(cmd,"KEY_RADIO")==0) {							                  // show radio overview
          vis_tv_oversigt=false;
          vis_film_oversigt=false;
          do_zoom_film_cover=false;
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          vis_music_oversigt=false;
          vis_radio_oversigt = true;
          vis_recorded_oversigt=false;
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc vis_film_oversigt
        if (strcmp(cmd,"KEY_VIDEO")==0) {						                  // Show video overview
          vis_tv_oversigt=false;
          vis_film_oversigt = true;
          vis_music_oversigt=false;
          vis_recorded_oversigt=false;
          vis_radio_oversigt=false;				   			                  // sluk radio oversigt
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc vis_recorded_oversigt
        if (strcmp(cmd,"KEY_PVR")==0) {                               // show recorded overview
          do_play_music_aktiv=false;							                  // sluk music info cover
          vis_tv_oversigt=false;								                    // sluk tv oversigt
          vis_film_oversigt=false; 						   	                  // sluk film oversigt
          vis_music_oversigt=false;  							                  // sluk music oversigt
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          vis_recorded_oversigt = true;	                  						// on recorded program oversigt
          vis_radio_oversigt=false;						   	                  // sluk radio oversigt
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc vis_tv_oversigt
        if (strcmp(cmd,"KEY_EPG")==0) {                               // show tv guide
          do_play_music_aktiv=false;							                  // sluk music info cover
          vis_tv_oversigt = true;             		                    // vis tv oversigt
          vis_film_oversigt=false; 						   	                  // sluk film oversigt
          vis_music_oversigt=false;  							                  // sluk music oversigt
          do_zoom_film_cover=false;
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          vis_recorded_oversigt=false;                							// sluk recorded program oversigt
          vis_radio_oversigt=false;						   	                  // sluk radio oversigt
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc show rss stream oversigt
        if (strcmp(cmd,"KEY_TUNER")==0) {                               // show tv guide
          do_play_music_aktiv=false;							                  // sluk music info cover
          vis_tv_oversigt=false;             		                    // vis tv oversigt
          vis_film_oversigt=false; 						   	                  // sluk film oversigt
          vis_music_oversigt=false;  							                  // sluk music oversigt
          do_zoom_film_cover=false;
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          vis_recorded_oversigt=false;                							// sluk recorded program oversigt
          vis_radio_oversigt=false;						   	                  // sluk radio oversigt
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt = true;                                 // show rss stream oversigt
        }
        // lirc
        if ((strcmp(cmd,"KEY_HOME")==0) || (strcmp(cmd,"KEY_MEDIA")==0)) {
          do_play_music_aktiv=false;							                  // sluk music info cover
          vis_tv_oversigt=false;								                    // sluk tv oversigt
          vis_film_oversigt=false; 						   	                  // sluk film oversigt
          vis_music_oversigt=false;  							                  // sluk music oversigt
          do_zoom_music_cover=false;							                  // sluk zoom cd cover
          do_zoom_stream_cover=false;
          do_zoom_film_cover=false;
          vis_recorded_oversigt=false;						                  // sluk recorded program oversigt
          vis_radio_oversigt=false;				   			                  // sluk radio oversigt
          vis_nyefilm_oversigt=false;
          vis_stream_oversigt=false;                                 // show rss stream oversigt
        }
        // lirc
        // Pause music player
        if (strcmp(cmd,"P")==0) {
          #if defined USE_FMOD_MIXER
          if (channel) {
            //channel->setMute(true);
            channel->setVolume(0.0);
            ERRCHECK(result,0);
          }
          #endif
        }
        //
        if (strcmp(cmd,"KEY_UP")==0) {
          // up key
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            if (do_show_play_open_select_line>0) do_show_play_open_select_line--; else
              if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
          }
          // lirc
          // up key
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
              switch (screen_size) {
                  case 1:
                          if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1)) ) {
                            if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1))) {
                               _mangley-=41;
                               music_select_iconnr-=numbers_cd_covers_on_line;
                               do_music_icon_anim_icon_ofset=-1;
                            } else music_select_iconnr-=numbers_cd_covers_on_line;
                            if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                              music_key_selected-=numbers_cd_covers_on_line;
                            }
                          }
                          break;
                  case 2:
                          if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1)) ) {
                            if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1))) {
                               _mangley-=41;
                               music_select_iconnr-=numbers_cd_covers_on_line;
                               do_music_icon_anim_icon_ofset=-1;
                            } else music_select_iconnr-=numbers_cd_covers_on_line;
                            if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                              music_key_selected-=numbers_cd_covers_on_line;
                            }
                          }
                          break;
                  case 3:
                          if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1)) ) {
                            if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1))) {
                               _mangley-=41;
                               music_select_iconnr-=numbers_cd_covers_on_line;
                               do_music_icon_anim_icon_ofset=-1;
                            } else music_select_iconnr-=numbers_cd_covers_on_line;
                            if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                              music_key_selected-=numbers_cd_covers_on_line;
                            }
                          }
                          break;
                  case 4:
                          if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1)) ) {
                            if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1))) {
                               _mangley-=41;
                               music_select_iconnr-=numbers_cd_covers_on_line;
                               do_music_icon_anim_icon_ofset=-1;
                            } else music_select_iconnr-=numbers_cd_covers_on_line;
                            if (music_key_selected>numbers_cd_covers_on_line) {
                              music_key_selected-=numbers_cd_covers_on_line;
                            }
                          }
                          break;
              }
          }
          // lirc
          // up key
          if (vis_film_oversigt) {
              switch(screen_size) {
                  case 1:
                          if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                            if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                               _fangley-=MOVIE_CS;
                               film_select_iconnr-=numbers_film_covers_on_line;
                            } else film_select_iconnr-=numbers_film_covers_on_line;
                            if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                          }

                          break;
                  case 2:
                          if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                            if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                               _fangley-=MOVIE_CS;
                               film_select_iconnr-=numbers_film_covers_on_line;
                            } else film_select_iconnr-=numbers_film_covers_on_line;
                            if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                          }
                          break;
                  case 3:
                          if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                            if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                               _fangley-=MOVIE_CS;
                               film_select_iconnr-=numbers_film_covers_on_line;
                            } else film_select_iconnr-=numbers_film_covers_on_line;
                            if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                          }
                          break;
                  case 4:
                          if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                            if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                               _fangley-=MOVIE_CS;
                               film_select_iconnr-=numbers_film_covers_on_line;
                            } else film_select_iconnr-=numbers_film_covers_on_line;
                            if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                          }
                          break;
              }
          }
          // lirc
          // up key
          if ((vis_radio_oversigt) && (show_radio_options==false)) {
            if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
              if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                _rangley-=MOVIE_CS;
                radio_select_iconnr-=numbers_radio_covers_on_line;
              } else radio_select_iconnr-=numbers_radio_covers_on_line;
              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
            }
/*
              switch(screen_size) {
                  case 1:
                          if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                              if (((int) radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                   _rangley-=MOVIE_CS;
                                   radio_select_iconnr-=numbers_radio_covers_on_line;
                              } else radio_select_iconnr-=numbers_radio_covers_on_line;
                              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                          }

                          break;
                  case 2:
                          if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                              if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                   _rangley-=MOVIE_CS;
                                   radio_select_iconnr-=numbers_radio_covers_on_line;
                              } else radio_select_iconnr-=numbers_radio_covers_on_line;
                              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                          }
                          break;
                  case 3:

                          if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                              if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                   _rangley-=MOVIE_CS;
                                   radio_select_iconnr-=numbers_radio_covers_on_line;
                              } else radio_select_iconnr-=numbers_radio_covers_on_line;
                              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                          }
                          break;
                  case 4:
                          if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                              if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                   _rangley-=MOVIE_CS;
                                   radio_select_iconnr-=numbers_radio_covers_on_line;
                              } else radio_select_iconnr-=numbers_radio_covers_on_line;
                              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                          }
                          break;
              }
*/
          }
          // lirc
          // up key
          if ((vis_radio_oversigt) && (show_radio_options)) {
            radiooversigt.lastradiooptselect();
          }
          // lirc
          // up key
          // stream
          if ((vis_stream_oversigt) && (show_stream_options==false)) {
            if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
              if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                 _rangley-=MOVIE_CS;
                 stream_select_iconnr-=numbers_stream_covers_on_line;
              } else stream_select_iconnr-=numbers_stream_covers_on_line;
              if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
            }
          }
          // lirc
          // up key
          if (vis_recorded_oversigt) {
            if ((visvalgtnrtype==1) && (valgtrecordnr>0)) {
              valgtrecordnr--;
              subvalgtrecordnr=0;
            } else if ((visvalgtnrtype==2) && (subvalgtrecordnr>0)) subvalgtrecordnr--;	// bruges til visning af optaget programmer
            reset_recorded_texture = true;		// load optaget programs texture gen by mythtv
          }
          // lirc
          // up key
          if (vis_tv_oversigt) {
            // tv stuf up key
            // if indside tv overoview
            if (vis_tv_oversigt) {
              if (tvsubvalgtrecordnr>0) {
                tvsubvalgtrecordnr--;
                if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)<hourtounixtime(aktiv_tv_oversigt.vistvguidekl)) {
                  if (aktiv_tv_oversigt.vistvguidekl>0) aktiv_tv_oversigt.vistvguidekl--;
                }
              }
            }
          }
          // lirc
          // up key
          if (do_show_setup) {
             if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
          }
        }
        if (strcmp(cmd,"KEY_DOWN")==0) {
          // lirc
          // down key
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            if ((unsigned int) do_show_play_open_select_line+do_show_play_open_select_line_ofset<(unsigned int) dirmusic.numbersinlist()-1) {
                if ((int) do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
            }
          }
          // lirc
          // down key
          // hvis ikke ask_open_dir_or_play
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) &&  (music_select_iconnr+9<musicoversigt_antal)) {
            if ((unsigned int) music_key_selected>=(unsigned int) ((numbers_cd_covers_on_line*4)+1)) {
              do_music_icon_anim_icon_ofset=1;						// do anim
              _mangley+=41.0f;								//scroll gfx down
              music_select_iconnr+=numbers_cd_covers_on_line;
            } else {
              music_key_selected+=numbers_cd_covers_on_line;
              music_select_iconnr+=numbers_cd_covers_on_line;
            }
          }
          // lirc
          // down key
          if (vis_film_oversigt) {
              switch (screen_size) {					// filmoversigt_antal
                  case 1: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                            if (film_select_iconnr+numbers_film_covers_on_line>11) {		// skal vi scroll liste up
                              do_movie_icon_anim_icon_ofset=1;
                              _fangley+=MOVIE_CS;
                              //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                              if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                            }
                            film_key_selected+=numbers_film_covers_on_line;
                            film_select_iconnr+=numbers_film_covers_on_line;
                          }
                          break;
                  case 2: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                            if (film_select_iconnr+numbers_film_covers_on_line>13) {		// skal vi scroll liste up
                              do_movie_icon_anim_icon_ofset=1;
                              _fangley+=MOVIE_CS;
                              //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                              if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                            }
                            film_key_selected+=numbers_film_covers_on_line;
                            film_select_iconnr+=numbers_film_covers_on_line;
                          }
                          break;
                  case 3:
                          if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                            if (film_select_iconnr+numbers_film_covers_on_line>19) {		// skal vi scroll liste up
                              do_movie_icon_anim_icon_ofset=1;
                              _fangley+=MOVIE_CS;
                              //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                              if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                            }
                            film_key_selected+=numbers_film_covers_on_line;
                            film_select_iconnr+=numbers_film_covers_on_line;
                          }
                          break;
                  case 4: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                            if (film_select_iconnr+numbers_film_covers_on_line>17) {		// skal vi scroll liste up
                              do_movie_icon_anim_icon_ofset=1;
                              _fangley+=MOVIE_CS;
                              //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                              if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                            }
                            film_key_selected+=numbers_film_covers_on_line;
                            film_select_iconnr+=numbers_film_covers_on_line;
                          }
                          break;
                  }
          }
          // lirc
          // down key
          // radio
          if ((vis_radio_oversigt) && (show_radio_options==false) && ((radio_select_iconnr+numbers_radio_covers_on_line)<radiooversigt.radioantal())) {
            if (radio_key_selected>=20) {
              _rangley+=RADIO_CS;
              radio_select_iconnr+=numbers_radio_covers_on_line;
            } else {
              radio_key_selected+=numbers_radio_covers_on_line;
              radio_select_iconnr+=numbers_radio_covers_on_line;
            }
          }
          if ((vis_radio_oversigt) && (show_radio_options)) radiooversigt.nextradiooptselect();
          // down key
          // stream stuf
          if (vis_stream_oversigt) {
            if ((vis_stream_oversigt) && (show_stream_options==false) && ((stream_select_iconnr+numbers_stream_covers_on_line)<streamoversigt.streamantal())) {
              if (stream_key_selected>=20) {
                _rangley+=RADIO_CS;
                stream_select_iconnr+=numbers_stream_covers_on_line;
              } else {
                stream_key_selected+=numbers_stream_covers_on_line;
                stream_select_iconnr+=numbers_stream_covers_on_line;
              }
            }
          }
          // lirc
          // down key
          if (vis_recorded_oversigt) {
              if (visvalgtnrtype==1) {
                if ((int) valgtrecordnr<(int) recordoversigt.top_antal()) {
                  valgtrecordnr++;
                  subvalgtrecordnr=0;
                }
              } else if (visvalgtnrtype==2) {
                if ((int) subvalgtrecordnr<(int) recordoversigt.programs_type_antal(valgtrecordnr)-1) {
                  subvalgtrecordnr++;
                }
              }
              reset_recorded_texture = true;
          }
          // lirc
          // down key
          if (vis_tv_oversigt) {
            // tv overview
            // if indside tv overoview
            if (vis_tv_oversigt) {
              if (debugmode) fprintf(stderr,"prg antal in tv kanal %d \n ",aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr));
              if (tvsubvalgtrecordnr+1<aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr)) {
                tvsubvalgtrecordnr++;
              }
              // check hvor vi er
              if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)>hourtounixtime(aktiv_tv_oversigt.vistvguidekl+3)) {
                if (aktiv_tv_oversigt.vistvguidekl<24*2) aktiv_tv_oversigt.vistvguidekl++;
              }
            }
/*
              if (tvvisvalgtnrtype==1) {
                  if (tvvalgtrecordnr<aktiv_tv_oversigt.tv_kanal_antal()) {
                      tvvalgtrecordnr++;
                      tvsubvalgtrecordnr=0;
                  }
              } else if (tvvisvalgtnrtype==2) {
                  if (tvsubvalgtrecordnr<aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr)) {
                      tvsubvalgtrecordnr++;
                  }
              }
*/
          }

        }

        if (strcmp(cmd,"KEY_LEFT")==0) {
          // lirc
          // left key
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
          }
          // lirc
          // left key
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
            if (music_key_selected>1) {
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                if (music_key_selected>1) {
                  music_key_selected--;
                  music_select_iconnr--;
                } else {
                  if (music_select_iconnr>0) {
                     _mangley-=41;
                     music_key_selected+=numbers_cd_covers_on_line-1;  // den viste på skærm af 1 til 20
                     music_select_iconnr--;                  	// den rigtige valgte af 1 til cd antal
                  }
                }
              }
            }
          }
          // lirc
          // left key
          if (vis_film_oversigt) {							// ved film oversigt
              if (film_key_selected>1) {
                  switch (screen_size) {
                      case 1: if (film_key_selected % 12==0) {
                                  if (film_select_iconnr>0) {
                                    film_key_selected--;			// den som er ramme om
                                    film_select_iconnr--;
                                  } else {
                                    if ((_fangley-MOVIE_CS)<0) {
                                      _fangley-=MOVIE_CS;
                                      film_key_selected--;
                                      film_select_iconnr=-3;
                                      do_movie_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((film_select_iconnr==1) && (_fangley>0)) {
                                  _fangley-=MOVIE_CS;
                                  film_select_iconnr+=4;
                                  do_movie_icon_anim_icon_ofset=-1;
                                }
                                film_key_selected--;
                                film_select_iconnr--;
                              }
                              break;
                      case 2: if (film_key_selected % 24==0) {
                                  if (film_select_iconnr>0) {
                                    film_key_selected--;
                                    film_select_iconnr--;
                                  } else {
                                    if (_fangley-MOVIE_CS<0) {
                                       _fangley-=MOVIE_CS;
                                      film_key_selected--;
                                      film_select_iconnr=-5;
                                      do_movie_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((film_select_iconnr==1) && (_fangley>0)) {
                                  _fangley-=MOVIE_CS;
                                  film_select_iconnr+=6;
                                  do_movie_icon_anim_icon_ofset=-1;
                                }
                                film_key_selected--;
                                film_select_iconnr--;
                              }
                              break;
                      case 3: if (film_key_selected % 36==0) {
                                  if (film_select_iconnr>0) {
                                    film_key_selected--;
                                    film_select_iconnr--;
                                  } else {
                                    if (_fangley-MOVIE_CS<0) {
                                      _fangley-=MOVIE_CS;
                                      film_key_selected--;
                                      film_select_iconnr=-8;
                                      do_movie_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((film_select_iconnr==1) && (_fangley>0)) {
                                  _fangley-=MOVIE_CS;
                                  film_select_iconnr+=9;
                                  do_movie_icon_anim_icon_ofset=-1;
                                }
                                film_key_selected--;
                                film_select_iconnr--;
                              }
                              break;
                      case 4: if (film_key_selected % 36==0) {
                                  if (film_select_iconnr>0) {
                                    film_key_selected--;
                                    film_select_iconnr--;
                                  } else {
                                    if (_fangley-MOVIE_CS<0) {
                                      _fangley-=MOVIE_CS;
                                      film_key_selected--;
                                      film_select_iconnr=-8;
                                      do_movie_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((film_select_iconnr==1) && (_fangley>0)) {
                                  _fangley-=MOVIE_CS;
                                  film_select_iconnr+=9;
                                  do_movie_icon_anim_icon_ofset=-1;
                                }
                                film_key_selected--;
                                film_select_iconnr--;
                              }
                              break;
                  }
              }
          }
          // lirc
          // left key
          if (vis_nyefilm_oversigt) {
            if (film_key_selected>0) {
              film_key_selected--;
              film_select_iconnr--;
            }
          }
          // lirc
          // left key
          if (vis_radio_oversigt) {							// ved film oversigt
              if (radio_key_selected>1) {
                  switch (screen_size) {
                      case 1: if (radio_key_selected % 12==0) {
                                if (radio_select_iconnr>0) {
                                  radio_key_selected--;			// den som er ramme om
                                  radio_select_iconnr--;
                                } else {
                                  if ((_rangley-MOVIE_CS)<0) {
                                    _rangley-=MOVIE_CS;
                                    radio_key_selected--;
                                    radio_select_iconnr=-3;
                                    do_radio_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((radio_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  radio_select_iconnr+=4;
                                  do_radio_icon_anim_icon_ofset=-1;
                                }
                                radio_key_selected--;
                                radio_select_iconnr--;
                              }
                              break;
                      case 2: if (radio_key_selected % 24==0) {
                                if (radio_select_iconnr>0) {
                                  radio_key_selected--;
                                  radio_select_iconnr--;
                                } else {
                                  if (_rangley-MOVIE_CS<0) {
                                     _rangley-=MOVIE_CS;
                                    radio_key_selected--;
                                    radio_select_iconnr=-5;
                                    do_radio_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((radio_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  radio_select_iconnr+=6;
                                  do_radio_icon_anim_icon_ofset=-1;
                                }
                                radio_key_selected--;
                                radio_select_iconnr--;
                              }
                              break;
                      case 3: if (radio_key_selected % 36==0) {
                                if (radio_select_iconnr>0) {
                                  radio_key_selected--;
                                  radio_select_iconnr--;
                                } else {
                                  if (_rangley-MOVIE_CS<0) {
                                    _rangley-=MOVIE_CS;
                                    radio_key_selected--;
                                    radio_select_iconnr=-8;
                                    do_radio_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((radio_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  radio_select_iconnr+=9;
                                  do_radio_icon_anim_icon_ofset=-1;
                                }
                                radio_key_selected--;
                                radio_select_iconnr--;
                              }
                              break;
                      case 4: if (radio_key_selected % 36==0) {
                                if (radio_select_iconnr>0) {
                                  radio_key_selected--;
                                  radio_select_iconnr--;
                                } else {
                                  if (_rangley-MOVIE_CS<0) {
                                    _rangley-=MOVIE_CS;
                                    radio_key_selected--;
                                    radio_select_iconnr=-8;
                                    do_radio_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((radio_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  radio_select_iconnr+=9;
                                  do_radio_icon_anim_icon_ofset=-1;
                                }
                                radio_key_selected--;
                                radio_select_iconnr--;
                              }
                              break;
                  }
              }
          }
          // left key
          // stream
          if (vis_stream_oversigt) {							// ved film oversigt
              if (stream_key_selected>1) {
                  switch (screen_size) {
                      case 1: if (stream_key_selected % 12==0) {
                                if (stream_select_iconnr>0) {
                                  stream_key_selected--;			// den som er ramme om
                                  stream_select_iconnr--;
                                } else {
                                  if ((_rangley-MOVIE_CS)<0) {
                                    _rangley-=MOVIE_CS;
                                    stream_key_selected--;
                                    stream_select_iconnr=-3;
                                    do_stream_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((stream_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  stream_select_iconnr+=4;
                                  do_stream_icon_anim_icon_ofset=-1;
                                }
                                stream_key_selected--;
                                stream_select_iconnr--;
                              }
                              break;
                      case 2: if (stream_key_selected % 24==0) {
                                if (stream_select_iconnr>0) {
                                  stream_key_selected--;
                                  stream_select_iconnr--;
                                } else {
                                  if (_rangley-MOVIE_CS<0) {
                                     _rangley-=MOVIE_CS;
                                    stream_key_selected--;
                                    stream_select_iconnr=-5;
                                    do_stream_icon_anim_icon_ofset=-1;
                                  }
                                }
                              } else {
                                if ((stream_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  stream_select_iconnr+=6;
                                  do_stream_icon_anim_icon_ofset=-1;
                                }
                                stream_key_selected--;
                                stream_select_iconnr--;
                              }
                              break;
                      case 3: if (stream_key_selected % 36==0) {
                                  if (stream_select_iconnr>0) {
                                    stream_key_selected--;
                                    stream_select_iconnr--;
                                  } else {
                                    if (_rangley-MOVIE_CS<0) {
                                      _rangley-=MOVIE_CS;
                                      stream_key_selected--;
                                      stream_select_iconnr=-8;
                                      do_stream_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((stream_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  stream_select_iconnr+=9;
                                  do_stream_icon_anim_icon_ofset=-1;
                                }
                                stream_key_selected--;
                                stream_select_iconnr--;
                              }
                              break;
                      case 4: if (stream_key_selected % 36==0) {
                                  if (stream_select_iconnr>0) {
                                    stream_key_selected--;
                                    stream_select_iconnr--;
                                  } else {
                                    if (_rangley-MOVIE_CS<0) {
                                      _rangley-=MOVIE_CS;
                                      stream_key_selected--;
                                      stream_select_iconnr=-8;
                                      do_stream_icon_anim_icon_ofset=-1;
                                    }
                                  }
                              } else {
                                if ((stream_select_iconnr==1) && (_rangley>0)) {
                                  _rangley-=MOVIE_CS;
                                  stream_select_iconnr+=9;
                                  do_stream_icon_anim_icon_ofset=-1;
                                }
                                stream_key_selected--;
                                stream_select_iconnr--;
                              }
                              break;
                  }
              }
          }
          // lirc
          // left key
          if (vis_recorded_oversigt) {
            visvalgtnrtype=1;
          }
          // lirc
          // left key
          if (vis_tv_oversigt) {
            if ((tvvisvalgtnrtype==1) && (tvvalgtrecordnr>0)) {
              tvvalgtrecordnr--;
              tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr+1,tvsubvalgtrecordnr));
            }
          }
        }

        if (strcmp(cmd,"KEY_RIGHT")==0) {
          // lirc
          // right key
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
          }
          // lirc
          // right key
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
            if (((int) music_key_selected<(int) musicoversigt_antal) && (music_icon_anim_icon_ofset==0)) {
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr<musicoversigt_antal)) {
                if (((int) music_key_selected % (numbers_cd_covers_on_line*4)==0) || (((int) music_select_iconnr==((numbers_cd_covers_on_line*4)-1)) && ((int) music_key_selected % numbers_cd_covers_on_line==0))) {
                  _mangley+=41;
                  music_key_selected-=numbers_cd_covers_on_line;			// den viste på skærm af 1 til 20
                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                } else {
                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                }
                music_key_selected++;
              }
            }
          }
          // lirc
          // right key
          if (vis_film_oversigt) {
            if ((vis_film_oversigt) && ((int unsigned) film_select_iconnr<film_oversigt.film_antal()-1)) {
              if ((int) film_select_iconnr<(int) film_oversigt.film_antal()) {
                if (((int) film_key_selected % (numbers_film_covers_on_line*3)==0) || (((int) film_select_iconnr==((numbers_film_covers_on_line*4)-1)) && ((int) film_key_selected % numbers_film_covers_on_line==0))) {
                  _fangley+=MOVIE_CS;
                  film_key_selected-=numbers_film_covers_on_line;			// den viste på skærm af 1 til 20
                  film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                } else {
                  film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                }
                film_key_selected++;
              }
            }
          }
          // lirc
          // right key
          if (vis_nyefilm_oversigt) {
            if ((film_key_selected+1<film_oversigt.film_antal()) && (film_key_selected<6)) {
              film_key_selected++;
              film_select_iconnr++;
            }
          }
          // lirc
          // right key
          if (vis_radio_oversigt) {
            if ((vis_radio_oversigt) && (radio_select_iconnr<(int) radiooversigt_antal-1)) {
              if (radio_select_iconnr<(int) radiooversigt_antal) {
                if (((int) radio_key_selected % (numbers_radio_covers_on_line*3)==0) || (((int) radio_select_iconnr==((numbers_radio_covers_on_line*4)-1)) && ((int) radio_key_selected % numbers_radio_covers_on_line==0))) {
                  _rangley+=MOVIE_CS;
                  radio_key_selected-=numbers_radio_covers_on_line;			// den viste på skærm af 1 til 20
                  radio_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                } else {
                  radio_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                }
                radio_key_selected++;
              }
            }
          }
          // lirc
          // right key
          // stream
          if (vis_stream_oversigt) {
            if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt.streamantal()-1)) {
              if (stream_select_iconnr<(int) streamoversigt.streamantal()) {
                if (((int) stream_key_selected % (numbers_stream_covers_on_line*3)==0) || (((int) stream_select_iconnr==((numbers_stream_covers_on_line*4)-1)) && ((int) stream_key_selected % numbers_stream_covers_on_line==0))) {
                  _rangley+=MOVIE_CS;
                  stream_key_selected-=numbers_stream_covers_on_line;			// den viste på skærm af 1 til 20
                  stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                } else {
                  stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                }
                stream_key_selected++;
              }
            }
          }
          // lirc
          // right key
          if (vis_recorded_oversigt) {
            visvalgtnrtype=2;
          }
          // lirc
          // right key
          if (vis_tv_oversigt) {
            if (vis_tv_oversigt) {
              if (tvvisvalgtnrtype==1) {
                if (tvvalgtrecordnr<aktiv_tv_oversigt.tv_kanal_antal()-1) tvvalgtrecordnr++;
                tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr-1,tvsubvalgtrecordnr));
              }
            }
          }
        }
        // lirc back button
        if (strcmp(cmd,"KEY_EXIT")==0) {
          if (vis_music_oversigt) {
             if (ask_open_dir_or_play) {
               ask_open_dir_or_play=false;
               do_zoom_film_cover=false;
             } else do_zoom_music_cover=!do_zoom_music_cover;		                 // show/hide music info
          }
          if (vis_radio_oversigt) {
            do_zoom_radio=!do_zoom_radio;
          }
          if (vis_stream_oversigt) {
            do_zoom_stream_cover=false;
            sknapnr=0;
          }
          if (do_zoom_film_cover) {
            do_zoom_film_cover=false;
            fknapnr=0;
            mknapnr=0;					// reset knapnr i alt
          }
          if (vis_tv_oversigt) {
            if (ask_tv_record) {
              ask_tv_record=false;
              if (debugmode) fprintf(stderr,"lirc Hide tvprogram info.\n");
            }
          }
        }
        // start info or play media file
        if (((strcmp(cmd,"KEY_OK")==0) || (strcmp(cmd,"KEY_INFO")==0)) || (strcmp(cmd,"KEY_PLAYPAUSE")==0)) {
          // music
          if (vis_music_oversigt) {
            if ((!(do_zoom_music_cover)) && (!(ask_open_dir_or_play))) {
              //mknapnr=music_key_selected;	                     	// hent valget
              mknapnr=music_select_iconnr+1;                                                        //
              // normal dir
              if (musicoversigt.get_album_type(mknapnr-1)==0) {
                if (debugmode & 2) fprintf(stderr,"Normal dir id load.\n");
                if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                if (debugmode & 2) fprintf(stderr,"dir id %d ",do_play_music_aktiv_nr);
                if (do_play_music_aktiv_nr>0) {
                  antal_songs=hent_antal_dir_songs(do_play_music_aktiv_nr);
                } else antal_songs=0;
                if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d\n",antal_songs);
                if (antal_songs==0) {
                  ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                } else {
                  ask_open_dir_or_play_aopen=false;
                }
                ask_open_dir_or_play = true;							// yes ask om vi skal spille den (play playlist)
                //do_zoom_music_cover=true;
              } else {
                // do playlist
                if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                // playlist loader
                do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                if (do_play_music_aktiv_nr>0) {
                            antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                } else antal_songs=0;
                if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d\n",antal_songs);
                if (antal_songs==0) {
                  ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                } else {
                  ask_open_dir_or_play_aopen=false;
                }
                ask_open_dir_or_play = true;							// yes ask om vi skal spille den
              }
            } else if (ask_open_dir_or_play) {
              ask_open_dir_or_play=false;                 // flag luk vindue igen
              do_play_music_cover = true;                   // der er trykket på cover play det
              do_zoom_music_cover=false;                  // ja den skal spilles lav zoom cover info window
              do_find_playlist = true;                      // find de sange som skal indsættes til playlist (og load playlist andet sted)
            }
          }
          if (vis_film_oversigt) {				                                                          // select movie to show info for
            if ((do_zoom_film_cover==false) || (strcmp(cmd,"KEY_INFO")==0)) {
              do_zoom_film_cover = true;
              do_swing_movie_cover=0;
              do_zoom_film_aktiv_nr=film_select_iconnr+1;		// OLD film_key_selected;
              fknapnr=film_select_iconnr+1;		           		// OLD film_key_selected;
              strcpy(cmd,"");
            }
            if (do_zoom_film_cover) {
              // start play movie
              if ((strcmp(cmd,"KEY_OK")==0) || (strcmp(cmd,"KEY_PLAYPAUSE")==0)) startmovie = true;
            }
          }
          // start menu (show movies for some time)
          if (vis_nyefilm_oversigt) {
            if (do_zoom_film_cover==false) {
              do_zoom_film_cover = true;
              do_swing_movie_cover=0;
              do_zoom_film_aktiv_nr=film_select_iconnr+1;		// OLD film_key_selected;
              fknapnr=film_select_iconnr+1;		           		// OLD film_key_selected;
              strcpy(cmd,"");
            }
            if (do_zoom_film_cover) {
              // start play movie
              if ((strcmp(cmd,"KEY_OK")==0) || (strcmp(cmd,"KEY_PLAYPAUSE")==0)) startmovie = true;
            }
          }
          if (vis_recorded_oversigt) {
            // play record/rss fil
            do_play_recorded_aktiv_nr=1;
          }
          // start play radio
          if ((vis_radio_oversigt) && (!(show_radio_options))) {
            // play radio station
            rknapnr=radio_key_selected;		// hent button
            if (rknapnr>0) do_play_radio=1;
          }
          // opdatere radio oversigt efter pressed on the remorte control from lirc
          if ((vis_radio_oversigt) && (show_radio_options)) {
            // write debug log
            write_logfile((char *) "Update radio overview.");
            radiooversigt.clean_radio_oversigt();			                        // clean old liste
            radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
            radiooversigt.load_radio_stations_gfx();
            show_radio_options=false;
          }
          if (vis_tv_oversigt) {
            tvknapnr=tvsubvalgtrecordnr;
            do_zoom_tvprg_aktiv_nr=tvknapnr;
            ask_tv_record=!ask_tv_record;
            if (ask_tv_record) fprintf(stderr,"lirc Show tvprogram info.\n"); else fprintf(stderr,"lirc Hide tvprogram info.\n");
          }
          if (vis_stream_oversigt) {
            //do_zoom_stream_cover=!do_zoom_stream_cover;
            hent_stream_search = true;				                                  // start stream station search
            sknapnr=stream_select_iconnr;                                   // selected
            stream_key_selected=1;
            stream_select_iconnr=0;         // tmp
            _sangley=0.0f;
            do_play_stream=0;
          }
          // stream oversigt do it
          if ((vis_stream_oversigt) && (sknapnr>=0)) {
            //if (debugmode) fprintf(stderr,"sknapnr %d  path_antal=%d type %d stream antal = %d \n",sknapnr,streamoversigt.get_stream_groupantal(sknapnr),streamoversigt.type,streamoversigt.streamantal());
            if (streamoversigt.type==0) {
              strncpy(temptxt,streamoversigt.get_stream_name(sknapnr),200);
              streamoversigt.clean_stream_oversigt();
              if (debugmode & 4) fprintf(stderr,"stream nr %d name %s \n ",sknapnr,temptxt);
              streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");
              do_play_stream=false;
            } else if (streamoversigt.type==1) {
              if (sknapnr>0) do_play_stream=1;						// select button do play
              // do back
              if (sknapnr==0) {
                streamoversigt.clean_stream_oversigt();
                streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                //streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr),streamoversigt.get_stream_path(sknapnr));
                do_play_stream=false;
                stream_key_selected=1;
                stream_select_iconnr=0;
                _sangley=0.0f;
              }
            } else {
              // back button
              fprintf(stderr,"stream nr %d \n ",sknapnr-1);
              if ((sknapnr)==0) {
                if (streamoversigt.type==2) {
                  // one level up
                  streamoversigt.clean_stream_oversigt();
                  streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr),(char *)"");
                  do_play_stream=false;
                  do_play_stream=false;
                  stream_key_selected=1;
                  stream_select_iconnr=0;
                  _sangley=0.0f;
                } else {
                  // jump to top
                  streamoversigt.clean_stream_oversigt();
                  streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                  do_play_stream=false;
                  do_play_stream=false;
                  stream_key_selected=1;
                  stream_select_iconnr=0;
                  _sangley=0.0f;
                }
              }
            }
            // play stream
            if ((sknapnr>=0) && (do_play_stream)) {
              if (strncmp(streamoversigt.get_stream_url(sknapnr),"mythflash",9)==0) {
                startstream = true;
                sknapnr+=1;
              } else {
                startstream = true;
                sknapnr+=1;
              }
            }
          }
        }
        // end start play
        if (strcmp("KEY_VOLUMEUP",cmd)==0) {
          if (configsoundvolume<1.0f) configsoundvolume+=0.1f;
          #if defined USE_FMOD_MIXER
          if (sndsystem) channel->setVolume(configsoundvolume);
          #endif
          if (vis_film_oversigt) {
            film_oversigt.volumeup();
            configsoundvolume+=0.1f;
          }
          show_volume_info = true;					// show volume info window
          vis_volume_timeout=80;
        }
        if (strcmp("KEY_VOLUMEDOWN",cmd)==0) {
          if (configsoundvolume>0) configsoundvolume-=0.1f;
          #if defined USE_FMOD_MIXER
          if (sndsystem) channel->setVolume(configsoundvolume);
          #endif
          if (vis_film_oversigt) {
            film_oversigt.volumedown();
            configsoundvolume-=0.1f;
          }
          show_volume_info = true;					// show volume info window
          vis_volume_timeout=80;
        }
        if (strcmp("KEY_NEXT",cmd)==0) {
           // next song
           if ((vis_music_oversigt) && (!(do_shift_song))) {
             if ((do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) && (do_shift_song==false)) {
               do_play_music_aktiv_table_nr++;
               do_shift_song = true;
               do_zoom_music_cover = true;
             }
           }
           if (vis_tv_oversigt) {
             aktiv_tv_oversigt.changetime(60*60*24);
             aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
           }
           if (vis_film_oversigt) {
             film_oversigt.next_movie_chapther();
           }
        }
        if (strcmp("KEY_PREVIOUS",cmd)==0) {
         // last song
          if ((vis_music_oversigt) && (!(do_shift_song))) {
           if ((do_play_music_aktiv_table_nr>1) && (do_shift_song==false)) {
             do_play_music_aktiv_table_nr--;						// skift aktiv sang
             do_shift_song = true;							// sæt flag til skift
             do_zoom_music_cover = true;
           }
          }
          if (vis_tv_oversigt) {
            aktiv_tv_oversigt.changetime(-(60*60*24));
            aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
          }
          if (vis_film_oversigt) {
            film_oversigt.prevous_movie_chapther();
          }
        }
        if (strcmp(cmd,"KEY_STOP")==0) {
          // stop all music
          do_stop_music = true;
          // stop movie
          film_oversigt.stopmovie();
          if (streamoversigt.stream_is_playing) {
            // stop player
            streamoversigt.stopstream();
            do_zoom_stream_cover=false;                                          // close window again after stop play
            do_zoom_stream=false;
          }
        }
        if (strcmp(cmd,"KEY_CHANNELUP")==0) {
        }
        if (strcmp(cmd,"KEY_CHANNELDOWN")==0) {
        }
        if (strcmp(cmd,"KEY_MUTE")==0) {
        }
        if (strcmp(cmd,"KEY_INFO")==0) {
        }
        if (strcmp(cmd,"KEY_SLEEP")==0) {
          exit(2);
        }
      }
    }
    if (code) free(code);
  }
  #if defined USE_FMOD_MIXER
  sndsystem->update();				// run update on fmod sound system
  #endif
  glutTimerFunc(25, update2, 0);
  glutPostRedisplay();
}




// ****************************************************************************************
//
// in use
//
// ****************************************************************************************

void update(int value) {
    float MOVIE_CS;		// movie dvd cover side
    float MUSIC_CS;		// music cd cover side
    float RADIO_CS;		// radio cover size
    int numbers_film_covers_on_line;
    int numbers_cd_covers_on_line;
    int numbers_radio_covers_on_line;
    int numbers_stream_covers_on_line;
    _angle+=0.5;				// for 3D screen saver and rotate movie cover
    if (_angle > 360) {
      _angle = 0;
    }
    char *code=0;
    char *c=0;
    int ret;
    if ((sock!=0) && (sock!=-1)) {
        while ((ret=lirc_nextcode(&code))==0 && code!=NULL) {
            if (code==NULL) continue;
            if (lircconfig) {
                while((ret=lirc_code2char(lircconfig,code,&c))==0 && c!=NULL) {
                    if (debugmode) fprintf(stderr,"Lirc Command %s \n ",c);
                    // write debug log
                    sprintf(debuglogdata,"Lirc Command %s ",c);
                    write_logfile((char *) debuglogdata);
                    saver_irq = true;				// RESET (sluk hvis aktiv) screen saver
                    if (strcmp(c,"Music")==0) {							// show music directoy
                      vis_tv_oversigt=false;
                      vis_film_oversigt=false;
                      do_zoom_film_cover=false;
                      vis_music_oversigt = true;
                      vis_recorded_oversigt=false;
                      vis_radio_oversigt=false;
                    }
                    if (strcmp(c,"Radio")==0) {							// show music directoy
                      vis_tv_oversigt=false;
                      vis_film_oversigt=false;
                      do_zoom_film_cover=false;
                      vis_music_oversigt=false;
                      vis_radio_oversigt = true;
                      vis_recorded_oversigt=false;
                    }

                    if (strcmp(c,"Videos")==0) {						// Show video directory
                      vis_tv_oversigt=false;
                      vis_film_oversigt = true;
                      vis_music_oversigt=false;
                      vis_recorded_oversigt=false;
                      vis_radio_oversigt=false;
                    }

                    if (strcmp(c,"RecTV")==0) {							// recorded tv
                      vis_tv_oversigt=false;
                      vis_film_oversigt=false;
                      vis_music_oversigt=false;
                      do_zoom_music_cover=false;
                      vis_recorded_oversigt = true;
                      vis_radio_oversigt=false;
                    }

                    if (strcmp(c,"Guide")==0) {							// tv program guide
                      vis_film_oversigt=0;
                      vis_music_oversigt=false;
                      do_zoom_music_cover=false;
                      vis_recorded_oversigt=false;
                      vis_radio_oversigt=false;
                      vis_tv_oversigt = true;
                    }

                    // Home back to starvis_radio_oversigtt of program init
                    if (strcmp(c,"Home")==0) {
                      do_play_music_aktiv = false;							// sluk music info cover
                      vis_tv_oversigt = false;								// sluk tv oversigt
                      vis_film_oversigt = false; 							// sluk film oversigt
                      vis_music_oversigt = false;  							// sluk music oversigt
                      do_zoom_music_cover = false;							// sluk zoom cd cover
                      do_zoom_film_cover = false;
                      vis_recorded_oversigt = false;							// sluk recorded program oversigt
                      vis_radio_oversigt = false;							// sluk radio oversigt
                    }
                    if (strcmp(c,"KEY_BACK")==0) {
                      if (vis_music_oversigt) vis_music_oversigt = false;
                      if (do_zoom_film_cover) do_zoom_film_cover = false;
                      if (vis_tv_oversigt) vis_tv_oversigt = false;
                      if (vis_film_oversigt) vis_film_oversigt = false;
                      if (vis_recorded_oversigt) vis_recorded_oversigt = false;
                      if (vis_radio_oversigt) vis_radio_oversigt = false;
                    }
                    // Pause music player
                    if (strcmp(c,"P")==0) {
                      #if defined USE_FMOD_MIXER
                      if (channel) {
                        //channel->setMute(true);
                        channel->setVolume(0.0);
                        ERRCHECK(result,0);
                      }
                      #endif
                    }
                    // lirc move down
                    if (strcmp(c,"Key-nav-down")==0) {
                        if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                          if ((unsigned int) do_show_play_open_select_line+do_show_play_open_select_line_ofset<(unsigned int) dirmusic.numbersinlist()-1) {
                            if ((int) do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                          }
                        }
                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                            if (((int) music_key_selected<(int) musicoversigt_antal-4) && (music_icon_anim_icon_ofset==0)) {
                                switch (screen_size) {
                                    case 1:
                                            if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                                              if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                                                if (do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                                              }
                                            }
                                            // hvis ikke ask_open_dir_or_play
                                            if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) &&  (music_select_iconnr+5<musicoversigt_antal)) {
                                              if ((unsigned int) music_key_selected>=(unsigned int) ((numbers_cd_covers_on_line*3)+1)) {
                                                do_music_icon_anim_icon_ofset=1;				// direction -1 = up 1 = down
                                                //_mangley+=(41.0f);						// scroll window down one icon
                                                music_select_iconnr += numbers_cd_covers_on_line;			// add to next line
                                              } else {
                                                music_key_selected += numbers_cd_covers_on_line;
                                                music_select_iconnr += numbers_cd_covers_on_line;
                                              }
                                            }
                                            break;
                                    case 2:
                                            if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                                                if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                                                    if (do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                                                }
                                            }
                                            // hvis ikke ask_open_dir_or_play
                                            if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) &&  (music_select_iconnr+5<musicoversigt_antal)) {
                                                if ((unsigned int) music_key_selected>=(unsigned int) ((numbers_cd_covers_on_line*3)+1)) {
                                                    do_music_icon_anim_icon_ofset=1;						// do anim
                                                    _mangley += 41.0f;								//scroll gfx down
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected += numbers_cd_covers_on_line;
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                }
                                            }
                                            break;

                                    case 3:
                                            if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                                                if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                                                    if (do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                                                }
                                            }
                                            // hvis ikke ask_open_dir_or_play
                                            if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) &&  (music_select_iconnr+9<musicoversigt_antal)) {
                                                if ((unsigned int) music_key_selected>=(unsigned int) ((numbers_cd_covers_on_line*4)+1)) {
                                                    do_music_icon_anim_icon_ofset=1;						// do anim
                                                    _mangley += 41.0f;								//scroll gfx down
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected += numbers_cd_covers_on_line;
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                }
                                            }
                                            break;
                                    case 4:
                                            if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                                                if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                                                    if (do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                                                }
                                            }
                                            // hvis ikke ask_open_dir_or_play
                                            if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) &&  (music_select_iconnr+8<musicoversigt_antal)) {
                                                if ((unsigned int) music_key_selected>=(unsigned int) ((numbers_cd_covers_on_line*3)+1)) {
                                                    do_music_icon_anim_icon_ofset = 1;						//
                                                    _mangley += 41.0f;								//scroll gfx down
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected += numbers_cd_covers_on_line;
                                                    music_select_iconnr += numbers_cd_covers_on_line;
                                                }
                                            }
                                            break;
                                }
                            }
                        }

                        if (vis_film_oversigt) {
                            switch (screen_size) {					// filmoversigt_antal
                                case 1: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                                          if (film_select_iconnr+numbers_film_covers_on_line>11) {		// skal vi scroll liste up
                                            do_movie_icon_anim_icon_ofset = 1;
                                            _fangley += MOVIE_CS;
                                            //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                                          }
                                          film_key_selected+=numbers_film_covers_on_line;
                                          film_select_iconnr+=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 2: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                                          if (film_select_iconnr+numbers_film_covers_on_line>13) {		// skal vi scroll liste up
                                            do_movie_icon_anim_icon_ofset = 1;
                                            _fangley += MOVIE_CS;
                                            //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                                          }
                                          film_key_selected+=numbers_film_covers_on_line;
                                          film_select_iconnr+=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                                          if (film_select_iconnr+numbers_film_covers_on_line>19) {		// skal vi scroll liste up
                                            do_movie_icon_anim_icon_ofset= 1 ;
                                            _fangley += MOVIE_CS;
                                            //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                                          }
                                          film_key_selected+=numbers_film_covers_on_line;
                                          film_select_iconnr+=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 4: if (((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) && (movie_icon_anim_icon_ofset==0)) {
                                          if (film_select_iconnr+numbers_film_covers_on_line>17) {		// skal vi scroll liste up
                                            do_movie_icon_anim_icon_ofset= 1 ;
                                            _fangley += MOVIE_CS;
                                            //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
                                          }
                                          film_key_selected+=numbers_film_covers_on_line;
                                          film_select_iconnr+=numbers_film_covers_on_line;
                                        }
                                        break;
                                }
                        }


                        if ((vis_radio_oversigt) && (show_radio_options==false)) {
                            if (((int) radio_select_iconnr<(int) radiomoversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                              if (radio_select_iconnr+numbers_radio_covers_on_line>16) {		// skal vi scroll liste up
                                do_radio_icon_anim_icon_ofset= 1 ;
                                _rangley += RADIO_CS;
                                //radio_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                if (radio_key_selected>0) radio_key_selected-=numbers_radio_covers_on_line;
                              }
                              radio_key_selected+=numbers_radio_covers_on_line;
                              radio_select_iconnr+=numbers_radio_covers_on_line;
                            }

/*
                            switch (screen_size) {
                                case 1: if ((radio_select_iconnr<radiooversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                                            if (radio_select_iconnr+numbers_radio_covers_on_line>11) {		// skal vi scroll liste up
                                                do_radio_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //radio_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (radio_key_selected>0) radio_key_selected-=numbers_radio_covers_on_line;
                                            }
                                            radio_key_selected+=numbers_radio_covers_on_line;
                                            radio_select_iconnr+=numbers_radio_covers_on_line;
                                        }
                                        break;
                                case 2: if ((radio_select_iconnr<radiooversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                                            if (radio_select_iconnr+numbers_radio_covers_on_line>13) {		// skal vi scroll liste up
                                                do_radio_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //radio_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (radio_key_selected>0) radio_key_selected-=numbers_radio_covers_on_line;
                                            }
                                            radio_key_selected+=numbers_radio_covers_on_line;
                                            radio_select_iconnr+=numbers_radio_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if (((int) radio_select_iconnr<(int) radiomoversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                                            if (radio_select_iconnr+numbers_radio_covers_on_line>19) {		// skal vi scroll liste up
                                                do_radio_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //radio_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (radio_key_selected>0) radio_key_selected-=numbers_radio_covers_on_line;
                                            }
                                            radio_key_selected+=numbers_radio_covers_on_line;
                                            radio_select_iconnr+=numbers_radio_covers_on_line;
                                        }
                                        break;
                                case 4: if ((radio_select_iconnr<radiooversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                                            if (radio_select_iconnr+numbers_radio_covers_on_line>17) {		// skal vi scroll liste up
                                                do_radio_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //radio_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (radio_key_selected>0) radio_key_selected-=numbers_radio_covers_on_line;
                                            }
                                            radio_key_selected+=numbers_radio_covers_on_line;
                                            radio_select_iconnr+=numbers_radio_covers_on_line;
                                        }
                                        break;
                                }
*/

                        }
                        // bruges ved radio_type_oversigt
                        if ((vis_radio_oversigt) && (show_radio_options)) {
                            radiooversigt.nextradiooptselect();
                        }

                        // stream stuf
                        if (vis_stream_oversigt) {
                            switch (screen_size) {
                                case 1: if ((stream_select_iconnr<(int ) streamoversigt.streamantal()-5) && (stream_icon_anim_icon_ofset==0)) {
                                          if (stream_select_iconnr+numbers_stream_covers_on_line>11) {		// skal vi scroll liste up
                                            do_stream_icon_anim_icon_ofset = 1;
                                            _rangley += RADIO_CS;
                                            //stream_select_iconnr-=numbers_stream_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                          }
                                          stream_key_selected+=numbers_stream_covers_on_line;
                                          stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 2: if ((stream_select_iconnr<(int ) streamoversigt.streamantal()-5) && (stream_icon_anim_icon_ofset==0)) {
                                          if (stream_select_iconnr+numbers_stream_covers_on_line>13) {		// skal vi scroll liste up
                                            do_stream_icon_anim_icon_ofset = 1;
                                            _rangley += RADIO_CS;
                                            //stream_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                          }
                                          stream_key_selected+=numbers_stream_covers_on_line;
                                          stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if (((int) stream_select_iconnr<(int) streamoversigt.streamantal()-5) && (stream_icon_anim_icon_ofset==0)) {
                                          if (stream_select_iconnr+numbers_stream_covers_on_line>19) {		// skal vi scroll liste up
                                              do_stream_icon_anim_icon_ofset = 1;
                                              _rangley += RADIO_CS;
                                              //stream select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                              if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                          }
                                          stream_key_selected+=numbers_stream_covers_on_line;
                                          stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 4: if ((stream_select_iconnr<(int) streamoversigt.streamantal()-5) && (stream_icon_anim_icon_ofset==0)) {
                                          if (stream_select_iconnr+numbers_stream_covers_on_line>17) {		// skal vi scroll liste up
                                            do_stream_icon_anim_icon_ofset = 1;
                                            _rangley += RADIO_CS;
                                            //stream_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                            if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                          }
                                          stream_key_selected += numbers_stream_covers_on_line;
                                          stream_select_iconnr += numbers_stream_covers_on_line;
                                        }
                                        break;
                                }
                        }
                        if (vis_recorded_oversigt) {
                            if (visvalgtnrtype==1) {
                                if ((int) valgtrecordnr<(int) recordoversigt.top_antal()) {
                                    valgtrecordnr++;
                                    subvalgtrecordnr=0;
                                }
                            } else if (visvalgtnrtype==2) {
                                if ((int) subvalgtrecordnr<(int) recordoversigt.programs_type_antal(valgtrecordnr)-1) {
                                    subvalgtrecordnr++;
                                }
                            }
                            reset_recorded_texture = true;
                        }
                        if (vis_tv_oversigt) {
                            if (tvvisvalgtnrtype==1) {
                                if (tvvalgtrecordnr<aktiv_tv_oversigt.tv_kanal_antal()) {
                                    tvvalgtrecordnr++;
                                    tvsubvalgtrecordnr = 0;
                                }
                            } else if (tvvisvalgtnrtype==2) {
                                if (tvsubvalgtrecordnr<aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr)) {
                                    tvsubvalgtrecordnr++;
                                }
                            }
                        }
                    }
                    // lirc move up
                    if (strcmp(c,"Key-nav-up")==0) {
                        if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                          if (do_show_play_open_select_line>0) do_show_play_open_select_line--; else
                            if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
                        }
                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                            switch (screen_size) {
                                case 1:
                                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1)) ) {
                                          if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1))) {
                                             _mangley -= 41;
                                             music_select_iconnr -= numbers_cd_covers_on_line;
                                             do_music_icon_anim_icon_ofset = -1;
                                          } else music_select_iconnr-=numbers_cd_covers_on_line;
                                          if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                                            music_key_selected-=numbers_cd_covers_on_line;
                                          }
                                        }
                                        break;
                                case 2:
                                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1)) ) {
                                          if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1))) {
                                             _mangley -= 41;
                                             music_select_iconnr -= numbers_cd_covers_on_line;
                                             do_music_icon_anim_icon_ofset = -1;
                                          } else music_select_iconnr-=numbers_cd_covers_on_line;
                                          if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                                            music_key_selected-=numbers_cd_covers_on_line;
                                          }
                                        }
                                        break;
                                case 3:
                                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1)) ) {
                                          if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1))) {
                                           _mangley -= 41;
                                           music_select_iconnr -= numbers_cd_covers_on_line;
                                           do_music_icon_anim_icon_ofset = -1;
                                          } else music_select_iconnr-=numbers_cd_covers_on_line;
                                          if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                                            music_key_selected-=numbers_cd_covers_on_line;
                                          }
                                        }
                                        break;
                                case 4:
                                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1)) ) {
                                          if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>(numbers_cd_covers_on_line-1))) {
                                             _mangley -= 41;
                                             music_select_iconnr -= numbers_cd_covers_on_line;
                                             do_music_icon_anim_icon_ofset = -1;
                                          } else music_select_iconnr-=numbers_cd_covers_on_line;
                                          if (music_key_selected>numbers_cd_covers_on_line) {
                                            music_key_selected-=numbers_cd_covers_on_line;
                                          }
                                        }
                                        break;
                            }
                        }

                        if (vis_film_oversigt) {
                            switch(screen_size) {
                                case 1:
                                        if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                                          if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                                             _fangley -= MOVIE_CS;
                                             film_select_iconnr -= numbers_film_covers_on_line;
                                          } else film_select_iconnr-=numbers_film_covers_on_line;
                                          if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 2:
                                        if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                                          if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                                             _fangley -= MOVIE_CS;
                                             film_select_iconnr -= numbers_film_covers_on_line;
                                          } else film_select_iconnr-=numbers_film_covers_on_line;
                                          if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                                          if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                                             _fangley -= MOVIE_CS;
                                             film_select_iconnr -= numbers_film_covers_on_line;
                                          } else film_select_iconnr -= numbers_film_covers_on_line;
                                          if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                                        }
                                        break;
                                case 4:
                                        if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
                                          if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                                             _fangley -= MOVIE_CS;
                                             film_select_iconnr -= numbers_film_covers_on_line;
                                          } else film_select_iconnr -= numbers_film_covers_on_line;
                                          if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
                                        }
                                        break;
                            }
                        }

                        if ((vis_radio_oversigt) && (show_radio_options==false)) {
                          if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                            if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                              _rangley -= MOVIE_CS;
                              radio_select_iconnr -= numbers_radio_covers_on_line;
                            } else radio_select_iconnr-=numbers_radio_covers_on_line;
                            if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                          }
/*
                            switch(screen_size) {
                                case 1:
                                        if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                                            if (((int) radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 radio_select_iconnr-=numbers_radio_covers_on_line;
                                            } else radio_select_iconnr-=numbers_radio_covers_on_line;
                                            if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                                        }

                                        break;
                                case 2:
                                        if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                                            if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 radio_select_iconnr-=numbers_radio_covers_on_line;
                                            } else radio_select_iconnr-=numbers_radio_covers_on_line;
                                            if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                                        }
                                        break;
                                case 3:

                                        if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                                            if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 radio_select_iconnr-=numbers_radio_covers_on_line;
                                            } else radio_select_iconnr-=numbers_radio_covers_on_line;
                                            if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                                        }
                                        break;
                                case 4:
                                        if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
                                            if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 radio_select_iconnr-=numbers_radio_covers_on_line;
                                            } else radio_select_iconnr-=numbers_radio_covers_on_line;
                                            if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
                                        }
                                        break;
                            }
*/
                        }

                        if ((vis_radio_oversigt) && (show_radio_options)) {
                            radiooversigt.lastradiooptselect();
                        }



                        // stream
                        if ((vis_stream_oversigt) && (show_stream_options==false)) {
                            switch(screen_size) {
                                case 1:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                          if (((int) stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                             _rangley -= MOVIE_CS;
                                             stream_select_iconnr -= numbers_stream_covers_on_line;
                                          } else stream_select_iconnr -= numbers_stream_covers_on_line;
                                          if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }

                                        break;
                                case 2:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                          if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                             _rangley -= MOVIE_CS;
                                             stream_select_iconnr -= numbers_stream_covers_on_line;
                                          } else stream_select_iconnr -= numbers_stream_covers_on_line;
                                          if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                          if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                             _rangley -= MOVIE_CS;
                                             stream_select_iconnr -= numbers_stream_covers_on_line;
                                          } else stream_select_iconnr -= numbers_stream_covers_on_line;
                                          if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 4:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                          if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                             _rangley -= MOVIE_CS;
                                             stream_select_iconnr -= numbers_stream_covers_on_line;
                                          } else stream_select_iconnr -= numbers_stream_covers_on_line;
                                          if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                            }
                        }



                        if (vis_recorded_oversigt) {
                          if ((visvalgtnrtype==1) && (valgtrecordnr>0)) {
                            valgtrecordnr--;
                            subvalgtrecordnr = 0;
                          } else if ((visvalgtnrtype==2) && (subvalgtrecordnr>0)) subvalgtrecordnr--;	// bruges til visning af optaget programmer
                          reset_recorded_texture = true;		// load optaget programs texture gen by mythtv
                        }
                        if (vis_tv_oversigt) {
                          if ((tvvisvalgtnrtype==1) && (tvvalgtrecordnr>0)) {
                            tvvalgtrecordnr--;
                            tvsubvalgtrecordnr=0;
                          } else if ((tvvisvalgtnrtype==2) && (tvsubvalgtrecordnr>0)) tvsubvalgtrecordnr--;	// bruges til visning af optaget programmer
                        }
                        if (do_show_setup) {
                         if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                        }
                    }
                    // lirc move left
                    if (strcmp(c,"Key-nav-left")==0) {
                        if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                          dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
                        }
                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                          if (music_key_selected>1) {
                            if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                              if (music_key_selected>1) {
                                music_key_selected--;
                                music_select_iconnr--;
                              } else {
                                if (music_select_iconnr>0) {
                                   _mangley -= 41;
                                   music_key_selected += numbers_cd_covers_on_line-1;  // den viste på skærm af 1 til 20
                                   music_select_iconnr--;                  	// den rigtige valgte af 1 til cd antal
                                }
                              }
                            }
                          }
                        }
                        if (vis_film_oversigt) {							// ved film oversigt
                            if (film_key_selected>1) {
                                switch (screen_size) {
                                    case 1: if (film_key_selected % 12==0) {
                                                if (film_select_iconnr>0) {
                                                  film_key_selected--;			// den som er ramme om
                                                  film_select_iconnr--;
                                                } else {
                                                  if ((_fangley-MOVIE_CS)<0) {
                                                    _fangley -= MOVIE_CS;
                                                    film_key_selected--;
                                                    film_select_iconnr = -3;
                                                    do_movie_icon_anim_icon_ofset = -1;
                                                  }
                                                }
                                            } else {
                                              if ((film_select_iconnr==1) && (_fangley>0)) {
                                                _fangley -= MOVIE_CS;
                                                film_select_iconnr += 4;
                                                do_movie_icon_anim_icon_ofset = -1;
                                              }
                                              film_key_selected--;
                                              film_select_iconnr--;
                                            }
                                            break;
                                    case 2: if (film_key_selected % 24==0) {
                                                if (film_select_iconnr>0) {
                                                  film_key_selected--;
                                                  film_select_iconnr--;
                                                } else {
                                                  if (_fangley-MOVIE_CS<0) {
                                                     _fangley -= MOVIE_CS;
                                                    film_key_selected--;
                                                    film_select_iconnr = -5;
                                                    do_movie_icon_anim_icon_ofset = -1;
                                                  }
                                                }
                                            } else {
                                              if ((film_select_iconnr==1) && (_fangley>0)) {
                                                _fangley-=MOVIE_CS;
                                                film_select_iconnr+=6;
                                                do_movie_icon_anim_icon_ofset=-1;
                                              }
                                              film_key_selected--;
                                              film_select_iconnr--;
                                            }
                                            break;
                                    case 3: if (film_key_selected % 36==0) {
                                              if (film_select_iconnr>0) {
                                                film_key_selected--;
                                                film_select_iconnr--;
                                              } else {
                                                if (_fangley-MOVIE_CS<0) {
                                                  _fangley -= MOVIE_CS;
                                                  film_key_selected--;
                                                  film_select_iconnr = -8;
                                                  do_movie_icon_anim_icon_ofset = -1;
                                                }
                                              }
                                            } else {
                                              if ((film_select_iconnr==1) && (_fangley>0)) {
                                                _fangley -= MOVIE_CS;
                                                film_select_iconnr += 9;
                                                do_movie_icon_anim_icon_ofset = -1;
                                              }
                                              film_key_selected--;
                                              film_select_iconnr--;
                                            }
                                            break;
                                    case 4: if (film_key_selected % 36==0) {
                                                if (film_select_iconnr>0) {
                                                  film_key_selected--;
                                                  film_select_iconnr--;
                                                } else {
                                                  if (_fangley-MOVIE_CS<0) {
                                                    _fangley -= MOVIE_CS;
                                                    film_key_selected--;
                                                    film_select_iconnr = -8;
                                                    do_movie_icon_anim_icon_ofset = -1;
                                                  }
                                                }
                                            } else {
                                              if ((film_select_iconnr==1) && (_fangley>0)) {
                                                _fangley -= MOVIE_CS;
                                                film_select_iconnr += 9;
                                                do_movie_icon_anim_icon_ofset = -1;
                                              }
                                              film_key_selected--;
                                              film_select_iconnr--;
                                            }
                                            break;
                                }
                            }
                        }
                        if (vis_radio_oversigt) {							// ved film oversigt
                            if (radio_key_selected>1) {
                                switch (screen_size) {
                                    case 1: if (radio_key_selected % 12==0) {
                                                if (radio_select_iconnr>0) {
                                                  radio_key_selected--;			// den som er ramme om
                                                  radio_select_iconnr--;
                                                } else {
                                                    if ((_rangley-MOVIE_CS)<0) {
                                                      _rangley -= MOVIE_CS;
                                                      radio_key_selected--;
                                                      radio_select_iconnr = -3;
                                                      do_radio_icon_anim_icon_ofset = -1;
                                                    }
                                                }
                                            } else {
                                                if ((radio_select_iconnr==1) && (_rangley>0)) {
                                                  _rangley -= MOVIE_CS;
                                                  radio_select_iconnr += 4;
                                                  do_radio_icon_anim_icon_ofset = -1;
                                                }
                                                radio_key_selected--;
                                                radio_select_iconnr--;
                                            }
                                            break;
                                    case 2: if (radio_key_selected % 24==0) {
                                              if (radio_select_iconnr>0) {
                                                radio_key_selected--;
                                                radio_select_iconnr--;
                                              } else {
                                                if (_rangley-MOVIE_CS<0) {
                                                   _rangley -= MOVIE_CS;
                                                  radio_key_selected--;
                                                  radio_select_iconnr = -5;
                                                  do_radio_icon_anim_icon_ofset = -1;
                                                }
                                              }
                                            } else {
                                              if ((radio_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                radio_select_iconnr += 6;
                                                do_radio_icon_anim_icon_ofset = -1;
                                              }
                                              radio_key_selected--;
                                              radio_select_iconnr--;
                                            }
                                            break;
                                    case 3: if (radio_key_selected % 36==0) {
                                                if (radio_select_iconnr>0) {
                                                  radio_key_selected--;
                                                  radio_select_iconnr--;
                                                } else {
                                                  if (_rangley-MOVIE_CS<0) {
                                                    _rangley-=MOVIE_CS;
                                                    radio_key_selected--;
                                                    radio_select_iconnr = -8;
                                                    do_radio_icon_anim_icon_ofset=-1;
                                                  }
                                                }
                                            } else {
                                              if ((radio_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                radio_select_iconnr += 9;
                                                do_radio_icon_anim_icon_ofset = -1;
                                              }
                                              radio_key_selected--;
                                              radio_select_iconnr--;
                                            }
                                            break;
                                    case 4: if (radio_key_selected % 36==0) {
                                              if (radio_select_iconnr>0) {
                                                radio_key_selected--;
                                                radio_select_iconnr--;
                                              } else {
                                                if (_rangley-MOVIE_CS<0) {
                                                  _rangley-=MOVIE_CS;
                                                  radio_key_selected--;
                                                  radio_select_iconnr=-8;
                                                  do_radio_icon_anim_icon_ofset=-1;
                                                }
                                              }
                                            } else {
                                              if ((radio_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                radio_select_iconnr += 9;
                                                do_radio_icon_anim_icon_ofset = -1;
                                              }
                                              radio_key_selected--;
                                              radio_select_iconnr--;
                                            }
                                            break;
                                }
                            }
                        }

                        // stream
                        if (vis_stream_oversigt) {							// ved film oversigt
                            if (stream_key_selected>1) {
                                switch (screen_size) {
                                    case 1: if (stream_key_selected % 12==0) {
                                              if (stream_select_iconnr>0) {
                                                stream_key_selected--;			// den som er ramme om
                                                stream_select_iconnr--;
                                              } else {
                                                if ((_rangley-MOVIE_CS)<0) {
                                                  _rangley-=MOVIE_CS;
                                                  stream_key_selected--;
                                                  stream_select_iconnr = -3;
                                                  do_stream_icon_anim_icon_ofset = -1;
                                                }
                                              }
                                            } else {
                                              if ((stream_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                stream_select_iconnr += 4;
                                                do_stream_icon_anim_icon_ofset = -1;
                                              }
                                              stream_key_selected--;
                                              stream_select_iconnr--;
                                            }
                                            break;
                                    case 2: if (stream_key_selected % 24==0) {
                                              if (stream_select_iconnr>0) {
                                                stream_key_selected--;
                                                stream_select_iconnr--;
                                              } else {
                                                if (_rangley-MOVIE_CS<0) {
                                                   _rangley -= MOVIE_CS;
                                                  stream_key_selected--;
                                                  stream_select_iconnr = -5;
                                                  do_stream_icon_anim_icon_ofset = -1;
                                                }
                                              }
                                            } else {
                                              if ((stream_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                stream_select_iconnr += 6;
                                                do_stream_icon_anim_icon_ofset =- 1;
                                              }
                                              stream_key_selected--;
                                              stream_select_iconnr--;
                                            }
                                            break;
                                    case 3: if (stream_key_selected % 36==0) {
                                              if (stream_select_iconnr>0) {
                                                stream_key_selected--;
                                                stream_select_iconnr--;
                                              } else {
                                                if (_rangley-MOVIE_CS<0) {
                                                  _rangley -= MOVIE_CS;
                                                  stream_key_selected--;
                                                  stream_select_iconnr = -8;
                                                  do_stream_icon_anim_icon_ofset=-1;
                                                }
                                              }
                                            } else {
                                              if ((stream_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                stream_select_iconnr += 9;
                                                do_stream_icon_anim_icon_ofset = -1;
                                              }
                                              stream_key_selected--;
                                              stream_select_iconnr--;
                                            }
                                            break;
                                    case 4: if (stream_key_selected % 36==0) {
                                              if (stream_select_iconnr>0) {
                                                stream_key_selected--;
                                                stream_select_iconnr--;
                                              } else {
                                                if (_rangley-MOVIE_CS<0) {
                                                  _rangley -= MOVIE_CS;
                                                  stream_key_selected--;
                                                  stream_select_iconnr = -8;
                                                  do_stream_icon_anim_icon_ofset = -1;
                                                }
                                              }
                                            } else {
                                              if ((stream_select_iconnr==1) && (_rangley>0)) {
                                                _rangley -= MOVIE_CS;
                                                stream_select_iconnr += 9;
                                                do_stream_icon_anim_icon_ofset = -1;
                                              }
                                              stream_key_selected--;
                                              stream_select_iconnr--;
                                            }
                                            break;
                                }
                            }
                        }
                        if (vis_recorded_oversigt) {
                          visvalgtnrtype=1;
                        }
                        if (vis_tv_oversigt) {
                          tvvisvalgtnrtype=1;
                        }
                    }
                    // lirc move right
                    if (strcmp(c,"Key-nav-right")==0) {
                        if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                          dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
                        }
                        if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                            if (((int) music_key_selected<(int) musicoversigt_antal) && (music_icon_anim_icon_ofset==0)) {
                              if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr<musicoversigt_antal)) {
                                if (((int) music_key_selected % (numbers_cd_covers_on_line*4)==0) || (((int) music_select_iconnr==((numbers_cd_covers_on_line*4)-1)) && ((int) music_key_selected % numbers_cd_covers_on_line==0))) {
                                  _mangley+=41;
                                  music_key_selected -= numbers_cd_covers_on_line;			// den viste på skærm af 1 til 20
                                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                } else {
                                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                }
                                music_key_selected++;
                              }
                            }
                        }
                        if (vis_film_oversigt) {
                            switch (screen_size) {
                                case 1:
                                        if ((vis_film_oversigt) && ((int unsigned) film_select_iconnr<film_oversigt.film_antal()-1)) {
                                          if ((int) film_select_iconnr<(int) film_oversigt.film_antal()) {
                                            if (((int) film_key_selected % (numbers_film_covers_on_line*3)==0) || (((int) film_select_iconnr==((numbers_film_covers_on_line*4)-1)) && ((int) film_key_selected % numbers_film_covers_on_line==0))) {
                                              _fangley += MOVIE_CS;
                                              film_key_selected -= numbers_film_covers_on_line;			// den viste på skærm af 1 til 20
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            } else {
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            }
                                            film_key_selected++;
                                          }
                                        }
                                        break;
                                case 2:
                                        if ((vis_film_oversigt) && (film_select_iconnr<(int) film_oversigt.film_antal()-1)) {
                                          if ((int) film_select_iconnr<(int) film_oversigt.film_antal()) {
                                            if (((int) film_key_selected % (numbers_film_covers_on_line*3)==0) || (((int) film_select_iconnr==((numbers_film_covers_on_line*4)-1)) && ((int) film_key_selected % numbers_film_covers_on_line==0))) {
                                              _fangley += MOVIE_CS;
                                              film_key_selected -= numbers_film_covers_on_line;			// den viste på skærm af 1 til 20
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            } else {
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            }
                                            film_key_selected++;
                                          }
                                        }
                                        break;

                                case 3:
                                        if ((vis_film_oversigt) && (film_select_iconnr<(int) film_oversigt.film_antal()-1)) {
                                          if (film_select_iconnr<(int) film_oversigt.film_antal()) {
                                            if (((int) film_key_selected % (numbers_film_covers_on_line*3)==0) || (((int) film_select_iconnr==((numbers_film_covers_on_line*4)-1)) && ((int) film_key_selected % numbers_film_covers_on_line==0))) {
                                              _fangley+=MOVIE_CS;
                                              film_key_selected-=numbers_film_covers_on_line;			// den viste på skærm af 1 til 20
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            } else {
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            }
                                            film_key_selected++;
                                          }
                                        }

                                        break;
                                case 4:
                                        if ((vis_film_oversigt) && (film_select_iconnr<(int) film_oversigt.film_antal()-1)) {
                                          if (film_select_iconnr<(int ) film_oversigt.film_antal()) {
                                            if (((int) film_key_selected % (numbers_film_covers_on_line*3)==0) || (((int) film_select_iconnr==((numbers_film_covers_on_line*4)-1)) && ((int) film_key_selected % numbers_film_covers_on_line==0))) {
                                              _fangley+=MOVIE_CS;
                                              film_key_selected-=numbers_film_covers_on_line;			// den viste på skærm af 1 til 20
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            } else {
                                              film_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            }
                                            film_key_selected++;
                                          }
                                        }
                                        break;
                            }
                        }

                        if (vis_radio_oversigt) {
                            switch (screen_size) {
                                default:
                                        if ((vis_radio_oversigt) && (radio_select_iconnr<(int) radiooversigt_antal-1)) {
                                          if (radio_select_iconnr<(int) radiooversigt_antal) {
                                            if (((int) radio_key_selected % (numbers_radio_covers_on_line*3)==0) || (((int) radio_select_iconnr==((numbers_radio_covers_on_line*4)-1)) && ((int) radio_key_selected % numbers_radio_covers_on_line==0))) {
                                              _rangley += MOVIE_CS;
                                              radio_key_selected -= numbers_radio_covers_on_line;			// den viste på skærm af 1 til 20
                                              radio_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            } else {
                                              radio_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                            }
                                            radio_key_selected++;
                                          }
                                        }
                                        break;
                            }
                        }

                        // stream
                        if (vis_stream_oversigt) {
                          switch (screen_size) {
                            case 1:
                              if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt.streamantal()-1)) {
                                if (stream_select_iconnr<(int) streamoversigt.streamantal()) {
                                  if (((int) stream_key_selected % (numbers_stream_covers_on_line*3)==0) || (((int) stream_select_iconnr==((numbers_stream_covers_on_line*4)-1)) && ((int) stream_key_selected % numbers_stream_covers_on_line==0))) {
                                    _rangley+=MOVIE_CS;
                                    stream_key_selected-=numbers_stream_covers_on_line;			// den viste på skærm af 1 til 20
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  } else {
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  }
                                  stream_key_selected++;
                                }
                              }
                              break;
                            case 2:
                              if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt.streamantal()-1)) {
                                if (stream_select_iconnr<(int) streamoversigt.streamantal()) {
                                  if (((int) stream_key_selected % (numbers_stream_covers_on_line*3)==0) || (((int) stream_select_iconnr==((numbers_stream_covers_on_line*4)-1)) && ((int) stream_key_selected % numbers_stream_covers_on_line==0))) {
                                    _rangley+=MOVIE_CS;
                                    stream_key_selected-=numbers_stream_covers_on_line;			// den viste på skærm af 1 til 20
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  } else {
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  }
                                  stream_key_selected++;
                                }
                              }
                              break;
                            case 3:
                              if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt.streamantal()-1)) {
                                if (stream_select_iconnr<(int) streamoversigt.streamantal()) {
                                  if (((int) stream_key_selected % (numbers_stream_covers_on_line*3)==0) || (((int) stream_select_iconnr==((numbers_stream_covers_on_line*4)-1)) && ((int) stream_key_selected % numbers_stream_covers_on_line==0))) {
                                    _rangley += MOVIE_CS;
                                    stream_key_selected -= numbers_stream_covers_on_line;			// den viste på skærm af 1 til 20
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  } else {
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  }
                                  stream_key_selected++;
                                }
                              }
                              break;
                            case 4:
                              if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt.streamantal()-1)) {
                                if (stream_select_iconnr<(int) streamoversigt.streamantal()) {
                                  if (((int) stream_key_selected % (numbers_stream_covers_on_line*3)==0) || (((int) stream_select_iconnr==((numbers_stream_covers_on_line*4)-1)) && ((int) stream_key_selected % numbers_stream_covers_on_line==0))) {
                                    _rangley+=MOVIE_CS;
                                    stream_key_selected-=numbers_stream_covers_on_line;			// den viste på skærm af 1 til 20
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  } else {
                                    stream_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                  }
                                  stream_key_selected++;
                                }
                              }
                              break;
                            }
                        }
                        if (vis_recorded_oversigt) {
                          visvalgtnrtype = 2;
                        }
                        if (vis_tv_oversigt) {
                          tvvisvalgtnrtype = 2;
                        }
                    }

                    // lirc back button
                    if (strcmp(c,"Back")==0) {
                      if (vis_music_oversigt) {
                       if (ask_open_dir_or_play) {
                         ask_open_dir_or_play=false;
                         do_zoom_film_cover=false;
                       } else do_zoom_music_cover=!do_zoom_music_cover;		// show/hide music info
                      }
                      if (do_zoom_film_cover) {
                        do_zoom_film_cover=false;
                        fknapnr=0;
                        mknapnr=0;					// reset knapnr i alt
                      }
                    }
                    if (strcmp("Next",c)==0) {
                      // lirc
                      // next song
                      if ((vis_music_oversigt) && (!(do_shift_song))) {
                        if ((do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) && (do_shift_song==false)) {
                          do_play_music_aktiv_table_nr++;
                          do_shift_song = true;
                          do_zoom_music_cover = true;
                        }
                      }
                      if (vis_tv_oversigt) {
                        aktiv_tv_oversigt.changetime(60*60*24);
                        aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                      }
                    }
                    // lirc
                    if (strcmp("Prev",c)==0) {
                      // lirc
                      // last song
                       if ((vis_music_oversigt) && (!(do_shift_song))) {
                          if ((do_play_music_aktiv_table_nr>1) && (do_shift_song==false)) {
                            do_play_music_aktiv_table_nr--;						// skift aktiv sang
                            do_shift_song = true;							// sæt flag til skift
                            do_zoom_music_cover = true;
                          }
                       }
                       if (vis_tv_oversigt) {
                        aktiv_tv_oversigt.changetime(-(60*60*24));
                        aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);
                      }
                    }
                    // lirc
                    if (strcmp("Key-vol-up",c)==0) {
                      if (configsoundvolume<1.0f) configsoundvolume+=0.1f;
                      #if defined USE_FMOD_MIXER
                      if (sndsystem) channel->setVolume(configsoundvolume);
                      #endif
                      show_volume_info = true;					// show volume info window
                      vis_volume_timeout = 80;
                    }
                    // lirc
                    if (strcmp("Key-vol-down",c)==0) {
                      if (configsoundvolume>0) configsoundvolume-=0.1f;
                      #if defined USE_FMOD_MIXER
                      if (sndsystem) channel->setVolume(configsoundvolume);
                      #endif
                      show_volume_info = true;					// show volume info window
                      vis_volume_timeout = 80;
                    }
                    // lirc
                    // start some func
                    if (strcmp("Return",c)==0) {
                      if ((vis_music_oversigt) && (!(do_zoom_music_cover))) {
                        if (ask_open_dir_or_play) {
                          ask_open_dir_or_play = 0;			// flag luk vindue igen
                          do_play_music_cover = 1;			// der er trykket på cover play det
                          do_zoom_music_cover = false;			// ja den skal spilles lav zoom cover info window
                          do_find_playlist = true;			// find de sange som skal indsættes til playlist (og load playlist andet sted)
                          mknapnr = music_select_iconnr;		// OLD VER music_key_selected;
                          if (debugmode & 2) fprintf(stderr,"music_key_selected =%d \n",music_key_selected);
                        } else {
                          // er det et normal dir
                          mknapnr = music_select_iconnr; // OLD VER music_key_selected-1;
                          if (debugmode & 2) fprintf(stderr,"Lirc music selected:%d \n",mknapnr);

                          // if (debugmode & 2) printf("Mouse pressed over %d husk knap= %d \n",mknapnr,husk_knapnr);
                          // ja hvis felts oversigttype=0
                          if (musicoversigt.get_album_type(mknapnr)==0) {
                            //                            mknapnr=music_key_selected;
                            do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr);		// set den aktive mappe dir id
                            antal_songs=hent_antal_dir_songs(musicoversigt.get_directory_id(mknapnr));		//
                            if (debugmode & 2) fprintf(stderr,"Found numers of songs in :%4d dirid:%4d  named:%s \n",antal_songs,musicoversigt.get_directory_id(mknapnr),musicoversigt.get_album_name(mknapnr));
                            if ((antal_songs==0) || (musicoversigt.get_directory_id(mknapnr)==0)) {	// er der ingen sange i dir lav en auto open
                              ask_open_dir_or_play_aopen = 1; 						// flag auto open
                              if (musicoversigt.get_directory_id(mknapnr)==0) antal_songs=0;
                            } else {
                              ask_open_dir_or_play_aopen = 0;						// ingen auto open
                            }
                          } else {
                          // nej det er playlister
                          mknapnr = music_select_iconnr;
                          do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr);			// = playlistnr
                          if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                          if (do_play_music_aktiv_nr>0) {
                            antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                          } else antal_songs = 0;
                          if (antal_songs==0) {					                    		// er der ingen sange i dir lav en auto open
                            ask_open_dir_or_play_aopen = 1; 						        // flag auto open
                          } else {
                            ask_open_dir_or_play_aopen = 0;						          // ingen auto open
                          }
                        }
                        // do auto open
                        if ((antal_songs==0) && (musicoversigt.get_album_type(mknapnr)==0)) {
                          // normalt dir (IKKE playlist)

                         musicoversigt.opdatere_music_oversigt(musicoversigt.get_directory_id(mknapnr-1));
                         musicoversigt.opdatere_music_oversigt_icons();                                  // load icons

                          //opdatere_music_oversigt(musicoversigt,musicoversigt[mknapnr].directory_id);
                          //opdatere_music_oversigt_icons();                      // load icons
                          music_icon_anim_icon_ofset = 0;
                          music_icon_anim_icon_ofsety = 0;
                          mknapnr = 0;
                          ask_open_dir_or_play_aopen = 1;
                          if (debugmode & 2) fprintf(stderr,"Set Autoopen on dir id %d \n ",musicoversigt.get_directory_id(mknapnr));
                          music_key_selected = 1;		// reset cursor position
                          music_select_iconnr = 1;		// reset this to (beskriver hvor vi er på skærmen og ikke på listen som music_key_selected
                        } else if (musicoversigt.get_album_type(mknapnr)==-1) {
                          // playliste
                          if (antal_songs>0) ask_open_dir_or_play_aopen=0;			// skal vi spørge slå auto fra
                          else ask_open_dir_or_play_aopen=1;					// else ja autoopen
                          // hent liste over mythtv playlist

                          // new ver
                         musicoversigt.opdatere_music_oversigt_playlists();

                          // old ver
                          //opdatere_music_oversigt_playlists(musicoversigt);			// vis alle playlister
                        }
                        if ((antal_songs>0) && (do_play_music_aktiv_nr)) {
                          do_swing_music_cover = 1;
                          ask_open_dir_or_play = 1;
                        }
                        if (ask_open_dir_or_play_aopen) {
                          /// reset mouse/key pos in vis_music_oversigt
                          mknapnr = 0;
                          music_key_selected = 1;
                          music_select_iconnr = 0;
                          music_icon_anim_icon_ofset = 0;
                          music_icon_anim_icon_ofsety = 0;
                          _mangley = 0.0f;
                        }
                      }
                      do_zoom_music_cover_remove_timeout=showtimeout;		// show music info timeout
                      // write debug log
                      write_logfile((char *) "lirc Set Play it flag");
                    }
                    if ((vis_film_oversigt) && (do_zoom_film_cover)) {
                      fprintf(stderr,"lirc Start movie player.\n");
                      startmovie = true;
                    }
                    if (vis_film_oversigt) {				// select movie to show info for
                      do_zoom_film_cover = true;
                      do_swing_movie_cover=0;
                      do_zoom_film_aktiv_nr=film_select_iconnr+1;		// OLD film_key_selected;
                      fknapnr=film_select_iconnr+1;				// OLD film_key_selected;
                    }
                    if (vis_recorded_oversigt) {
                      // play record/rss fil
                      do_play_recorded_aktiv_nr=1;
                    }
                    if (vis_tv_oversigt) {
                      tvknapnr=tvsubvalgtrecordnr;
                      do_zoom_tvprg_aktiv_nr=tvknapnr;
                      ask_tv_record=!ask_tv_record;
                      if (ask_tv_record) fprintf(stderr,"lirc Show tvprogram info.\n"); else fprintf(stderr,"lirc Hide tvprogram info.\n");
                    }
                    // start play radio
                    if ((vis_radio_oversigt) && (!(show_radio_options))) {
                      // play radio station
                      rknapnr=radio_key_selected;		// hent button
                      if (rknapnr>0) do_play_radio=1;
                    }
                    // opdatere radio oversigt efter efter pressed on the remorte control from lirc
                    if ((vis_radio_oversigt) && (show_radio_options)) {
                      // write debug log
                      write_logfile((char *) "Update radio overview.");
                      radiooversigt.clean_radio_oversigt();			// clean old liste
                      radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
                      radiooversigt.load_radio_stations_gfx();
                      show_radio_options=false;
                    }
                  }
                  // lirc
                  if (strcmp("Stop",c)==0) {
                    // stop all music
                    do_stop_music = true;
                  }
                  // lirc
                  if (strcmp("Power",c)==0) {
                    if (!remove("mythtv-controler.lock")) {
                      if (debugmode) fprintf(stderr,"No lock file.\n");
                    }
                    system("/sbin/shutdown -P");
                    exit(2);				// QUIT program
                  }
                }
            }
            free(code);
        }
    }

    if (do_music_icon_anim_icon_ofset) {
      sinusofset+=2;		// add to table
      if (sinusofset>7200) sinusofset=0;
      // stop sinus igen
      if ((do_music_icon_anim_icon_ofset==1) && ((int) sinusofset>50)) {
        sinusofset = 0;
        do_music_icon_anim_icon_ofset=0;
      }
      if ((do_music_icon_anim_icon_ofset==-1) && ((int) sinusofset>50)) {
        sinusofset = 0;
        do_music_icon_anim_icon_ofset=0;
      }
    }
    #if defined USE_FMOD_MIXER
    sndsystem->update();				// run update on fmod sound system
    #endif
    glutTimerFunc(25, update2, 0);
    glutPostRedisplay();
}


// ****************************************************************************************
//
// init screen setup
//
// ****************************************************************************************

bool init(void) {
    #ifndef ALLOW_RUN_AS_ROOT
    if (geteuid () == 0) {
        fprintf(stderr,"MYTHTV-CONTROLLER is not supposed to be run as root. Sorry.\n");
        fprintf(stderr,"If you need to use real-time priorities and/or privileged TCP ports\n");
        return 1;
    }
    #endif
    // select clearing (background) color
    glClearColor (0.0, 0.0, 0.0, 0.0);
    // initialize viewing values
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, orgwinsizex, 0.0f, orgwinsizey, -1000.0f,1000.0f);
}



// ****************************************************************************************
//
// setup sound system
//
// ****************************************************************************************


int init_sound_system(int devicenr) {
    int num;
    char fmoddrivername[512];
    int count;
    unsigned int handle=0;
    char name[256];
    #if defined USE_FMOD_MIXER
    fprintf(stderr,"Setup FMOD soundsystem\n");
    result = FMOD::System_Create(&sndsystem);
    ERRCHECK(result,0);
    result = sndsystem->getVersion(&fmodversion);
    ERRCHECK(result,0);
    if (fmodversion < FMOD_VERSION) {
       fprintf(stderr,"Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", fmodversion, FMOD_VERSION);
       runwebserver=false;
       exit(0);
    }
    //result = sndsystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
    //ERRCHECK(result,0);
    for(int n=0;n<9;n++) strcpy(avalible_device[n],"");
    fprintf(stderr,"\nSound cards \n");
    fprintf(stderr,"-----------\n");
    int numdrivers;
    result = sndsystem->getNumDrivers(&numdrivers);
    ERRCHECK(result,0);
    numbersofsoundsystems=numdrivers;				// numbers of devices
    for (count=0; count < numdrivers; count++) {
       char name[256];
       result = sndsystem->getDriverInfo(count, name, sizeof(name), 0,0,0,0);
       ERRCHECK(result,0);
       strcpy(avalible_device[count],name);			// save device name to list
       fprintf(stderr," %d - %s\n", count , name);
    }
    if (devicenr>numdrivers) devicenr=0;			// hvis der er valgt et soundcard i config filen som ikke findes vælg default
    // setlect default soundcard=0
    result = sndsystem->getDriverInfo(devicenr, fmoddrivername, sizeof(name),0,0,0,0);
    ERRCHECK(result,0);
    fprintf(stderr,"\nUse FMOD Driver :%s\n",fmoddrivername);
    result = sndsystem->getNumPlugins(FMOD_PLUGINTYPE_CODEC, &num);
    ERRCHECK(result,0);
    for (count = 0; count < num; count++) {
      result = sndsystem->getPluginHandle(FMOD_PLUGINTYPE_CODEC, count, &handle);
      ERRCHECK(result,0);
      result = sndsystem->getPluginInfo(handle, 0, name, 256, 0);
      ERRCHECK(result,0);
    }
    result = sndsystem->getNumPlugins(FMOD_PLUGINTYPE_OUTPUT, &num);
    ERRCHECK(result,0);
    for (count = 0; count < num; count++) {
        result = sndsystem->getPluginHandle(FMOD_PLUGINTYPE_OUTPUT, count, &handle);
        ERRCHECK(result,0);
        result = sndsystem->getPluginInfo(handle, 0, name, 256, 0);
        ERRCHECK(result,0);
    }
    result = sndsystem->getPluginHandle(FMOD_PLUGINTYPE_OUTPUT, 2, &handle);
    ERRCHECK(result,0);
    if (handle==3) strcpy(configdeviceid,"FMOD ESD");
    else if (handle==2) strcpy(configdeviceid,"FMOD ALSA");
    else if (handle==1) strcpy(configdeviceid,"FMOD OSS");
    else strcpy(configdeviceid,"FMOD Default");
    result = sndsystem->setOutput(FMOD_OUTPUTTYPE_ALSA);
    ERRCHECK(result,0);
    result = sndsystem->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result,0);
    //  Bump up the file buffer size a little bit for netstreams (to account for lag).  Decode buffer is left at default.
    result = sndsystem->setStreamBufferSize(64*1024, FMOD_TIMEUNIT_RAWBYTES);
    ERRCHECK(result,0);
    strcpy(configmythsoundsystem,avalible_device[0]);
    #endif
    #if defined USE_SDL_MIXER
    fprintf(stderr,"Setup SDL_MIXER soundsystem\n");
    // load support for the OGG and MOD sample/music formats
    int flags=MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC;
    SDL_Init(SDL_INIT_AUDIO);
    /* This is where we open up our audio device.  Mix_OpenAudio takes
    as its parameters the audio format we'd /like/ to have. */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
        fprintf(stderr,"Unable to open audio!\n");
        exit(1);
    }
    /* If we actually care about what we got, we can ask here.
    In this program we don't, but I'm showing the function call here anyway in case we'd want to know later. */
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    sdlmusic=Mix_Init(flags);
    if(sdlmusic&flags != flags) {
      fprintf(stderr,"Mix_Init: Failed to init required ogg,mp3,flac support!\n");
      fprintf(stderr,"Mix_Init: %s\n", Mix_GetError());
      // handle error
    }
    #endif
    return(2);
}




// ****************************************************************************************
//
// phread dataload check radio stations if it is online
//
// ****************************************************************************************

void *radio_check_statusloader(void *data) {
  bool notdone=false;
  //pthread_mutex_lock(&count_mutex);
  fprintf(stderr,"loader thread starting - Start checkling radio status's thread\n");
  //pthread_mutex_unlock(&count_mutex);
  if (strcmp(configbackend,"mythtv")==0) {
    do {
      notdone=radiooversigt.check_radio_online(0);
    } while (notdone);
  }
  if (strcmp(configbackend,"xbmc")==0) {
    do {
      notdone=radiooversigt.check_radio_online(0);
    } while (notdone);
  }
  fprintf(stderr,"radio thread done\n");
  pthread_exit(NULL);
}



// ****************************************************************************************
//
// phread dataload Music
//
// ****************************************************************************************

void *datainfoloader_music(void *data) {
  //pthread_mutex_lock(&count_mutex);
  // write debug log
  write_logfile((char *) "loader thread starting - Loading music info.");
  if (strcmp(configbackend,"mythtv")==0) {
    // opdatere music oversigt
    // hent alt music info fra database
    // check if internal music db exist if yes do set global use it
    if (global_use_internal_music_loader_system_exist() == true) {
      if (debugmode % 2) fprintf(stderr,"******** Use global music database ********\n");
      // set use global loader
      global_use_internal_music_loader_system = true;
    } else {
      if (debugmode & 2) fprintf(stderr,"Search for music in :%s\n",configdefaultmusicpath);
      // build new db (internal db loader)

      // mew ver
     musicoversigt.opdatere_music_oversigt_nodb();

      // old ver
      //opdatere_music_oversigt_nodb(configdefaultmusicpath,musicoversigt);
      if (debugmode & 2) fprintf(stderr,"Done update db from datasource.\n");
      write_logfile((char *) "Done update db from datasource.");
      global_use_internal_music_loader_system = true;
    }
    // update music db from disk
    if ((do_update_music) || (do_update_music_now)) {
      // update the music db

      // mew ver
     musicoversigt.opdatere_music_oversigt_nodb();

      // old ver
      //opdatere_music_oversigt_nodb(configdefaultmusicpath,musicoversigt);
      do_update_music_now = false;                                              // do not call update any more
      do_update_music = false;                                                  // stop show music update
    }
    // load music db created by opdatere_music_oversigt_nodb function
    // first time
    // New ver
    if (musicoversigt.opdatere_music_oversigt(0)>0) {
     musicoversigt.opdatere_music_oversigt_icons();                                  // load icons
      write_logfile((char *) "Music db loaded..");
    }
  } else {
    if (debugmode % 2) fprintf(stderr,"Search for music in :%s\n",configdefaultmusicpath);

    // New ver
   musicoversigt.opdatere_music_oversigt_nodb();

    // update music db from disk
    // old
    //if (opdatere_music_oversigt_nodb(configdefaultmusicpath,musicoversigt)==0) {
    //   if (debugmode & 2) fprintf(stderr,"No music db loaded\n");
    //}

    // new ver
   musicoversigt.opdatere_music_oversigt(0);

    // old ver
    //opdatere_music_oversigt(musicoversigt,0);                                   // load the db again
  }
  do_update_music=false;
  // write debug log
  write_logfile((char *) "loader thread done loaded music info.");
  pthread_exit(NULL);
}


// ****************************************************************************************
//
// phread dataload Film
//
// ****************************************************************************************

void *datainfoloader_movie(void *data) {
  //pthread_mutex_lock(&count_mutex);

  //pthread_mutex_unlock(&count_mutex);
  if (strcmp(configbackend,"mythtv")==0) {
    // write debug log
    write_logfile((char *) "loader thread starting - Loading movie info from mythtv.");
    film_oversigt.opdatere_film_oversigt();     	              // gen covers 3d hvis de ikke findes.
    do_update_moviedb=false;                                    // set done
  } else {
    if (debugmode & 16) fprintf(stderr,"Load movie from xbmc/kodi\n");
  }
  // write debug log
  sprintf(debuglogdata,"loader thread done loaded %d movie.",film_oversigt.get_film_antal());
  write_logfile((char *) debuglogdata);
  pthread_exit(NULL);
}


// ****************************************************************************************
//
// phread dataload stream
//
// ****************************************************************************************

void *datainfoloader_stream(void *data) {
  // write debug log
  write_logfile((char *) "loader thread starting - Loading stream info from rss feed.");
  streamoversigt.loadrssfile(0);                                              // download rss files (())
  streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");             // load all stream from rss files
  write_logfile((char *) "loader thread done loaded stream stations.");
  do_update_rss_show=false;
  pthread_exit(NULL);
}

// ****************************************************************************************
//
// phread dataload spotify
//
// ****************************************************************************************

void *datainfoloader_spotify(void *data) {
  #ifdef ENABLE_SPOTIFY
  spotify_oversigt_loaded_begin=true;
  // write debug log
  write_logfile((char *) "loader thread starting - Loading spotify info from db.");
  spotify_oversigt.opdatere_spotify_oversigt(0);                                // update from db
  spotify_oversigt.set_search_loaded();                           // triger icon loader
  //spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(keybuffer,0);   //
  //spotify_oversigt.load_spotify_iconoversigt();
  write_logfile((char *) "loader thread done loaded spotify.");
  spotify_oversigt_loaded_begin=false;
  #endif
  pthread_exit(NULL);
}

// ****************************************************************************************
//
// Phread datadb update from spotify (online)
// add all the users play list to the system db to be show in spotifyoversigt
//
// ****************************************************************************************

void *webupdate_loader_spotify(void *data) {
  #ifdef ENABLE_SPOTIFY
  bool loadedspotify=false;
  int spotify_user_id_check;
  // write debug log
  write_logfile((char *) "Loader thread starting - Loading spotify info from web.");
  if (!(spotify_oversigt.get_spotify_update_flag())) {
    // check if spotify user info is loaded.
    if ((spotify_oversigt.spotify_get_user_id()==404) && (strcmp(spotify_oversigt.spotify_get_token(),"")!=0)) {
      loadedspotify=true;
      spotify_update_loaded_begin=true;
      spotify_oversigt.set_spotify_update_flag(true);
      // add default playlists from spotify
      // you have to call clean_spotify_oversigt after earch spodify_get_playlist
      spotify_oversigt.spotify_get_playlist("37i9dQZF1EpfknyBUWzyB7",1,true);        // songs on repeat playlist
      spotify_oversigt.clean_spotify_oversigt();                                  // clear old stuf
      spotify_oversigt.spotify_get_playlist("37i9dQZEVXcU9Ndp82od6b",1,true);        // Your discovery weekly tunes
      spotify_oversigt.clean_spotify_oversigt();                                  // clear old stuf
      spotify_oversigt.spotify_get_playlist("37i9dQZF1DWZQZGknjUJWV",1,true);        // dansk dancehall
      spotify_oversigt.clean_spotify_oversigt();                                  // clear old stuf
      spotify_oversigt.spotify_get_playlist("37i9dQZF1DX60OAKjsWlA2",1,true);        // hot Hits dk playlist
      spotify_oversigt.clean_spotify_oversigt();                                  // clear old stuf
      // get user playlists
      spotify_oversigt.spotify_get_user_playlists(true,0);                        // get all playlist and update db (force update)
      spotify_oversigt.clean_spotify_oversigt();                                  // clear old stuf
      // update the playback device list
      spotify_oversigt.active_spotify_device=spotify_oversigt.spotify_get_available_devices();
      // update view from db
      spotify_oversigt.opdatere_spotify_oversigt(0);                              // reset spotify overview to default
    }
  }
  // write debug log
  if (loadedspotify) write_logfile((char *) "Loader thread done update spotify from web.");
  else {
    write_logfile((char *) "Loader thread done update spotify.");
    if (strcmp(spotify_oversigt.spotify_get_token(),"")==0) write_logfile((char *) "Error on spotify token.");
    else {
      spotify_user_id_check=spotify_oversigt.spotify_get_user_id();
      sprintf(debuglogdata,"Error loading spotify user data. Error code %d",spotify_user_id_check);
      write_logfile((char *) debuglogdata);
    }
  }
  spotify_update_loaded_begin=false;
  spotify_oversigt.set_spotify_update_flag(false);
  firsttimespotifyupdate=false;                                                 // close firsttime update window after update
  #endif
  pthread_exit(NULL);
}


// ****************************************************************************************
//
// phread dataload webserver
//
// ****************************************************************************************

void *datainfoloader_webserver(void *data) {
  struct tm* t;
  static time_t lasttime=0;
  static time_t nowdate;
  time(&lasttime);
  time(&nowdate);
  // run the webserver
  while((true) && (runwebserver)) {
    #ifdef ENABLE_SPOTIFY
    mg_mgr_poll(&spotify_oversigt.mgr, 50);
    // run time server to update spotify token
    if (difftime(nowdate, lasttime)>500) {                                     // 3500
      time(&lasttime);
      write_logfile((char *) "Update spotify token");
      if ((spotify_oversigt.spotify_get_token(),"")!=0) {
        //spotify_oversigt.spotify_refresh_token();       // old ver
        spotify_oversigt.spotify_refresh_token2();        // new ver
      }
    }
    // get time
    time(&nowdate);
    // get search result after search text is done.
    if (do_hent_spotify_search_online) {
      spotify_oversigt.search_spotify_online_done=false;
      fprintf(stderr,"Update spotify search result thread.\n");
      do_hent_spotify_search_online=false;
      spotify_oversigt_loaded_begin=true;
      spotify_oversigt.clean_spotify_oversigt();
      spotify_oversigt.opdatere_spotify_oversigt_searchtxt_online(keybuffer,0);
      // spotify_oversigt.load_spotify_iconoversigt();                       // load icons
      printf("Done Update spotify search result thread.\n");
      spotify_oversigt.search_spotify_online_done=true;
      spotify_oversigt_loaded_begin=false;
      spotify_oversigt.set_search_loaded();
      //spotify_oversigt.type=2;
    }
    #endif
  }
  pthread_exit(NULL);
}

// ****************************************************************************************
//
// phread dataload xmltv
// called from update_xmltv_phread_loader
//
// ****************************************************************************************

void *datainfoloader_xmltv(void *data) {
  int error;
  //pthread_mutex_lock(&count_mutex);
  // write debug log
  write_logfile((char *) "loader thread starting - xmltv file parser starting.");
  //
  // multi thread
  // load xmltvguide from web
  if (get_tvguide_fromweb()!=-1) {
    error=aktiv_tv_oversigt.parsexmltv("tvguide.xml");
    if (error==0) {
      aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
      //save array to disk
      aktiv_tv_oversigt.saveparsexmltvdb();
      write_logfile((char *) "tvguidedb file saved ok.");
    } else {
      write_logfile((char *) "parser xmltv tv guide error.");
      fprintf(stderr,"Parse xmltv error\n");
    }
  }
  write_logfile((char *) "parser xmltv guide done.");
  // save config again
  save_config((char *) "/etc/mythtv-controller.conf");
  // set update flag for done
  do_update_xmltv_show=false;
  pthread_exit(NULL);
}

// ****************************************************************************************
//
// xmltv loader start from main loop then trigged by date
//
// ****************************************************************************************

void *update_xmltv_phread_loader() {
  if (true) {
    pthread_t loaderthread2;           // load tvguide xml file in to db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_xmltv,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
}


// ****************************************************************************************
//
// rss loader start from main loop then trigged by date
//
// ****************************************************************************************

void *update_rss_phread_loader() {
  if (true) {
    pthread_t loaderthread2;           // load tvguide xml file in to db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_stream,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
}


// ****************************************************************************************
//
// xmltv loader start from main loop then trigged by date
//
// ****************************************************************************************

void *update_music_phread_loader() {
  if (true) {
    pthread_t loaderthread2;           // load tvguide xml file in to db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_music,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
}

// ****************************************************************************************
//
// spotify db loader.
//
// ****************************************************************************************

void *update_spotify_phread_loader() {
  #ifdef ENABLE_SPOTIFY
  if (true) {
    pthread_t loaderthread2;           // load spotify into db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_spotify,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
  #endif
}


// ****************************************************************************************
//
// Spotify db update online from spotify (by userid(token))
// used by first time update of * key
//
// ****************************************************************************************

void *update_spotifyonline_phread_loader() {
  #ifdef ENABLE_SPOTIFY
  if (true) {
    pthread_t loaderthread2;           // load spotify into db
    int rc2=pthread_create(&loaderthread2,NULL,webupdate_loader_spotify,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR webupdate_loader_spotify function\nreturn code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
  #endif
}

// ****************************************************************************************
//
// rss loader start from main loop start webserver
//
// ****************************************************************************************

void *update_webserver_phread_loader() {
  if (true) {
    pthread_t loaderthread2;           // load tvguide xml file in to db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_webserver,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() webserver is %d\n", rc2);
      exit(-1);
    }
  }
}


//
// xbmc client
// sql lite
//

CXBMCClient *xbmcclient=new CXBMCClient("");
int configxbmcver=1;
xbmcsqlite *xbmcSQL=NULL;


// ****************************************************************************************
//
// phread dataload xbmc/kodi music db
//
// ****************************************************************************************

void *xbmcdatainfoloader(void *data) {
  char userhomedir[200];
  DIR *dirp=NULL;
  char *ext;
  struct dirent *de=NULL;
  char filename[256];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  bool dbexist=false;
  char sqlselect[1024];
  char videohomedirpath[1024];
  char musichomedirpath[1024];
  bool allokay=false;
  //pthread_mutex_lock(&count_mutex);
  // write debug log
  write_logfile((char *) "loader thread starting - Loading music from xbmc/kodi).");
  //pthread_mutex_unlock(&count_mutex);
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    allokay = true;
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
    mysql_query(conn,"SHOW TABLES LIKE 'music_albums'");
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
      dbexist = true;
    }
    // create databases if not exist
    if (!(dbexist)) {
      strcpy(sqlselect,"create table IF NOT EXISTS music_directories(directory_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,path text, parent_id int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_albums(album_id int  NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_id int, album_name varchar(255) ,year int, compilation int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_songs(song_id int NOT NULL AUTO_INCREMENT PRIMARY KEY,filename text,name varchar(255),track int, artist_id int, album_id int, genre_id int,year int,length int,numplays int,rating int,lastplay datetime, date_entered  datetime, date_modified datetime,format varchar(4), mythdigest varchar(255) ,size int,description  varchar(255), comment varchar(255), disc_count int, disc_number int, track_count  int, start_time  int, stop_time int, eq_preset   varchar(255) , relative_volume int, sample_rate int, bitrate  int,bpm int, directory_id int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_artists(artist_id  int NOT NULL AUTO_INCREMENT PRIMARY KEY, artist_name varchar(255))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS music_genres(genre_id int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(255))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
    }
    mysql_close(conn);
  }
  if ((allokay) && (strcmp(configbackend,"xbmc")==0) && (!(dbexist))) {
    if (debugmode & 2) fprintf(stderr,"XBMC - Loader starting.....\n");
    // get user homedir
    getuserhomedir(userhomedir);
    strcat(userhomedir,"/.kodi/userdata/Database/");
    dirp=opendir(userhomedir);                                // "~/.kodi/userdata/Database/");
    if (dirp==NULL) {
        if (debugmode & 2) fprintf(stderr,"No xbmc/kodi db found\nOpen dir error %s \n","~/.kodi/userdata/Database/");
        runwebserver=false;
        exit(0);
    }
    // loop dir and update music songs db
    while((de = readdir(dirp)) && (!(kodiverfound))) {
      if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
        ext = strrchr(de->d_name, '.');
        if (ext) strcpy(filename,de->d_name);
        if ((strncmp(filename,"MyMusic",7)==0) && (!(kodiverfound))) {
          if (strcmp(filename,kodiver[0])==0) {
            kodiverfound=16;
            if (debugmode & 2) fprintf(stderr,"Kodi version 16 is found \n");
          }
          if (strcmp(filename,kodiver[1])==0) {
            kodiverfound=15;
            if (debugmode & 2) fprintf(stderr,"Kodi version 15 is found \n");
          }
          if (strcmp(filename,kodiver[2])==0) {
            kodiverfound=14;
            if (debugmode & 2) fprintf(stderr,"Kodi version 14 is found \n");
          }
          if (strcmp(filename,kodiver[3])==0) {
            kodiverfound=13;
            if (debugmode & 2) fprintf(stderr,"Kodi version 13 is found \n");
          }
          if (strcmp(filename,kodiver[4])==0) {
            kodiverfound=12;
            if (debugmode & 2) fprintf(stderr,"Kodi version 12 is found \n");
          }
          if (strcmp(filename,kodiver[5])==0) {
            kodiverfound=11;
            if (debugmode & 2) fprintf(stderr,"Kodi version 11 is found \n");
          }
        }
      }
    }
    getuserhomedir(userhomedir);
    strcpy(videohomedirpath,userhomedir);
    strcpy(musichomedirpath,userhomedir);
    switch (kodiverfound) {
      case 19:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos109.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic70.db");
                break;
      case 17:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos107.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic60.db");
                break;
      case 16:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos104.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic60.db");
                break;
      case 15:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos99.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic56.db");
                break;
      case 14:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos93.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic48.db");
                break;
      case 13:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos78.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic46.db");
                break;
      case 12:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos75.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic32.db");
                break;

    }
    xbmcSQL=new xbmcsqlite((char *) configmysqlhost,videohomedirpath,musichomedirpath,videohomedirpath);
    if (xbmcSQL) {
        xbmcSQL->xbmcloadversion();									// get version number fropm mxbc db
        sprintf(debuglogdata,"XBMC - Loader is running.");
        // write debug log
        write_logfile((char *) debuglogdata);
        xbmcSQL->xbmc_readmusicdb();     // IN use
        sprintf(debuglogdata,"XBMC - Loader is done.");
        write_logfile((char *) debuglogdata);
        //create_radio_oversigt();									// Create radio mysql database if not exist
        //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
    } else {
      write_logfile((char *) "Error loading kodi db.");
      exit(1);
    }
    write_logfile((char *) "loader thread done loaded kodi.");
  }
  // set use internal db for music
  global_use_internal_music_loader_system = true;

  //new ver
  sprintf(debuglogdata,"Numbers of music records loaded %d.",musicoversigt.opdatere_music_oversigt(0));

  // load db
  // write debug log
  //sprintf(debuglogdata,"Numbers of music records loaded %d.", opdatere_music_oversigt(musicoversigt,0));
  write_logfile((char *) debuglogdata);
  pthread_exit(NULL);
}



// ****************************************************************************************
// load xbmc/kodi movies to db
// create movie db if not exist
// ****************************************************************************************

void *xbmcdatainfoloader_movie(void *data) {
  char userhomedir[200];
  // mxbc/kodi file names for sqlite
  int kodiverfound=0;
  char videohomedirpath[1024];
  char musichomedirpath[1024];
  char *ext;
  char filename[256];
  struct dirent *de=NULL;
  DIR *dirp=NULL;
  bool allokay=false;
  // mysql def
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  bool dbexist=false;
  char sqlselect[1024];
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    allokay = true;
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
    mysql_query(conn,"SHOW TABLES LIKE 'Videometadata'");
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
      dbexist = true;
    }
    // create databases/tables if not exist
    // needed by movie loader
    if (!(dbexist)) {
      strcpy(sqlselect,"create table IF NOT EXISTS videometadata(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(120), subtitle text, tagline varchar(255), director varchar(128), studio varchar(128), plot text, rating varchar(128), inetref  varchar(255), collectionref int, homepage text,year int, releasedate date, userrating float, length int, playcount int, season int, episode int,showlevel int, filename text,hash varchar(128), coverfile text, childid int, browse int, watched int, processed int, playcommand varchar(255), category int, trailer text,host text, screenshot text, banner text, fanart text,insertdate timestamp, contenttype int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videocategory(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, category varchar(128))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videogenre(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, genre varchar(128))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videocountry(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, country varchar(128))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videocollection(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, title varchar(256), contenttype int, plot text,network varchar(128), collectionref varchar(128), certification varchar(128), genre varchar(128),releasedate date, language varchar(10),status varchar(64), rating float, ratingcount int, runtime int, banner text,fanart text,coverart text)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videopathinfo(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, path text, contenttype int, collectionref int,recurse  int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS videotypes(intid int NOT NULL AUTO_INCREMENT PRIMARY KEY, extension varchar(128),playcommand varchar(255), f_ignore int,  use_default int)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'txt','',1,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'log','',1,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'mpg','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'avi','',0,1)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'vob','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'mpeg','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'VIDEO_TS','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'iso','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"insert into videotypes values (0,'img','',0,0)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
    }
    mysql_close(conn);
  }
  if (allokay) {
    // get user homedir
    getuserhomedir(userhomedir);
    strcat(userhomedir,"/.kodi/userdata/Database");
    dirp=opendir(userhomedir);                                                          // "~/.kodi/userdata/Database/");
    if (dirp==NULL) {
      fprintf(stderr,"No xbmc/kodi db found\nOpen dir error %s \n",userhomedir);
      runwebserver=false;
      exit(0);
    }
    // loop dir and update music songs db
    // and find kodi db version
    while((de = readdir(dirp)) && (!(kodiverfound))) {
      if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
        ext = strrchr(de->d_name, '.');
        if (ext) strcpy(filename,de->d_name);
        if ((strncmp(filename,"MyVideos",8)==0) && (!(kodiverfound))) {
          if (strcmp(filename,kodivermovie[0])==0) {
            kodiverfound = 17;
            fprintf(stderr,"Kodi version 17 is found \n");
          }
          if (strcmp(filename,kodivermovie[1])==0) {
            kodiverfound = 15;
            fprintf(stderr,"Kodi version 15 is found \n");
          }
          if (strcmp(filename,kodivermovie[2])==0) {
            kodiverfound = 14;
            fprintf(stderr,"Kodi version 14 is found \n");
          }
          if (strcmp(filename,kodivermovie[3])==0) {
            kodiverfound = 13;
            fprintf(stderr,"Kodi version 13 is found \n");
          }
          if (strcmp(filename,kodivermovie[4])==0) {
            kodiverfound = 12;
            fprintf(stderr,"Kodi version 12 is found \n");
          }
          if (strcmp(filename,kodivermovie[5])==0) {
            kodiverfound = 11;
            fprintf(stderr,"Kodi version 11 is found \n");
          }
        }
      }
    }
    // check/get user homedir
    getuserhomedir(userhomedir);
    strcpy(videohomedirpath,userhomedir);
    // add kodi dir ro db files
    switch (kodiverfound) {
      case 17:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos107.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic60.db");
                break;
      case 16:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos104.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic60.db");
                break;
      case 15:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos99.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic56.db");
                break;
      case 14:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos93.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic48.db");
                break;
      case 13:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos78.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic46.db");
                break;
      case 12:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos75.db");
                strcat(musichomedirpath,"/.kodi/userdata/Database/MyMusic32.db");
                break;
    }
    // write debug log
    write_logfile((char *) "loader thread starting - Loading movies from xbmc/kodi.");
    xbmcSQL=new xbmcsqlite((char *) configmysqlhost,videohomedirpath,musichomedirpath,videohomedirpath);
    //xbmcSQL=new xbmcsqlite((char *) configmysqlhost,(char *)"~/.kodi/userdata/Database/MyVideos75.db",(char *)"~/.kodi/userdata/Database/MyMusic18.db",(char *)"~/.kodi/userdata/Database/MyVideos75.db");
    if (xbmcSQL) {
      xbmcSQL->xbmcloadversion();									// get version number from mxbc db
      if (debugmode & 16) fprintf(stderr,"XBMC - Load running\n");
      // load xbmc movie db
      xbmcSQL->xbmc_readmoviedb();                // load movies from kodi db to internal db
      // set use internal db for movies
      global_use_internal_music_loader_system = true;
      //xbmcSQL->xbmc_readmusicdb();     // IN use
      if (debugmode & 16) fprintf(stderr,"XBMC - loader done.\n");
      // load movies in from db
      xbmcSQL->getxmlfilepath();                   // get path info from xml file
      film_oversigt.opdatere_film_oversigt();     // gen covers 3d hvis de ikke findes.
    }
  }
  // write debug log
  sprintf(debuglogdata,"loader thread done loaded %d movie(s)",film_oversigt.get_film_antal());
  write_logfile((char *) debuglogdata);
  do_update_moviedb = false;
  pthread_exit(NULL);
}


// ****************************************************************************************
//
// load img file
//
// ****************************************************************************************

GLuint loadgfxfile(char *temapath,char *dir,char *file) {
    GLuint gl_img=0;
    char fileload[2000];
    strcpy(fileload,"");
    strcat(fileload,temapath);
    strcat(fileload,dir);
    strcat(fileload,file);							// filename - type
    strcat(fileload,".png");            // add png to file name
    if (file_exists(fileload)) gl_img = loadTexture ((char *) fileload);
    else {
      // check if file exist af .jpg file
      strcpy(fileload,"");
      strcat(fileload,temapath);
      strcat(fileload,dir);
      strcat(fileload,file);
      strcat(fileload,".jpg");
      if (file_exists(fileload)) gl_img = loadTexture ((char *) fileload);
      else gl_img = 0;
    }
    if (gl_img == 0) fprintf(stderr,"GFXFILE %s in dir %s NOT FOUND \n",fileload,dir);
    return(gl_img);
}



// ****************************************************************************************
//
// Load all gfx tema data
//
// ****************************************************************************************


void loadgfx() {
    unsigned int i;
    char tmpfilename[256];
    char fileload[256];
    char temapath[256];
    char temapath1[256];
    printf ("Loading init graphic.\n");
    strcpy(temapath,"");
    strcpy(temapath1,"");
    if (tema == 1) strcpy(temapath,"/opt/mythtv-controller/tema1/"); else
    if (tema == 2) strcpy(temapath,"/opt/mythtv-controller/tema2/"); else
    if (tema == 3) strcpy(temapath,"/opt/mythtv-controller/tema3/"); else
    if (tema == 4) strcpy(temapath,"/opt/mythtv-controller/tema4/"); else
    if (tema == 5) strcpy(temapath,"/opt/mythtv-controller/tema5/"); else
    if (tema == 6) strcpy(temapath,"/opt/mythtv-controller/tema6/"); else
    if (tema == 7) strcpy(temapath,"/opt/mythtv-controller/tema7/"); else
    if (tema == 8) strcpy(temapath,"/opt/mythtv-controller/tema8/"); else
    if (tema == 9) strcpy(temapath,"/opt/mythtv-controller/tema9/"); else
    if (tema == 10) strcpy(temapath,"/opt/mythtv-controller/tema10/"); else {
      // default tema
      strcpy(temapath,"tema1/");
      tema=1;
    }
    _textureutvbgmask     = loadgfxfile(temapath,(char *) "images/",(char *) "tv_carbon");
    _defaultdvdcover      = loadgfxfile(temapath,(char *) "images/",(char *) "dvdcover");
    if (screen_size<3)
    _texturemovieinfobox  = loadgfxfile(temapath,(char *) "images/",(char *) "movie-infobox");   		// small screen 4/3
    else
    _texturemovieinfobox  = loadgfxfile(temapath,(char *) "images/",(char *) "movie-infobox3-4");		// big screen  16/9
    _textureId_dir       = loadgfxfile(temapath,(char *) "images/",(char *) "dir1");
    _textureId_song      = loadgfxfile(temapath,(char *) "images/",(char *) "song1");
    _texturemusicplayer 	= loadgfxfile(temapath,(char *) "images/",(char *) "musicplayer-info");
    _textureId9_askbox   	= loadgfxfile(temapath,(char *) "images/",(char *) "askbox");
    _textureId9_2        	= loadgfxfile(temapath,(char *) "images/",(char *) "askbox_cd_cover");
    _textureIdplayicon   	= loadgfxfile(temapath,(char *) "images/",(char *) "play");
    _textureopen         	= loadgfxfile(temapath,(char *) "images/",(char *) "open");
    _textureclose        	= loadgfxfile(temapath,(char *) "images/",(char *) "close");
    _textureswap         	= loadgfxfile(temapath,(char *) "images/",(char *) "swap");
    _textureId11         	= loadgfxfile(temapath,(char *) "images/",(char *) "tvprogram_oversigt");
    // main background
    _textureIdback_main   = loadgfxfile(temapath,(char *) "images/",(char *) "background_main");
    // music oversigt background
    _textureIdback_music  = loadgfxfile(temapath,(char *) "images/",(char *) "background_music");
    // setup background
    _textureIdback_setup  = loadgfxfile(temapath,(char *) "images/",(char *) "tuxsetup");
    // other back
    _textureIdback_other  = loadgfxfile(temapath,(char *) "images/",(char *) "background_other");
    // mask for knap
    _textureId14         	= loadgfxfile(temapath,(char *) "images/",(char *) "stor_knap3_2_pause");
    _texture_nocdcover   	= loadgfxfile(temapath,(char *) "images/",(char *) "nocdcover");
    _textureId20         	= loadgfxfile(temapath,(char *) "images/",(char *) "lillecoverdefault");
    _textureId22         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox1");
    _textureId23         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox2");
    _textureId24         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox3");
    _textureId26         	= loadgfxfile(temapath,(char *) "images/",(char *) "volbar");
    _textureId27         	= loadgfxfile(temapath,(char *) "images/",(char *) "volbar_back");
    _textureId28         	= loadgfxfile(temapath,(char *) "images/",(char *) "dirplaylist");
    _textureIdback       	= loadgfxfile(temapath,(char *) "images/",(char *) "back-icon");
    setuptexture         	= loadgfxfile(temapath,(char *) "images/",(char *) "setup");
    setupupdatebutton     = loadgfxfile(temapath,(char *) "images/",(char *) "updatebutton");
    setuptvgraberback    	= loadgfxfile(temapath,(char *) "images/",(char *) "setuptvgraberback");
    _textureIdtv         	= loadgfxfile(temapath,(char *) "buttons/",(char *) "tv");
    _textureIdmusic     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "music");
    _textureIdfilm       	= loadgfxfile(temapath,(char *) "buttons/",(char *) "movie");
    _textureIdrecorded  	= loadgfxfile(temapath,(char *) "buttons/",(char *) "recorded");
    _texturemlast       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaylast");
    _texturemlast2      	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaylast");
    _texturemnext       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaynext");
    _texturemplay       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplay");
    _texturempause        = loadgfxfile(temapath,(char *) "images/",(char *) "mpause");
    _textureIdpup       	= loadgfxfile(temapath,(char *) "buttons/",(char *) "pup");
    _textureIdpdown     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "pdown");
    _texturemstop       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaystop");
    _textureIdrecorded_aktiv=loadgfxfile(temapath,(char *) "buttons/",(char *) "recorded_selected");
    _textureIdfilm_aktiv  = loadgfxfile(temapath,(char *) "buttons/",(char *) "movie1");
    _textureIdmusicsearch = loadgfxfile(temapath,(char *) "images/",(char *) "music_search");
    _textureIdradiosearch = loadgfxfile(temapath,(char *) "images/",(char *) "radio_search");
    _textureIdmusicsearch1= loadgfxfile(temapath,(char *) "images/",(char *) "playlist_search");
    _textureIdmoviesearch = loadgfxfile(temapath,(char *) "images/",(char *) "movie_search");
    _textureIdloading   	= loadgfxfile(temapath,(char *) "images/",(char *) "loading");			// window
    _textureIdplayinfo  	= loadgfxfile(temapath,(char *) "buttons/",(char *) "playinfo");
    _textureIdclose     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "close");
    _textureIdclose1    	= loadgfxfile(temapath,(char *) "buttons/",(char *) "close1");
    _texturelock        	= loadgfxfile(temapath,(char *) "images/",(char *) "lock");
    // ************************ icons ******************************************
    _texturesetupmenu   	= loadgfxfile(temapath,(char *) "buttons/",(char *) "setupmenu");				// setup menu
    _texturesetupmenu_select	= loadgfxfile(temapath,(char *) "buttons/",(char *) "setupmenu1");		// setup menu selected
    _texturesoundsetup		= loadgfxfile(temapath,(char *) "images/",(char *) "setupsound");
    _texturesourcesetup		= loadgfxfile(temapath,(char *) "images/",(char *) "setupsource");
    _textureimagesetup		= loadgfxfile(temapath,(char *) "images/",(char *) "setupimg");
    _texturetemasetup 		= loadgfxfile(temapath,(char *) "images/",(char *) "setuptema");
    _texturemythtvsql 		= loadgfxfile(temapath,(char *) "images/",(char *) "setupsql");
    _texturesetupfont 		= loadgfxfile(temapath,(char *) "images/",(char *) "setupfont");
    _texturekeyssetup 		= loadgfxfile(temapath,(char *) "images/",(char *) "setupkeys");
    _texturekeysrss		    = loadgfxfile(temapath,(char *) "images/",(char *) "setuprss");
    _texturespotify       = loadgfxfile(temapath,(char *) "images/",(char *) "setupspotify");
    _texturevideoplayersetup	= loadgfxfile(temapath,(char *) "images/",(char *) "setupplayer");
    _texturetvgrabersetup = loadgfxfile(temapath,(char *) "images/",(char *) "setupxmltv");
    _texturesetupclose		= loadgfxfile(temapath,(char *) "images/",(char *) "setupclose");
    setupkeysbar1     		= loadgfxfile(temapath,(char *) "images/",(char *) "setupkeybar1");
    setupkeysbar2     		= loadgfxfile(temapath,(char *) "images/",(char *) "setupkeybar2");
    tvprginfobig      		= loadgfxfile(temapath,(char *) "images/",(char *) "tvprginfo");
    _tvprgrecorded    		= loadgfxfile(temapath,(char *) "images/",(char *) "tvprgrecorded");
    _tvprgrecordedr   		= loadgfxfile(temapath,(char *) "images/",(char *) "tvprgrecordedr");
    _tvrecordbutton   		= loadgfxfile(temapath,(char *) "images/",(char *) "tvrecord");
    _tvrecordcancelbutton	= loadgfxfile(temapath,(char *) "images/",(char *) "tvrecord_cancel");
    _tvoldprgrecordedbutton	= loadgfxfile(temapath,(char *) "images/",(char *) "oldrecordedbutton");
    _tvnewprgrecordedbutton	= loadgfxfile(temapath,(char *) "images/",(char *) "newrecordedbutton");
    // ****************** backgrounds **************************************
    setupsoundback      	= loadgfxfile(temapath,(char *) "images/",(char *) "setup-soundback");
    setupsqlback        	= loadgfxfile(temapath,(char *) "images/",(char *) "setupsqlback");
    setuptemaback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setuptemaback");
    setupnetworkback    	= loadgfxfile(temapath,(char *) "images/",(char *) "setupnetworkback");
    setupnetworkwlanback  = loadgfxfile(temapath,(char *) "images/",(char *) "setupnetworkwlanback");
    setupscreenback     	= loadgfxfile(temapath,(char *) "images/",(char *) "setupscreenback");
    setupfontback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setupfontback");
    setupkeysback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setupkeysback");
    setuprssback         	= loadgfxfile(temapath,(char *) "images/",(char *) "setuprssback");
    _texturesaveplaylist  = loadgfxfile(temapath,(char *) "images/",(char *) "filename");
    mobileplayer_icon     = loadgfxfile(temapath,(char *) "images/",(char *) "mobileplayer");
    pcplayer_icon         = loadgfxfile(temapath,(char *) "images/",(char *) "pcplayer");
    unknownplayer_icon    = loadgfxfile(temapath,(char *) "images/",(char *) "unknownplayer");

// ************************* screen shot *******************************
    screenshot1           = loadTexture ((char *) "images/screenshot1.png");
    screenshot2           = loadTexture ((char *) "images/screenshot2.png");
    screenshot3           = loadTexture ((char *) "images/screenshot3.png");
    screenshot4           = loadTexture ((char *) "images/screenshot4.png");
    screenshot5           = loadTexture ((char *) "images/screenshot5.png");
    screenshot6           = loadTexture ((char *) "images/screenshot6.png");
    screenshot7           = loadTexture ((char *) "images/screenshot7.png");
    screenshot8           = loadTexture ((char *) "images/screenshot8.png");
    screenshot9           = loadTexture ((char *) "images/screenshot9.png");
    screenshot10          = loadTexture ((char *) "images/screenshot10.png");
// ************************* Tv guide ***********************************
    _tvbar1               = loadgfxfile(temapath,(char *) "images/",(char *) "tvbar1");
    _tvoverskrift         = loadgfxfile(temapath,(char *) "images/",(char *) "tvbar_top");
    _tvbar1_1             = loadgfxfile(temapath,(char *) "images/",(char *) "tvbar1_1");
    _tvbar3               = loadgfxfile(temapath,(char *) "images/",(char *) "tvbar3");
// ************************ radio buttons *******************************
    spotify_icon_border   = loadgfxfile(temapath,(char *) "images/",(char *) "spotify_icon_border.png");

    onlineradio_empty     = loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio_empty");
    onlineradio           = loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio");
    onlineradio192        = loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio192");
    onlineradio320        = loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio320");
    radiobutton           = loadgfxfile(temapath,(char *) "buttons/",(char *) "radio_button");
    musicbutton           = loadgfxfile(temapath,(char *) "buttons/",(char *) "music_button");
// ************************** spotify buttons ****************************
    spotify_askplay       = loadgfxfile(temapath,(char *) "buttons/",(char *) "spotify_askplay");
    spotify_askopen       = loadgfxfile(temapath,(char *) "buttons/",(char *) "spotify_askopen");
    spotify_search        = loadgfxfile(temapath,(char *) "buttons/",(char *) "search");
    spotify_search_back   = loadgfxfile(temapath,(char *) "buttons/",(char *) "search_back");
    spotifybutton         = loadgfxfile(temapath,(char *) "buttons/",(char *) "spotify_button");
    tidalbutton           = loadgfxfile(temapath,(char *) "buttons/",(char *) "tidal_button");
    spotify_ecover        = loadgfxfile(temapath,(char *) "images/",(char *) "spotify_ecover");
    spotify_pil           = loadgfxfile(temapath,(char *) "images/",(char *) "spotify_pil");
    big_search_bar_playlist= loadgfxfile(temapath,(char *) "images/",(char *) "big_search_bar_playlist");
    big_search_bar_track   = loadgfxfile(temapath,(char *) "images/",(char *) "big_search_bar_song");
    big_search_bar_albumm  = loadgfxfile(temapath,(char *) "images/",(char *) "big_search_bar_album");
    big_search_bar_artist  = loadgfxfile(temapath,(char *) "images/",(char *) "big_search_bar_artist");
    // radio options (O) key in radio oversigt
    radiooptions          = loadgfxfile(temapath,(char *) "images/",(char *) "radiooptions");
    // radio options mask (O) key in radio oversigt
    onlinestream          = loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream");
    onlinestream_empty    = loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream_empty");
    onlinestream_empty1   = loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream_empty1");
    // podcast button
    streambutton          = loadgfxfile(temapath,(char *) "buttons/",(char *) "stream_button");
    // movie button
    moviebutton           = loadgfxfile(temapath,(char *) "buttons/",(char *) "movie_button");
    // main logo
    _mainlogo             = loadgfxfile(temapath,(char *) "images/",(char *) "logo");
    // mask for flags
    strcpy(fileload,(char *) "/opt/mythtv-controller/images/landemask.jpg");
    gfxlandemask          = loadTexture (fileload);
// ************************** screen saver boxes **************************************
    screensaverbox        = loadgfxfile(temapath,(char *) "images/",(char *) "3d_brix");
    screensaverbox1       = loadgfxfile(temapath,(char *) "images/",(char *) "3d_brix1");
    texturedot            = loadgfxfile(temapath,(char *) "images/",(char *) "dot");
    texturedot1           = loadgfxfile(temapath,(char *) "images/",(char *) "dot1");
    _errorbox             = loadgfxfile(temapath,(char *) "images/",(char *) "errorbox");
    // new stuf mini icon
    newstuf_icon          = loadgfxfile(temapath,(char *) "images/",(char *) "new_stuf");
    // exit
    _textureexit          = loadgfxfile(temapath,(char *) "images/",(char *) "exit");
    _textureIdreset_search = loadgfxfile(temapath,(char *) "buttons/",(char *) "reset_search");
    // analog clock background
    analog_clock_background = loadgfxfile(temapath,(char *) "images/",(char *) "clock_background");
    strcpy(tmpfilename,temapath);
    strcat(tmpfilename,(char *) "buttons/music1.png");
    if (file_exists(tmpfilename)) {
      _textureIdmusic_aktiv=loadTexture ((char *) tmpfilename);
    } else _textureIdmusic_aktiv=0;
    printf ("Done loading init graphic.\n");
}

// ****************************************************************************************
//
// free loaded gfx
//
// ****************************************************************************************

void freegfx() {
    int i;
    glDeleteTextures( 1, &_textureutvbgmask);
    glDeleteTextures( 1, &_defaultdvdcover);		    // default dvd cover hvis der ikke er nogle at loade
    glDeleteTextures( 1, &_texturemovieinfobox);		// movie info box
    glDeleteTextures( 1, &_textureId_dir);				      // cd/dir icon in music oversigt (hvis ingen cd cover findes)
    glDeleteTextures( 1, &_textureId_song);				      // cd/dir icon in music oversigt (hvis ingen cd cover findes)
    glDeleteTextures( 1, &_texturemusicplayer);     // show music info player
    glDeleteTextures( 1, &_textureId9_askbox);			// ask box
    glDeleteTextures( 1, &_textureId9_2);			      // ask box
    glDeleteTextures( 1, &_textureIdplayicon);			// play icon
    glDeleteTextures( 1, &_textureopen);            // open icon
    glDeleteTextures( 1, &_textureclose);			      // no dont play icon
    glDeleteTextures( 1, &_textureswap);			      // no dont play icon
    glDeleteTextures( 1, &_textureId11);			      // tv program oversigt logo
    glDeleteTextures( 1, &_textureIdback_main);     // main background
    glDeleteTextures( 1, &_textureIdback_music);    // music background
    glDeleteTextures( 1, &_textureIdback_setup);    // setup background
    glDeleteTextures( 1, &_textureIdback_other);		// other background
    glDeleteTextures( 1, &_textureId14);	          // pause knap
    glDeleteTextures( 1, &_texture_nocdcover);      // hvis ingen texture (music cover) set default (box2.bmp)
    glDeleteTextures( 1, &_textureId20);		        // bruges af 3d screen saver (lille logo)
    glDeleteTextures( 1, &_textureId22);		        // bruges ved recorded programs
    glDeleteTextures( 1, &_textureId23);	          // bruges ved recorded programs
    glDeleteTextures( 1, &_textureId24);			      // bruges ved recorded programs
    glDeleteTextures( 1, &_textureId26);			      // vol control
    glDeleteTextures( 1, &_textureId27);	          // vol control
    glDeleteTextures( 1, &_textureId28);		        // playlist default icon
    glDeleteTextures( 1, &_textureIdback);		      // bruges ved music
    glDeleteTextures( 1, &setuptexture);			      // bruges af setup
    glDeleteTextures( 1, &setupupdatebutton);			  // global update button
    glDeleteTextures( 1, &setuptvgraberback);       // bryges af setup tv graber
    glDeleteTextures( 1, &_textureIdtv);						// bruges ikke
    glDeleteTextures( 1, &_textureIdmusic);			    // music
    glDeleteTextures( 1, &_textureIdfilm);			    // default film icon
    glDeleteTextures( 1, &_textureIdrecorded);			// default recorded icon
    glDeleteTextures( 1, &_texturemlast);						// bruges ikke
    glDeleteTextures( 1, &_texturemlast2);			    // bruges
    glDeleteTextures( 1, &_texturemnext);			      // next song
    glDeleteTextures( 1, &_texturemplay);		        // play song
    glDeleteTextures( 1, &_texturempause);          // pause play
    glDeleteTextures( 1, &_textureIdpup);		        //
    glDeleteTextures( 1, &_textureIdpdown);		      //
    glDeleteTextures( 1, &_texturemstop);			      // stop
    glDeleteTextures( 1, &_textureIdrecorded_aktiv);// film
    glDeleteTextures( 1, &_textureIdfilm_aktiv);	  // film
    glDeleteTextures( 1, &_textureIdmusicsearch);	  // search felt til music
    glDeleteTextures( 1, &_textureIdradiosearch);		// sang search
    glDeleteTextures( 1, &_textureIdmusicsearch1);	// artist search
    glDeleteTextures( 1, &_textureIdmoviesearch);	  // artist search
    glDeleteTextures( 1, &_textureIdloading);       //
    glDeleteTextures( 1, &_textureIdplayinfo);			// default show musicplay info
    glDeleteTextures( 1, &_textureIdclose);         //
    glDeleteTextures( 1, &_textureIdclose1);        //
    glDeleteTextures( 1, &_texturelock);			      // en lille hænge lås bruges i tvguide
    glDeleteTextures( 1, &_texturesetupmenu);			  // icons
    glDeleteTextures( 1, &_texturesetupmenu_select);//
    glDeleteTextures( 1, &_texturesoundsetup);      // setup
    glDeleteTextures( 1, &_texturesourcesetup);	  	// setup
    glDeleteTextures( 1, &_textureimagesetup);		  // setup
    glDeleteTextures( 1, &_texturetemasetup);		    // setup
    glDeleteTextures( 1, &_texturemythtvsql);		    // setup
    glDeleteTextures( 1, &_texturesetupfont);			  // setup
    glDeleteTextures( 1, &_texturesetupclose);		  //
    glDeleteTextures( 1, &_texturekeyssetup);		    // setup
    glDeleteTextures( 1, &_texturekeysrss);	  	    // setup rss
    glDeleteTextures( 1, &_texturespotify);         // setup spotify
    glDeleteTextures( 1, &_texturevideoplayersetup);// setup
    glDeleteTextures( 1, &_texturetvgrabersetup);   //
    glDeleteTextures( 1, &setupkeysbar1);			      // bruges af myth_setup.cpp
    glDeleteTextures( 1, &setupkeysbar2);			      // setupkeysbar1
    glDeleteTextures( 1, &tvprginfobig);			      // bruges til tv oversigt kanal info
    glDeleteTextures( 1, &_tvoverskrift);           // tv oversigt top window
    glDeleteTextures( 1, &_tvprgrecorded);			    // tv
    glDeleteTextures( 1, &_tvprgrecordedr);					// bruges ikke mere
    glDeleteTextures( 1, &_tvrecordbutton);			    // tv
    glDeleteTextures( 1, &_tvrecordcancelbutton);   //
    glDeleteTextures( 1, &_tvoldprgrecordedbutton); //
    glDeleteTextures( 1, &_tvnewprgrecordedbutton); //
    glDeleteTextures( 1, &setupsoundback);          //
    glDeleteTextures( 1, &setupsqlback);            //
    glDeleteTextures( 1, &setuptemaback);           //
    glDeleteTextures( 1, &setupnetworkback);        //
    glDeleteTextures( 1, &setupnetworkwlanback);    //
    glDeleteTextures( 1, &setupscreenback);         //
    glDeleteTextures( 1, &setupfontback);           //
    glDeleteTextures( 1, &setupkeysback);           //
    glDeleteTextures( 1, &setuprssback);            //
    glDeleteTextures( 1, &_texturesaveplaylist);    //
    glDeleteTextures( 1, &mobileplayer_icon);       // mobile icon
    glDeleteTextures( 1, &pcplayer_icon);           // pc player icon
    glDeleteTextures( 1, &unknownplayer_icon);      //
    glDeleteTextures( 1, &screenshot1);		          // screen shots
    glDeleteTextures( 1, &screenshot2);			        // screen shots
    glDeleteTextures( 1, &screenshot3);			        // screen shots
    glDeleteTextures( 1, &screenshot4);				      // screen shots
    glDeleteTextures( 1, &screenshot5);				      // screen shots
    glDeleteTextures( 1, &screenshot6);             // screen shots
    glDeleteTextures( 1, &screenshot7);             // screen shots
    glDeleteTextures( 1, &screenshot8);             // screen shots
    glDeleteTextures( 1, &screenshot9);             // screen shots
    glDeleteTextures( 1, &screenshot10);            // screen shots
    glDeleteTextures( 1, &_tvbar1);                 //
    glDeleteTextures( 1, &_tvbar3);                 //
    glDeleteTextures( 1, &spotify_icon_border);     // spotify icon border
    glDeleteTextures( 1, &onlineradio_empty);       //
    glDeleteTextures( 1, &onlineradio);		          // radio icon
    glDeleteTextures( 1, &onlineradio192);			    // radio icon
    glDeleteTextures( 1, &onlineradio320);			    // radio icon
    glDeleteTextures( 1, &radiobutton);             //
    glDeleteTextures( 1, &onlinestream);            // stream default icons
    glDeleteTextures( 1, &onlinestream_empty);      // stream default icons
    glDeleteTextures( 1, &onlinestream_empty1);     // stream default icons
    glDeleteTextures( 1, &musicbutton);             //
    glDeleteTextures( 1, &spotify_askopen);         //
    glDeleteTextures( 1, &spotify_search);          // button in spotify nomal view
    glDeleteTextures( 1, &spotify_search_back);     // back button in spotify search view
    glDeleteTextures( 1, &spotify_askplay);         //
    glDeleteTextures( 1, &spotifybutton);           //
    glDeleteTextures( 1, &spotify_ecover);          //
    glDeleteTextures( 1, &spotify_pil);             //
    glDeleteTextures( 1, &big_search_bar_playlist); //
    glDeleteTextures( 1, &big_search_bar_track);    //
    glDeleteTextures( 1, &big_search_bar_artist);   //
    glDeleteTextures( 1, &big_search_bar_albumm);   //
    glDeleteTextures( 1, &radiooptions);            //
    glDeleteTextures( 1, &_mainlogo);								// Main logo not in use any more
    glDeleteTextures( 1, &gfxlandemask);			      // lande mask
    glDeleteTextures( 1, &texturedot);              //
    glDeleteTextures( 1, &texturedot1);             //
    glDeleteTextures( 1, &_errorbox);               // error box
    glDeleteTextures( 1, &_textureexit);            //
    glDeleteTextures( 1, &_textureIdreset_search);  //
    // delete radio lande flags
    i = 0;
    while(i < 69) {
      if (gfxlande[i]) glDeleteTextures( 1, &gfxlande[i]);
      i++;
    }
    glDeleteTextures( 1,&_textureIdmusic_aktiv);      //
    glDeleteTextures( 1,&screensaverbox);             //
    glDeleteTextures( 1,&screensaverbox1);            //
    glDeleteTextures( 1,&newstuf_icon);               //
    glDeleteTextures( 1,&analog_clock_background);    // analog clock
}


// ****************************************************************************************
//
// Load contry flags
//
// ****************************************************************************************

void load_lande_flags() {
    int i;
    char tmpfilename[1024];
    char path[1024];
    char path2[10];
    // *********************************************************************
    // 1-9
    // 10-19
    // 20-29
    // 30-39
    // 40-49
    // 50-59
    // 60-69
    char *lande[]={(char *) "",(char *) "",(char *) "",(char *) "yu.png",(char *) "fr.png",(char *) "luxembourg.png",(char *) "nl.png",(char *) "usa.png",(char *) "de.png",(char *) "uk.png", //
                   (char *) "ru.png",(char *) "israel.png",(char *) "Austria.png",(char *) "lebanon.png",(char *) "latvia.png",(char *) "",(char *) "Vietnam.png",(char *) "Saudi-Arabia.png",(char *) "as.png",(char *) "brazil.png", //
                   (char *) "Egypt.png",(char *) "no.png",(char *) "pl.png",(char *) "se.png",(char *) "sw.png",(char *) "mexico.png",(char *) "be.png",(char *) "ca.png",(char *) "as.png",(char *) "ru.png", //
                   (char *) "sp.png",(char *) "ae.jpg",(char *) "hu.png",(char *) "th.png",(char *) "gr.png",(char *) "bk.png",(char *) "nu.png",(char *) "in.png",(char *) "po.png",(char *) "ir.png", //
                   (char *) "ks.png",(char *) "romania.png",(char *) "it.png",(char *) "fi.png",(char *) "bahrain.png",(char *) "dk.png",(char *) "chile.png",(char *) "chile.png",(char *) "Slovakia.png",(char *) "Ukraine.png", //
                   (char *) "hu.png",(char *) "co.png",(char *) "do.png",(char *) "Azerbaijan.png",(char *) "Lithuania.png",(char *) "Andorra.png",(char *) "Estonia.png",(char *) "Tajikistan.png",(char *) "Turkey.png",(char *) "Mongolia.png", //
                   (char *) "Belarus.png",(char *) "Slovenia.png",(char *) "Cyprus.png",(char *) "China.png",(char *) "Cambodia.png",(char *) "Indonesia.png",(char *) "Singapore.png",(char *) "Croatia.png",(char *) "Czech Republic.png",(char *) ""};
    i = 0;
    strcpy(path2,"");
    while(i < 69) {
      fprintf(stderr,"load flag %d %s \n",i,lande[i]);
      strcpy(tmpfilename,"/opt/mythtv-controller/images/");
      strcat(tmpfilename,lande[i]);							// add lande kode id fra table lande.
      strcpy(path,"/opt/mythtv-controller/images/");
      if (strcmp(lande[i],"") != 0) {
        gfxlande[i]=loadgfxfile(path2,path,(char *) lande[i]);
        //gfxlande[i]=loadTexture ((char *) tmpfilename);
      }
      i++;
    }
}

// ****************************************************************************************
//
// Init tema path
//
// ****************************************************************************************


void InitGL()              // We call this right after our OpenGL window is created.
{
    unsigned int i;
    char tmpfilename[256];
    char fileload[256];
    char temapath[256];
    strcpy(temapath,"");
    if (tema == 1) strcpy(temapath,"/opt/mythtv-controller/tema1/"); else
    if (tema == 2) strcpy(temapath,"/opt/mythtv-controller/tema2/"); else
    if (tema == 3) strcpy(temapath,"/opt/mythtv-controller/tema3/"); else
    if (tema == 4) strcpy(temapath,"/opt/mythtv-controller/tema4/"); else
    if (tema == 5) strcpy(temapath,"/opt/mythtv-controller/tema5/"); else
    if (tema == 6) strcpy(temapath,"/opt/mythtv-controller/tema6/"); else
    if (tema == 7) strcpy(temapath,"/opt/mythtv-controller/tema7/"); else
    if (tema == 8) strcpy(temapath,"/opt/mythtv-controller/tema8/"); else
    if (tema == 9) strcpy(temapath,"/opt/mythtv-controller/tema9/"); else
    if (tema == 10) strcpy(temapath,"/opt/mythtv-controller/tema10/"); else
    if (tema == 11) strcpy(temapath,"/opt/mythtv-controller/tema11/"); else
    if (tema == 12) strcpy(temapath,"/opt/mythtv-controller/tema12/"); else
    if (tema == 13) strcpy(temapath,"/opt/mythtv-controller/tema13/"); else
    if (tema == 14) strcpy(temapath,"/opt/mythtv-controller/tema14/"); else
    if (tema == 15) strcpy(temapath,"/opt/mythtv-controller/tema15/"); else
    if (tema == 16) strcpy(temapath,"/opt/mythtv-controller/tema16/"); else
    if (tema == 17) strcpy(temapath,"/opt/mythtv-controller/tema17/"); else
    if (tema == 18) strcpy(temapath,"/opt/mythtv-controller/tema18/"); else
    if (tema == 19) strcpy(temapath,"/opt/mythtv-controller/tema19/"); else
    if (tema == 20) strcpy(temapath,"/opt/mythtv-controller/tema20/"); else {
      strcpy(temapath,"tema1/");
      tema = 1;
    }

// Load radio stations gfx **********************************************
// virker vis aktiveres
// bliver loaded første gang de bruges
//    radiooversigt.load_radio_stations_gfx();
// *********************************************************************
//    mybox.settexture(musicoversigt);
}

// ****************************************************************************************
//
// bruges til at checke_copy radio icons som virker til nyt dir
//
// ****************************************************************************************

int check_radio_stations_icons() {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  bool dbexist=false;
  char resl[1024];
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
    mysql_query(conn,"select gfx_link from radio_stations where online=1 and gfx_link!=''");
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
      strcpy(resl,"cp radiostations/");
      strcat(resl,row[0]);
      strcat(resl," images/radiostations/");
      system(resl);
    }
  }
  return(1);
}


// ****************************************************************************************
//
// main
//
// ****************************************************************************************

int main(int argc, char** argv) {
    int dircreatestatus;
    Display *dpy = NULL;
    Window rootxwindow;
    strcpy(playlistfilename,"playlist");
    strcpy(movie_search_name,"");                                               // used then search for movies in movie view
    //printf("Build date  : %lu\n", (unsigned long) & __BUILD_DATE);
    //printf("Build number: %lu\n", (unsigned long) & __BUILD_NUMBER);
    printf("\n\nMythtv-controller Version %s \n",SHOWVER);
    sprintf(debuglogdata,"Mythtv-controller Version %s",SHOWVER);
    write_logfile((char *) debuglogdata);
    if (argc>1) {
      //if (strcmp(argv[1],"-f")==0) full_screen=1;
      if (strcmp(argv[1],"-h")==0) {
        printf("\n");
        printf("-f For full screen mode\n");
        printf("-b For border less screen mode\n");
        printf("-p For program guide mode\n");
        printf("-m For music mode\n");
        printf("-r For radio mode\n");
        printf("-f For film mode\n");
        printf("-s For podcast mode\n");
        printf("-v Show version\n");
        printf("-h This help screen\n\n");
        exit(0);
      }
      // show version and exit
      if (strcmp(argv[1],"-v")==0) {
        printf("\n\nVersion %s \n",SHOWVER);
        exit(0);
      }
    }
    numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    // write cpu info to log file
    sprintf(debuglogdata,"Numbers of cores :%d found.",numCPU);
    write_logfile((char *) debuglogdata);
    // Load config
    load_config((char *) "/etc/mythtv-controller.conf");				// load setup config
    // create dir for json files and icon files downloaded
    if (!(file_exists("~/spotify_json"))) {
      dircreatestatus = mkdir("~/spotify_json", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    // create dir
    if (!(file_exists("~/spotify_gfx"))) {
      dircreatestatus = mkdir("~/spotify_gfx", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    if ((strncmp(configbackend,"mythtv",5)==0) || (strncmp(configbackend,"any",3)==0)) configmythtvver=hentmythtvver(); 		// get mythtv-backend version
    if (strncmp(configbackend,"mythtv",5)==0) {
      write_logfile((char *) "mythtv - Backend");
      sprintf(debuglogdata,"configmysqluser   =%s ",configmysqluser);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"configmysqlhost   =%s ",configmysqlhost);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config movie path =%s ",configmoviepath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config music path =%s ",configmusicpath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config record path=%s ",configrecordpath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config hostname   =%s ",confighostname);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config fontname   =%s ",configfontname);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"Sound interface   =%s ",configsoundoutport);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"Default player    =%s ",configdefaultplayer);
      write_logfile((char *) debuglogdata);
      //fprintf(stderr,"Mythtv database version %d\n",configmythtvver);
      //fprintf(stderr,"configmysqluser   =%s \n",configmysqluser);
      //fprintf(stderr,"configmysqlhost   =%s \n",configmysqlhost);
      //fprintf(stderr,"config movie path =%s \n",configmoviepath);
      //fprintf(stderr,"config music path =%s \n",configmusicpath);
      //fprintf(stderr,"config record path=%s \n",configrecordpath);
      //fprintf(stderr,"config hostname   =%s \n",confighostname);
      //fprintf(stderr,"config fontname   =%s \n",configfontname);
      //fprintf(stderr,"Sound interface   =%s \n",configsoundoutport);
      //fprintf(stderr,"Default player    =%s \n",configdefaultplayer);
    }
    if (strncmp(configbackend,"xbmc",4)==0) {
      write_logfile((char *) "XBMC - Backend");
      sprintf(debuglogdata,"configmysqluser   =%s ",configmysqluser);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"configmysqlhost   =%s ",configmysqlhost);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config movie path =%s ",configmoviepath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config music path =%s ",configmusicpath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config record path=%s ",configrecordpath);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config hostname   =%s ",confighostname);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"config fontname   =%s ",configfontname);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"Sound interface   =%s ",configsoundoutport);
      write_logfile((char *) debuglogdata);
      sprintf(debuglogdata,"Default player    =%s ",configdefaultplayer);
      write_logfile((char *) debuglogdata);
      //fprintf(stderr,"sqluser           =%s \n",configmysqluser);
      //fprintf(stderr,"host              =%s \n",configmysqlhost);
      //fprintf(stderr,"config movie path =%s \n",configmoviepath);
      //fprintf(stderr,"config music path =%s \n",configmusicpath);
      //fprintf(stderr,"config record path=%s \n",configrecordpath);
      //fprintf(stderr,"config hostname   =%s \n",confighostname);
      //fprintf(stderr,"config fontname   =%s \n",configfontname);
      //fprintf(stderr,"Sound interface   =%s \n",configsoundoutport);
      //fprintf(stderr,"Default player    =%s \n",configdefaultplayer);
    }
    if (debugmode) {
      fprintf(stderr,"Debug mode selected ");
      switch (debugmode) {
        case 1: fprintf(stderr,"Wifi network. \n");
                break;
        case 2: fprintf(stderr,"Music. \n");
                break;
        case 4: fprintf(stderr,"Stream. \n");
                break;
        case 8: fprintf(stderr,"Keyboard/other events. \n");
                break;
        case 16:fprintf(stderr,"Movie. \n");
                break;
        case 32:fprintf(stderr,"Search. \n");
                break;
        case 64:fprintf(stderr,"Stream search. \n");
                break;
        case 128:fprintf(stderr,"Stream search. \n");
                break;
        case 256:fprintf(stderr,"Tv program stuf. \n");
                break;
        case 512:fprintf(stderr,"Media importer. \n");
                break;
      }
    }

    #ifdef ENABLE_TIDAL
    tidal_oversigt = new tidal_class;
    if (tidal_oversigt) {
      //tidal_oversigt->start_webserver();
      //tidal_oversigt->tidal_login();
    }
    //tidal_oversigt->tidal_login_token();
    // in use tridal_oversigt.tridal_login_token2
    //tridal_oversigt.tridal_login_token2();
//    tridal_oversigt.tridal_play_playlist("742185f0-fc32-4865-870a-c251a20dc160");
    #endif



    // Create radio mysql database if not exist
    if (create_radio_oversigt()) {
      radiooversigt_antal = radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
      strcpy(configbackend_tvgraber_old,"");
    } else {
      write_logfile((char *) "Exit program, Not posible to load radio overview.");
      exit(1);
    }
    // if kodi
    if ((strncmp(configbackend,"xbmc",4)==0) || (strncmp(configbackend,"kodi",4)==0)) {
      // music loader
      pthread_t loaderthread;           // the load
      int rc=pthread_create(&loaderthread,NULL,xbmcdatainfoloader,NULL);
      if (rc) {
        fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
      // movie loader
      pthread_t loaderthread1;           // the load
      int rc1=pthread_create(&loaderthread1,NULL,xbmcdatainfoloader_movie,NULL);
      if (rc1) {
        fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
        exit(-1);
      }
    } else if (strncmp(configbackend,"mythtv",5)==0) {
      // music loader
      if (configmythtvver>=0) {
        pthread_t loaderthread;
        int rc = pthread_create(&loaderthread,NULL,datainfoloader_music,NULL);
        if (rc) {
         fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
        }
      }
      // movie loader
      if (configmythtvver>=0) {
        pthread_t loaderthread1;           // the load
        int rc1 = pthread_create(&loaderthread1,NULL,datainfoloader_movie,NULL);
        if (rc1) {
          fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
          exit(-1);
        }
      }
    }
    // stream loader
    pthread_t loaderthread2;           // the load
    int rc2 = pthread_create(&loaderthread2,NULL,datainfoloader_stream,NULL);
    if (rc2) {
      fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
    // Load the VLC engine
//    vlc_inst = libvlc_new(5,opt);
/*
    vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");
    // Create a media player playing environement
    vlc_mp = libvlc_media_player_new_from_media(vlc_m);
    libvlc_media_release(vlc_m);
    libvlc_media_player_play(vlc_mp);
*/
    // stop vlc play again
    //sleep(10); // play
    //libvlc_media_player_stop(vlc_mp);
    //libvlc_media_player_release(vlc_mp);
    sock=initlirc();
    // bruges til at checke_copy radio icons som virker til nyt dir
    //check_radio_stations_icons();
    glutInit(&argc, argv);
    // init return(1) if error / 2 if ok
    init_sound_system(soundsystem);                             // Init sound
    ilInit();                                                   // Initialization of DevIL
    // rember screeen size
    orgwinsizex=glutGet(GLUT_SCREEN_WIDTH);
    orgwinsizey=glutGet(GLUT_SCREEN_HEIGHT);
    if (orgwinsizex==1366) screen_size=4;
    if (orgwinsizex==1920) screen_size=3;
    sprintf(debuglogdata,"Real size %dx%d",orgwinsizex,orgwinsizey);
    write_logfile((char *) debuglogdata);
    if (orgwinsizex>1920) orgwinsizex=1920;
    if (orgwinsizey>1080) orgwinsizey=1080;
    sprintf(debuglogdata,"Screen size %dx%d",orgwinsizex,orgwinsizey);
    write_logfile((char *) debuglogdata);
    sprintf(debuglogdata,"Screen mode %d",screen_size);
    write_logfile((char *) debuglogdata);
    // get first monitor screen size (pixel)
    dpy = XOpenDisplay(":1");
    if (dpy) {
      rootxwindow = RootWindow(dpy, 0);
      XRRScreenResources *xscreen_conf = XRRGetScreenResources(dpy, rootxwindow);
      XRRCrtcInfo *crtc_info = XRRGetCrtcInfo (dpy, xscreen_conf, xscreen_conf->crtcs[0]);
      if (orgwinsizex>crtc_info->width) {
        printf("Monitor 0 screen size x:y %d:%d \n",crtc_info->width,crtc_info->height);
        // change screen size to monitor 0 screen mode
        orgwinsizex=crtc_info->width;
        orgwinsizey=crtc_info->height;
      }
      int nrofscreens=XScreenCount(dpy);
    }
    //printf("Nr of screens found : %d\n",nrofscreens);
    // create loader xorg window
    //Window w = XCreateWindow(dpy, DefaultRootWindow(dpy), 100, 100, 400,200, 0, CopyFromParent, CopyFromParent,CopyFromParent, 0, 0);
    // Show the window
    //XMapWindow(dpy, w);
    //GC gc=XCreateGC(dpy, w, 0,0);
    // set title
    //XStoreName(dpy, w, "Mythtv-controller status window");
    //XDrawString(dpy,w,gc,1,1, "test", 4);
    //XFlush(dpy);
    //XDrawString(dpy, w,  gc ,	0,	0,	"abc",	3);
    //XDrawString(dpy, w, gc, 10, 10, "TEST", strlen("TEST"));
    //XFlush(dpy);
    //sleep(10);
     // Sleep long enough to see the window.
    InitGL();
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE); // (GLUT_SINGLE | GLUT_RGB);
    // set screen size
    glutInitWindowSize (orgwinsizex, orgwinsizey);
    //
    glutInitWindowPosition (0, 0);
    char overskrift[200];
    char overskrift1[200];
    strcpy(overskrift,"mythtv-controller ");
    //sprintf(overskrift1,"%l",(long unsigned int) & __BUILD_NUMBER);
    //strcat(overskrift,overskrift1);
    glutCreateWindow (overskrift);
    init();                                           // init gopengl
    write_logfile((char *) "Mythtv-controller startup.");
    write_logfile((char *) "Loading graphic.");
    loadgfx();                                        // load gfx stuf
    write_logfile((char *) "Graphic loaded.");
    if (full_screen) {
      write_logfile((char *) "Enter full screen mode.");
      glutFullScreen();                // set full screen mode
    }
    glutDisplayFunc(display);                         // main loop func
    glutIdleFunc(NULL);                               // idle func
    glutKeyboardFunc(handleKeypress);                 // setup normal key handler
    glutSpecialFunc(handlespeckeypress);              // setup spacial key handler
    glutMouseFunc(handleMouse);                       // setup mousehandler
    glutTimerFunc(25, update2, 0);                    // set start loop
    init_ttf_fonts();                                 // init fonts
    #ifdef ENABLE_SPOTIFY
    if (spotify_oversigt.spotify_check_spotifydb_empty()==true) firsttimespotifyupdate=false; // if true show update option first time
    #endif
    // select start func if argc is this
    if ((argc>1) && (strcmp(argv[1],"-p")==0)) vis_tv_oversigt = true;
    if ((argc>1) && (strcmp(argv[1],"-r")==0)) vis_radio_oversigt = true;
    if ((argc>1) && (strcmp(argv[1],"-m")==0)) vis_music_oversigt = true;
    if ((argc>1) && (strcmp(argv[1],"-f")==0)) vis_film_oversigt = true;
    if ((argc>1) && (strcmp(argv[1],"-s")==0)) vis_stream_oversigt = true;
    #ifdef ENABLE_SPOTIFY
    if ((argc>1) && (strcmp(argv[1],"-y")==0)) vis_spotify_oversigt = true;
    #endif
    #ifdef ENABLE_TIDAL
    if ((argc>1) && (strcmp(argv[1],"-t")==0)) vis_tidal_oversigt = true;
    #endif
    // select font from configfile (/etc/mythtv-controller.conf)
    aktivfont.selectfont(configfontname);
    printf("\nHardware           %s\n",(char *)glGetString(GL_RENDERER));                         // Display Renderer
    printf("OpenGL Render      %s\n",(char *)glGetString(GL_VENDOR));                           // Display Vendor Name
    printf("Version            %s\n",(char *)glGetString(GL_VERSION));
    // start main loop now
    // start main loop now
    glutMainLoop();
    write_logfile((char *) "Close down.");
    #if defined USE_FMOD_MIXER
    result=sound->release();
    ERRCHECK(result,0);
    result=sndsystem->close();
    ERRCHECK(result,0);
    result=sndsystem->release();                                // release sound system
    ERRCHECK(result,0);
    #endif
    #if defined USE_SDL_MIXER
    Mix_Quit();
    #endif
    freegfx();                                                  // free gfx

    write_logfile((char *) "Exit program.");
    return(EXIT_SUCCESS);
}
