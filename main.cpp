#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// opengl
#include <GL/gl.h>
#include <GL/glut.h>
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
#include <time.h>                       // have strftime
#include <sqlite3.h>                    // sqlite interface to xbmc(kodi)
// vlc plugin
#include <vlc/vlc.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
// time
#include <ctime>
// file io
#include <iostream>

extern char   __BUILD_DATE;
extern char   __BUILD_NUMBER;

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

#include <pthread.h>                   // multi thread support

// sound system include fmod
#if defined USE_FMOD_MIXER
#include "/usr/share/mythtv-controller/fmodstudioapi10906linux/api/lowlevel/inc/fmod.hpp"
#include "/usr/share/mythtv-controller/fmodstudioapi10906linux/api/lowlevel/inc/fmod_errors.h"
#endif

FMOD::DSP* dsp = 0;                   // fmod Sound device

// from compiler (debuger)
extern char __BUILD_DATE;
extern char __BUILD_NUMBER;
//

#include "main.h"
#include "myctrl_storagedef.h"
#include "myth_saver.h"
#include "myth_setup.h"
#include "myth_setupsql.h"
#include "myctrl_xbmc.h"
#include "myctrl_movie.h"
#include "myctrl_music.h"
#include "myctrl_radio.h"
#include "myctrl_tvprg.h"
#include "myctrl_stream.h"
#include "myctrl_recorded.h"
#include "myctrl_mplaylist.h"
#include "checknet.h"
#include "myth_ttffont.h"
#include "readjpg.h"
// #include "myth_picture.h"


// struct used by keyboard config of functions keys

struct configkeytype {
    char cmdname[200];
    unsigned int scrnr;
};

int numCPU;                                             // have the numbers of cpu cores
char systemcommand[2000];                               // shell command to do to play recorded program mplayer eks.
const char *dbname="mythtvcontroller";                  // internal database name in mysql (music,movie,radio)
// koki db names (by version)
const char *kodiver[6]={"MyMusic60.db","MyMusic56.db","MyMusic52.db","MyMusic48.db","MyMusic46.db","MyMusic32.db"};
// ************************************************************************************************
char keybuffer[512];                                    // keyboard buffer
unsigned int keybufferindex=0;                          // keyboard buffer index
int findtype=0;					                              	// bruges af search kunster/sange
unsigned int do_show_setup_select_linie=0;              // bruges af setup
bool do_save_config=false;                              // flag to save config

channel_list_struct channel_list[MAXCHANNEL_ANTAL];     // channel_list array used in setup graber (default max 400) if you wats to change it look in myth_setup.h
channel_configfile  xmltv_configcontrol;                //

bool firsttime_xmltvupdate=true;                        // update tvguide xml files first start (force)
// ************************************************************************************************
char configmysqluser[256];                              // /mythtv/mysql access info
char configmysqlpass[256];                              //
char configmysqlhost[256];                              //
// ************************************************************************************************
char configxbmcuser[256];                               // /mythtv/mysql access info
char configxbmcpass[256];                               //
char configxbmchost[256];                               //
// ************************************************************************************************
bool hent_tv_channels=false;
long configtvguidelastupdate=0;                         // last date /unix time_t type) tvguide update
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
bool do_shift_song=false;				                        // jump to next song to play
bool do_stop_music=false;                               // stop all play
bool do_stop_radio=false;                               // stop all play
int soundsystem=0;		      		                      	// used sound system 1=FMOD 2=OPENSOUND
int numbersofsoundsystems=0;                            // antal devices
unsigned int musicoversigt_antal=0;                     // antal aktive sange
int do_zoom_music_cover_remove_timeout=0;
int showtimeout=600;
int orgwinsizex,orgwinsizey;
                                                        // 1  = wifi net
                                                        // 2  = music
                                                        // 4  = stream
                                                        // 8  = keyboard/mouse move
                                                        // 16 = movie
                                                        // 32 = searcg
int debugmode=2+1;                                        // 64 = radio station land icon loader
                                                        // 128= stream search
                                                        // 256 = tv program stuf
                                                        // 512 = media importer
                                                        // 1024 = flag loader

char music_db_update_loader[256];                       //
bool loading_tv_guide=false;                            // loading_tv_guide true if loading
int tvchannel_startofset=0;
bool showfps=true;
int configmythtvver=0;            			                // mythtv config found version
bool stopmovie=false;
int film_key_selected=1;                                // den valgte med keyboard i film oversigt
int vis_volume_timeout=0;
int music_key_selected=0;
bool ask_open_dir_or_play=false;
bool ask_open_dir_or_play_aopen=false;
bool do_swing_music_cover=true;                         // default swing music cover
int music_selected_iconnr=0;                            // default valgt icon i music oversigt
float _angle=0.00;                                      // bruges af 3d screen saver
bool full_screen=true;
int do_play_music_aktiv_table_nr=0;                     // aktiv sang nr
int do_play_music_aktiv_nr_select_array=0;
bool show_uv=true;                                      // default show uv under menu
bool vis_uv_meter=false;                                 // uv meter er igang med at blive vist
bool hent_music_search=false;                           // skal vi søge efter music
bool keybufferopenwin=false;                            // er vindue open
bool do_play_music_cover=false;
bool do_find_playlist=false;
bool do_play_music_aktiv_play=false;

bool do_play_music_aktiv=false;
bool do_stop_music_all=false;
char aktivplay_music_path[1024];

bool hent_radio_search=false;                           // skal vi søge efter music
bool hent_stream_search=false;                          // skal vi søge efter stream

int do_music_icon_anim_icon_ofset=0;                  	// sin scrool ofset for show fast music

int sinusofset=0;      					                        // bruges af show_music_oversigt

int setupsinofset=0;

bool show_volume_info=false;                        	// show vol info
int show_volume_info_timeout=0;                   		// time out to close vol info again

bool stream_loadergfx_started=false;                	// thread stream gfx loader running ?
bool stream_loadergfx_started_done=false;            	// thread stream gfx loader running ?
bool stream_loadergfx_started_break=false;

bool vis_music_oversigt=false;                          // vis music player
bool vis_film_oversigt=false;				                    // vis film oversigt
bool vis_recorded_oversigt=false;                       // vis recorded oversigt
bool vis_tv_oversigt=false;                             // vis tv oversigt
bool vis_radio_oversigt=false;                          // vis radio player

bool vis_old_recorded=false;
bool vis_tvrec_list=false;

bool saver_irq=false;

bool radio_oversigt_loaded=false;
bool radio_oversigt_loaded_done=0;

unsigned int do_show_play_open_select_line=0;           // bruges til at vælge hvilken music filer som skal spilles
unsigned int do_show_play_open_select_line_ofset=0;     // bruges til at vælge hvilken music filer som skal spilles (start ofset)

bool show_radio_options=false;
int radio_select_iconnr=0;
float _rangley;

bool do_show_setup=false;                               // show setup menu
bool do_show_setup_sound=false;
bool do_show_setup_screen=false;
bool do_show_setup_tema=false;
bool do_show_setup_sql=false;
bool do_show_setup_network=false;
bool do_show_setup_font=false;
bool do_show_setup_keys=false;
bool do_show_videoplayer=false;
bool do_show_tvgraber=false;
bool use3deffect=false;                                 // use 3d scroll effect default no
bool do_zoom_music_cover=false;
bool do_zoom_radio=false;

bool show_wlan_select=false;

bool do_zoom_film_cover=false;
bool vis_movie_options=false;
bool vis_movie_sort_option=false;

bool vis_stream_oversigt=false;
bool startstream=false;
bool do_play_stream=false;

int rknapnr=0;                                           // buttons vars
int sknapnr=0;
int mknapnr=0;
int tvknapnr=0;
int fknapnr=0;
int swknapnr=0;

int stream_key_selected=0;
int stream_select_iconnr=0;
int do_zoom_tvprg_aktiv_nr=0;

int PRGLIST_ANTAL=0;                                      // used in tvguide xml program selector

int music_select_iconnr;

int antal_songs=0;

int _sangley;
int _mangley;
int _angley;

int music_icon_anim_icon_ofset=0;
int music_icon_anim_icon_ofsety=0;


int do_play_music_aktiv_nr=0;                           // den aktiv dirid som er trykket på


int screenx=1920;
int screeny=1080;


// xbmc/kodi db version files
int kodiverfound=0;


const float textcolor[3]={0.8f,0.8f,0.8f};
const float selecttextcolor[3]={0.4f,0.4f,0.4f};


float _fangley=0.0f;					// bruges af vis_film_oversigt glob

int do_zoom_film_aktiv_nr=0;

bool vis_radio_or_music_oversigt=false;
bool vis_stream_or_movie_oversigt=false;
bool global_use_internal_music_loader_system=false;     // use internal db for musicdb or mysql/kodi/

bool ask_tv_record=false;
bool do_play_radio=false;

GLint ctx, myFont;

bool do_swing_movie_cover=false;                        // do anim
bool vis_nyefilm_oversigt=true;                         // start med at vise nye film

// stream

stream_class streamoversigt;
bool show_stream_options=false;


bool startmovie=false;                  		// start play movie


int sleep_ok=0;
int sleeper=1;

unsigned int percent;
bool starving;

char aktivsongname[40];                         	// song name
char aktivartistname[40];                      		// navn på aktiv artist (som spilles)

bool check_radio_thread=true;            			  	// DO NOT check radio station online status

const unsigned int ERROR_TIMEOUT=120;                    // show error timeout
int vis_error_timeout=ERROR_TIMEOUT;
bool vis_error=false;
int vis_error_flag=0;
int vis_error_songnr;

int do_stream_icon_anim_icon_ofset=0;                   //
int stream_icon_anim_icon_ofset=0;                      //
unsigned int streamoversigt_antal=0;                            // antal radio stationer


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
int fmodbuffersize=32*1024;
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

extern mplaylist aktiv_playlist;

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
            for(unsigned int i=0;i<listesize;i++) {			// reset all music info
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
            for(unsigned int i=0;i<listesize;i++) songliste[i].aktiv=true;	// set play flag for all music
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
// setup fmod sound device
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

int urtype=1;                           // set default screen saver

// *************************************************************************************************

recorded_overigt recordoversigt;       // optaget programer oversigt
bool reset_recorded_texture=false;


// *************************************************************************************************

wifinetdef wifinets;                            // wifi net class

// music oversigt struct
struct music_oversigt_type musicoversigt[MUSIC_OVERSIGT_TYPE_SIZE+1];

// *************************************************************************************************
// film oversigt type class
film_oversigt_typem film_oversigt(FILM_OVERSIGT_TYPE_SIZE+1);

// Screen saver box ********************************************************************************
boxarray mybox;                         // 3D screen saver
// *************************************************************************************************


// radio
radiostation_class radiooversigt;         // lav top radio statio oversigt
int aktiv_radio_station=0;                //
int radio_key_selected=0;                 //
int radiooversigt_antal=0;                // antal aktive sange
GLint cur_avail_mem_kb = 0;               // free nvidia memory (hvis 0 så ændres gfx zoom lidt så det passer på ati/intel)
GLuint _textureutvbgmask;                 // background in tv guide programs
GLuint _textureId1;                     	// The id of the texture
GLuint _textureId2;                     	// error window
GLuint _defaultdvdcover;                	// The id of the texture
GLuint _defaultdvdcover2;	                // The id of the texture
GLuint _texturemovieinfobox;	            //  movie image
GLuint _textureId7; 	                    // folder image
GLuint _textureId7_1; 	                  // folder image
GLuint _textureId7_2;	                    // folder image
GLuint _textureId7_3;	                    // folder mask image
GLuint _textureId7_4;	                    // folder trans mask image
GLuint _texturemusicplayer; 	            // music image		// show player
GLuint _textureId9_askbox; 	              // askbox image
GLuint _textureId9_2; 	                  // askbox music image
GLuint _textureId10; 	                    // play icon
GLuint _textureopen; 	                    // open icon
GLuint _textureclose; 	                  // close icon
GLuint _textureswap; 	                    // swap icon
GLuint _textureId11; 	                    // tv program oversigt logo
GLuint _textureId12; 	                    // background 1
GLuint _textureId12_1; 	                  // background 2
GLuint _textureId14; 	                    // pause knap
GLuint _textureId15; 	                    // lille cover mask
GLuint _textureId16; 	                    // box2.bmp
GLuint _dvdcovermask; 	                  // dvdcovermask
GLuint _textureId20; 	                    // mask movie options box
GLuint _textureId21; 	                    // mask movie options box
GLuint _textureId22; 	                    // move options box
GLuint _textureId23; 	                    // movie options box
GLuint _textureId24; 	                    // movie options box
GLuint _textureId25; 	                    //
GLuint _textureId26; 	                    //
GLuint _textureId27; 	                    //
GLuint _textureId28; 	                    // dir playlist_icon
GLuint _textureIdback; 	                  //
GLuint _textureId29_1; 	                  // mask
GLuint _textureuv1;                       // uv img
GLuint _textureuv1_top;                   // uv img
GLuint _errorbox;	                        //


// radio view icons
GLuint onlineradio;                       //
GLuint onlineradio_empty;                 //
GLuint onlineradio192;                    //
GLuint onlineradio320;                    //
GLuint onlineradiomask;                   //
GLuint radiooptions;                      //
GLuint radiooptionsmask;                  //
GLuint radiobutton;                       //
GLuint musicbutton;                       //
GLuint streambutton;                      //
GLuint moviebutton;                       //

GLuint gfxlandemask;
GLuint gfxlande[80];
GLuint texturedot;

// loading window
GLuint _textureIdloading;                   // loading window
GLuint _textureIdloading1;                  // empty window

// setup menu textures
GLuint setuptexture;
GLuint setuptvgraberback;
GLuint setupsoundback;
GLuint setupsqlback;
GLuint setupnetworkback;
GLuint setupnetworkwlanback;
GLuint setupscreenback;
GLuint setuptemaback;
GLuint setupfontback;
GLuint setupkeysback;
GLuint _texturesetupclose;

// setup menu textures
GLuint _texturesoundsetup;                  // setup icon
GLuint _texturesourcesetup;                 //
GLuint _textureimagesetup;                  //
GLuint _texturetemasetup;                   //
GLuint _texturemythtvsql;                   //
GLuint _texturesetupfont;                   //
GLuint _texturekeyssetup;                   //
GLuint _texturevideoplayersetup;            //
GLuint _texturetvgrabersetup;            //

GLuint _texturelock;

GLuint setupkeysbar1;                       // bruges af setup
GLuint setupkeysbar2;                       // bruges af setup

GLuint screenshot1,screenshot2,screenshot3,screenshot4,screenshot5,screenshot6,screenshot7,screenshot8,screenshot9,screenshot10;

GLuint _textureIdback_main=0,_textureIdback_music=0;            // main background
GLuint _textureIdback_setup;
GLuint  screensaverbox;
//GLuint _tvoldrecordedmask;
GLuint _textureIdmusic_aktiv;
GLuint _tvoldrecorded;
GLuint screensaverbox_mask;
GLuint _tvmaskprgrecordedbutton;
GLuint _tvbar1,_tvbar3;
GLuint _tv_prgtype;
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
GLuint _textureIdclosemask;
GLuint _texturesetupmenu;
GLuint _textureIdclose;
GLuint _textureIdclose1;
GLuint _texturemplay;
GLuint _texturemstop;
GLuint _texturemnext;
GLuint _textureIdradiosearch;
GLuint _textureIdplayinfo;
GLuint _textureIdmusicsearch1;
GLuint _textureIdmusicsearch;
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
GLuint radiomusicbuttonmask;
GLuint onlinestreammask;
GLuint onlinestreammaskicon;
GLuint streammoviebuttonmask;

GLuint _textureIdmusic_mask_anim[10];    // texture array to anim of music menu icon

fontctrl aktivfont;                     // font control

unsigned int setupfontselectofset=0;            // valgte font i oversigt
unsigned int setupwlanselectofset=0;            // valgte wlan nr i oversigt

const int TEMA_ANTAL=10;                                        // numbers of tema


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
        mysql_close(conn);
    }
    return(atoi(txtversion));
}


// ********************************************************** sound system error handler


#if defined USE_FMOD_MIXER

// fmod error handler

void ERRCHECK(FMOD_RESULT result,unsigned int songnr)

{
    char file_path[1024];
    if (result != FMOD_OK) {
      fprintf(stderr,"FMOD error! (%d): %s on songnr %d \n", result, FMOD_ErrorString(result),songnr);
      if (vis_music_oversigt) {
        aktiv_playlist.m_play_playlist(file_path,songnr);
        fprintf(stderr,"File name %s\n",file_path);
      }
      vis_error_flag=result;
      vis_error=true;			// set vis error flag
      vis_error_songnr=songnr;		// gem fil navn som ikke kan spilles
      vis_error_timeout=ERROR_TIMEOUT;
      do_zoom_radio=false;		// close play info
      if ((result!=23) && (result!=37) && (result!=27) && (result!=20)) {
        fprintf(stderr,"FMOD ERROR %d %s ",result,FMOD_ErrorString(result));
//          exit(-1);
      }
    }
}

#endif



void ERRCHECK_SDL(char *text,unsigned int songnr)

{
    char file_path[1024];
    if (vis_music_oversigt) {
      aktiv_playlist.m_play_playlist(file_path,songnr);
      fprintf(stderr,"File name %s error : %s\n",file_path,text);
    }
    vis_error=1;			// vis error
    if (strcmp(text,"music parameter was NULL")==0) vis_error_flag=36;
    vis_error=true;			// set vis error falg
    do_zoom_radio=false;            // close play info
    vis_error_songnr=songnr;          // gem fil navn som ikke kan spilles
    vis_error_timeout=ERROR_TIMEOUT;
}




//
// config file paser
//
// enum def commands


int parse_config(char *filename) {
    char buffer[513];
    FILE *fil;
    int n,nn;
    enum commands {setmysqlhost, setmysqluser, setmysqlpass, setsoundsystem, setsoundoutport, setscreensaver, setscreensavername,setscreensize, \
                   settema, setfont, setmouse, setuse3d, setland, sethostname, setdebugmode, setbackend, setscreenmode, setvideoplayer,setconfigdefaultmusicpath,setconfigdefaultmoviepath,setuvmetertype,setvolume,settvgraber,tvgraberupdate,tvguidercolor};
    int commandlength;
    char value[200];
    bool command=false;
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
            command=false;
            valueok=false;
            // remove all spaces
            while((n<strlen(buffer)) && (*(buffer+n)==32)) n++;

            if (n!=strlen(buffer)) {
                // test for command
                if (buffer[n]!='#') {
                    if (strncmp(buffer+n,"backend",6)==0) {
                        command=true;
                        command_nr=setbackend;
                        commandlength=6;
                    } else if (strncmp(buffer+n,"mythhost",7)==0)  {
                        command=true;
                        command_nr=sethostname;
                        commandlength=7;
                    } else if (strncmp(buffer+n,"mysqlhost",8)==0) {
                        command=true;
                        command_nr=setmysqlhost;
                        commandlength=8;
                    } else if (strncmp(buffer+n,"mysqluser",8)==0) {
                        command=true;
                        command_nr=setmysqluser;
                        commandlength=8;
                    } else if (strncmp(buffer+n,"mysqlpass",8)==0) {
                        command=true;
                        command_nr=setmysqlpass;
                        commandlength=8;
                    } else if (strncmp(buffer+n,"soundsystem",10)==0) {
                        command=true;
                        command_nr=setsoundsystem;
                        commandlength=10;
                    } else if (strncmp(buffer+n,"soundoutport",11)==0) {
                        command=true;
                        command_nr=setsoundoutport;
                        commandlength=11;
                    } else if (strncmp(buffer+n,"screensaver=",12)==0) {
                        command=true;
                        command_nr=setscreensaver;
                        commandlength=10;
                    } else if (strncmp(buffer+n,"screensavername",14)==0) {
                        command=true;
                        command_nr=setscreensavername;
                        commandlength=14;
                    } else if (strncmp(buffer+n,"screensize",9)==0) {
                        command=true;
                        command_nr=setscreensize;
                        commandlength=9;
                    } else if (strncmp(buffer+n,"tema",3)==0) {
                        command=true;
                        command_nr=settema;
                        commandlength=3;
                    } else if (strncmp(buffer+n,"font",3)==0) {
                        command=true;
                        command_nr=setfont;
                        commandlength=3;
                    } else if (strncmp(buffer+n,"mouse",4)==0) {
                        command=true;
                        command_nr=setmouse;
                        commandlength=4;
                    } else if (strncmp(buffer+n,"use3d",4)==0) {
                        command=true;
                        command_nr=setuse3d;
                        commandlength=4;
                    } else if (strncmp(buffer+n,"land",3)==0) {
                        command=true;
                        command_nr=setland;
                        commandlength=3;
                    } else if (strncmp(buffer+n,"fullscreen",9)==0) {
                        command=true;
                        command_nr=setscreenmode;
                        commandlength=9;
                    } else if (strncmp(buffer+n,"configdefaultmusicpath",21)==0) {
                      command=true;
                      command_nr=setconfigdefaultmusicpath;
                      commandlength=21;
                    } else if (strncmp(buffer+n,"configdefaultmoviepath",21)==0) {
                      command=true;
                      command_nr=setconfigdefaultmoviepath;
                      commandlength=21;
                    } else if (strncmp(buffer+n,"videoplayer",10)==0) {
                        command=true;
                        command_nr=setvideoplayer;
                        commandlength=10;
                    } else if (strncmp(buffer+n,"debug",4)==0) {
                        command_nr=setdebugmode;
                        command=true;
                        commandlength=4;
                        debugmode=atoi(value);		// set debug mode from config file
                        showfps=true;
                    } else  if (strncmp(buffer+n,"uvmetertype",10)==0) {
                        command=true;
                        command_nr=setuvmetertype;
                        commandlength=10;
                    } else if (strncmp(buffer+n,"defaultvolume",12)==0) {
                        command=true;
                        command_nr=setvolume;
                        commandlength=12;
                    } else if (strncmp(buffer+n,"tvgraberupdate",13)==0) {
                      command=true;
                      command_nr=tvgraberupdate;
                      commandlength=13;
                      // vi do not have tvgraberpath in config file
                    } else if ((strncmp(buffer+n,"tvgraber",7)==0) && (strncmp(buffer+n,"tvgraberpath",11)!=0)) {
                      command=true;
                      command_nr=settvgraber;
                      commandlength=7;
                    } else if (strncmp(buffer+n,"tvguidercolor",12)==0) {
                      command=true;
                      command_nr=tvguidercolor;
                      commandlength=12;
                    } else command=false;
                }
                strcpy(value,"");
                if (command) {
                    while((n<strlen(buffer)) && (!(valueok))) {
                        if ((buffer[n]!=10) && (buffer[n]!='=')) {
                            if ((*(buffer+n)!='=') && (*(buffer+n)!=' ') && (*(buffer+n)!=10) && (*(buffer+n)!='\'') && (*(buffer+n)!=13)) {
                                valueok=true;
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
                      printf("*********** Set config mode MYTHTV/XBMC+KODI  ***********\n");
                      printf("Mode selected : %s\n",configbackend);
                      printf("*********************************************************\n");
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

                      printf("**************** Set config xmltv graber ****************\n");
                      printf("Tv graber ....: %s\n",configbackend_tvgraber);
                      printf("*********************************************************\n");
                    } else if (command_nr==tvgraberupdate) configtvguidelastupdate=atol(value);
                    // set tvguide color or no color
                    else if (command_nr==tvguidercolor) {
                     if (strcmp(value,"yes")==0) aktiv_tv_oversigt.vistvguidecolors=true;
                     else aktiv_tv_oversigt.vistvguidecolors=false;
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
                            showfps=false;
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
                      if (strcmp(value,"true")==0) full_screen=true; else full_screen=false;
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
                            use3deffect=true;
                            strcpy(configuse3deffect,"yes");
                        } else {
                            use3deffect=false;
                            strcpy(configuse3deffect,"no");
                        }
                    } else if (command_nr==setland) {
                      configland=1; // set default land code
                    } else if (command_nr==setuvmetertype) {
                      configuvmeter=atoi(value);
                    } else if (command_nr==setvolume) {
                      configsoundvolume=atof(value);                         // set default volume under play
                    }
                }
            }
            strcpy(buffer,"");
        }
        fclose(fil);
    } else return(0);
    if (check_zerro_bytes_file(filename)>0) return(1); else return(0);
}




// save config to file

int save_config(char * filename) {
    bool error=false;
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
        //aktiv_tv_oversigt.vistvguidecolors=true;
        if (aktiv_tv_oversigt.vistvguidecolors) sprintf(temp,"tvguidercolor=yes\n");
        else sprintf(temp,"tvguidercolor=no\n");
        fputs(temp,file);
        fclose(file);
    } else error=true;
    file = fopen("mythtv-controller.keys", "w");
    if (file) {
      fwrite(configkeyslayout,sizeof(configkeytype)*12,1,file);
      fclose(file);
    } else error=true;
    return(!(error));
}




// load all config from config file

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
    for(i = 0; i < storagegroupantal; i++) {
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
    strcpy(configbackend_tvgraber,"tv_grab_uk_tvguide");      // default tv guide tv_grab_uk_tvguide
    strcpy(configbackend_tvgraberland,"");                    // default tv guide tv_grab_uk_tvguide other command
    configtvguidelastupdate=0;                                // default 0
    configsoundvolume=1.0f;
    configuvmeter=1;                                          // default uv meter type
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
           fputs("tvgraber=tv_grab_uk_tvguide\n",file);
           fputs("tvgraberupdate=0\n",file);
           fputs("tvgrabercolor=yes\n",file);
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
        printf("mediacenter server name is : %s\n", remoteHost->h_name);
        for(i = 0; addr_list[i] != NULL; i++) {
            printf("mediacenter server ip is  : %s\n", inet_ntoa(*addr_list[i]));
        }
        strcpy(confighostname,hostname);
        strcpy(configmysqlhost,remoteHost->h_name);
        strcpy(configmythhost,remoteHost->h_name);
        strcpy(configmysqlip,inet_ntoa(*addr_list[0]));			// ip adress on sql server

        remoteHost = gethostbyname(confighostname);
        if (remoteHost) {
            addr_list = (struct in_addr **) remoteHost->h_addr_list;
            printf("Hostname : %s\n", remoteHost->h_name);
            for(i = 0; addr_list[i] != NULL; i++) {
                printf("Ip is  : %s\n", inet_ntoa(*addr_list[i]));
            }
            strcpy(confighostip,inet_ntoa(*addr_list[0]));
        } else strcpy(confighostip,"127.0.0.1");
    } else {
        printf("Error recolving hostname.\n");
    }

    strcpy(sqlselect,"SELECT data from settings where value like 'MusicLocation' and hostname like '");
    strcat(sqlselect,configmysqlhost);
    strcat(sqlselect,"' ");

    printf("start reading database setup.....\n");

    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
              strcpy(configmusicpath,row[0]);
              if (debugmode & 32) fprintf(stderr,"Fundet music config directorys %s \n",row[0]);
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
                    if (debugmode & 32) fprintf(stderr,"Search on 'MusicLocation' give config dir %s \n",row[0]);
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
              if (debugmode & 32) fprintf(stderr,"Search on 'VideoStartupDir' give config dir %s \n",row[0]);
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
          printf("No access to mysql database... Can not read GalleryDir table.\n");
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
            printf("No storagegroup table or access to mysql database... Can not read storagegroup infomations from mythtv.\n");
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
            printf("No access to settings table searching for RecordFilePrefix in mysql database... Can not read config infomations from mythtv.\n");
            exit(-1);
          }
          if (conn) mysql_close(conn);
        }
    } else {
        strcpy(configmusicpath,"");
        printf("No access to mysql database... Can not read config infomations.\n\nUse setup (F1) to config mythtv sql access.\n");
//        exit(-1);                        // stop program
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
      } else printf("Disk write error, saving mythtv-controller.keys\n");
    }
}








//
// find sangs id og opdateret antal af afspilinger samt afspilings dato
//

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





// parameret til mysql er dirid = directory_id i database
// retunere path og parent_id som bliver udfyldt fra mysql databasen

void hent_dir_id1(char *path,char *parent_id,char *dirid) {
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






// ************************************************************ music play list loader fra mythtv
// load playlist til oversigt ask_open_dir_play

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

    if (debugmode & 2) fprintf(stderr,"Hent info om playlist nr: %d \n",playlistnr);
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
        sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,',',%d),',',-1) as songs,songcount FROM music_playlists where playlist_id=%d",songnr,playlistnr);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
//        printf("SQL = %s\n",sqlselect);
        if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
//                printf("Fundet recnr %d datanr %s \n",songnr,row[0]);
                songintnr=atoi(row[0]);
                songantal=atol(row[1]);

                printf("Song antal fundet = %ld \n",songantal);

                // find cd cover samt sange info i mythtv music database
                sprintf(sqlselect,"select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where song_id=%d and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id",songintnr);
                mysql_query(conn,sqlselect);
                res1 = mysql_store_result(conn);
//                printf("SQL = %s\n",sqlselect);
                if ((res1) && (songantal>0)) {
                    while ((row1 = mysql_fetch_row(res1)) != NULL) {
                        strcpy(songid,row1[0]);
                        strcpy(artistid,row1[5]);
                        strcpy(albumname,row1[3]);
                        strcpy(songname,row1[4]);
                        strcpy(artistname,row1[6]);
                        strcpy(songlength,row1[7]);

                        if (debugmode & 2) fprintf(stderr,"Fundet sang song_id=%s artist id=%s filename=%40s  \n",songid,row1[5],row1[1]);

                        strcpy(tmptxt,configmusicpath);		// start path
                        sprintf(tmptxt2,"%s",row1[2]);			// hent dir id
                        hent_dir_id1(tmptxt1,parent_id,tmptxt2);		// hent path af tmptxt2 som er = dir_id
                        strcat(tmptxt,tmptxt1);				// add path
                        strcat(tmptxt,"/");
                        strcpy(tmptxt3,tmptxt);			// er = path
                        strcat(tmptxt3,"mythcFront.jpg");		// add filename til cover
                        strcat(tmptxt,row1[1]);				// add filename til sang

                        if (file_exists(tmptxt3)) {
                        // printf("Loader music cover til playlist\n");

                            texture=loadTexture(tmptxt3);				// load texture

                        } else {
                            printf(" Error loading texture file : %s \n",tmptxt3);
                            texture=0;
                        }
                    }
                    aktiv_playlist.m_add_playlist(tmptxt,songid,artistid,albumname,songname,artistname,songlength,0,texture);	// add (gem) info i playlist
                } else {
                    finish=true;
                    error=1;
                }
            }
            songnr++;
        }
        if ((res==0) || (songnr==songantal)) finish=true;
        if (songantal==1) finish=true;
    }
    mysql_close(conn);
    if ((finish) && (error==0)) return(songantal); else return(0);
}







// MUSIC stuf *****************************************************************************************
//
// load dir icons efter et update.

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
      // load covers file into opengl as textures (jpg)
      musicoversigt[i].textureId = loadTexture((char *) tmpfilename);
    } else {
      musicoversigt[i].textureId=0;
    }
    i++;
  }
}






// hent antal af songs fra mythtv playlist database og fyld music play array

unsigned int hent_antal_dir_songs_playlist(int playlistnr) {
    char sqlselect[512];
    long i=0;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1;
    MYSQL_ROW row;
    // mysql stuf
    char database[256];
    bool finish=false;
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
    if (debugmode & 2) fprintf(stderr,"Henter info om playlistnr = %d \n",playlistnr);
    dirmusic.emtydirmusic();
    conn=mysql_init(NULL);
    // Connect to mythtv database
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    while(!(finish)) {
      sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,',',%d),',',-1) as songs,songcount FROM music_playlists where playlist_id=%d",songnr,playlistnr);
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
          sprintf(sqlselect,"select song_id,filename,directory_id,music_albums.album_name,name,music_artists.artist_id,music_artists.artist_name,length from music_songs,music_artists,music_albums where song_id=%d and music_artists.artist_id=music_songs.artist_id and music_songs.album_id=music_albums.album_id",songintnr);
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
      if (i==songantal) finish=true;
  } // endwhile
  mysql_close(conn);
  return(songnr);		// antal sange fundet i dir id
}




// hent antal af songs i et dir (dirid) og fyld music play array fra dir path

unsigned int hent_antal_dir_songs(int dirid) {
    char tmpfilename[200];
//    char convert_command[256];
//    char convert_newfilename[256];
    char sqlselect[512];
    char tmptxt[200];
    unsigned int i,ii;
//    float xofset=0.0f;
    GLuint textureId;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    char database[256];

    if (global_use_internal_music_loader_system) strcpy(database,dbname); else strcpy(database,"mythconverg");
    if (debugmode & 2) fprintf(stderr,"Hent info om directory_id = %d \n",dirid);

    dirmusic.emtydirmusic();

    strcpy(sqlselect,"SELECT song_id,name,artist_id FROM music_songs where directory_id=");
    sprintf(tmptxt,"%d limit %d",dirid,dirliste_size);
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



// init lirc

int initlirc() {
  // LIRC SETUP
  int flags;
  sock=lirc_init((char *) "mythtv-controller",1);                  // print error to stderr
  if (sock!=-1) {
    if (lirc_readconfig(NULL,&lircconfig,NULL)!=0) {
      if (debugmode) fprintf(stderr,"No lirc mythtv-controller config file found.\n");
      lirc_deinit();
      sock=-1;				// lirc error code
    } else {
      // fjern vente tid fra lirc getchar
      fcntl(sock,F_SETOWN,getpid());
      flags=fcntl(sock,F_GETFL,0);
      if(flags!=-1) {
        fcntl(sock,F_SETFL,flags|O_NONBLOCK);
      }
      printf("Lirc mythtv-controller info loaded.\n");
    }
    printf("Remote control init finish.\n");
    return(sock);
  } else {
    printf("Remote control not found...\n");
    printf("No lirc. (No remote control support)\n");
    // no lirc support error
  }
  return(-1);
}




// Load ttf fonts list

int init_ttf_fonts() {
    // uni font config *****************************************************************************************
    ctx = glcGenContext();
    glcContext(ctx);
    // *********************************************************************************************************
    static GLint glc_font_id;
    //GLint  master,master_count,ii;
    //GLint face_count;
    //int i,j;

    // Get the number of entries in the catalog list

    /* Get a unique font ID. */
    glc_font_id = glcGenFontID();
    glcAppendCatalog("/usr/share/fonts/truetype");
    myFont = glcGenFontID();
    glcNewFontFromFamily(myFont, configfontname);                                       // Droid Serif,UbuntumFreeMono
    if (glcFontFace(myFont, "Bold")!=GL_TRUE) printf("Open ttf font select error.\n");  // Regular
    glcFont(myFont);

    aktivfont.updatefontlist();

    /* Draw letters as filled polygons. */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
/*
    // Get the number of entries in the catalog list
    count = glcGeti(GLC_CATALOG_COUNT);
    // Print the path to the catalogs
    for (ii = 0; ii < count; ii++) printf("%s\n", glcGetListc(GLC_CATALOG_LIST, ii));
*/
    // note FROM quesoglc-doc-0.7.0 DOC.
    // If the rendering style of the text is not GLC_BITMAP, then you should use glTranslate() and
    // glScale() instead of glRasterPos() and glcScale() (p. 64).

    glcDisable(GLC_GL_OBJECTS);
//    glcRenderStyle(GLC_LINE);                 // lines
//    glcRenderStyle(GLC_TEXTURE);
    glcRenderStyle(GLC_TRIANGLE);               // filled                       // NORMAL used in mythtv-controller
    //glcRenderStyle(GLC_BITMAP);
    glcStringType(GLC_UTF8_QSO);
//    glcEnable(GLC_HINTING_QSO);
    return(1);
}




// save command to be exectued to script exec

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



// find sangs id til playlist opdatering/restore
// returnere songid fra music_songstablen

long find_music_song_placering(char *filename,char *songname) {
    char sqlselect[1024];
    char *database = (char *) "mythconverg";
    long recnr=0;
    bool fundet=false;
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
        fundet=true;						// sang fundet i music_songs
      }
    }
    mysql_close(conn);
    return(recnr);
}


// restore database til mythtv music playlist datbase

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
    char *database = (char *) "mythconverg";
    bool fundet;
    char playlistname[512];
//    int songintnr;				// sang nr som skal i playliste
//    long songantal=0;				// antal sange i array i database
//    int songnr=1;

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
            fundet=false;
            while(!(fundet)) {
                sprintf(sqlselect,"select * from music_playlists where playlist_name like '%s'",playlistname);
                mysql_query(conn,"set NAMES 'utf8'");
                res1 = mysql_store_result(conn);
                mysql_query(conn,sqlselect);
                res1 = mysql_store_result(conn);
                if (res1) {
                    while ((row1 = mysql_fetch_row(res1)) != NULL) {
                        if (fundet==false) {
                            printf("Found playlist %s updating ",playlistname);
                            sprintf(sqlselect,"update music_playlists set music_playlist.playlist_songs='', music_playlist.length=0, music_playlist.songcount=0 where playlist_id=%s",row1[0]);
                            mysql_query(conn,"set NAMES 'utf8'");
                            res2 = mysql_store_result(conn);
                            mysql_query(conn,sqlselect);
                            res2 = mysql_store_result(conn);
                            // missing Check done
                        }
                        fundet=true;
                        // find song in music database
                        songplacering=find_music_song_placering(row[1],row[2]);				// get song recnr
                        songlength=atol(row[8]);							// get song length
                        // if song exist update playlist
                        if (songplacering>0) {
                            printf(".");
                            sprintf(sqlselect,"update music_playlists set music_playlist.playlist_songs=concat(playlist_songs,',%ld'), music_playlist.songcount=music_playlist.songcount+1, music_playlist.length=music_playlist.length+%s where music_playlist.playlist_id=%ld",songplacering,row1[0],songlength);
                            mysql_query(conn,"set NAMES 'utf8'");
                            res2 = mysql_store_result(conn);
                            mysql_query(conn,sqlselect);
                            res2 = mysql_store_result(conn);
                        } // endif
                    } // endwhile
                } // endif
                printf("\n");
                if (!(fundet)) {
                    // else create new playlist
                    printf("Create new playlist %s......\n",playlistname);
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





//
// lav en backup af sange i playlister
// til en table ved navn music_songs_tmp
//

unsigned int do_playlist_backup_playlist() {
//    char tmpfilename[200];
//    char convert_command[256];
//    char convert_newfilename[256];
    int playlistnr;
    char sqlselect[8192];
//    char tmptxt[200];
    long i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res,*res1,*res2,*res3;
    MYSQL_ROW row,row3;
    // mysql stuf
    char *database = (char *) "mythconverg";
    bool finish=false;
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
    if (!(res2)) {
        printf("NO create temp database\n ");
        return(0);
    }
    // Hent playlistes med sange tilknyttet elementer i playlist_songs array
    strcpy(sqlselect,"select playlist_id,playlist_name from music_playlists where playlist_songs!=''");
    mysql_query(conn,sqlselect);
    res3 = mysql_store_result(conn);
    if (res3) {
        while ((row3 = mysql_fetch_row(res3)) != NULL) {
            playlistnr=atol(row3[0]);							// hent playlist nr
            strcpy(playlistname,row3[1]);						// hent playlistname
            finish=false;
            printf("Save music info from playlist name:%s \n",playlistname);
            i=0;
            while(!(finish)) {
                sprintf(sqlselect,"SELECT substring_index(substring_index(playlist_songs,',',%d),',',-1) as songs,songcount FROM music_playlists where playlist_id=%d",songnr,playlistnr);
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
                	if (debugmode & 2) fprintf(stderr,"Playlist %s Hentet music nummer = %ld af %ld \n",playlistname,i,songantal);
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
                if (i>songantal) finish=true;
            } // endwhile
        } // endwhile
    } // endif
    mysql_close(conn);
    return(songnr);		// antal sange fundet i dir id
}



// Bruges af ur pause display

void myglprint(char *string)
{
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}



void show_background() {
  // make background
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  // glBlendFunc(GL_ONE, GL_ONE);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
  if (screen_size!=4) {
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


//
// *********************** MAIN LOOP *********************************************************************************
//

static bool do_update_xmltv_show=false;

void display() {
    // used by xmltv updater func
    static bool getstarttidintvguidefromarray=true;
    static time_t today=0;
    static time_t lasttoday=0;
    static bool do_update_xmltv=false;

    static int starttimer=0;                                     // show logo timeout
    bool do_play_music_aktiv_nr_select_array[1000];             // array til at fortælle om sange i playlist askopendir er aktiv
    char temptxt[200];
    char temprgtxt[2000];
    int i;
    struct tm *timeinfo;
    float mgrader,tgrader;
    static time_t rawtime,rawtime1=0;
    int min,tim;
    static bool remove_log_file=true;
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
//    int fps;
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
    bool startup=true;

    // uv color table
    float uvcolortable[]={0.0,0.8,0.8, \
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
    float uvcolortable2[]={0.8,0.8,0.8, \
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



    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();

    rawtime=time(NULL);                                 // hent now time
    int savertimeout=atoi(configscreensavertimeout);
    if ((rawtime1==0) || (saver_irq)) {                 // ur timer
        rawtime1=rawtime+(60*savertimeout);             // x minuter hentet i config
        visur=false;                                    // if (debug) printf("Start screen saver timer.\n");
        saver_irq=false;
    }

    timeinfo = localtime ( &rawtime );
    strftime(strhour, 20, "%I", timeinfo );
    strftime(strmin, 20, "%M", timeinfo ); //%M
    min=atoi(strmin);
    tim=atoi(strhour);
    // update clock
    if (rawtime>rawtime1) {
        visur=true;
    }

    // make xmltv update
    today=time(NULL);
    // update interval
    // set in main.h
    if (((lasttoday+(doxmltvupdateinterval)<today) && (do_update_xmltv==false)) || (firsttime_xmltvupdate)) {
      if (debugmode) fprintf(stdout,"start timer xmltvguide update process.\n");
      lasttoday=today;                                      // rember last update
      do_update_xmltv=true;                                 // do update tvguide
      do_update_xmltv_show=true;                            // show we are updating
      firsttime_xmltvupdate=false;                          // only used first time
    }

    glPushMatrix();
    // background picture
    if ((!(visur)) && (_textureIdback_music) && (_textureIdback_main) && (!(vis_radio_oversigt)) && (!(vis_stream_oversigt)) && (!(vis_tv_oversigt))) show_background();
    //visur=1;
    if (visur) {
      glPushMatrix();
      //urtype=SAVER3D;
      switch (urtype) {
        case DIGITAL:
            glPushMatrix();
            glDisable(GL_TEXTURE_2D);
            glTranslatef(orgwinsizex/2, orgwinsizey/2, 0.0f);
            glRasterPos2f(0.0f, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glcScale(40.0, 40.0);
            strftime(temptxt, 20, "%H:%M", timeinfo );
            myglprint(temptxt);
            glcScale(20.0, 20.0);
            strftime(temptxt, 80, "%d %B %Y", timeinfo );
            glRasterPos2f(-44.0f, -30.0f);
            myglprint(temptxt);
            glPopMatrix();
            break;
        case ANALOG:
            glDisable(GL_TEXTURE_2D);
            glTranslatef(orgwinsizex/2, orgwinsizey/2, 0.0f);

            // draw analog watch/ur
            for(i=0;i<360;i+=45) {
                //glTranslatef(100.0f, 100.0f, 0.0f);
                glColor3f(0.4f, 0.4f, 0.4f);

                glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS); //Begin quadrilateral coordinates

                if ((i==0) || (i==90) || (i==180) || (i==270)) {
                    glTexCoord2f(0.0, 0.0); glVertex3f(300.0, 0.0, 0.0);
                    glTexCoord2f(0.0, 1.0); glVertex3f(300.0, 10.0, 0.0);
                    glTexCoord2f(1.0, 1.0); glVertex3f(320.0, 10.0, 0.0);
                    glTexCoord2f(1.0, 0.0); glVertex3f(320.0, 0.0, 0.0);
                } else {
                    glTexCoord2f(0.0, 0.0); glVertex3f(300.0, 0.0, 0.0);
                    glTexCoord2f(0.0, 1.0); glVertex3f(300.0, 20.0, 0.0);
                    glTexCoord2f(1.0, 1.0); glVertex3f(400.0, 20.0, 0.0);
                    glTexCoord2f(1.0, 0.0); glVertex3f(400.0, 0.0, 0.0);
                }
                glEnd(); //End quadrilateral coordinates
            }
            mgrader=min*(360/60);
            tgrader=tim*(360/12);

            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f);
            glColor3f(0.8f, 0.8f, 0.8f);
            // tgrader
            glRotatef(-(tgrader-90), 0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 0.0);
            glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 10.0, 0.0);
            glTexCoord2f(1.0, 1.0); glVertex3f(180.0, 10.0, 0.0);
            glTexCoord2f(1.0, 0.0); glVertex3f(180.0, 0.0, 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();
            // mgrader
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f);
            glTranslatef(orgwinsizex/2, orgwinsizey/2, 0.0f);
            glRotatef(-(mgrader-90), 0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 0.0);
            glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 10.0, 0.0);
            glTexCoord2f(1.0, 1.0); glVertex3f(240.0, 10.0, 0.0);
            glTexCoord2f(1.0, 0.0); glVertex3f(240.0, 0.0, 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();
            break;
        case SAVER3D:
            //reset pos
            glLoadIdentity();
            glTranslatef(0.0f, 0.0f, -7.0f);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);


            // music gfx loaded
            // if not load before screen saver
            if (!(music_oversigt_loaded)) {
                music_oversigt_loaded=true;
                load_music_covergfx(musicoversigt);
                mybox.settexture(musicoversigt);
            }

            mybox.show_music_3d(_angle,_textureId16,_textureId16,_textureId7_2);
            break;
        case SAVER3D2:
            // reset pos
            glLoadIdentity();
            glTranslatef(0.0f, 0.0f, -2.0f);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            //mybox.show_music_3d1(_angle,screensaverbox);	//_textureId19
            mybox.show_music_3d_new(_angle,screensaverbox);	//_textureId19
            break;
        case PICTURE3D:
            // picture screen saver
            glLoadIdentity();
            glDisable(GL_BLEND);

//            picture_saver psaver;

            //if (psaver==NULL) psaver=new picture_saver();

//            psaver.show_pictures();

            break;
      }
      glPopMatrix();
    }
    if ((visur) && (sleeper)) {
        sleeper=0;
        FILE *sfile;
        sleep_ok=1;
        sfile=fopen("mythtv-controller.lock","w");
        if (sfile) {
            fputs("Lockfile",sfile);
            fclose(sfile);
            remove_log_file=true;
        }
    }
    if ((visur==0) && (sleep_ok==0) && (remove_log_file)) {
        printf("remove logfile\n");
        remove("mythtv-controller.lock");       // fjern lock file
        sleeper=1;
        sleep_ok=0;
        rawtime1=0;                             // reset timer så den henter ti
        remove_log_file=false;                  // clear remove lock file flag
    }

    int iconsizex=200;
    int iconspacey=210;
    if ((screen_size==3) || (screen_size==4)) {
      iconsizex=192;                            // 200
      iconspacey=192;
    }

    // vis menu **********************************************************************
    if ((!(visur)) && (!(vis_tv_oversigt)) && (starttimer==0)) {
        // tv icon
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        // tv icon
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        if ((vis_music_oversigt) || (vis_radio_oversigt)) {
            glBindTexture(GL_TEXTURE_2D, _textureIdplayinfo);                         // default show musicplay info
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);
            glLoadName(27);                  // Overwrite the first name in the buffer
        } else {
            glBindTexture(GL_TEXTURE_2D, _textureIdtv);		                         		// default tv
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);
            glLoadName(1);                  // Overwrite the first name in the buffer

        }
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*1) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*1)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*1)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*1) , 0.0);
        glEnd();

        // movie stuf
        //glBlendFunc(GL_ONE, GL_ONE);
        if ((vis_film_oversigt) || (vis_stream_oversigt)) {
            glBindTexture(GL_TEXTURE_2D,_textureIdfilm_aktiv);
//          glBindTexture(GL_TEXTURE_2D, _textureIdfilm);				// film icon
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);

            glLoadName(3); 			// load film icon name

        } else {
            if ((vis_music_oversigt) || (vis_radio_oversigt)) {
                glBindTexture(GL_TEXTURE_2D,_textureIdmusic_aktiv);
            } else {
                glBindTexture(GL_TEXTURE_2D, _textureIdmusic);                              // music icon
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

        //film  icon
        //glBlendFunc(GL_ONE, GL_ONE);
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt)) {
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
        //glBlendFunc(GL_ONE, GL_ONE);
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt)) {
            glBindTexture(GL_TEXTURE_2D,_textureIdpdown);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);

            glLoadName(24); 			// load film icon name

        } else {
            if (vis_recorded_oversigt) {
                glBindTexture(GL_TEXTURE_2D,_textureIdrecorded_aktiv);
//                glBindTexture(GL_TEXTURE_2D, _textureIdmusic);
            } else {
                glBindTexture(GL_TEXTURE_2D, _textureIdrecorded);                              // music icon
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glColor4f(1.0f, 1.0f, 1.0f,1.0f);
            glLoadName(4); 			// Overwrite the first name in the buffer
        }                                                                                    // _textureId13			// default button mask
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( orgwinsizex-200 ,  orgwinsizey-(iconspacey*4) , 0.0);
        glTexCoord2f(0, 1); glVertex3f( orgwinsizex-200,   orgwinsizey-(iconspacey*4)+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( orgwinsizex-200+iconsizex,orgwinsizey-(iconspacey*4)+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( orgwinsizex-200+iconsizex,   orgwinsizey-(iconspacey*4) , 0.0);
        glEnd();

        if (vis_uv_meter==false) {
          if ((!(vis_music_oversigt)) && (!(vis_film_oversigt))  && (!(vis_recorded_oversigt)) &&  (!(vis_stream_oversigt)) && (!(vis_radio_oversigt))) {

              //glBlendFunc(GL_ONE, GL_ONE);

            //   setup icon
              if (do_show_setup) glBindTexture(GL_TEXTURE_2D, _texturesetupmenu_select); else  glBindTexture(GL_TEXTURE_2D, _texturesetupmenu);
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
        glPopMatrix();
    }

    // radio stuf
    if ((vis_radio_or_music_oversigt) && (!(visur))) {				//
        // img
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, radiobutton);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor3f(1.0f, 1.0f, 1.0f);
        //glBlendFunc(GL_ONE, GL_ONE);

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
        glBindTexture(GL_TEXTURE_2D, musicbutton);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor3f(1.0f, 1.0f, 1.0f);
        //glBlendFunc(GL_ONE, GL_ONE);
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


    // stream and movie stuf
    if ((vis_stream_or_movie_oversigt) && (!(visur))) {
        // img
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, streambutton);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor3f(1.0f, 1.0f, 1.0f);
        //glBlendFunc(GL_ONE, GL_ONE);
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
        //glBlendFunc(GL_ONE, GL_ONE);
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



    if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_recorded_oversigt) || (vis_tv_oversigt) || (vis_radio_or_music_oversigt) || (vis_stream_or_movie_oversigt)) {
        show_newmovietimeout=0;
        vis_nyefilm_oversigt=false;
        if ((vis_tv_oversigt) && (getstarttidintvguidefromarray==true)) {
          printf("fundet start record in tv guide nr %d \n",aktiv_tv_oversigt.find_start_pointinarray(0));
          tvsubvalgtrecordnr=aktiv_tv_oversigt.find_start_pointinarray(0)-1;
          tvvalgtrecordnr=0;
          getstarttidintvguidefromarray=false;
        }
    }

    // shopw oversigt over nye film
    if ((vis_nyefilm_oversigt) && (do_show_setup==false)) {
        if (show_newmovietimeout==0) vis_nyefilm_oversigt=false;
        if (fknapnr==0) show_newmovietimeout--;
        film_oversigt.show_minifilm_oversigt(0,0);
    }


    // search radio station buffer search
    if ((vis_radio_oversigt) && (!(visur)))  {
        if (keybufferindex>0) {						// er der kommet noget i keyboard buffer
           keybufferopenwin=true;					// yes open filename window
           // hent søgte sange oversigt
           if ((keybufferopenwin) && (hent_radio_search)) {				// vi har søgt og skal reset view ofset til 0 = start i 3d visning.
               hent_radio_search=false;

               if (debugmode & 8) fprintf(stderr,"Search string: %s \n ",keybuffer);

               radiooversigt.clean_radio_oversigt();			// clean old liste
               radiooversigt.opdatere_radio_oversigt(keybuffer);	// load new
               radiooversigt.load_radio_stations_gfx();

               keybuffer[0]=0;
               keybufferindex=0;
               _angley=0.0f;
               rknapnr=0;
               radio_key_selected=1;   						        // reset cursor position
               do_play_radio=false;							// Do not play station
           }
        }
    }


    if ((vis_music_oversigt) && (!(visur)))  {
        if (keybufferindex>0) {						// er der kommet noget i keyboard buffer
           keybufferopenwin=true;					// yes open filename window
           // hent søgte sange oversigt
           if ((keybufferopenwin) && (hent_music_search)) {				// vi har søgt og skal reset view ofset til 0 = start i 3d visning.
               hent_music_search=false;

               if (debugmode & 2) fprintf(stderr,"Search string: %s \n ",keybuffer);
               if (findtype==0)
                   opdatere_music_oversigt_searchtxt(musicoversigt,keybuffer,0);	// find det som der søges kunster
               else
                   opdatere_music_oversigt_searchtxt(musicoversigt,keybuffer,1);        // find det som der søges efter sange navn
               opdatere_music_oversigt_icons(); 					// load gfx icons

               keybuffer[0]=0;
               keybufferindex=0;
               _angley=0.0f;

               do_music_icon_anim_icon_ofset=-1;

               music_icon_anim_icon_ofset=0;						// vis music oversigt fra start
               music_icon_anim_icon_ofsety=0;						//
               mknapnr=0;
               music_key_selected=1;                    // reset cursor position

               ask_open_dir_or_play_aopen=1;		//
               ask_open_dir_or_play=0;
           }
        }
    }

    if (!(visur)) {
      // music view
      if (vis_music_oversigt) {
        //load_music_covergfx(musicoversigt);
        show_music_oversigt(musicoversigt,_textureId7,_textureIdback,_textureId28,0,_mangley);
      } else if (vis_film_oversigt) {
        glPushMatrix();
        //aktivfont.selectfont("DejaVu Sans");
        film_oversigt.show_film_oversigt(_fangley,fknapnr);
        glPopMatrix();

      } else if (vis_stream_oversigt) {
        glPushMatrix();
        streamoversigt.show_stream_oversigt1(onlineradio, onlinestreammask , onlineradio_empty ,_sangley);
        glPopMatrix();
      } else if (vis_radio_oversigt) {
          radio_pictureloaded=radiooversigt.show_radio_oversigt1(_textureId7,_textureId7_1,_textureIdback,_textureId28,_rangley);
      } else if (vis_tv_oversigt) {
        // show tv guide
        // take time on it
        std::clock_t start;
        start = std::clock();
        aktiv_tv_oversigt.show_fasttv_oversigt(tvvalgtrecordnr,tvsubvalgtrecordnr,do_update_xmltv_show);
        //if (debugmode & 1) std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
        //
        // show tv program info about selected program in tv guide
        //
        if ((do_zoom_tvprg_aktiv_nr)>0) {
          glPushMatrix();
          // show info om program selected
          aktivfont.selectfont("FreeMono");
          aktiv_tv_oversigt.showandsetprginfo(tvvalgtrecordnr,tvsubvalgtrecordnr);
          glPopMatrix();
        }
        // show record program menu
      } else if (vis_recorded_oversigt) {
        recordoversigt.show_recorded_oversigt1(0,0);
      }
        // show radio options menu
      if ((vis_radio_oversigt) && (show_radio_options) && (!(visur))) {
        radiooversigt.show_radio_options();
      }
    }


    if (vis_tvrec_list) {
      glPushMatrix();
      aktiv_crecordlist.showtvreclist();
      glPopMatrix();
    }

    // show search box and text for radio and music
    if ((vis_radio_oversigt) || (vis_music_oversigt)) {
      if ((keybufferopenwin) && (strcmp(keybuffer,"")!=0)) {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        if (vis_radio_oversigt) glBindTexture(GL_TEXTURE_2D,_textureIdradiosearch); // mask _textureId5_!
        else if (vis_music_oversigt) glBindTexture(GL_TEXTURE_2D,_textureIdmusicsearch1); // mask _textureId5_!
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
      if (vis_volume_timeout<=0) show_volume_info=false;
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glColor4f(1.0f,1.0f,1.0f,0.2f);
      glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE);
      glBindTexture(GL_TEXTURE_2D, _textureuv1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTranslatef(10, 19, 0.0f);                                          // orgwinsizey
      int aa=0;
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
    if ((vis_error) && (!(visur)) && (vis_error_timeout)) {
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
      glBegin(GL_QUADS); //Begin quadrilateral coordinates
      glTexCoord2f(0, 0); glVertex3f( 10.0, 0.0, 0.0);
      glTexCoord2f(0, 1); glVertex3f( 10.0, 50.0, 0.0);
      glTexCoord2f(1, 1); glVertex3f( 640.0, 50.0, 0.0);
      glTexCoord2f(1, 0); glVertex3f( 640.0, 0.0, 0.0);
      glEnd(); //End quadrilateral coordinates
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


    // skal vi til at spørge ask open dir or play
    if ((vis_music_oversigt) && (!(visur)) && (ask_open_dir_or_play) && (mknapnr>0)) {
        do_swing_music_cover=false;
        if (do_swing_music_cover) {
            do_swing_music_cover=false;

        }
        if (do_swing_music_cover==false) {
          xof=500;
          yof=200;
          buttonsize=200;
            // background MASK

/*
          glPushMatrix();
//            glLoadIdentity(); 					//Reset the drawing perspective
          glEnable(GL_TEXTURE_2D);
//            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
          glColor4f(1.0f,1.0f,1.0f,0.2f);
          glEnable(GL_BLEND);
          glDisable(GL_DEPTH_TEST);
          glBlendFunc(GL_DST_COLOR, GL_ZERO);
          glBindTexture(GL_TEXTURE_2D, _textureId5_1);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
                                                                                    // draw ask box mask
          glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof,yof+800, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+800, yof+800 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+800,yof , 0.0);
          glEnd(); //End quadrilateral coordinates
          glPopMatrix();
*/

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

          glEnd(); //End quadrilateral coordinates
          glPopMatrix();


// ***************************************************************** play icon
          xof=550;
          yof=250;
          buttonsize=100;
          glPushMatrix();
          glEnable(GL_TEXTURE_2D);
          glColor3f(1.0f, 1.0f, 1.0f);
//            glColor4f(1.0f,1.0f,1.0f,1.0f);
//            glBlendFunc(GL_ONE, GL_ONE);
          glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
          glBlendFunc(GL_ONE, GL_ONE);
          glBindTexture(GL_TEXTURE_2D, _textureId10);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(20);                      				  // play icon nr
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          // play icon

          glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
          glEnd(); //End quadrilateral coordinates
          glPopMatrix();

// ************************************************************ Open/or not open
          xof=650;
          yof=250;
          buttonsize=100;
          glPushMatrix();
          glEnable(GL_TEXTURE_2D);
          glColor3f(1.0f, 1.0f, 1.0f);
          //            glColor4f(1.0f,1.0f,1.0f,1.0f);
          //            glBlendFunc(GL_ONE, GL_ONE);
          glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
          glBlendFunc(GL_ONE, GL_ONE);
          if (dirmusic.numbersindirlist()>0) {
              glBindTexture(GL_TEXTURE_2D, _textureopen);
          } else {
              glBindTexture(GL_TEXTURE_2D, _textureopen);                // _textureclose);
          }
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glLoadName(21);                        // Overwrite the first name in the buffer
          glBegin(GL_QUADS); //Begin quadrilateral coordinates
          // play icon
          glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
          glEnd(); //End quadrilateral coordinates
          glPopMatrix();




// swap ************************************************************** icon
          xof=750;
          yof=250;
          buttonsize=100;                glPushMatrix();
          glEnable(GL_TEXTURE_2D);
          glColor3f(1.0f, 1.0f, 1.0f);
//            glColor4f(1.0f,1.0f,1.0f,1.0f);
//            glBlendFunc(GL_ONE, GL_ONE);
         glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
         glBlendFunc(GL_ONE, GL_ONE);
         glBindTexture(GL_TEXTURE_2D, _textureswap);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glLoadName(22);                        // Overwrite the first name in the buffer
         glBegin(GL_QUADS); //Begin quadrilateral coordinates
         // play icon

        glTexCoord2f(0, 0); glVertex3f( xof, yof , 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof,yof+buttonsize, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize, yof+buttonsize , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize,yof , 0.0);
        glEnd(); //End quadrilateral coordinates
        glPopMatrix();

// **********************************************************
        no_open_dir=0;

        buttonsize=300;
        if (dirmusic.numbersindirlist()==0) {						// er der nogle dirs

                    // draw cd cover
//                    glLoadIdentity();
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
          no_open_dir=1;
          glColor4f(0.8f, 0.8f, 0.8f,1.0f);
          glRotatef(0.0f, 0.0f, 0.5f, 0.1f);
          glBlendFunc(GL_ONE, GL_ONE);
          if (dirmusic.textureId) {
              glBindTexture(GL_TEXTURE_2D, dirmusic.textureId);		// cover
          } else {
              glBindTexture(GL_TEXTURE_2D, _textureId16);                	// box no cd cover
          }
          // draw cd cover
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
        strcpy(temptxt1,musicoversigt[mknapnr-1].album_name);						// hent albun navn
        if (strcmp(temptxt1,"")==0) strcpy(temptxt1,music_noartistfound[configland]);
        // check for maxlength
        if (strlen(temptxt1)>24) {
            strcpy(temptxt1,"..");
            strcat(temptxt1,musicoversigt[mknapnr-1].album_name+(strlen(musicoversigt[mknapnr-1].album_name)-24));
        }
        sprintf(temptxt,music_nomberofsongs[configland],dirmusic.numbersinlist(),temptxt1);

        glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
        glcRenderString(temptxt);			// show numbers of songs in list
        glPopMatrix();


        i=0;
        if (dirmusic.numbersindirlist()==0) dirmusiclistemax=20; else dirmusiclistemax=8;		// hvis ingen dir mere plads til flere sange i sangliste

        // show cd song list så man kan vælge
        while (((unsigned int) i<(unsigned int) dirmusic.numbersinlist()) && ((unsigned int) i<(unsigned int) dirmusiclistemax)) {	// er der nogle sange navne som skal vises
            ofset=18*i;
            dirmusic.popsong(temptxt,&aktiv,i+do_show_play_open_select_line_ofset);				// hent sang info
            pos=strrchr(temptxt,'/');
            if (pos>0) strcpy(temptxt,pos+1);

            pos=strrchr(temptxt,'.');
            if (pos>0) temptxt[pos-temptxt]=0;
            if (i<12) temptxt[54]=0; else temptxt[35]=0;
            sprintf(temptxt1,"%-45s",temptxt);

//  	              printf("******* sang nr %d, name = %s \n",i,temptxt);
//                    glLoadIdentity();



            if (i==do_show_play_open_select_line) glColor4f(textcolor[0],textcolor[1],textcolor[2],1.0f);
               else glColor4f(selecttextcolor[0],selecttextcolor[1],selecttextcolor[2],1.0f);

//                    glTranslatef(560.0f, 750.0f , 0.0f);                                                    // pos
//                    glRasterPos2f(0.0f, 0.0f);
            glPushMatrix();
            glTranslatef(560.0f, 850.0f -ofset, 0.0f);
            glRasterPos2f(0.0f, 0.0f);
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
            //aktivfont.selectfont("Courier 10 Pitch");
            glcRenderString(temptxt1);
            i++;
            glTranslatef(5.0f, 0.0f, 0.0f);
             if (aktiv==true) {
              glcRenderString("[X]");
            } else {
              glcRenderString("[ ]");
            }
            //aktivfont.selectfont(configfontname);
            glPopMatrix();
          }
        }
    }




    // start play radio station
    if (vis_radio_oversigt) {
        if ((do_play_radio) && (rknapnr>0) && (rknapnr<=radiooversigt.radioantal())) {
            // do we play now ?
            if (snd) {
                #if defined USE_FMOD_MIXER
                // yes stop play
                // stop old playing
                sound->release();                                                                       // stop last playing song
                dsp=0;                                                                                  // reset uv
                ERRCHECK(result,0);
                #endif
                #if defined USE_SDL_MIXER
                if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
                sdlmusicplayer=NULL;
                #endif
                snd=0;                                // set play new flag

            }
            if (snd==0) {
                snd=1;
                if (debugmode & 4) fprintf(stderr,"Play radio station nr %d url %s \n",rknapnr-1,radiooversigt.get_stream_url(rknapnr-1));

                if (snd==0) {
                    #if defined USE_FMOD_MIXER
                    result = sndsystem->init(32, FMOD_INIT_NORMAL, 0);
                    ERRCHECK(result,0);
                    #endif
                }
                #if defined USE_SDL_MIXER
                if (sdlmusicplayer==0) {
                    printf("Setup SDL_MIXER soundsystem\n");
                    // load support for the OGG and MOD sample/music formats
                    int flags=MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC;
                    SDL_Init(SDL_INIT_AUDIO);

                    /* This is where we open up our audio device.  Mix_OpenAudio takes
                    as its parameters the audio format we'd /like/ to have. */
                    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
                        printf("Unable to open audio!\n");
                        exit(1);
                    }
                    /* If we actually care about what we got, we can ask here.
                    In this program we don't, but I'm showing the function call here anyway in case we'd want to know later. */
                    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);

                    sdlmusic=Mix_Init(flags);
                    if(sdlmusic&flags != flags) {
                        printf("Mix_Init: Failed to init required ogg,mp3,flac support!\n");
                        printf("Mix_Init: %s\n", Mix_GetError());
                        // handle error
                    }
                }
                #endif



                if (strcmp(radiooversigt.get_stream_url(rknapnr-1),"")!=0) {
                    strcpy(aktivplay_music_path,radiooversigt.get_stream_url(rknapnr-1));


                    if (debugmode & 2) fprintf(stderr,"play radio path = %s \n ",aktivplay_music_path);

                    // fmod player
                    #if defined USE_FMOD_MIXER
                    // set big sound buffer to stop lag
                    result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);
                    result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
                    ERRCHECK(result,rknapnr);
                    //result = sound->getOpenState(&openstate,&percent,&starving,false);
                    //result = sndsystem->createSound(aktivplay_music_path, FMOD_HARDWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_NONBLOCKING , 0, &sound);
                    //ERRCHECK(result,rknapnr);
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


//                    radiooversigt.get_radio_online(rknapnr-1);				// station virker fint ok status igen

                    // play online radio by sdl
                    #if defined USE_SDL_MIXER

                    //if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
                    sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
                    Mix_PlayMusic(sdlmusicplayer, 0);
                    if (!(sdlmusicplayer)) {
                        printf("Error load music. %s\n",aktivplay_music_path);
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
                aktiv_radio_station=rknapnr-1;		// Husk aktiv radio station så vi kan vi den senere
                radio_key_selected=rknapnr;		// husk den valgte radio station
                rknapnr=0;
                mknapnr=0;				// reset select fire again
                do_play_radio=false;
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
                            printf("FMOD_TAGTYPE_SHOUTCAST\n");
                            //printf("%s = <binary> (%d bytes)\n", tag.name, tag.datalen);
                        } else if ((FMOD_TAGTYPE) tag.datatype==FMOD_TAGTYPE_ID3V1) {
                            printf("FMOD_TAGTYPE_ID3V1\n");
                        }
                    }
                }
            }
        }



//        printf("rknapnr = %d online = %d \n",rknapnr-1,radiooversigt.get_radio_online(2));


        // create radio station online check tread

        #endif
        #if defined USE_SDL_MIXER

        #endif
        if (!(check_radio_thread)) {
            check_radio_thread=true;
            pthread_t loaderthread;           // check radio status thread
            int rc=pthread_create(&loaderthread,NULL,radio_check_statusloader,NULL);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }


    }

    if (vis_stream_oversigt) {
        if ((do_play_stream) && (sknapnr>0) && (sknapnr<=streamoversigt.streamantal())) {
            #if defined USE_FMOD_MIXER
            if (snd) {
               // stop old playing
               sound->release();                                                                       // stop last playing song
               ERRCHECK(result,0);
               snd=0;
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


    // alt music er her under

    if (do_play_music_cover) {
        // play list
        if (do_find_playlist) {
            do_find_playlist=false;                                     // sluk func igen
            do_play_music_cover=0;
            if (((do_zoom_music_cover==false) || (do_play_music_aktiv_play==0)) && (mknapnr!=0)) {
                // playliste funktion set start play
                if (debugmode & 2) fprintf(stderr,"Type af sange nr %d som skal loades %d\n ",mknapnr-1,musicoversigt[mknapnr-1].oversigttype);
                if (musicoversigt[mknapnr-1].oversigttype==-1) {
//                    mknapnr=music_key_selected-1;
                    if (debugmode & 2) fprintf(stderr,"Loading song from mythtv playlist: %4d %d\n",do_play_music_aktiv_nr,mknapnr);
                    if (hent_mythtv_playlist(musicoversigt[mknapnr-1].directory_id)==0) {		// tilføj musik valgte til playliste + load af covers
                        printf("**** PLAYLIST LOAD ERROR **** No songs. mythtv playlist id =%d\n",musicoversigt[mknapnr-1].directory_id);
                        exit(2);
                    }
                    do_play_music_aktiv_table_nr = 1;						// sæt start play aktiv nr
                // normal cd cover browser funktion set start play
                } else if (musicoversigt[mknapnr-1].oversigttype==0) {
                    if (debugmode & 2) fprintf(stderr,"Loading songs from id:%4d \n",do_play_music_aktiv_nr);
                     // reset valgt liste
//                            for(int tt=0;tt<500;tt++) do_play_music_aktiv_nr_select_array[tt]=true;

                    bool eraktiv;
                    for(int t=0;t<dirmusic.numbersinlist();t++) {
                        dirmusic.popsong(temptxt1,&eraktiv,t);
                                //printf("Nr t=%d Aktiv %d \n",t,eraktiv);
                        do_play_music_aktiv_nr_select_array[t]=eraktiv;
//                                dirmusic.set_songaktiv(true,t);
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
                    //if (debugmode & 2) printf("START PLAY....do play aktiv=%d, song to play = %s \n",do_play_music_aktiv,aktivplay_music_path);
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
    // vis vi player music
    if ((vis_music_oversigt) && (!(visur))) {
        // spiller vi en sang vis status info i 3d   (do_play_music_aktiv=1 hvis der er status vindow
        if (do_zoom_music_cover) {
            //  printf("numbers of songs = %d aktiv song =%d in array  play position %d sec   \n",aktiv_playlist.numbers_in_playlist(),do_play_music_aktiv_nr,(snd->getPlayPosition())/1000);
            // show background mask
            int buttonsize=800;
            int buttonsizey=500;
            yof=200;

            // background
            glPushMatrix();
            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
            glColor4f(1.0f,1.0f,1.0f,1.0f);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            //glDisable(GL_DEPTH_TEST);
//            glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            if (do_stop_music_all) {						// SKAL checkes om gfx er ok
                glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);
            } else {
                glBindTexture(GL_TEXTURE_2D, _texturemusicplayer);			//  _textureId1
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glColor4f(1.0f, 1.0f, 1.0f,1.00f);					// lav alpha blending. 80%
            //glLoadName(5);				                        // Overwrite the first name in the buffer
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
            glLoadName(8);                        // Overwrite the first name in the buffer
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            // last

            glTexCoord2f(0, 0); glVertex3f(20+ (orgwinsizex/4), 20+yof , 0.0);
            glTexCoord2f(0, 1); glVertex3f(20+ (orgwinsizex/4),20+yof+100, 0.0);
            glTexCoord2f(1, 1); glVertex3f(20+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
            glTexCoord2f(1, 0); glVertex3f(20+ (orgwinsizex/4)+100,20+yof , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();



// ************************************************************ last

            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glColor3f(1.0f, 1.0f, 1.0f);
            //            glColor4f(1.0f,1.0f,1.0f,1.0f);
            //            glBlendFunc(GL_ONE, GL_ONE);
            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
            glBlendFunc(GL_ONE, GL_ONE);
            glBindTexture(GL_TEXTURE_2D, _texturemlast2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(6);                        // Overwrite the first name in the buffer
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            // last
            glTexCoord2f(0, 0); glVertex3f(120+ (orgwinsizex/4),20+yof , 0.0);
            glTexCoord2f(0, 1); glVertex3f(120+ (orgwinsizex/4),20+yof+100, 0.0);
            glTexCoord2f(1, 1); glVertex3f(120+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
            glTexCoord2f(1, 0); glVertex3f(120+ (orgwinsizex/4)+100,20+yof , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();


// ************************************************************ next

            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
//            glLoadIdentity();
            glColor3f(1.0f, 1.0f, 1.0f);
            //            glColor4f(1.0f,1.0f,1.0f,1.0f);
            //            glBlendFunc(GL_ONE, GL_ONE);
            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
            glBlendFunc(GL_ONE, GL_ONE);
            glBindTexture(GL_TEXTURE_2D, _texturemnext);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(7);                     						   // Overwrite the first name in the buffer
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            // last
            glTexCoord2f(0, 0); glVertex3f(220+ (orgwinsizex/4),20+yof , 0.0);
            glTexCoord2f(0, 1); glVertex3f(220+ (orgwinsizex/4),20+yof+100, 0.0);
            glTexCoord2f(1, 1); glVertex3f(220+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
            glTexCoord2f(1, 0); glVertex3f(220+ (orgwinsizex/4)+100,20+yof , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();


// ************************************************************stop

            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
//            glLoadIdentity();

            glColor3f(1.0f, 1.0f, 1.0f);
            //            glColor4f(1.0f,1.0f,1.0f,1.0f);
            //            glBlendFunc(GL_ONE, GL_ONE);
            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
            glBlendFunc(GL_ONE, GL_ONE);
            glBindTexture(GL_TEXTURE_2D, _texturemstop);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glLoadName(9);                        // Overwrite the first name in the buffer
            glBegin(GL_QUADS); //Begin quadrilateral coordinates
            // last
            glTexCoord2f(0, 0); glVertex3f(320+ (orgwinsizex/4),20+yof , 0.0);
            glTexCoord2f(0, 1); glVertex3f(320+ (orgwinsizex/4),20+yof+100, 0.0);
            glTexCoord2f(1, 1); glVertex3f(320+ (orgwinsizex/4)+100, 20+yof+100 , 0.0);
            glTexCoord2f(1, 0); glVertex3f(320+ (orgwinsizex/4)+100,20+yof , 0.0);
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();



// ************************************************************* draw cover
            // Draw music cover
            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glColor3f(1.0f, 1.0f, 1.0f);
            //            glColor4f(1.0f,1.0f,1.0f,1.0f);
            //            glBlendFunc(GL_ONE, GL_ONE);
            glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
            glBlendFunc(GL_ONE, GL_ONE);
            textureId=aktiv_playlist.get_textureid(do_play_music_aktiv_table_nr-1);		// get cd texture opengl id
            if (textureId==0) textureId=_textureId16;		                       				// hvis ingen texture (music cover) set default (box2.bmp) / use default if no cover
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
            glEnd(); //End quadrilateral coordinates
            glPopMatrix();

            // show artist
//            glLoadIdentity();
            glDisable(GL_TEXTURE_2D);

            glPushMatrix();
            glColor3f(0.5f, 0.5f, 0.5f);
            glTranslatef(520.0f, 650.0f, 0.0f);
            glRasterPos2f(0.0f, 0.0f);
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
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
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
            glcRenderString(temptxt);
            glPopMatrix();



            // show cd album name
            glPushMatrix();
            glTranslatef(520.0f, 630.0f, 0.0f);
            glColor3f(0.6f, 0.6f, 0.6f);
            strcpy(temptxt,music_album[configland]);                // music album text
            temptxt[50]=0;
            glRasterPos2f(0.0f, 0.0f);
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
            glcRenderString(temptxt);
             glPopMatrix();


            // show album name
            glPushMatrix();
            glTranslatef(700.0f, 630.0f, 0.0f);
            glColor3f(0.6f, 0.6f, 0.6f);
            glRasterPos2f(0.0f, 0.0f);
            aktiv_playlist.get_albumname(temptxt,do_play_music_aktiv_table_nr-1);
            temptxt[40]=0;
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
            glcRenderString(temptxt);
            glPopMatrix();


            // show song name
//            glLoadIdentity();
            glPushMatrix();
            glColor3f(0.6f, 0.6f, 0.6f);
            glTranslatef(520.0f, 610.0f, 0.0f);
            strcpy(temptxt,music_songname[configland]);
            glRasterPos2f(0.0f, 0.0f);
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
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
            glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
            glColor3f(0.6f, 0.6f, 0.6f);
            glcRenderString(temptxt);
            glPopMatrix();



            if (!(do_stop_music_all)) {

                // play position
                unsigned int ms = 0;
                float frequency;

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
                    ms=2000;
                    playtime_songlength=20000;
                    lenbytes=200;

                    frequency=audio_rate;		// show rate

                    //Mix_QuerySpec(int *frequency, Uint16 *format, int *channels);
                    #endif

                    // do the calc
                    #if defined USE_FMOD_MIXER
                    ms=2000;
                    if ((playtime_songlength>0) && (result==FMOD_OK)) {
                        kbps = (lenbytes/(playtime_songlength/1000)*8)/1000;			// calc bit rate
                    } else {
                        kbps=0;
                    }
                    if (result==FMOD_OK) {
                        playtime_songlength=playtime_songlength/1000;
                        playtime=ms/1000;
                    } else {
                        playtime_songlength=0;
                        playtime=0;
                    }
                    #endif
                } else if (vis_error) {
                    kbps=0;
                    ms=0;
                }
                glPushMatrix();

                glColor3f(0.6f, 0.6f, 0.6f);
                playtime_hour=(playtime/60)/60;
                playtime_min=(playtime/60);
                playtime_sec=playtime-(playtime_min*60);
                playtime_min=playtime_min-(playtime_hour*60);

                // old metode.
//                playtime_songlength=aktiv_playlist.get_length(do_play_music_aktiv_table_nr-1)/1000;


                playtime_length_min=playtime_songlength/60;
                playtime_length_sec=playtime_songlength-(playtime_length_min*60);
                // configland = lande kode
                strcpy(temptxt,music_play_time[configland]);
                temptxt[40]=0;
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
                if (pos>0) {
                    temptxt[pos-temptxt]='\0';
                }
                temptxt[40]=0;
                glRasterPos2f(0.0f, 0.0f);
                glScalef(20.5, 20.5, 1.0);                    // danish charset ttf
                glcRenderString(temptxt);
                glPopMatrix();
            }
        }
    }



    //
    // *************** RADIO stuf *******************************************************************************
    // show player

    if (!(visur)) {
        if (vis_radio_oversigt) {
            // show playing radio station
            if ((snd) && (do_zoom_radio)) {
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
                glLoadName(9	);                        // 9 = stop
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
                    if (vis_error==false) {
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

                    } else if (vis_error) {
                        kbps=0;
                        ms=0;
                    }

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

//                    glRasterPos2f(1, 1);
                    glScalef(20,20, 1.0);                    // danish charset ttf
                    glColor4f(1.0f,1.0f,1.0f,1.0f);
                    glcRenderString(temptxt);
                    glPopMatrix();

                    glPushMatrix();
                    glDisable(GL_TEXTURE_2D);
                    // show song name
                    sprintf(temptxt,": %-30s",aktivsongname);
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
//                    glRasterPos2f(1, 1);
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
                    sprintf(temptxt,": %s",radiooversigt.get_station_name(aktiv_radio_station));
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
                    sprintf(temptxt,": %02d:%02d:%02d ",radio_playtime_hour,radio_playtime_min,radio_playtime_sec);
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
                    sprintf(temptxt,": %3.0f Kbits",frequency);
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
                    sprintf(temptxt,": %-20s",aktivsongstatus);
                    temptxt[40]=0;
                    glColor4f(1.0f,1.0f,1.0f,1.0f);
                    glRasterPos2f(120.f, 100.f);
                    glScalef(20,20, 1.0);                    // danish charset ttf
                    glcRenderString(temptxt);
                    glPopMatrix();


                }
            }
        }

        // create uv meter
        if ((snd) && (show_uv)) vis_uv_meter=true;
        if (((snd) && (vis_uv_meter) && (configuvmeter) && (radio_pictureloaded)) || (vis_music_oversigt)) {
          // getSpectrum() performs the frequency analysis, see explanation below
          sampleSize = 1024;                // nr of samples default 64
          specLeft = new float[sampleSize];
          specRight = new float[sampleSize];
          for(int ii=0;ii<60;ii++) {
            specLeft[ii]=0.0f;
            specRight[ii]=0.0f;
          }

          // uv works only on fmod for now
          #if defined USE_FMOD_MIXER
          FMOD_DSP_PARAMETER_FFT *fft=0;
          int chan;
          if (!(dsp)) {
            sndsystem->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
            dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, 128);
            channel->addDSP(FMOD_DSP_PARAMETER_DATA_TYPE_FFT, dsp);
            dsp->setActive(true);
          }
          dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);
          if (result!=FMOD_OK) printf("Error DSP %s\n",FMOD_ErrorString(result));
          #endif

          spec = new float[sampleSize];
          spec2 = new float[sampleSize];
          if (fft) {
            for (chan = 0; chan < fft->numchannels; chan++) {
              float average = 0.0f;
              float power = 0.0f;
              for (int i = 0; i < fft->length; ++i) {
                  if (fft->spectrum[chan][i]) {
                      specLeft[i]=(float) fft->spectrum[0][i];
                      specRight[i]=(float) fft->spectrum[1][i];
                  }
              }
            }
          }
          for (i = 0; i < sampleSize; i++) {
            spec[i] = specLeft[i]*2;
            spec2[i] = specRight[i]*2;
          }

          // draw uv meter
          if ((configuvmeter==1) && (screen_size!=4)) {
            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
            glBindTexture(GL_TEXTURE_2D,texturedot);         //texturedot);
            //glColor4f(1.0f,1.0f,1.0f,1.0f);
            int uvypos=0;
            int high;
            int qq;
            // Draw uv lines 16
            for(qq=0;qq<16;qq++) {
              uvypos=0;
              high=sqrt(spec[(qq*2)+1])*30.0f;
              if (high>14) high=14;
              for(i=0;i<high;i+=1) {
                switch(i) {
                  case 0: glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                    break;
                    case 1: glColor4f(uvcolortable1[3],uvcolortable1[4],uvcolortable1[5],1.0);
                    break;
                    case 2: glColor4f(uvcolortable1[6],uvcolortable1[7],uvcolortable1[8],1.0);
                    break;
                    case 3: glColor4f(uvcolortable1[9],uvcolortable1[10],uvcolortable1[11],1.0);
                    break;
                    case 4: glColor4f(uvcolortable1[12],uvcolortable1[13],uvcolortable1[14],1.0);
                    break;
                    case 5: glColor4f(uvcolortable1[15],uvcolortable1[16],uvcolortable1[17],1.0);
                    break;
                    case 6: glColor4f(uvcolortable1[18],uvcolortable1[19],uvcolortable1[20],1.0);
                    break;
                    case 7: glColor4f(uvcolortable1[21],uvcolortable1[22],uvcolortable1[23],1.0);
                    break;
                    case 8: glColor4f(uvcolortable1[24],uvcolortable1[25],uvcolortable1[26],1.0);
                    break;
                    case 9: glColor4f(uvcolortable1[27],uvcolortable1[28],uvcolortable1[29],1.0);
                    break;
                    case 10:glColor4f(uvcolortable1[30],uvcolortable1[31],uvcolortable1[32],1.0);
                    break;
                    case 11:glColor4f(uvcolortable1[33],uvcolortable1[34],uvcolortable1[35],1.0);
                    break;
                    case 12:glColor4f(uvcolortable1[36],uvcolortable1[37],uvcolortable1[38],1.0);
                    break;
                    case 13:glColor4f(uvcolortable1[39],uvcolortable1[40],uvcolortable1[41],1.0);
                    break;
                    case 14:glColor4f(uvcolortable1[42],uvcolortable1[43],uvcolortable1[44],1.0);
                    break;
                    default:glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                    break;
                }
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f((orgwinsizex/4)+1250 +(qq*11),  4 +uvypos, 0.0);
                glTexCoord2f(0, 1); glVertex3f((orgwinsizex/4)+1250 +(qq*11),  14+4+uvypos, 0.0);
                glTexCoord2f(1, 1); glVertex3f((orgwinsizex/4)+1250+10 +(qq*11),  14+4+uvypos , 0.0);
                glTexCoord2f(1, 0); glVertex3f((orgwinsizex/4)+1250+10 +(qq*11),  4+uvypos, 0.0);
                glEnd();
                uvypos+=16;
              }
            }

            glPopMatrix();

          } else if ((configuvmeter==2) && (screen_size!=4)) {
            glPushMatrix();

            //glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
            //glBindTexture(GL_TEXTURE_2D,texturedot);         //texturedot);
            glColor4f(1.0f,1.0f,1.0f,1.0f);
            int uvypos=0;
            int uvyypos=0;
            int high;
            int qq;
            // Draw uv lines
            for(qq=0;qq<32;qq++) {
              uvypos=0;
              uvyypos=0;
              high=sqrt(spec[(qq*1)+1])*10.0f;
              if (high>7) high=6;
              // draw 1 bar
              for(i=0;i<high;i+=1) {
                //glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
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

              high=sqrt(spec2[(qq*1)+1])*10.0f;
              if (high>7) high=6;
              for(i=0;i<high;i+=1) {
                switch(i) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                            glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                            glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
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
                            glBindTexture(GL_TEXTURE_2D,_textureuv1_top);         //texturedot)
                    break;
                    default:
                            glColor4f(uvcolortable1[0],uvcolortable1[1],uvcolortable1[2],1.0);
                            glBindTexture(GL_TEXTURE_2D,_textureuv1);         //texturedot);
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
          delete [] spec;
          delete [] specLeft;
          delete [] specRight;
        }

        // Show setup stuf windows
        if (do_show_setup) {
            // reset color to nomal after uv
            glColor4f(1.0,1.0,1.0,1.0);
            show_setup_interface();
            if (do_show_setup_sound) show_setup_sound();
            if (do_show_setup_screen) show_setup_screen();
            if (do_show_videoplayer) show_setup_video();
            if (do_show_setup_sql) show_setup_sql();
            if (do_show_setup_tema) show_setup_tema();
            if (do_show_setup_network) {
                show_setup_network();
                if (show_wlan_select) {
                    show_wlan_networks((int) setupwlanselectofset);                         // show wlan list in opengl
                }
            }
            if (do_show_setup_font) show_setup_font(setupfontselectofset);
            if (do_show_setup_keys) show_setup_keys();
            if (do_show_tvgraber) show_setup_tv_graber(tvchannel_startofset);
        }
        glPopMatrix();
    }

    // load new team gfx files from config
    if (do_save_config) {
        do_save_config=false;
        if (save_config((char *) "/etc/mythtv-controller.conf")==0) {
            printf("Error saving config file mythtv-controller.conf\n");
        } else printf("Saving config ok.\n");
        // load all new textures
        // free all loaded menu + icon gfx

        freegfx();
        // reload all menu + icon gfx
        loadgfx();
        // reload lande flags
        //load_lande_flags();
    }

    // do start movie player
    if ((startmovie) && (do_zoom_film_cover)) {
        if (strcmp("default",configdefaultplayer)!=0)  {
            if (debugmode && 16) fprintf(stderr,"Start movie nr %d Player is vlc path :%s \n",fknapnr,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());
            strcpy(systemcommand,"/bin/sh /usr/bin/startmovie.sh ");
            strcat(systemcommand,"'");
            strcat(systemcommand,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());      // old strcat(systemcommand,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmfilename());
            strcat(systemcommand,"'");
            system(systemcommand);
//        spawn("./startmovie.pl","");
//        posix_spawn(0,"./startmovie.pl","",0);
        } else {
            // start internal player (vlc)
            if (debugmode && 16) fprintf(stderr,"Start play use default player film nr: %d name: %s \n",fknapnr,film_oversigt.filmoversigt[fknapnr-1].getfilmfilename());
            // if we play music/stream (radio) stop that before play movie stream (vlc)
            // stop music if play before start movie
            if (debugmode & 16) fprintf(stderr,"Stop playing music/radio \n");
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
            snd=0;
            sound=0;
            // clean music playlist
            aktiv_playlist.clean_playlist();                // clean play list (reset) play list
            do_play_music_aktiv_table_nr=1;			// reset play start nr

            if (debugmode & 16) fprintf(stderr,"Stop playing media/wideo if any \n");
            if (film_oversigt.film_is_playing) {
              if (debugmode) printf("Stop playing last movie before start new\n");
              // stop playing (active movie)
              film_oversigt.softstopmovie();
            }
            // start movie
            if (film_oversigt.playmovie(fknapnr-1)==0) {
              vis_error=true;
              vis_error_timeout=60;
            }
        }
        startmovie=false;                   // start kun 1 instans
    }
    if (stopmovie) {
      if (debugmode && 16) fprintf(stderr,"Stop movie\n");
      //sleep(10); // play
      if (strcmp("default",configdefaultplayer)!=0) {
        // close non default player
      } else {
        // close default player (vlc plugin)
        film_oversigt.stopmovie();
      }
      //stop do it again next loop
      stopmovie=false;
    }

    // start play stream
    // still use the old system call bach file
    if (startstream) {
        if (strcmp("default",configdefaultplayer)!=0)  {
            printf("Start stream nr %d Player is firefox \n",sknapnr);
            strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
            strcat(systemcommand,"'");
            if (sknapnr-1>0) {
                if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash:",10)==0) {
                    strcat(systemcommand,"http://");
                    strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1)+10);
                } else strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1));
                strcat(systemcommand,"'");

                printf("start web stream %s \n",systemcommand);

                system(systemcommand);
            }
//        spawn("./startmovie.pl","");
//        posix_spawn(0,"./startmovie.pl","",0);
        } else {
            printf("Start play use default firefox player \n");
            strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
            strcat(systemcommand,"'");
            if ((sknapnr-1)>0) {
                if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash:",10)==0) {
                    strcat(systemcommand,"http://");
                    strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1)+10);
               } else strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1));
            }
            strcat(systemcommand,"'");

            printf("start web stream %s \n",systemcommand);

            system(systemcommand);

        }
        sknapnr=0;
        stream_key_selected=1;
        stream_select_iconnr=0;
        startstream=false;                      // start kun 1 instans
    }

    // play recorded program
    if (do_play_recorded_aktiv_nr) {
        if (debugmode & 64) fprintf(stderr,"Start playing recorded program\n");
        if (strcmp("default",configdefaultplayer)!=0) {
            strcpy(systemcommand,"./startrecorded.sh ");
            recordoversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
            //strcat(systemcommand,configrecordpath);
            strcat(systemcommand,temptxt);

            printf("Start command :%s \n",systemcommand);

            system(systemcommand);
            do_play_recorded_aktiv_nr=0;                        // start kun 1 player
        } else {
            if (debugmode & 64) fprintf(stderr,"Start default playing recorded program\n");
            strcpy(systemcommand,"./startrecorded.sh ");
            recordoversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
            //strcat(systemcommand,configrecordpath);
            strcat(systemcommand,temptxt);

             if (debugmode & 64) fprintf(stderr,"Start command :%s \n",systemcommand);

            system(systemcommand);
            do_play_recorded_aktiv_nr=0;                        // start kun 1 player
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
      glBindTexture(GL_TEXTURE_2D,_dvdcovermask);
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
      if (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating()) {
        sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating());
      } else {
        strcpy(temptxt,"None");
      }
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
      if (strcmp(temptxt,"")!=0) {
        sprintf(temptxt,"%s ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmimdbnummer());
      } else {
        strcpy(temptxt,"None");
      }
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
        //printf("%0.0f FPS\n", fps);
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
        do_stop_music=false;
        do_stop_music_all=true;
        #if defined USE_FMOD_MIXER
        if ((sound) && (snd)) result=sound->release();			// stop all music if user press show playlist stop button
        ERRCHECK(result,do_play_music_aktiv_table_nr);
        #endif
        #if defined USE_SDL_MIXER
        if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);	// stop music and free music
        #endif
        snd=0;
        do_zoom_music_cover=false;
        aktiv_playlist.clean_playlist();                // clean play list (reset) play list
        do_play_music_aktiv_table_nr=1;			// reset play start nr
    }

    if (do_stop_radio) {
        #if defined USE_FMOD_MIXER
        result=sound->release();								// stop all music if user press stop button on show radio station info player
        ERRCHECK(result,0);
        #endif
        #if defined USE_SDL_MIXER
        if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);
        #endif

        snd=0;
        do_stop_radio=false;
        do_stop_music_all=true;
    }


    // is sound system working
    #if defined USE_FMOD_MIXER
    if ((channel) && (!(do_stop_music_all))) {
    #endif
    #if defined USE_SDL_MIXER
    if (!(do_stop_music_all)) {
    #endif
        // vent på sang er færdig
        saver_irq=true;						// stop screen saver at starte timeout når vi spiller music
        #if defined USE_FMOD_MIXER
        result = channel->isPlaying(&playing);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
            ERRCHECK(result,do_play_music_aktiv_table_nr);
        }
        #endif
        #if defined USE_SDL_MIXER
        playing=Mix_Playing(0);
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
                #endif

                #if defined USE_SDL_MIXER
                // aktivplay_music_path = next song to play
                if (debugmode & 2) fprintf(stderr,"Auto1 Next song %s \n",aktivplay_music_path);
                sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
                Mix_PlayMusic(sdlmusicplayer, 0);
                if (!(sdlmusicplayer)) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);

                #endif


                do_zoom_music_cover_remove_timeout=showtimeout;			// set close info window timeout
                do_zoom_music_cover=true;					// show music cover info til timeout showtimeout
            } else {						// else slet playliste (reset player)
                do_play_music_aktiv_table_nr=1;
                #if defined USE_FMOD_MIXER
                result=sound->release();          		// stop last played sound on soundsystem fmod
                ERRCHECK(result,do_play_music_aktiv_table_nr);
                #endif

                #if defined USE_SDL_MIXER
                Mix_FreeMusic(sdlmusicplayer);
                sdlmusicplayer=NULL;
                #endif

                if (debugmode & 2) fprintf(stderr,"STOP player and clear playlist\n");
                do_stop_music_all=true;				// stop all music
                snd=0;						// clear music pointer for irrsound
                do_zoom_music_cover=false;			// remove play info window
                aktiv_playlist.clean_playlist();		// clean play list (reset) play list
//                do_play_music_aktiv_table_nr=1;
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
            sdlmusicplayer=NULL;
            sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
            Mix_PlayMusic(sdlmusicplayer, 0);
            if (sdlmusicplayer==NULL) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
            #endif


//            do_zoom_music_cover_remove_timeout=showtimeout;
//            do_zoom_music_cover=true;
            if (debugmode & 2) fprintf(stderr,"User Next song %s \n",aktivplay_music_path);

        }
    } else if (vis_music_oversigt) {
            // press play on music

        if (do_shift_song) {
            aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);			// hent første sang ,0
            if (debugmode & 2) fprintf(stderr,"Auto2 Next song %s \n",aktivplay_music_path);

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
            sdlmusicplayer=Mix_LoadMUS(aktivplay_music_path);
            Mix_PlayMusic(sdlmusicplayer, 0);
            if (sdlmusicplayer==NULL) ERRCHECK_SDL(Mix_GetError(),do_play_music_aktiv_table_nr);
            #endif
        }
    }

    if (do_update_xmltv) {
      // call update xmltv multi phread
      update_xmltv_phread_loader();
      //aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
      do_update_xmltv=false;
      //do_update_xmltv_show=false;
    }

/*  don't wait!
 *  start processing buffered OpenGL routines
 */
    glFlush();
    glutSwapBuffers();
}





int list_hits(GLint hits, GLuint *names,int x,int y) {
    int i=hits;			// numbers of hits
    bool fundet=false;
    int returnfunc=0;
         /* For each hit in the buffer are allocated 4 bytes:
            1. Number of hits selected (always one, beacuse when we draw each object we use glLoadName, so we replace the prevous name in the stack)
           2. Min Z  3. Max Z  4. Name of the hit (glLoadName)
        */

    do {
        // setup menu
        if ((fundet==false) && (do_show_setup)) {
            // test for setup menu sound
            if (((GLubyte) names[i*4+3]==30) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
                do_show_setup_sound=true;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_screen=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
                fundet=true;
            }

            // test screen setup
            if (((GLubyte) names[i*4+3]==31)  && (do_show_setup_sql==false) && (do_show_setup_sound==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=true;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
                fundet=true;
            }
            // test for gfx opløsning
            if (((GLubyte) names[i*4+3]==32) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_font=false;
                do_show_setup_tema=true;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
               fundet=true;
            }
            // test for tema setup
            if (((GLubyte) names[i*4+3]==33)  && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
                do_show_setup_sound=false;
                do_show_setup_screen=true;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
                fundet=true;
            }
            // test for sql setup/info
            if (((GLubyte) names[i*4+3]==34) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=true;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
                fundet=true;
            }

            // test for ttffont setup/info
            if (((GLubyte) names[i*4+3]==35) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_setup_font=true;
                do_show_tvgraber=false;
                fundet=true;
            }

            // test for F keys setup/info
            if (((GLubyte) names[i*4+3]==36) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_videoplayer=false;
                do_show_setup_keys=true;
                do_show_tvgraber=false;
                fundet=true;
            }

            // test for setupclose
            if (((GLubyte) names[i*4+3]==37) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_setup=false;
                do_show_tvgraber=false;
                fundet=true;
                do_save_config=true;             // save setup now

            }

            if (((GLubyte) names[i*4+3]==38) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
                do_show_videoplayer=true;
                do_show_setup_sound=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_screen=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_tvgraber=false;
                fundet=true;
            }

            if (((GLubyte) names[i*4+3]==39) && (do_show_setup_sql==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
                do_show_setup_sound=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_screen=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=true;
                fundet=true;
            }



            // test for close windows again icon for all other windows
            if (((GLubyte) names[i*4+3]==40) && ((do_show_setup_sound) || (do_show_setup_screen) || (do_show_setup_sql) || (do_show_setup_network) || (do_show_setup_tema) || (do_show_setup_font) || (do_show_setup_keys) || (do_show_videoplayer))) {
                do_show_setup_sound=false;
                do_show_setup_screen=false;
                do_show_setup_sql=false;
                do_show_setup_network=false;
                do_show_setup_tema=false;
                do_show_setup_font=false;
                do_show_setup_keys=false;
                do_show_videoplayer=false;
                do_show_tvgraber=false;
                fundet=true;
            }

            // test for tema setup/info
            if (((GLubyte) names[i*4+3]==41) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_font==false) && (do_show_setup_tema))  {
                tema++;
                if (tema>TEMA_ANTAL) tema=1;
                fundet=true;
            }
        }

        // main menu
        if ((fundet==false) && (do_show_setup==false)) {
            // test for menu select tv
            if ((GLubyte) names[i*4+3]==1) {
                vis_music_oversigt=false;
                vis_film_oversigt=false;
                vis_tv_oversigt=!vis_tv_oversigt;
                vis_recorded_oversigt=false;
                vis_radio_or_music_oversigt=false;
                vis_stream_oversigt=false;
                vis_stream_or_movie_oversigt=false;
                do_show_tvgraber=false;
                fundet=true;
            }
            // test for menu select music
            if ((GLubyte) names[i*4+3]==2) {
                if (debugmode) printf("Select vis_radio_and_music \n");

                vis_radio_or_music_oversigt=!vis_radio_or_music_oversigt;
                //vis_radio_oversigt=!vis_radio_oversigt;
                //vis_music_oversigt=!vis_music_oversigt;
                vis_radio_oversigt=false;
                vis_music_oversigt=false;
                vis_film_oversigt=false;
                vis_tv_oversigt=false;
                vis_recorded_oversigt=false;
                vis_stream_oversigt=false;
                vis_stream_or_movie_oversigt=false;
                do_show_tvgraber=false;
                fundet=true;
            }


            // test for menu select film/streams
            if ((GLubyte) names[i*4+3]==3) {
                vis_stream_or_movie_oversigt=!vis_stream_or_movie_oversigt;
                vis_radio_oversigt=false;
                vis_music_oversigt=false;
                vis_film_oversigt=false;
                vis_tv_oversigt=false;
                vis_recorded_oversigt=false;
                vis_stream_oversigt=false;
                vis_radio_or_music_oversigt=false;
                do_show_tvgraber=false;
                fundet=true;
            }


/*            // test for menu select film
            if ((GLubyte) names[i*4+3]==3) {
                vis_music_oversigt=false;
                vis_film_oversigt=!vis_film_oversigt;
                vis_tv_oversigt=false;
                vis_recorded_oversigt=false;
                vis_radio_or_music_oversigt=false;
                fundet=true;
            }
*/

            // test for menu select recorded
            if ((GLubyte) names[i*4+3]==4) {
                vis_music_oversigt=false;
                vis_film_oversigt=false;
                vis_tv_oversigt=false;
                vis_radio_or_music_oversigt=false;
                vis_recorded_oversigt=!vis_recorded_oversigt;
                vis_stream_oversigt=false;
                vis_stream_or_movie_oversigt=false;
                do_show_tvgraber=false;
                fundet=true;
            }
        }
        // setup menu i main
        if ((!(vis_radio_oversigt)) && (!(vis_music_oversigt)) && (!(vis_recorded_oversigt)) && (!(vis_tv_oversigt)) && (!(vis_stream_oversigt))) {
            // test for menu select setup
            if ((GLubyte) names[i*4+3]==5) {
                do_show_setup=!do_show_setup;
                vis_music_oversigt=false;
                vis_film_oversigt=false;
                vis_tv_oversigt=false;
                vis_stream_oversigt=false;
                vis_stream_or_movie_oversigt=false;
                vis_radio_or_music_oversigt=false;
                do_show_tvgraber=false;
                fundet=true;
            }
        }

        if (vis_stream_oversigt) {
            if (!(fundet)) {
                // we have a select mouse/touch element dirid
                if ((GLubyte) names[i*4+3]==23) {
                    printf("scroll down\n");
                    returnfunc=1;
                    fundet=true;
                }
                if ((GLubyte) names[i*4+3]==24) {
                    printf("scroll up\n");
                    returnfunc=2;
                    fundet=true;
                }
            }
        }


        if (vis_film_oversigt) {
            if (!(fundet)) {
                // we have a select mouse/touch element dirid
                if ((GLubyte) names[i*4+3]==23) {
                    printf("scroll down\n");
                    returnfunc=1;
                    fundet=true;
                }
                if ((GLubyte) names[i*4+3]==24) {
                    printf("scroll up\n");
                    returnfunc=2;
                    fundet=true;
                }
            }
        }

        if (vis_music_oversigt) {
            if (!(fundet)) {		// hvis vi ikke har en aaben dirid så er det muligt at vælge dirid
                // we have a select mouse/touch element dirid
                if ((GLubyte) names[i*4+3]==23) {
                    if (debugmode & 2) fprintf(stderr,"scroll down\n");
                    returnfunc=1;
                    fundet=true;
                }
                if ((GLubyte) names[i*4+3]==24) {
                    if (debugmode & 2) fprintf(stderr,"scroll up\n");
                    returnfunc=2;
                    fundet=true;
                }
                if ((GLubyte) names[i*4+3]==27) {
                    if (debugmode & 2) fprintf(stderr,"Show music info\n");
                    do_zoom_music_cover=!do_zoom_music_cover;
                    fundet=true;
                }
            }

            // Bruges vist kun til mus/touch skærm
            if ((!(fundet)) && (!(do_zoom_music_cover)) && (!(ask_open_dir_or_play))) {		// hvis vi ikke har en aaben dirid så er det muligt at vælge dirid
                // we have a select mouse/touch element dirid
                if ((GLuint) names[i*4+3]>=100) {
                    mknapnr=(GLuint) names[i*4+3]-99;				// hent music knap nr
                    if (debugmode & 2) fprintf(stderr,"music selected=%u  \n",mknapnr);
                    fundet=true;
                }
                if (mknapnr!=0) swknapnr=mknapnr;
//                mknapnr=mknapnr+(music_icon_anim_icon_ofsety*4);
                switch(screen_size) {
                    case 1: mknapnr=mknapnr+(_mangley/41)*8;
                            break;
                    case 2: mknapnr=mknapnr+(_mangley/41)*8;
                            break;
                    case 3: mknapnr=mknapnr+(_mangley/41)*8;
                            break;
                    case 4: mknapnr=mknapnr+(_mangley/41)*8;
                            break;
                    default: mknapnr=mknapnr+(_mangley/41)*8;
                            break;
                }
            }

            // hvis vis ask_open_dir_or_play window
            if ((!(fundet)) && (ask_open_dir_or_play) && (!(do_zoom_music_cover))) {
                // play button
                if ((GLubyte) names[i*4+3]==20) {
                    ask_open_dir_or_play=false;                 // flag luk vindue igen
                    do_play_music_cover=1;                      // der er trykket på cover play det
                    do_zoom_music_cover=false;                  // ja den skal spilles lav zoom cover info window
                    do_find_playlist=true;                      // find de sange som skal indsættes til playlist (og load playlist andet sted)
                    fundet=true;
                }

                // no drop
                if ((GLubyte) names[i*4+3]==21) {
                    // pressed close
                    if (debugmode) fprintf(stderr,"Close window again\n");
                    ask_open_dir_or_play=false;				// flag luk vindue igen
                    mknapnr=0;
                    fundet=true;
                }
                // swap selected
                if ((GLubyte) names[i*4+3]==22) {
                    // pressed Swap
                    if (debugmode & 2) fprintf(stderr,"Swap button pressed\n");
                    for(int x=0;x<dirmusic.numbersinlist();x++) {
                        dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(x)),x);
                    }
                    fundet=true;
                }
            }

            // hvis show player
            // use as controller
            if ((!(fundet)) && (do_zoom_music_cover)) {
                if ((GLubyte) names[i*4+3]==5) {
                    // if touch/mouse click on window then close windows again
                    do_zoom_music_cover=false;
                    fundet=true;
                }
                // last song
                if ((GLubyte) names[i*4+3]==6) {
                    if ((do_play_music_aktiv_table_nr>1) && (do_shift_song==false)) {
                        do_play_music_aktiv_table_nr--;                                         // skift aktiv sang
                        do_shift_song=true;                                                     // sæt flag til skift
                    }
                    fundet=true;
                }
                // next song
                if ((GLubyte) names[i*4+3]==7) {
                    if ((do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) && (do_shift_song==false)) {
                        do_play_music_aktiv_table_nr++;
                        do_shift_song=true;
                    }
                    fundet=true;
                }
                // stop song
                if ((GLubyte) names[i*4+3]==9) {
                    do_stop_music=1;
                    fundet=true;
                }
                // play song
                if ((GLubyte) names[i*4+3]==8) {
                    do_stop_music=0;
                    do_shift_song=true;
                    if (do_play_music_aktiv_table_nr>=aktiv_playlist.numbers_in_playlist()) {
                        if (aktiv_playlist.numbers_in_playlist()==0) {
                            do_stop_music_all=true;            // stop play music
                            do_shift_song=false;
                        } else {
                            do_play_music_aktiv_table_nr=1;
                        }
                    }
                    fundet=true;
                    if (debugmode & 2) fprintf(stderr,"Start play \n");
                }
            }
        }


        // vælg skal der spilles music eller radio
        if ((vis_radio_or_music_oversigt) && (!(fundet))) {
            // Radio
            if ((GLubyte) names[i*4+3]==80) {
                fundet=true;
                vis_radio_oversigt=true;
                vis_radio_or_music_oversigt=false;
            }
            // Music
            if ((GLubyte) names[i*4+3]==81) {
                fundet=true;
                vis_music_oversigt=true;
                vis_radio_or_music_oversigt=false;
            }
        }

        // kun til mus/touch skærm (radio stationer)
        // luk show play radio
        if ((vis_radio_oversigt)  && (!(fundet))) {
            if ((GLubyte) names[i*4+3]==23) {
                if (debugmode & 8) fprintf(stderr,"scroll down\n");
                returnfunc=1;
                fundet=true;
            }
            if ((GLubyte) names[i*4+3]==24) {
                if (debugmode & 8) fprintf(stderr,"scroll up\n");
                returnfunc=2;
                fundet=true;
            }
            if ((GLubyte) names[i*4+3]==27) {
                if (debugmode & 8) fprintf(stderr,"Show/close radio info\n");
                do_zoom_radio=!do_zoom_radio;
                fundet=true;
            }
        }


        if ((vis_radio_oversigt) && (show_radio_options==false)) {
            // Bruges vist kun til mus/touch skærm (radio stationer)
            if (!(fundet)) {		// hvis ingen valgt
                // we have a select mouse/touch element dirid
                if ((GLuint) names[i*4+3]>=100) {
                    rknapnr=(GLuint) names[i*4+3]-99;				// hent music knap nr
                    if (debugmode & 8) fprintf(stderr,"radio station selected=%d glID=%u  \n",rknapnr,names[i*4+3]-99);
                    fundet=true;
                }
                // husk last
                if (rknapnr!=0) swknapnr=rknapnr;

                rknapnr=rknapnr+(_mangley/41)*8;
/*
                switch(screen_size) {
                    case 1: rknapnr=rknapnr+(_mangley/41)*5;
                            break;
                    case 2: rknapnr=rknapnr+(_mangley/41)*5;
                           break;
                    case 3: rknapnr=rknapnr+(_mangley/41)*8;
                            break;
                    case 4: rknapnr=rknapnr+(_mangley/41)*8;
                            break;
                }
*/
            }
            // stop radio player if vis_radio_oversigt
            if (!(fundet)) {
                // tryk stop radio
                if ((GLubyte) names[i*4+3]==9) {
                    if (debugmode & 8) fprintf(stderr,"stop radio\n");
                    do_stop_radio=1;
                    do_play_radio=false;			// no playing
                    fundet=true;
                    rknapnr=0;
                }
            }
        }

        // vælg skal der spilles film eller stream
        if ((vis_stream_or_movie_oversigt) && (!(fundet))) {
            // stream
            if ((GLubyte) names[i*4+3]==80) {
                fundet=true;
                vis_stream_oversigt=true;
                vis_stream_or_movie_oversigt=false;
            }
            // stream
            if ((GLubyte) names[i*4+3]==81) {
                fundet=true;
                vis_film_oversigt=true;
                vis_stream_or_movie_oversigt=false;
            }

        }

        if ((vis_stream_or_movie_oversigt) && (!(fundet))) {
            if ((GLubyte) names[i*4+3]==3) {
                fundet=true;
                vis_stream_oversigt=false;
                vis_stream_or_movie_oversigt=false;
            }
        }

        // stream oversigt
        if ((vis_stream_oversigt) && (!(fundet))) {
            if ((GLuint) names[i*4+3]>=100) {
                sknapnr=(GLuint) names[i*4+3]-99;				// hent stream knap nr
                if (debugmode & 128) fprintf(stderr,"stream selected=%u  \n",sknapnr);
                fundet=true;
            }

            if ((GLubyte) names[i*4+3]==3) {
                fundet=true;
                vis_stream_or_movie_oversigt=false;
                vis_stream_oversigt=false;
            }
        }

        // film oversigt
        if ((vis_film_oversigt) && (!(fundet))) {
            if ((GLubyte) names[i*4+3]==25) {
                if (debugmode & 16) fprintf(stderr,"Start movie player.\n");
                fundet=true;
                startmovie=true;
            }
            if ((GLubyte) names[i*4+3]==26) {
                if (debugmode & 16) fprintf(stderr,"Stop movie.\n");
                fundet=true;
                // stop movie playing
                stopmovie=true;
            }
            // we have a select mouse/touch
            if ((!(fundet)) && ((GLuint) names[i*4+3]>=100)) {
                fknapnr=(GLuint) names[i*4+3]-99;			// hent filmknap nr
                printf("Film selected=%d\n",fknapnr);
                fundet=true;
            }
        }

        // vis ny film oversigt
        if ((vis_nyefilm_oversigt) && (!(fundet))) {
            if ((GLubyte) names[i*4+3]==25) {
                if (debugmode & 16) fprintf(stderr,"Start movie player.\n");
                fundet=true;
                startmovie=true;
            }
            if ((GLubyte) names[i*4+3]==26) {
                if (debugmode & 16) fprintf(stderr,"Stop movie.\n");
                fundet=true;
                stopmovie=true;
            }
            if ((!(fundet)) && ((GLuint) names[i*4+3]>=100)) {
                fknapnr=(GLuint) names[i*4+3]-99;                       // hent filmknap nr
                printf("Film new selected=%d\n",fknapnr);
                fundet=true;
            }
        }


        if ((vis_tv_oversigt) && (!(fundet))) {
            if ((GLubyte) names[i*4+3]==27) {
                if (debugmode & 256) fprintf(stderr,"Close tv oversigt 1\n");
                vis_tv_oversigt=false;
                fundet=true;
            }
            if (((GLubyte) names[i*4+3]==28) && (!(fundet))) {
                if (debugmode & 64) fprintf(stderr,"show start record tv program.\n");
                vis_tv_oversigt=false;
                fundet=true;
            }
            if (((GLubyte) names[i*4+3]==29) && (!(fundet))) {
                if (debugmode & 64) fprintf(stderr,"close start record tv program.\n");
                ask_tv_record=true;
                do_zoom_tvprg_aktiv_nr=0;
                //                vis_tv_oversigt=false;
                fundet=true;
            }
            //
            // hvis vi viser tv guide og der ikke er valgt vis old rec/vis optager liste
            //
            if ((!(vis_old_recorded)) && (!(vis_tvrec_list))) {
                // er der trykket på et tv program
                if ((!(fundet)) && ((GLubyte) names[i*4+3]>=100) && ((GLubyte) names[i*4+3]<=1000)) {
                    tvknapnr=(GLuint) names[i*4+3]-100;					                // hent tv knap nr
                    fundet=true;
                }
            }

            // show old recordings
            if ((!(fundet)) && (!(vis_tvrec_list)) && ((GLubyte) names[i*4+3]==44)) {
                fprintf(stderr,"Show old recordings \n");
                vis_old_recorded=!vis_old_recorded;							// SKAL fixes
                fundet=true;
            }

            // show new recordings
            if ((!(fundet)) && (!(vis_old_recorded)) && ((GLubyte) names[i*4+3]==45)) {
                fprintf(stderr,"Show new recordings \n");
                vis_tvrec_list=!vis_tvrec_list;
                fundet=true;
            }


            // er vi igang med at spørge om vi skal optage programmet
            if (ask_tv_record) {
                if (((GLubyte) names[i*4+3]==40) && (!(fundet))) {
                    if (debugmode & 256) fprintf(stderr,"Close window again.\n");
                    ask_tv_record=false;
                    fundet=true;
                    returnfunc=3;
                    do_zoom_tvprg_aktiv_nr=0;
                }
                if (((GLubyte) names[i*4+3]==41) && (!(fundet))) {
                    if (debugmode & 256) fprintf(stderr,"Set program to record.\n");
                    ask_tv_record=false;
                    fundet=true;
                    returnfunc=3;
                    do_zoom_tvprg_aktiv_nr=0;
                    // set start record tv prgoram
                    //aktiv_tv_oversigt.gettvprogramrecinfo(tvvalgtrecordnr,tvsubvalgtrecordnr,prgtitle,prgstarttid,prgendtid);
                    aktiv_tv_oversigt.tvprgrecord_addrec(tvvalgtrecordnr,tvsubvalgtrecordnr);					// put tv prgoram into table record in mythtv backend (to set mythtv to record the program)
                    // opdatere tv guide med nyt info
                    aktiv_tv_oversigt.set_program_torecord(tvvalgtrecordnr,tvsubvalgtrecordnr);       // set record flag to show in tv_guide
                }
            }
        }
        if (!(ask_tv_record)) {
            // show old recorded and close
            if ((!(fundet)) && (vis_old_recorded)) {
                if ((GLubyte) names[i*4+3]==40) {
                    vis_old_recorded=!vis_old_recorded;
                    fundet=1;
                    returnfunc=3;
                }
            }
            // show active tv rec list and close
            if ((!(fundet)) && (vis_tvrec_list)) {
                if ((GLubyte) names[i*4+3]==40) {
                    vis_tvrec_list=!vis_tvrec_list;
                    fundet=1;
                }
            }
        }
        i--;
    } while ((i>=0) && (!(fundet)));

    if (debugmode & 1) fprintf(stderr,"%d hits:\n", hits);
    if (hits) {
        if (debugmode & 1) {
            for(i=0;i<hits;i++) {
              fprintf(stderr,"nr %2d stacknr: %d Icon nr on stack %d \n",i, (GLuint)names[i * 4], (GLuint)names[i * 4 + 3]);
            }
        }
    }
    return(returnfunc);
}




// get selected icon

int gl_select(int x,int y) {
    GLuint buff[64] = {0};		// info buffer
    GLint hits, view[4];
//    int id;
    glSelectBuffer(64,buff);		// This choose the buffer where store the values for the selection data
    glGetIntegerv(GL_VIEWPORT, view);	// This retrieve info about the viewport
    glRenderMode(GL_SELECT);		// Switching in selecton mode
    glInitNames();			// clear name stack
    glPushName(0);			// fill stack with one element (or glloadname give error)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, y, 1.0, 1.0, view);	// restrict the draw to an area around the cursor
    //gluPerspective(45.0, (double)screenx / (double) screeny, 0.0001, 10000.0);

    glOrtho(0.0f, (float) orgwinsizex, 0.0f,(float) orgwinsizey, -0.0f,10.0f);

    glMatrixMode(GL_MODELVIEW);		// Draw the objects onto the screen
    glutSwapBuffers();			// draw only the names in the stack, and fill the array

    // drawScene();				// draw scene again
    display();

    glMatrixMode(GL_PROJECTION);		// Do you remeber? We do pushMatrix in PROJECTION mode
    glPopMatrix();
    hits = glRenderMode(GL_RENDER);	// get number of objects drawed in that area
                                        // and return to render mode
    return(list_hits(hits, buff,x,y));	// return 1 = scroll up 2 = scroll down 0 = no scroll
}




// ************* mouse handler ********************************************************************************************

void handleMouse(int button,int state,int mousex,int mousey) {
    char tmp[80];
    char temptxt[200];
    int retfunc;
    int numbers_cd_covers_on_line;
    int numbers_film_covers_on_line;
    int numbers_radio_covers_on_line;
    int numbers_stream_covers_on_line;

    numbers_cd_covers_on_line=8;        // 9
    numbers_film_covers_on_line=9;
    numbers_radio_covers_on_line=8;
    numbers_stream_covers_on_line=9;

    saver_irq=true;					// stop screen saver
    if (visur==false) {
        switch(button) {
            case GLUT_LEFT_BUTTON:
                if (state==GLUT_UP) {
                    retfunc=gl_select(mousex,screeny-mousey);	// hent den som er trykket på
                    // nu er mknapnr/fknapnr/rknapnr=den som er trykket på bliver sat i gl_select
                    // retfunc er !=0 hvis der er trykket på en knap up/down
                    // give error
                    if (debugmode & 2) {
                        if ((show_music_oversigt) && (vis_stream_oversigt==false)) fprintf(stderr,"mknapnr = %d type = %d \n",mknapnr-1,musicoversigt[mknapnr-1].oversigttype);
                        else if (vis_stream_oversigt) fprintf(stderr,"sknapnr = %d\n",sknapnr-1);
                        else if (vis_tv_oversigt) fprintf(stderr,"tv prg knapnr = %d\n",mknapnr-1);
                    }
                    // any music buttons active
                    if (mknapnr>0) {
                        if ((retfunc==0) && (vis_music_oversigt) && ((mknapnr-1==0) || (musicoversigt[mknapnr-1].directory_id!=0)) && (!(do_zoom_music_cover))) {
                            if (musicoversigt[mknapnr-1].oversigttype==0) {
                                if (debugmode & 2) fprintf(stderr,"Normal dir id load.\n");
                                do_play_music_aktiv_nr=musicoversigt[mknapnr-1].directory_id; 	// set det aktiv dir id
                                antal_songs=hent_antal_dir_songs(musicoversigt[mknapnr-1].directory_id);    // loader antal dir/song i dir id

                                if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d name %s \n",antal_songs,musicoversigt[mknapnr-1].album_name);
                                if ((antal_songs==0) || (musicoversigt[mknapnr-1].directory_id==0)) {
                                    ask_open_dir_or_play_aopen=true;
                                } else {
                                    ask_open_dir_or_play_aopen=false;
                                }
                            } else {
                                if (debugmode & 2) fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt[mknapnr-1].directory_id);
                                // playlist loader
                                do_play_music_aktiv_nr=musicoversigt[mknapnr-1].directory_id;
                                if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                                if (do_play_music_aktiv_nr>0) {
                                    antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                                } else antal_songs=0;
                                if (debugmode & 2) fprintf(stderr,"Found numbers of songs:%2d\n",antal_songs);
                                if (antal_songs==0) {
                                    ask_open_dir_or_play_aopen=true;					// ask om de skal spilles
                                } else {
                                     ask_open_dir_or_play_aopen=false;
                                }
                            }
                            if (do_play_music_aktiv_nr) {						// er der et dirid/playlistid
                                ask_open_dir_or_play=true;						// yes ask om de skal spilles
                            } else ask_open_dir_or_play=true;
                        }
                    }

                    // stream stuf
                    if ((vis_stream_oversigt) && (retfunc==0)) {
                        fprintf(stderr,"Set do_play_stream flag %d \n",sknapnr);
                        if (sknapnr>0) do_play_stream=1;						// select button do play
                    }



                    // radio stuf
                    if ((vis_radio_oversigt) && (retfunc==0)) {
                        if (debugmode) fprintf(stderr,"Set do_play_radio flag rknapnr=%d \n",rknapnr);
                        if (rknapnr>0) do_play_radio=1;						// select button do play

                    }

                    // ved vis film oversigt
                    if ((vis_film_oversigt) & (retfunc==0)) {
                        do_zoom_film_cover=true;
                        do_zoom_film_aktiv_nr=fknapnr;
                        do_swing_movie_cover=1;
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
                        do_zoom_film_cover=true;
                        do_zoom_film_aktiv_nr=fknapnr;
                        do_swing_movie_cover=1;
                    }

                }


                /*
                if (state==GLUT_DOWN) {

                    int retfunc1=gl_select(mousex,screeny-mousey);   // hent den som er trykket på
                    // nu er mknapnr/fknapnr/rknapnr=den som er trykket på bliver sat i gl_select
                    // retfunc er !=0 hvis der er trykket på en knap up/down
                    //if (debugmode & 0) {
                        printf("state = %d mknapnr = %d type = %d \n",state,mknapnr-1,musicoversigt[mknapnr-1].oversigttype);
                    //}
                    if (mknapnr==1) {
                    }
                    mknapnr=-1;
                }
                */

                break;

            case GLUT_RIGHT_BUTTON:
                if (vis_music_oversigt) {
                    if ((ask_open_dir_or_play) && (state==GLUT_UP)) {
                        ask_open_dir_or_play=false;
                        mknapnr=0;
                    } else if ((ask_open_dir_or_play==false) && (do_play_music_aktiv_table_nr>0) && (state==0) && (button==2)) {
                        if (do_zoom_music_cover) {
                            do_zoom_music_cover=false;
                        } else {
                            do_zoom_music_cover=true;
                        }
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


                if ((vis_nyefilm_oversigt) && (state==GLUT_UP)) {
                    vis_nyefilm_oversigt=!vis_nyefilm_oversigt;
                }



                break;
        }
        if (vis_music_oversigt) {
            // auto opendir
            if ((ask_open_dir_or_play_aopen) && (retfunc==0)) {
                ask_open_dir_or_play=false;
                ask_open_dir_or_play_aopen=false;
                if (musicoversigt[mknapnr-1].oversigttype==-1) {
                    if (debugmode & 2) fprintf(stderr,"Open/read playlist id %d \n",musicoversigt[mknapnr-1].directory_id);
                    if (debugmode & 2) fprintf(stderr,"Opdatere musicarray henter playlist oversigt \n");
                    // hent playlist oversigt
                    opdatere_music_oversigt_playlists(musicoversigt);	// hent list over mythtv playlistes
                } else {
                    if (debugmode & 2) fprintf(stderr,"Opdatere musicarray Henter oversigt dir id = %d \n",musicoversigt[mknapnr-1].directory_id);
                    // opdate fra mythtv-backend if avable
                    if ((opdatere_music_oversigt(musicoversigt,musicoversigt[mknapnr-1].directory_id))>0) {
                      opdatere_music_oversigt_icons();
                    } else {
                      // opdatere music oversigt fra internpath
                      fprintf(stderr,"nr %d path=%s\n",mknapnr-1,musicoversigt[mknapnr-1].album_path);

                      if (opdatere_music_oversigt_nodb(musicoversigt[mknapnr].album_path,musicoversigt)==0) {
                        fprintf(stderr,"No Music loaded/found by internal loader\n");
                      }
                    }
                }
                /// reset mouse/key pos in vis_music_oversigt
                mknapnr=0;
                music_key_selected=1;
                music_select_iconnr=0;
                music_icon_anim_icon_ofset=0;
                music_icon_anim_icon_ofsety=0;
                _mangley=0.0f;
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

        // scroll tv guide up/down
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
        if (vis_film_oversigt) {
            if (((button==4) || (retfunc==2)) && ((unsigned int) film_select_iconnr+8<film_oversigt.film_antal()-1)) {
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
        if (vis_stream_oversigt) {
            if ((retfunc==0) && (sknapnr>0) && (do_play_stream)) {

                if (debugmode) fprintf(stderr,"knapnr %d  path_antal=%d type %d antal = %d \n",sknapnr-1,streamoversigt.get_stream_groupantal(sknapnr-1),streamoversigt.type,streamoversigt.streamantal());
                if (streamoversigt.type==0) {
                    strncpy(temptxt,streamoversigt.get_stream_name(sknapnr-1),200);

                    streamoversigt.clean_stream_oversigt();

                    printf("stream nr %d name %s \n ",sknapnr-1,temptxt);
                    streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");


                    do_play_stream=false;
                } else if ((sknapnr-1>0) && (streamoversigt.type!=2)) {
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
                    if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash",9)==0) {
                        startstream=true;
                    } else {
                        startstream=true;
                    }
                }
                //sknapnr=0;
                stream_key_selected=1;
                stream_select_iconnr=0;
                _sangley=0.0f;
            }

            if (((retfunc==2) || (button==4)) && ((_sangley/41.0f)+4<(int) (streamoversigt.streamantal()/numbers_stream_covers_on_line))) { // scroll button
                //do_music_icon_anim_icon_ofset=1;				// direction -1 = up 1 = down
                _sangley+=(41.0f);						// scroll window down one icon
                stream_select_iconnr+=numbers_stream_covers_on_line;			// add to next line
            }
            // scroll up
            if (((retfunc==1) || (button==3)) && (_sangley>0)) {
                //do_music_icon_anim_icon_ofset=-1;				// direction -1 = up 1 = down
                _sangley-=(41.0f);						// scroll window up
                stream_select_iconnr-=numbers_stream_covers_on_line;			// add to next line
            }
        }
    }
//    printf("button = %d \n",button);
}



unsigned int hourtounixtime(int hour) {
  time_t nutid;
  struct tm *tid;
  time(&nutid);                                                            // get time
  tid=localtime(&nutid);                                                   // fillout struct
  tid->tm_min=0;
  tid->tm_hour=hour;
  return(mktime(tid));
}


// handle keys

void handlespeckeypress(int key,int x,int y) {
    float MOVIE_CS;
    float MUSIC_CS;
    float RADIO_CS;
    unsigned int mnumbersoficonline;
    int fnumbersoficonline;
    int rnumbersoficonline;
    int snumbersoficonline;
    saver_irq=true;                                     // stop screen saver


    mnumbersoficonline=8;		// antal i music oversigt
    fnumbersoficonline=8;	  // antal i film oversigt
    rnumbersoficonline=8;   // antal i radio oversigt
    snumbersoficonline=8;   // antal i stream oversigt
    MOVIE_CS=46.0f;					// movie dvd cover side
    MUSIC_CS=41.0;					// music cd cover side
    RADIO_CS=41.0;					// radio cd cover side

    switch(key) {
                // F1 setup menu
        case 1: if (vis_music_oversigt) {
                    if (findtype==0) findtype=1;
                    else if (findtype==1) findtype=0;
                } else if ((!(vis_radio_oversigt)) && (!(vis_radio_or_music_oversigt))) {
                    if (do_show_setup) do_save_config=true;		// save setup
                    vis_radio_oversigt=false;
                    vis_tv_oversigt=false;
                    vis_film_oversigt=false;
                    vis_music_oversigt=false;
                    vis_recorded_oversigt=false;
                    vis_stream_oversigt=false;
                    vis_radio_or_music_oversigt=false;
                    vis_stream_or_movie_oversigt=false;
                    do_show_setup=!do_show_setup;
                }
                break;
        case 2:
                // F2 exit app key
                printf("Close down now exit(2)\n");
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
                    exit(100);
                }
                break;
        case 4: // F4 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[1].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[1].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[1].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[1]);
                    exit(100);
                }
                break;
        case 5: // F5 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[2].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[2].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[2].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[2]);
                    exit(100);
                }
                break;
        case 6: // F6 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[3].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[3].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[3].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[3]);
                    exit(100);
                }
                break;
        case 7: // F7 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[4].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[4].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[4].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[4]);
                    exit(100);
                }
                break;
        case 8: // F8 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[5].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[5].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[5].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[5]);
                    exit(100);
                }
                break;
        case 9: // F9 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[6].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[6].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[6].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[6]);
                    exit(100);
                }
                break;
        case 10: // F10 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[7].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[7].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[7].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[7]);
                    exit(100);
                }
                break;
        case 11: // F11 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[8].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[8].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[8].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[8]);
                    exit(100);
                }
                break;
        case 12: // F12 start mythtv og luk mythtv_controller
                if (strcmp(configkeyslayout[9].cmdname,"playlistbackup")==0) {
                    do_playlist_backup_playlist();
                } else if (strcmp(configkeyslayout[9].cmdname,"playlistrestore")==0) {
                    do_playlist_restore_playlist();
                } else  if (strcmp(configkeyslayout[9].cmdname,"")!=0) {
                    saveexitcommand(configkeyslayout[9]);
                    exit(100);
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
                        stream_select_iconnr--;
                    } else {
                        if ((stream_select_iconnr>0) && (_sangley>0)) {
                             _sangley-=RADIO_CS;
                             stream_key_selected+=snumbersoficonline-1;	// den viste på skærm af 1 til 20
                             stream_select_iconnr--;			// den rigtige valgte af 1 til cd antal
                        }
                    }
                }

                // if indside tv overoview
                if (vis_tv_oversigt) {
                    if ((tvvisvalgtnrtype==1) && (tvvalgtrecordnr>0)) {
                      tvvalgtrecordnr--;
                      tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr+1,tvsubvalgtrecordnr));
                    }
                }

/*
                if ((vis_tv_oversigt) && (tvstartxofset>0)) {
                    tvstartxofset-=tvprgsstep;
                    glDeleteLists(tvoversigt, 1);				// delete old list
                    tvoversigt=glGenLists(1);				// make new
                    glNewList(tvoversigt,GL_COMPILE);
                    aktiv_tv_oversigt.build_tv_oversigt(tvstartxofset,0);
                    glEndList();


//                    _anglez+=1.5f;
                }

*/
                // if indside a setup menu
                if (do_show_setup) {
                  setupsinofset++;
                  if (setupsinofset>99) setupsinofset=0;
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
                        _mangley+=MUSIC_CS;
                        music_key_selected-=mnumbersoficonline;			// den viste på skærm af 1 til 20
                        music_select_iconnr++;	                 		// den rigtige valgte af 1 til cd antal
                    } else {
                        music_select_iconnr++;			                // den rigtige valgte af 1 til cd antal
                    }
                    music_key_selected++;
                }

                if ((vis_film_oversigt) && ((int unsigned) film_select_iconnr<film_oversigt.film_antal()-1)) {
                    if ((film_key_selected % (mnumbersoficonline*3)==0) || ((film_select_iconnr==14) && (film_key_selected % mnumbersoficonline==0))) {
                        _fangley+=MOVIE_CS;
                        film_key_selected-=mnumbersoficonline;	// den viste på skærm af 1 til 20
                        film_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                    } else {
                        film_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                    }
                    film_key_selected++;
                }

                if (vis_recorded_oversigt) {
                    if (visvalgtnrtype==1) visvalgtnrtype=2;
                    else if (visvalgtnrtype==2) visvalgtnrtype=1;
                }

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

                if ((vis_stream_oversigt)  && (stream_select_iconnr<streamoversigt.streamantal())) {
                    if ((stream_key_selected % (snumbersoficonline*3)==0) || ((stream_select_iconnr==19) && (stream_key_selected % snumbersoficonline==0))) {
                        _sangley+=RADIO_CS;
                        stream_key_selected-=snumbersoficonline;	// den viste på skærm af 1 til 20
                        stream_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                    } else {
                        stream_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                    }
                    stream_key_selected++;
                }

                // if indside tv overoview
                if (vis_tv_oversigt) {
                  if (tvvisvalgtnrtype==1) {
                    if (tvvalgtrecordnr<aktiv_tv_oversigt.tv_kanal_antal()-1) tvvalgtrecordnr++;
                    tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr-1,tvsubvalgtrecordnr));
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
                    //
                  }
                }
                break;
        case 103:  // key down
                // bruges af ask_open_dir_or_play
                if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                    if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                        if (do_show_play_open_select_line<19) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                    }
                }
                // hvis ikke ask_open_dir_or_play
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr+mnumbersoficonline<=musicoversigt_antal)) {
                    if ((unsigned int) music_key_selected>=((mnumbersoficonline*3)+1)) {
                        _mangley+=MUSIC_CS;								//scroll gfx down
                        music_select_iconnr+=mnumbersoficonline;
                        do_music_icon_anim_icon_ofset=1;                       // set scroll
                    } else {
                        music_key_selected+=mnumbersoficonline;
                        music_select_iconnr+=mnumbersoficonline;
                    }
                }
                if ((vis_film_oversigt) && (debugmode)) fprintf(stderr,"select = %d Antal=%d /n",film_select_iconnr,film_oversigt.film_antal());
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
                if ((vis_radio_oversigt) && (show_radio_options==false) && ((radio_select_iconnr+rnumbersoficonline)<radiooversigt.radioantal())) {
                    if (radio_key_selected>=20) {
                        _rangley+=RADIO_CS;
                        radio_select_iconnr+=rnumbersoficonline;
                    } else {
                        radio_key_selected+=rnumbersoficonline;
                        radio_select_iconnr+=rnumbersoficonline;
                    }
                }
                if ((vis_radio_oversigt) && (show_radio_options)) radiooversigt.nextradiooptselect();
                // stream
                if ((vis_stream_oversigt) && (show_stream_options==false) && (stream_select_iconnr+snumbersoficonline<streamoversigt.streamantal())) {
                    if (stream_key_selected>=20) {
                        _sangley+=RADIO_CS;
                        stream_select_iconnr+=fnumbersoficonline;
                    } else {
                        stream_key_selected+=snumbersoficonline;
                        stream_select_iconnr+=snumbersoficonline;
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
                  reset_recorded_texture=true;                                //
                }

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


                // if indside a setup menu
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
                    // tv graber setup
                    if (do_show_tvgraber) {
                      if ((do_show_setup_select_linie+tvchannel_startofset)>0) {
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
                    keybuffer[0]=0;
                    keybufferindex=0;
                }


                break;
        case 101: // up key
                // bruges af ask_open_dir_or_play
                if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                    if (do_show_play_open_select_line>0) do_show_play_open_select_line--; else
                        if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
                }
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr>(mnumbersoficonline-1)) ) {
                    if ((_mangley>0) && ((unsigned int) music_key_selected<=mnumbersoficonline) && (music_select_iconnr>(mnumbersoficonline-1))) {
                         _mangley-=MUSIC_CS;
                         do_music_icon_anim_icon_ofset=-1;			// set scroll
                         music_select_iconnr-=mnumbersoficonline;
                    } else music_select_iconnr-=mnumbersoficonline;

                    if (music_key_selected>(int ) mnumbersoficonline) music_key_selected-=mnumbersoficonline;
                }
                // movie stuf
                if (vis_film_oversigt) {
                    if ((vis_film_oversigt) && (film_select_iconnr>(fnumbersoficonline-1))) {
                        if ((film_key_selected<=fnumbersoficonline) && (film_select_iconnr>(fnumbersoficonline-1))) {
                             _fangley-=MOVIE_CS;
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
                // stream stuf
                if ((vis_stream_oversigt) && (show_stream_options==false)) {
                    if ((vis_stream_oversigt) && (stream_select_iconnr>(snumbersoficonline-1))) {
                        if ((_sangley>0) && (stream_key_selected<=snumbersoficonline) && (stream_select_iconnr>(snumbersoficonline-1))) {
                             _sangley-=MOVIE_CS;
                             stream_select_iconnr-=snumbersoficonline;
                        } else stream_select_iconnr-=snumbersoficonline;

                        if (stream_key_selected>snumbersoficonline) stream_key_selected-=snumbersoficonline;
                    }
                }


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
                  if (tvsubvalgtrecordnr>0) {
                    tvsubvalgtrecordnr--;
                    if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)<hourtounixtime(aktiv_tv_oversigt.vistvguidekl)) {
                      if (aktiv_tv_oversigt.vistvguidekl>0) aktiv_tv_oversigt.vistvguidekl--;
                    }
                  }
                }


                // if indside a setup menu
                if (do_show_setup) {
                    // sql setup
                    if (do_show_setup_sql) {
                        if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    }
                    if (do_show_setup_sound) {
                        if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    }
                    if (do_show_setup_screen) {
                        if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    }
                    if (do_show_setup_network) {
                        // hvis vi ikke viser select wlan window hop med coursor i networksetup
                        if (!(show_wlan_select)) {
                            if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                        } else {
                            if (setupwlanselectofset>0) setupwlanselectofset--;
                        }
                    }
                    if (do_show_setup_font) {
                        if (setupfontselectofset>0) setupfontselectofset--;
                    }
                    if (do_show_setup_keys) {
                        if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                    }
                    if (do_show_videoplayer) {
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
                break;
        case GLUT_KEY_PAGE_UP:
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
                // if indside tv overview reset show time to now (localtime)
                if (vis_tv_oversigt) {
                    // reset tvgide time to now
                    aktiv_tv_oversigt.reset_tvguide_time();
                    if (ask_tv_record) {
                      ask_tv_record=false;
                      tvknapnr=0;
                      do_zoom_tvprg_aktiv_nr=0;			                          		// slet valget
                    }

                }
                if ((vis_radio_oversigt) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                    if ((_rangley>0) && (radio_key_selected<=fnumbersoficonline) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                         _rangley-=0;
                         radio_select_iconnr=1;
                    }
                    _rangley=MOVIE_CS;
                    radio_key_selected=1;
                    radio_select_iconnr=1;
                }
                // if indside a setup menu
                if (do_show_setup) {
                  if (do_show_tvgraber) {
                    // select line 0
                    tvchannel_startofset=0;
                    do_show_setup_select_linie=0;
                  }
                }

                break;
        case GLUT_KEY_END:
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
                }
                break;

    }
    if (debugmode) {
      if (vis_music_oversigt) fprintf(stderr,"Music_key_selected = %d  music_select_iconnr = %d musicoversigt_antal= %d \n ",music_key_selected,music_select_iconnr,musicoversigt_antal);
      if (vis_film_oversigt) fprintf(stderr,"ang = %4f film_key_selected = %d  film_select_iconnr = %d filmoversigt_antal=%d \n ",_fangley,film_key_selected,film_select_iconnr,film_oversigt.film_antal());
      if (do_show_tvgraber) fprintf(stderr,"line %2d of %2d ofset = %d \n",do_show_setup_select_linie,PRGLIST_ANTAL,tvchannel_startofset);
      if (vis_tv_oversigt) fprintf(stderr,"tvvalgtrecordnr %2d tvsubvalgtrecordnr %2d antal kanler %2d kl %2d \n",tvvalgtrecordnr,tvsubvalgtrecordnr,aktiv_tv_oversigt.tv_kanal_antal(),aktiv_tv_oversigt.vistvguidekl);
    }
}







// keyboard handler *******************************************************************************************************

void handleKeypress(unsigned char key, int x, int y) {
    const char optionmenukey='O';
    char id[80];		// bruges af wlan setup
    char tmptxt[80];
    char temptxt[200];
    saver_irq=true;                                     // stop screen saver

    char path[1024];

    stream_loadergfx_started_break=true;		// break tread stream gfx loader

    if (key=='+') {
      if (configsoundvolume<1.0f) configsoundvolume+=0.05f;
      #if defined USE_FMOD_MIXER
      if (sndsystem) channel->setVolume(configsoundvolume);
      #endif
      save_config((char *) "/etc/mythtv-controller.conf");
      show_volume_info=true;					// show volume info window
      vis_volume_timeout=80;
    }

    if (key=='-') {                               // volume down
      if (configsoundvolume>0) configsoundvolume-=0.05f;
      #if defined USE_FMOD_MIXER
      if (sndsystem) channel->setVolume(configsoundvolume);
      #endif
      save_config((char *) "/etc/mythtv-controller.conf");
      show_volume_info=true;					// show volume info window
      vis_volume_timeout=80;
    }

    if ((key!=27) && (key!='*') && (key!=13) && (key!='+') && (key!='-') && ((vis_music_oversigt) || ((vis_radio_oversigt) && (key!=optionmenukey)) || (do_show_setup))) {
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
              // søg sang/artist navn
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
                  if (key!=13) {
                      keybuffer[keybufferindex]=key;
                      keybufferindex++;
                      keybuffer[keybufferindex]='\0';       // else input key text in buffer
                  }
              }
              // søg efter radio station navn
              if ((vis_radio_oversigt) && (!(show_radio_options))) {
                  if (key!=13) {
                      keybuffer[keybufferindex]=key;
                      keybufferindex++;
                      keybuffer[keybufferindex]='\0';       // else input key text in buffer

                      if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);

                  }
              }

              if (do_show_setup) {				// if setup window
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
                  } else if (do_show_videoplayer) {
                      // video player setting
                      if (do_show_setup_select_linie==0) {
                        if (key!=13) {
                            keybuffer[keybufferindex]=key;
                            keybufferindex++;
                            keybuffer[keybufferindex]='\0';	// else input key text in buffer
                            if (debugmode) printf("Keybuffer=%s\n",keybuffer);
                        }
                      }
                      // video player screen mode
                      if (do_show_setup_select_linie==1) {
                        if (key==32) {
                          if (configdefaultplayer_screenmode<3) configdefaultplayer_screenmode++;
                          else configdefaultplayer_screenmode=0;
                        }
                      }
                      // select debug mode to show in console
                      if (do_show_setup_select_linie==2) {
                        if (key==32) {
                          if (debugmode>1) debugmode=debugmode*2; else debugmode++;
                          if (debugmode>512) debugmode=0;
                        }
                      }
                      // uv meter mode
                      if (do_show_setup_select_linie==3) {
                        if (key==32) {
                          configuvmeter=+1;
                          if (configuvmeter>2) configuvmeter=0;
                        }
                        if (configuvmeter==0) strcpy(keybuffer,"none");
                        if (configuvmeter==1) strcpy(keybuffer,"Simple");
                        if (configuvmeter==2) strcpy(keybuffer,"Dual");

                        sprintf(keybuffer,"%d",configuvmeter);
                      }
                  } else if (do_show_tvgraber) {
                    // in setup menu
                    if ((key==32) && (do_show_setup_select_linie==0)) {
                      if (aktiv_tv_graber.graberaktivnr<aktiv_tv_graber.graberantal+1) aktiv_tv_graber.graberaktivnr++; else aktiv_tv_graber.graberaktivnr=0;
                      // husk last selected
                      strcpy(configbackend_tvgraber_old,configbackend_tvgraber);
                      strcpy(configbackend_tvgraber,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
                      // set load flag to show_setup_tv_graber() func not good way to do it global var
                      // and delete old db file to get the graber to update it
                    }
/*
                    if (do_show_setup_select_linie==0) {
                      if (strcmp(configbackend_tvgraber_old,configbackend_tvgraber)!=0) {
                        unlink("~/tvguide_channels.dat");
                        hent_tv_channels=false;
                        strcpy(configbackend_tvgraber_old,configbackend_tvgraber);
                      }
                    }
*/
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
           } else if (do_show_setup_keys) {
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
    } else {
        switch(key) {
            case 27:
                    // close setup windows again or close proram of not in menu
                    if (do_show_setup) {
                      if (do_show_tvgraber) {
                        // kill running graber
                        killrunninggraber();
                        // clear old tvguide in db
                        aktiv_tv_oversigt.cleartvguide();
                        // save chennel list info to internal datafile
                        order_channel_list();
                        save_channel_list();
                        // buid new config file for xmltv from saved db
                        xmltv_configcontrol.graber_configbuild();
                        // hent ny tv guide
                        //if (get_tvguide_fromweb()!=-1)
                        // update db med tvguide
                        aktiv_tv_oversigt.parsexmltv("tvguide.xml");
                        // order channels in db (mysqldb)
                        order_channel_list_in_tvguide_db();
                        // hent/update tv guide from db
                        aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
                        // set update flag in display() func
                        firsttime_xmltvupdate=true;
                        // close tv graber windows again
                        do_show_tvgraber=false;
                        do_show_setup=false;
                      } else if (do_show_videoplayer) do_show_videoplayer=false; else
                      if (do_show_setup_sql) do_show_setup_sql=false; else
                      if (do_show_setup_font) do_show_setup_font=false; else
                      if (do_show_setup_keys) do_show_setup_keys=false; else
                      if (do_show_setup_tema) do_show_setup_tema=false; else
                      if (do_show_setup_sound) do_show_setup_sound=false; else
                      if (do_show_setup_screen) do_show_setup_screen=false; else
                      do_show_setup=false;
                    }
                    else if (vis_music_oversigt) vis_music_oversigt=false;
                    else if (vis_radio_oversigt) vis_radio_oversigt=false;
                    else if (vis_film_oversigt) vis_film_oversigt=false;
                    else if (vis_stream_oversigt) vis_stream_oversigt=false;
                    else if (vis_tv_oversigt) vis_tv_oversigt=false;
                    else if (vis_recorded_oversigt) vis_recorded_oversigt=false;
                    else {
                      remove("mythtv-controller.lock");
                      exit(0);
                    }
                    break;
            case '*':
                    if (vis_music_oversigt) do_zoom_music_cover=!do_zoom_music_cover;               // show/hide music info
                    if (vis_radio_oversigt) do_zoom_radio=!do_zoom_radio;               // show/hide music info
                    if (vis_film_oversigt) do_zoom_film_cover=!do_zoom_film_cover;
                    if ((vis_tv_oversigt) && (do_zoom_tvprg_aktiv_nr>0)) {
                      do_zoom_tvprg_aktiv_nr=0;
                    } else if (vis_tv_oversigt) {
                      // spørg kan/skal vi optage den ?
                      ask_tv_record=true;
                      tvknapnr=tvsubvalgtrecordnr;
                      do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
                    }
                    break;
            case optionmenukey:
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
                      if (debugmode) printf("Loading tvguidedb file\n");
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
                    if (vis_movie_options) {
                      vis_movie_sort_option=1;
                      vis_movie_options=false;			// luk option window igen
                    }
                    break;
            case 't':
                    if (vis_movie_options) {
                        vis_movie_sort_option=2;
                        vis_movie_options=false;                        // luk option window igen
                    }
                    break;
            case 'u':
                    if ((vis_tv_oversigt) && (loading_tv_guide==false)) {
                      // u key
                      // Update tv guide
                      printf("Update tv guide\n");
                      // set flag for show update
                      do_update_xmltv_show=true;
                      loading_tv_guide=true;
                      if (strcmp(configbackend,"mythtv")==0) {
                        update_xmltv_phread_loader();                   // start thred update flag in main loop
                      } else if (strcmp(configbackend,"xbmc")==0) {
                        update_xmltv_phread_loader();
                      }
                    }
                    break;
            case 13:
                    if (debugmode) {
                        if (vis_music_oversigt) fprintf(stderr,"Enter key pressed, update music list.\n");
                        else if (vis_radio_oversigt) fprintf(stderr,"Enter key pressed, play radio station.\n");
                        else if (vis_stream_oversigt) fprintf(stderr,"Enter key pressed, update stream view.\n");
                        else if (do_show_setup_network) fprintf(stderr,"Enter key pressed in set network\n");
                        else if (vis_tv_oversigt) fprintf(stderr,"Enter key pressed in vis tv oversigt\n");
                        else if (do_show_tvgraber) fprintf(stderr,"Enter key pressed in vis show tvgraber\n");
                    }
                    if (vis_radio_oversigt) {
                        rknapnr=0;
                        hent_radio_search=true;			  	// start radio station search
                        radio_key_selected=1;
                        _rangley=0.0f;
                    }


                    if (vis_stream_oversigt) {
                        hent_stream_search=true;				// start radio station search
                        stream_key_selected=1;
                        _sangley=0.0f;
                        sknapnr=stream_select_iconnr;
                        do_play_stream=1;
                    }

                    if ((vis_music_oversigt) && (keybufferopenwin)) {
                        hent_music_search=true;                         // start music search  (set flag)
                        mknapnr=0;				                            	// reset mouse/key pos in vis_music_overs
                        music_key_selected=1;
                        music_select_iconnr=0;
                        music_icon_anim_icon_ofset=0;
                        music_icon_anim_icon_ofsety=0;
                        _mangley=0.0f;
                    }


                    if ((vis_music_oversigt) && (!(do_zoom_music_cover))) {
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
//                        do_zoom_music_cover_remove_timeout=showtimeout;		// set show music info timeout
                    }

                    // enter key pressed
                    if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
                      ask_open_dir_or_play=false;                 // flag luk vindue igen
                      do_play_music_cover=1;                      // der er trykket på cover play det
                      do_zoom_music_cover=false;                  // ja den skal spilles lav zoom cover info window
                      do_find_playlist=true;                      // find de sange som skal indsættes til playlist (og load playlist andet sted)
                    }


                    if ((vis_radio_oversigt) && (show_radio_options==false)) {
                        rknapnr=radio_select_iconnr;					// hent button
                        fprintf(stderr,"Set do_play_radio flag rknapnr=%d \n",rknapnr);
                        if (rknapnr>0) do_play_radio=1;					// start play
                    }

                    // opdatere radio oversigt igen efter vis radio options
                    if ((vis_radio_oversigt) && (show_radio_options)) {
                        radiooversigt.clean_radio_oversigt();				// clean old liste
                        radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
                        radiooversigt.load_radio_stations_gfx();
                        show_radio_options=false;
                        _rangley=0;
                    }

                    if ((do_show_setup) && (do_show_setup_font)) {
                        if (debugmode & 4) fprintf(stderr,"Set aktiv font to %s \n",aktivfont.typeinfo[setupfontselectofset].fontname);
                        strcpy(configfontname,aktivfont.typeinfo[setupfontselectofset].fontname);
                        aktivfont.selectfont(configfontname);
                    }
                    if (vis_recorded_oversigt) {
                        do_play_recorded_aktiv_nr=1;							// set play aktiv recorded program flag (bliver sat igang i draw)
                    }
                    // tv guide oversigt
                    if (vis_tv_oversigt) {
                        // hvis der trykkes enter på default ask_tv_record (yes)
                        // blivere den sat til record mode (create mysql data in record table)
                        printf("Ask om vi skal optage program \n");
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
                            ask_tv_record=true;
                            tvknapnr=tvsubvalgtrecordnr;
                            do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
                        }
                    }

                    // stream oversigt
                    if ((vis_stream_oversigt) && (sknapnr-1>=0)){
                        if (streamoversigt.type==0) {
                            strncpy(temptxt,streamoversigt.get_stream_name(sknapnr-1),200);

                            streamoversigt.clean_stream_oversigt();

                            if (debugmode & 128) fprintf(stderr,"stream nr %d name %s \n ",sknapnr-1,temptxt);
                            streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");

                            do_play_stream=false;
                        } else if (streamoversigt.type==1) {
                            streamoversigt.clean_stream_oversigt();
                            streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr-1),streamoversigt.get_stream_path(sknapnr-1));
                            do_play_stream=false;
                        } else {
                            // back button
                            if (debugmode & 128) fprintf(stderr,"stream nr %d \n ",sknapnr-1);
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
                            if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash",9)==0) {
                                startstream=true;
                            } else {
                                startstream=true;
                            }
                        }
                        //sknapnr=0;
                        stream_key_selected=1;
                        stream_select_iconnr=0;
                        _sangley=0.0f;
                    }

                    // enter pressed in setup window xmltv
                    // select new tv guide provider
                    if ((do_show_tvgraber) && (do_show_setup_select_linie==0)) {
                      if (strcmp(configbackend_tvgraber_old,configbackend_tvgraber)!=0) {
                        // clean all tv guide data and reload
                        // remove config dat file
                        printf("* Delete old tvguide *\n");
                        printf("* Update new tvguide *\n");
                        getuserhomedir(path);
                        strcat(path,"/tvguide_channels.dat");
                        unlink(path);
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
                        wifinets.get_networkid(setupwlanselectofset,id);
                        strcpy(confighostwlanname,id);
                        sprintf(tmptxt,"sudo /sbin/iwconfig wlan0 essid %s",id);
                        fprintf(stderr,"Charge network by %s \n",tmptxt);
                        system(tmptxt);
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



// controll by the remove controller


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
                    if (debugmode & 1) fprintf(stderr,"Lirc Command %s \n ",c);

                    saver_irq=true;				// RESET (sluk hvis aktiv) screen saver


                    if (strcmp(c,"Music")==0) {							// show music directoy
                        vis_tv_oversigt=false;
                        vis_film_oversigt=false;
                        do_zoom_film_cover=false;
                        vis_music_oversigt=!vis_music_oversigt;
                        vis_recorded_oversigt=false;
                        vis_radio_oversigt=false;

                    }

                    if (strcmp(c,"Radio")==0) {							// show music directoy
                        vis_tv_oversigt=false;
                        vis_film_oversigt=false;
                        do_zoom_film_cover=false;
                        vis_music_oversigt=false;
                        vis_radio_oversigt=!vis_radio_oversigt;
                        vis_recorded_oversigt=false;
                    }

                    if (strcmp(c,"Videos")==0) {						// Show video directory
                        vis_tv_oversigt=false;
                        vis_film_oversigt=!vis_film_oversigt;
                        vis_music_oversigt=false;
                        vis_recorded_oversigt=false;
                        vis_radio_oversigt=false;
                    }

                    if (strcmp(c,"RecTV")==0) {							// recorded tv
                        vis_tv_oversigt=false;
                        vis_film_oversigt=false;
                        vis_music_oversigt=false;
                        do_zoom_music_cover=false;
                        vis_recorded_oversigt=!vis_recorded_oversigt;
                        vis_radio_oversigt=false;
                    }

                    if (strcmp(c,"Guide")==0) {							// tv program guide
                        vis_film_oversigt=0;
                        vis_music_oversigt=false;
                        do_zoom_music_cover=false;
                        vis_recorded_oversigt=false;
                        vis_radio_oversigt=false;
                        vis_tv_oversigt=!vis_tv_oversigt;
                    }


                    // Home back to starvis_radio_oversigtt of program init
                    if (strcmp(c,"Home")==0) {
                        do_play_music_aktiv=false;							// sluk music info cover
                        vis_tv_oversigt=false;								// sluk tv oversigt
                        vis_film_oversigt=false; 							// sluk film oversigt
                        vis_music_oversigt=false;  							// sluk music oversigt
                        do_zoom_music_cover=false;							// sluk zoom cd cover
                        vis_recorded_oversigt=false;							// sluk recorded program oversigt
                        vis_radio_oversigt=false;							// sluk radio oversigt
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
                                                    music_select_iconnr+=numbers_cd_covers_on_line;			// add to next line
                                                } else {
                                                    music_key_selected+=numbers_cd_covers_on_line;
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
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
                                                    _mangley+=41.0f;								//scroll gfx down
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected+=numbers_cd_covers_on_line;
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
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
                                                    _mangley+=41.0f;								//scroll gfx down
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected+=numbers_cd_covers_on_line;
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
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
                                                    do_music_icon_anim_icon_ofset=1;						//
                                                    _mangley+=41.0f;								//scroll gfx down
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
                                                } else {
                                                    music_key_selected+=numbers_cd_covers_on_line;
                                                    music_select_iconnr+=numbers_cd_covers_on_line;
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


                        if ((vis_radio_oversigt) && (show_radio_options==false)) {
                            if (((int) radio_select_iconnr<(int) radiomoversigt_antal-5) && (radio_icon_anim_icon_ofset==0)) {
                              if (radio_select_iconnr+numbers_radio_covers_on_line>16) {		// skal vi scroll liste up
                                do_radio_icon_anim_icon_ofset=1;
                                _rangley+=RADIO_CS;
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
                                case 1: if ((stream_select_iconnr<(int ) streamoversigt_antal-5) && (stream_icon_anim_icon_ofset==0)) {
                                            if (stream_select_iconnr+numbers_stream_covers_on_line>11) {		// skal vi scroll liste up
                                                do_stream_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //stream_select_iconnr-=numbers_stream_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                            }
                                            stream_key_selected+=numbers_stream_covers_on_line;
                                            stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 2: if ((stream_select_iconnr<(int ) streamoversigt_antal-5) && (stream_icon_anim_icon_ofset==0)) {
                                            if (stream_select_iconnr+numbers_stream_covers_on_line>13) {		// skal vi scroll liste up
                                                do_stream_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //stream_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                            }
                                            stream_key_selected+=numbers_stream_covers_on_line;
                                            stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 3:
                                        if (((int) stream_select_iconnr<(int) streamoversigt_antal-5) && (stream_icon_anim_icon_ofset==0)) {
                                            if (stream_select_iconnr+numbers_stream_covers_on_line>19) {		// skal vi scroll liste up
                                                do_stream_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //stream select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                            }
                                            stream_key_selected+=numbers_stream_covers_on_line;
                                            stream_select_iconnr+=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 4: if ((stream_select_iconnr<(int) streamoversigt_antal-5) && (stream_icon_anim_icon_ofset==0)) {
                                            if (stream_select_iconnr+numbers_stream_covers_on_line>17) {		// skal vi scroll liste up
                                                do_stream_icon_anim_icon_ofset=1;
                                                _rangley+=RADIO_CS;
                                                //stream_select_iconnr-=numbers_radio_covers_on_line;		// husk at trække fra da vi står samme sted
                                                if (stream_key_selected>0) stream_key_selected-=numbers_stream_covers_on_line;
                                            }
                                            stream_key_selected+=numbers_stream_covers_on_line;
                                            stream_select_iconnr+=numbers_stream_covers_on_line;
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
                            reset_recorded_texture=true;
                        }


                        if (vis_tv_oversigt) {

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

//                           _anglex+=1.0*5;
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

                        if ((vis_radio_oversigt) && (show_radio_options)) {
                            radiooversigt.lastradiooptselect();
                        }



                        // stream
                        if ((vis_stream_oversigt) && (show_stream_options==false)) {
                            switch(screen_size) {
                                case 1:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                            if (((int) stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 stream_select_iconnr-=numbers_stream_covers_on_line;
                                            } else stream_select_iconnr-=numbers_stream_covers_on_line;
                                            if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }

                                        break;
                                case 2:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                            if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 stream_select_iconnr-=numbers_stream_covers_on_line;
                                            } else stream_select_iconnr-=numbers_stream_covers_on_line;
                                            if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 3:

                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                            if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 stream_select_iconnr-=numbers_stream_covers_on_line;
                                            } else stream_select_iconnr-=numbers_stream_covers_on_line;
                                            if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                                case 4:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
                                            if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
                                                 _rangley-=MOVIE_CS;
                                                 stream_select_iconnr-=numbers_stream_covers_on_line;
                                            } else stream_select_iconnr-=numbers_stream_covers_on_line;
                                            if (stream_key_selected>numbers_stream_covers_on_line) stream_key_selected-=numbers_stream_covers_on_line;
                                        }
                                        break;
                            }
                        }



                        if (vis_recorded_oversigt) {
                            if ((visvalgtnrtype==1) && (valgtrecordnr>0)) {
                                valgtrecordnr--;
                                subvalgtrecordnr=0;
                            } else if ((visvalgtnrtype==2) && (subvalgtrecordnr>0)) subvalgtrecordnr--;	// bruges til visning af optaget programmer
                            reset_recorded_texture=true;		// load optaget programs texture gen by mythtv
                        }



                        if (vis_tv_oversigt) {
                            if ((tvvisvalgtnrtype==1) && (tvvalgtrecordnr>0)) {
                                tvvalgtrecordnr--;
                                tvsubvalgtrecordnr=0;
                            } else if ((tvvisvalgtnrtype==2) && (tvsubvalgtrecordnr>0)) tvsubvalgtrecordnr--;	// bruges til visning af optaget programmer

// old ver                           _anglex-=1.0*5;
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
                                             _mangley-=41;
                                             music_key_selected+=numbers_cd_covers_on_line-1;  // den viste på skærm af 1 til 20
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
                        if (vis_recorded_oversigt) {
                            visvalgtnrtype=1;
                        }
                        if (vis_tv_oversigt) {
                            tvvisvalgtnrtype=1;
                        /*
                            if (tvstartxofset<2300) {
                                tvstartxofset+=tvprgsstep;
                                glDeleteLists(tvoversigt, 1);				// delete old list
                                tvoversigt=glGenLists(1);				// make new
                                glNewList(tvoversigt,GL_COMPILE);
                                aktiv_tv_oversigt.build_tv_oversigt(tvstartxofset,0);
                                glEndList();

                            }
                         */
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
                                        music_key_selected-=numbers_cd_covers_on_line;			// den viste på skærm af 1 til 20
                                        music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                    } else {
                                        music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                                    }
                                    music_key_selected++;
                                }

/*
                                switch (screen_size) {
                                    case 1:

                                        printf("music_select_iconnr =%d  music_select_iconnr mod 33= %d  music_key_selected mod 7=%d  \n",music_select_iconnr, music_select_iconnr % 33,music_key_selected % 7);
                                        if ((music_key_selected % 12==0) || ((music_select_iconnr==11) && (music_key_selected % 4==0))) {
                                            do_music_icon_anim_icon_ofset=1;
                                            _mangley+=(6.5f)/2;
                                            music_select_iconnr-=3;
                                        } else {
                                            music_select_iconnr++;
                                        }
                                        music_key_selected++;
                                        break;
                                    case 2:
                                        //
//                                printf("music_select_iconnr =%d  music_select_iconnr mod 33= %d  music_key_selected mod 7=%d  \n",music_select_iconnr, music_select_iconnr % 33,music_key_selected % 7);
                                        if ((music_key_selected % 7==0) && (music_select_iconnr==34)) {
                                            do_music_icon_anim_icon_ofset=1;
                                            _mangley+=(6.5f)/2;
                                            music_select_iconnr-=6;
                                        } else {
                                            music_select_iconnr++;
                                        }
                                        music_key_selected++;
                                        break;
                                    case 3:
//      		                          printf("music_select_iconnr =%d  music_select_iconnr mod 54= %d  music_key_selected mod 11=%d  \n",music_select_iconnr, music_select_iconnr % 54,music_key_selected % 11);
                                        if ((music_key_selected % 11==0) && (music_select_iconnr==54))  {
                                            do_music_icon_anim_icon_ofset=1;
                                            _mangley+=(6.5f)/2;
                                            music_select_iconnr-=10;
                                        } else {
                                            music_select_iconnr++;
                                        }
                                        music_key_selected++;
                                        break;
                                    case 4:
//      		                          printf("music_select_iconnr =%d  music_select_iconnr mod 54= %d  music_key_selected mod 11=%d  \n",music_select_iconnr, music_select_iconnr % 54,music_key_selected % 11);
                                        if ((music_key_selected % 11==0) && (music_select_iconnr==54))  {
                                            do_music_icon_anim_icon_ofset=1;
                                            _mangley+=(6.5f)/2;
                                            music_select_iconnr-=10;
                                        } else {
                                            music_select_iconnr++;
                                        }
                                        music_key_selected++;
                                        break;
                                }

*/

                            }
                        }
                        if (vis_film_oversigt) {
                            switch (screen_size) {
                                case 1:
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
                                        break;

                                case 2:
                                        if ((vis_film_oversigt) && (film_select_iconnr<(int) film_oversigt.film_antal()-1)) {
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
/*
                                case 1:
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
                                        break;

                                case 2:
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
                                        break;
*/
                                case 3:
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

                                        break;
/*
                                case 4:
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
                                        break;
*/
                            }
                        }

                        // stream
                        if (vis_stream_oversigt) {
                            switch (screen_size) {
                                case 1:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt_antal-1)) {
                                            if (stream_select_iconnr<(int) streamoversigt_antal) {
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
                                        if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt_antal-1)) {
                                            if (stream_select_iconnr<(int) streamoversigt_antal) {
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
                                        if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt_antal-1)) {
                                            if (stream_select_iconnr<(int) streamoversigt_antal) {
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
                                case 4:
                                        if ((vis_stream_oversigt) && (stream_select_iconnr<(int) streamoversigt_antal-1)) {
                                            if (stream_select_iconnr<(int) streamoversigt_antal) {
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
                            visvalgtnrtype=2;
                        }

                        if (vis_tv_oversigt) {
                            tvvisvalgtnrtype=2;
                            /*
                            tvstartxofset-=tvprgsstep;
                            glDeleteLists(tvoversigt, 1);				// delete old list
                            tvoversigt=glGenLists(1);				// make new
                            glNewList(tvoversigt,GL_COMPILE);
                            aktiv_tv_oversigt.build_tv_oversigt(tvstartxofset,0);
                            glEndList();
                            */
//                           _anglez-=1.0*5;
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



//            if (strcmp("play",c)==0) {                             // start music play
//                do_stop_music=0;
//                do_shift_song=true;
//                if (do_play_music_aktiv_table_nr>=aktiv_playlist.numbers_in_playlist()) {
//                    // reset play list
//                    do_play_music_aktiv_table_nr=1;
//                }
//                printf("Start play \n");
//            }


                   if (strcmp("Next",c)==0) {
                        // next song
                        if ((vis_music_oversigt) && (!(do_shift_song))) {
                            if ((do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) && (do_shift_song==false)) {
                                do_play_music_aktiv_table_nr++;
                                do_shift_song=true;
                                do_zoom_music_cover=true;
                            }
                        }
                        if (vis_tv_oversigt) {
                            aktiv_tv_oversigt.changetime(60*60*24);
                            aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);

                        }
                    }

                    if (strcmp("Prev",c)==0) {
                        // last song
                         if ((vis_music_oversigt) && (!(do_shift_song))) {
                            if ((do_play_music_aktiv_table_nr>1) && (do_shift_song==false)) {
                                do_play_music_aktiv_table_nr--;						// skift aktiv sang
                                do_shift_song=true;							// sæt flag til skift
                                do_zoom_music_cover=true;
                            }
                         }
                         if (vis_tv_oversigt) {
                            aktiv_tv_oversigt.changetime(-(60*60*24));
                            aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,1);

                         }
                    }

                    if (strcmp("Key-vol-up",c)==0) {
                        if (configsoundvolume<1.0f) configsoundvolume+=0.1f;
                        #if defined USE_FMOD_MIXER
                        if (sndsystem) channel->setVolume(configsoundvolume);
                        #endif
                        show_volume_info=true;					// show volume info window
                        vis_volume_timeout=80;
                    }

                    if (strcmp("Key-vol-down",c)==0) {
                        if (configsoundvolume>0) configsoundvolume-=0.1f;
                        #if defined USE_FMOD_MIXER
                        if (sndsystem) channel->setVolume(configsoundvolume);
                        #endif
                        show_volume_info=true;					// show volume info window
                        vis_volume_timeout=80;
                    }

                    if (strcmp("Return",c)==0) {
                        if ((vis_music_oversigt) && (!(do_zoom_music_cover))) {
                            if (ask_open_dir_or_play) {
                                ask_open_dir_or_play=0;			// flag luk vindue igen
                                do_play_music_cover=1;			// der er trykket på cover play det
                                do_zoom_music_cover=false;			// ja den skal spilles lav zoom cover info window
                                do_find_playlist=true;			// find de sange som skal indsættes til playlist (og load playlist andet sted)
                                mknapnr=music_select_iconnr;		// OLD VER music_key_selected;
                                if (debugmode & 2) fprintf(stderr,"music_key_selected =%d \n",music_key_selected);
                            } else {
                                // er det et normal dir
                                mknapnr=music_select_iconnr; // OLD VER music_key_selected-1;
                                if (debugmode & 2) fprintf(stderr,"Lirc music selected:%d \n",mknapnr);

                                // if (debugmode & 2) printf("Mouse pressed over %d husk knap= %d \n",mknapnr,husk_knapnr);
                                // ja hvis felts oversigttype=0
                                if (musicoversigt[mknapnr].oversigttype==0) {
                                    //                            mknapnr=music_key_selected;
                                    do_play_music_aktiv_nr=musicoversigt[mknapnr].directory_id;		// set den aktive mappe dir id
                                    antal_songs=hent_antal_dir_songs(musicoversigt[mknapnr].directory_id);		//
                                    if (debugmode & 2) fprintf(stderr,"Found numers of songs in :%4d dirid:%4d  named:%s \n",antal_songs,musicoversigt[mknapnr].directory_id,musicoversigt[mknapnr].album_name);
                                    if ((antal_songs==0) || (musicoversigt[mknapnr].directory_id==0)) {	// er der ingen sange i dir lav en auto open
                                        ask_open_dir_or_play_aopen=1; 						// flag auto open
                                        if (musicoversigt[mknapnr].directory_id==0) antal_songs=0;
                                    } else {
                                        ask_open_dir_or_play_aopen=0;						// ingen auto open
                                    }

                                } else {
                                    // nej det er playlister
                                    mknapnr=music_select_iconnr;
                                    do_play_music_aktiv_nr=musicoversigt[mknapnr].directory_id;			// = playlistnr
                                    if (debugmode & 2) fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                                    if (do_play_music_aktiv_nr>0) {
                                        antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                                    } else antal_songs=0;

                                    if (antal_songs==0) {							// er der ingen sange i dir lav en auto open
                                        ask_open_dir_or_play_aopen=1; 						// flag auto open
                                    } else {
                                        ask_open_dir_or_play_aopen=0;						// ingen auto open
                                    }
                                }
                                // do auto open
                                if ((antal_songs==0) && (musicoversigt[mknapnr].oversigttype==0)) {
                                    // normalt dir (IKKE playlist)
                                    opdatere_music_oversigt(musicoversigt,musicoversigt[mknapnr].directory_id);
                                    opdatere_music_oversigt_icons();
                                    music_icon_anim_icon_ofset=0;
                                    music_icon_anim_icon_ofsety=0;
                                    mknapnr=0;

                                    ask_open_dir_or_play_aopen=1;

                                    if (debugmode & 2) fprintf(stderr,"Set Autoopen on dir id %d \n ", musicoversigt[mknapnr].directory_id);

                                    music_key_selected=1;		// reset cursor position
                                    music_select_iconnr=1;		// reset this to (beskriver hvor vi er på skærmen og ikke på listen som music_key_selected
                                } else if (musicoversigt[mknapnr].oversigttype==-1) {
                                    // playliste
                                    if (antal_songs>0) ask_open_dir_or_play_aopen=0;			// skal vi spørge slå auto fra
                                    else ask_open_dir_or_play_aopen=1;					// else ja autoopen
                                    // hent liste over mythtv playlist
                                    opdatere_music_oversigt_playlists(musicoversigt);			// vis alle playlister
                                }


                                if ((antal_songs>0) && (do_play_music_aktiv_nr)) {
                                    do_swing_music_cover=1;
                                    ask_open_dir_or_play=1;
                                }
                                if (ask_open_dir_or_play_aopen) {
                                    /// reset mouse/key pos in vis_music_oversigt
                                    mknapnr=0;
                                    music_key_selected=1;
                                    music_select_iconnr=0;
                                    music_icon_anim_icon_ofset=0;
                                    music_icon_anim_icon_ofsety=0;
                                    _mangley=0.0f;
                                }
//                                if (debugmode & 2) printf("music_key_selected %d musik knap nr = %d  music_select_iconnr= %d \n ",music_key_selected,mknapnr,music_select_iconnr);
                            }
                            do_zoom_music_cover_remove_timeout=showtimeout;		// show music info timeout
                            if (debugmode & 2) fprintf(stderr,"lirc Set Play it flag \n");
                        }

                        if ((vis_film_oversigt) && (do_zoom_film_cover)) {
                            fprintf(stderr,"lirc Start movie player.\n");
                            startmovie=true;
                        }

                        if (vis_film_oversigt) {				// select movie to show info for
                            do_zoom_film_cover=true;
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
                            if (ask_tv_record) fprintf(stderr,"lirc Show tvprogram info.\n"); else printf("lirc Hide tvprogram info.\n");
                        }


                        // start play radio
                        if ((vis_radio_oversigt) && (!(show_radio_options))) {
                            // play radio station
                            rknapnr=radio_key_selected;		// hent button
                            if (rknapnr>0) do_play_radio=1;
                        }

                        // opdatere radio oversigt efter efter pressed on the remorte control from lirc
                        if ((vis_radio_oversigt) && (show_radio_options)) {
                            radiooversigt.clean_radio_oversigt();			// clean old liste
                            radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
                            radiooversigt.load_radio_stations_gfx();
                            show_radio_options=false;
                        }
                    }

                    if (strcmp("Stop",c)==0) {
                        // stop all music
                        do_stop_music=true;

                    }
                    if (strcmp("Power",c)==0) {
                        if (!remove("mythtv-controler.lock")) {
                            if (debugmode) fprintf(stderr,"No lock file.\n");
                        }
                        exit(2);				// QUIT program
                    }
                }
            }
            free(code);
            if (debugmode) {
//                if (vis_music_oversigt) printf("music_select_iconnr=%d _mangley = %g \n",music_select_iconnr,(_mangley/41)*5);
//                if (vis_film_oversigt) printf("film_select_iconnr=%d _fangley=%g  film_key_selected=%d \n",film_select_iconnr,(_fangley/51)*5,film_key_selected);
            }
        }

    }


    if (do_music_icon_anim_icon_ofset) {
        sinusofset+=2;		// add to table
        if (sinusofset>7200) sinusofset=0;

        //printf("%d do_music_icon_anim_icon_ofset=%d \n",sinusofset,do_music_icon_anim_icon_ofset);


        // stop sinus igen
        if ((do_music_icon_anim_icon_ofset==1) && ((int) sinusofset>50)) {
            sinusofset=0;
            do_music_icon_anim_icon_ofset=0;
        }
        if ((do_music_icon_anim_icon_ofset==-1) && ((int) sinusofset>50)) {
            sinusofset=0;
            do_music_icon_anim_icon_ofset=0;
        }

    }
    #if defined USE_FMOD_MIXER
    sndsystem->update();				// run update on fmod sound system
    #endif
//    printf("music_key_selected %d musik knap nr = %d  husk knapnr %d  music_select_iconnr= %d \n ",music_key_selected,mknapnr,music_select_iconnr);
    glutTimerFunc(25, update, 0);
    glutPostRedisplay();


}




// init screen setup

bool init(void)
{

   #ifndef ALLOW_RUN_AS_ROOT
    if (geteuid () == 0) {
        fprintf (stderr, "MYTHTV-CONTROLLER is not supposed to be run as root. Sorry.\n"
        "If you need to use real-time priorities and/or privileged TCP ports\n");
        return 1;
    }
    #endif

/*  select clearing (background) color       */
    glClearColor (0.0, 0.0, 0.0, 0.0);

/*  initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, orgwinsizex, 0.0, orgwinsizey, -0.0,10.0);
}




// setup sound system

int init_sound_system(int devicenr) {
    int num;
//    FMOD_CAPS caps;
//    FMOD_SPEAKERMODE speakermode;
    char fmoddrivername[512];
    int count;
    unsigned int handle=0;
    char name[256];

#if defined USE_FMOD_MIXER
    printf("Setup FMOD soundsystem\n");
    result = FMOD::System_Create(&sndsystem);
    ERRCHECK(result,0);
    result = sndsystem->getVersion(&fmodversion);
    ERRCHECK(result,0);

    if (fmodversion < FMOD_VERSION) {
       printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", fmodversion, FMOD_VERSION);
       exit(0);
    }

    //result = sndsystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
    //ERRCHECK(result,0);
    for(int n=0;n<9;n++) strcpy(avalible_device[n],"");
    printf("\nSound cards \n");
    printf("-----------\n");
    int numdrivers;
    result = sndsystem->getNumDrivers(&numdrivers);
    ERRCHECK(result,0);
    numbersofsoundsystems=numdrivers;				// numbers of devices
    for (count=0; count < numdrivers; count++) {
         char name[256];
         result = sndsystem->getDriverInfo(count, name, sizeof(name), 0,0,0,0);
         ERRCHECK(result,0);
         strcpy(avalible_device[count],name);			// save device name to list
         printf(" %d - %s\n", count , name);
    }
    if (devicenr>numdrivers) devicenr=0;			// hvis der er valgt et soundcard i config filen som ikke findes vælg default
    // setlect default soundcard=0
    result = sndsystem->getDriverInfo(devicenr, fmoddrivername, sizeof(name),0,0,0,0);
    ERRCHECK(result,0);

    printf("\nUse FMOD Driver :%s\n",fmoddrivername);

    //    result = sndsystem->setOutput(FMOD_OUTPUTTYPE_ALSA);
    //    ERRCHECK(result);
    //result = sndsystem->setPluginPath("fmodapi43005linux/api/plugins");
    //ERRCHECK(result,0);

//    printf("Codec plugins found\n");
//    printf("-------------------\n");
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

//        result = sndsystem->setSpeakerMode(FMOD_SPEAKERMODE_5POINT1);
//        ERRCHECK(result,0);

    // stereo
//    result = sndsystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
  //  ERRCHECK(result,0);

    result = sndsystem->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result,0);

    //  Bump up the file buffer size a little bit for netstreams (to account for lag).  Decode buffer is left at default.
    result = sndsystem->setStreamBufferSize(64*1024, FMOD_TIMEUNIT_RAWBYTES);
    ERRCHECK(result,0);
    strcpy(configmythsoundsystem,avalible_device[0]);

#endif

#if defined USE_SDL_MIXER
    printf("Setup SDL_MIXER soundsystem\n");
    // load support for the OGG and MOD sample/music formats
    int flags=MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC;
    SDL_Init(SDL_INIT_AUDIO);

    /* This is where we open up our audio device.  Mix_OpenAudio takes
    as its parameters the audio format we'd /like/ to have. */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
        printf("Unable to open audio!\n");
        exit(1);
    }
    /* If we actually care about what we got, we can ask here.
    In this program we don't, but I'm showing the function call here anyway in case we'd want to know later. */
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    sdlmusic=Mix_Init(flags);
    if(sdlmusic&flags != flags) {
        printf("Mix_Init: Failed to init required ogg,mp3,flac support!\n");
        printf("Mix_Init: %s\n", Mix_GetError());
        // handle error
    }

#endif
    return(1);
}





//
// phread dataload  check radio stations if it is online
//

void *radio_check_statusloader(void *data) {
  bool notdone=false;
  //pthread_mutex_lock(&count_mutex);
  printf("loader thread starting - Start checkling radio status's thread\n");
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


  printf("radio thread done\n");
  pthread_exit(NULL);
}




//
// phread dataload Music
//

void *datainfoloader_music(void *data) {
  //pthread_mutex_lock(&count_mutex);
  if (debugmode % 2) printf("loader thread starting - Loading music info from mythtv.\n");
  if (strcmp(configbackend,"mythtv")==0) {
      // opdatere music oversigt
      // hent alt music info fra database
      // check if internal music db exist if yes do set global use it
      if (global_use_internal_music_loader_system_exist()==true) {
        if (debugmode % 2) printf("******** Use global music database ********\n");
        global_use_internal_music_loader_system=true;
      } else {
        printf("Search for music in :%s\n",configdefaultmusicpath);
        // build new db (internal db loader)
        opdatere_music_oversigt_nodb(configdefaultmusicpath,musicoversigt);
        printf("Done update db from datasource.\n");
        global_use_internal_music_loader_system=true;
      }
      // load music db created by opdatere_music_oversigt_nodb function
      if (opdatere_music_oversigt(musicoversigt,0)>0) {
          //opdatere_music_oversigt_icons(); 					// load gfx icons
          printf("Nusic db loaded.\n");
      }
  } else {
    if (debugmode % 2) printf("Search for music in :%s\n",configdefaultmusicpath);
    if (opdatere_music_oversigt_nodb(configdefaultmusicpath,musicoversigt)==0) {
      printf("No music db loaded\n");
    }
  }
  printf("loader thread done loaded music info\n");
  pthread_exit(NULL);
}



//
// phread dataload Film
//


void *datainfoloader_movie(void *data) {
  //pthread_mutex_lock(&count_mutex);

  //pthread_mutex_unlock(&count_mutex);
  if (strcmp(configbackend,"mythtv")==0) {
    printf("loader thread starting - Loading movie info from mythtv.\n");
      film_oversigt.opdatere_film_oversigt();     	        // gen covers 3d hvis de ikke findes.
                                                            // load record file list
/*
      recordoversigt.opdatere_recorded_oversigt();    	    					// recorded program from mythtv
      // load old recorded list not some recorded any more
      oldrecorded.earlyrecordedload(configmysqlhost,configmysqluser,configmysqlpass);
      // load new tv schecule program
      newtcrecordlist.getrecordprogram(configmysqlhost,configmysqluser,configmysqlpass);		//
*/
      //create_radio_oversigt();										                          // Create radio mysql database if not exist
      //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
      // stream
      //streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");       // load all stream from mythtv
  } else {
    printf("Load movie from xbmc/kodi\n");
  }
  printf("loader thread done loaded %d movie \n",film_oversigt.get_film_antal());
  pthread_exit(NULL);
}



//
// phread dataload stream
//

void *datainfoloader_stream(void *data) {
  printf("loader thread starting - Loading stream info from mythtv.\n");
  if (strcmp(configbackend,"mythtv")==0) {
                                                                    // load record file list
/*
      recordoversigt.opdatere_recorded_oversigt();    	    					// recorded program from mythtv
      // load old recorded list not some recorded any more
      oldrecorded.earlyrecordedload(configmysqlhost,configmysqluser,configmysqlpass);
      // load new tv schecule program
      newtcrecordlist.getrecordprogram(configmysqlhost,configmysqluser,configmysqlpass);		//
*/
      //create_radio_oversigt();										                          // Create radio mysql database if not exist
      //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
      // stream
      streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");       // load all stream from mythtv
  }
  printf("loader thread done loaded stream stations \n");
  pthread_exit(NULL);
}


//
// phread dataload xmltv
//

void *datainfoloader_xmltv(void *data) {
  int error;
  //pthread_mutex_lock(&count_mutex);
  printf("Thread xmltv file parser starting....\n");
  //
  // multi thread
  // load xmltvguide from web
  if (get_tvguide_fromweb()!=-1) {
    error=aktiv_tv_oversigt.parsexmltv("tvguide.xml");
    if (error==0) {
      aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
      //save array to disk
      aktiv_tv_oversigt.saveparsexmltvdb();
    } else printf("Parse xmltv error (mysql connection error)\n");
  }
  // save config again
  save_config((char *) "/etc/mythtv-controller.conf");
  printf("parser xmltv guide done.\n");
  // set update flag for done
  do_update_xmltv_show=false;
  //pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}


//
// xmltv loader start from main loop then trigged by date
//

void *update_xmltv_phread_loader() {
  if (true) {
    pthread_t loaderthread2;           // load tvguide xml file in to db
    int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_xmltv,NULL);
    if (rc2) {
      printf("ERROR; return code from pthread_create() is %d\n", rc2);
      exit(-1);
    }
  }
}


//
// phread dataload
// NOT IN USE

void *datainfoloader(void *data) {
  //pthread_mutex_lock(&count_mutex);
  printf("loader thread starting\nLoading data from mythtv.\n");
  //pthread_mutex_unlock(&count_mutex);
  if (strcmp(configbackend,"mythtv")==0) {
      // Opdatere tv oversigt fra mythtv db
      aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);

      // opdatere music oversigt
      opdatere_music_oversigt(musicoversigt,0);        							// hent alt music info fra database                                                                                                    // opdatere film oversigt

      film_oversigt.opdatere_film_oversigt();      					        // gen covers 3d hvis de ikke findes.
                                                                    // load record file list
/*
      recordoversigt.opdatere_recorded_oversigt();    	    					// recorded program from mythtv
      // load old recorded list not some recorded any more
      oldrecorded.earlyrecordedload(configmysqlhost,configmysqluser,configmysqlpass);
      // load new tv schecule program
      newtcrecordlist.getrecordprogram(configmysqlhost,configmysqluser,configmysqlpass);		//
*/
      //create_radio_oversigt();										                          // Create radio mysql database if not exist
      //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
      // stream
      streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");       // load all stream from mythtv
  }
  printf("loader thread done loaded %d radio stations \n",radiooversigt_antal);
  pthread_exit(NULL);
}




CXBMCClient *xbmcclient=new CXBMCClient("");
int configxbmcver=1;
xbmcsqlite *xbmcSQL=NULL;


//
// phread dataload xbmc/kodi music db
//

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
  printf("loader thread starting - Loading music from xbmc/kodi).\n");
  //pthread_mutex_unlock(&count_mutex);

  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    allokay=true;
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
    mysql_query(conn,"SHOW TABLES LIKE 'music_albums'");
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
      dbexist=true;
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
    printf("XBMC - Loader starting.....\n");

    // get user homedir
    getuserhomedir(userhomedir);
    strcat(userhomedir,"/.kodi/userdata/Database/");
    dirp=opendir(userhomedir);                                // "~/.kodi/userdata/Database/");
    if (dirp==NULL) {
        printf("No xbmc/kodi db found\nOpen dir error %s \n","~/.kodi/userdata/Database/");
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
            printf("Kodi version 16 is found \n");
          }
          if (strcmp(filename,kodiver[1])==0) {
            kodiverfound=15;
            printf("Kodi version 15 is found \n");
          }
          if (strcmp(filename,kodiver[2])==0) {
            kodiverfound=14;
            printf("Kodi version 14 is found \n");
          }
          if (strcmp(filename,kodiver[3])==0) {
            kodiverfound=13;
            printf("Kodi version 13 is found \n");
          }
          if (strcmp(filename,kodiver[4])==0) {
            kodiverfound=12;
            printf("Kodi version 12 is found \n");
          }
          if (strcmp(filename,kodiver[5])==0) {
            kodiverfound=11;
            printf("Kodi version 11 is found \n");
          }
        }
      }
    }
    getuserhomedir(userhomedir);
    strcpy(videohomedirpath,userhomedir);
    strcpy(musichomedirpath,userhomedir);
    switch (kodiverfound) {
      case 16:  strcat(videohomedirpath,"/.kodi/userdata/Database/MyVideos107.db");
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
//    xbmcSQL=new xbmcsqlite((char *) configmysqlhost,(char *)"~/.kodi/userdata/Database/MyVideos75.db",(char *)"~/.kodi/userdata/Database/MyMusic18.db",(char *)"~/.kodi/userdata/Database/MyVideos75.db");
    if (xbmcSQL) {
        xbmcSQL->xbmcloadversion();									// get version number fropm mxbc db
        printf("XBMC - Load running\n");
        //xbmcclient->SendNOTIFICATION("test", "message", 0);

        // load xbmc movie db
        //xbmcSQL->xbmc_readmoviedb();   // IN use
        xbmcSQL->xbmc_readmusicdb();     // IN use
        printf("XBMC - loader done.\n");
        //create_radio_oversigt();									// Create radio mysql database if not exist
        //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
    } else {
      printf("Error loading kodi db\n");
      exit(1);
    }
    printf("loader thread done loaded kodi \n");
  }
  // set use internal db for music
  global_use_internal_music_loader_system=true;
  // load db
  printf("Numbers of music records loaded %d \n", opdatere_music_oversigt(musicoversigt,0));
  printf("Nusic db loaded.\n");
  pthread_exit(NULL);
}




// load xbmc/kodi movies to db
// create movie db if not exist

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
    allokay=true;
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, dbname, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test fpom musik table exist
    mysql_query(conn,"SHOW TABLES LIKE 'Videometadata'");
    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
      dbexist=true;
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
        printf("No xbmc/kodi db found\nOpen dir error %s \n",userhomedir);
        exit(0);
    }
    // loop dir and update music songs db
    // and find kodi db version
    while((de = readdir(dirp)) && (!(kodiverfound))) {
      if ((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)) {
        ext = strrchr(de->d_name, '.');
        if (ext) strcpy(filename,de->d_name);
        if ((strncmp(filename,"MyMusic",7)==0) && (!(kodiverfound))) {
          if (strcmp(filename,kodiver[0])==0) {
            kodiverfound=16;
            printf("Kodi version 16 is found \n");
          }
          if (strcmp(filename,kodiver[1])==0) {
            kodiverfound=15;
            printf("Kodi version 15 is found \n");
          }
          if (strcmp(filename,kodiver[2])==0) {
            kodiverfound=14;
            printf("Kodi version 14 is found \n");
          }
          if (strcmp(filename,kodiver[3])==0) {
            kodiverfound=13;
            printf("Kodi version 13 is found \n");
          }
          if (strcmp(filename,kodiver[4])==0) {
            kodiverfound=12;
            printf("Kodi version 12 is found \n");
          }
          if (strcmp(filename,kodiver[5])==0) {
            kodiverfound=11;
            printf("Kodi version 11 is found \n");
          }
        }
      }
    }
    // check/get user homedir
    getuserhomedir(userhomedir);
    strcpy(videohomedirpath,userhomedir);
    // add kodi dir ro db files
    switch (kodiverfound) {
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
    printf("loader thread starting - Loading movies from xbmc/kodi.\n");
    xbmcSQL=new xbmcsqlite((char *) configmysqlhost,videohomedirpath,musichomedirpath,videohomedirpath);
    //xbmcSQL=new xbmcsqlite((char *) configmysqlhost,(char *)"~/.kodi/userdata/Database/MyVideos75.db",(char *)"~/.kodi/userdata/Database/MyMusic18.db",(char *)"~/.kodi/userdata/Database/MyVideos75.db");
    if (xbmcSQL) {
      xbmcSQL->xbmcloadversion();									// get version number from mxbc db
      printf("XBMC - Load running\n");
      // load xbmc movie db
      xbmcSQL->xbmc_readmoviedb();                // load movies from kodi db to internal db
      // set use internal db for movies
      global_use_internal_music_loader_system=true;
      //xbmcSQL->xbmc_readmusicdb();     // IN use
      printf("XBMC - loader done.\n");
      // load movies in from db
      xbmcSQL->getxmlfilepath();                   // get path info from xml file
      film_oversigt.opdatere_film_oversigt();     // gen covers 3d hvis de ikke findes.
  //xbmcclient->SendNOTIFICATION("test", "message", 0);

  //pthread_mutex_lock(&count_mutex);
  //pthread_mutex_unlock(&count_mutex);
  //  printf("loader thread starting - Loading movie info from xbmc/kodi.\n");
  // Opdatere tv oversigt fra mythtv db
  //    aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);

                                                                // load record file list
/*
      recordoversigt.opdatere_recorded_oversigt();    	    					// recorded program from mythtv
      // load old recorded list not some recorded any more
      oldrecorded.earlyrecordedload(configmysqlhost,configmysqluser,configmysqlpass);
      // load new tv schecule program
      newtcrecordlist.getrecordprogram(configmysqlhost,configmysqluser,configmysqlpass);		//
*/
      //create_radio_oversigt();										                          // Create radio mysql database if not exist
      //radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations
      // stream
      //streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");       // load all stream from mythtv
    }
  }
  printf("loader thread done loaded %d movie(s) \n",film_oversigt.get_film_antal());
  pthread_exit(NULL);
}





//
// load img file
//

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
        else gl_img=0;
    }
    if (gl_img==0) printf("GFXFILE %s in dir %s NOT FOUND \n",fileload,dir);
    return(gl_img);
}






void loadgfx() {
    unsigned int i;
    char tmpfilename[256];
    char fileload[256];
    char temapath[256];
    char temapath1[256];
    printf ("Loading init graphic.\n");
    strcpy(temapath,"");
    strcpy(temapath1,"");
    if (tema==1) strcpy(temapath,"/usr/share/mythtv-controller/tema1/"); else
    if (tema==2) strcpy(temapath,"/usr/share/mythtv-controller/tema2/"); else
    if (tema==3) strcpy(temapath,"/usr/share/mythtv-controller/tema3/"); else
    if (tema==4) strcpy(temapath,"/usr/share/mythtv-controller/tema4/"); else
    if (tema==5) strcpy(temapath,"/usr/share/mythtv-controller/tema5/"); else
    if (tema==6) strcpy(temapath,"/usr/share/mythtv-controller/tema6/"); else
    if (tema==7) strcpy(temapath,"/usr/share/mythtv-controller/tema7/"); else
    if (tema==8) strcpy(temapath,"/usr/share/mythtv-controller/tema8/"); else
    if (tema==9) strcpy(temapath,"/usr/share/mythtv-controller/tema9/"); else
    if (tema==10) strcpy(temapath,"/usr/share/mythtv-controller/tema10/"); else {
        // default tema
        strcpy(temapath,"tema1/");
        tema=1;
    }

    _textureutvbgmask     = loadgfxfile(temapath,(char *) "images/",(char *) "tv_carbon");
    _textureuv1           = loadgfxfile(temapath1,(char *) "images/",(char *) "uv_map1");
    _textureuv1_top       = loadgfxfile(temapath1,(char *) "images/",(char *) "uv_map2");
    _textureId1           = loadgfxfile(temapath,(char *) "images/",(char *) "dvdcover1");
    _textureId2           = loadgfxfile(temapath,(char *) "images/",(char *) "error");
    _defaultdvdcover      = loadgfxfile(temapath,(char *) "images/",(char *) "dvdcover");
    _defaultdvdcover2	    = loadgfxfile(temapath,(char *) "images/",(char *) "dvdcover1");
    if (screen_size<3)
    _texturemovieinfobox  = loadgfxfile(temapath,(char *) "images/",(char *) "movie-infobox");   		// small screen 4/3
    else
    _texturemovieinfobox  = loadgfxfile(temapath,(char *) "images/",(char *) "movie-infobox3-4");		// big screen  16/9
    _textureId7           = loadgfxfile(temapath,(char *) "images/",(char *) "dir1");
    _textureId7_1         = loadgfxfile(temapath,(char *) "images/",(char *) "dir1_mask");
    _textureId7_2       	= loadgfxfile(temapath,(char *) "images/",(char *) "dir1_mask1");
    _textureId7_4       	= loadgfxfile(temapath,(char *) "images/",(char *) "lillecovermask");
    _texturemusicplayer 	= loadgfxfile(temapath,(char *) "images/",(char *) "musicplayer-info");
    _textureId9_askbox   	= loadgfxfile(temapath,(char *) "images/",(char *) "askbox");
    _textureId9_2        	= loadgfxfile(temapath,(char *) "images/",(char *) "askbox_cd_cover");
    _textureId10         	= loadgfxfile(temapath,(char *) "images/",(char *) "play");
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
    _textureId15         	= loadgfxfile(temapath,(char *) "images/",(char *) "lillecovermask");
    _textureId16         	= loadgfxfile(temapath,(char *) "images/",(char *) "box2");
    _dvdcovermask       	= loadgfxfile(temapath,(char *) "images/",(char *) "dvdcover_mask");
    _textureId20         	= loadgfxfile(temapath,(char *) "images/",(char *) "lillecoverdefault");
    _textureId21         	= loadgfxfile(temapath,(char *) "images/",(char *) "textbox");
    _textureId22         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox1");
    _textureId23         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox2");
    _textureId24         	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbox3");
    _textureId25         	= loadgfxfile(temapath,(char *) "images/",(char *) "recorded_vis_mask");
    _textureId26         	= loadgfxfile(temapath,(char *) "images/",(char *) "volbar");
    _textureId27         	= loadgfxfile(temapath,(char *) "images/",(char *) "volbar_back");
    _textureId28         	= loadgfxfile(temapath,(char *) "images/",(char *) "dirplaylist");
    _textureIdback       	= loadgfxfile(temapath,(char *) "images/",(char *) "back-icon");
    _textureId29_1       	= loadgfxfile(temapath,(char *) "images/",(char *) "back-icon_mask");
    setuptexture         	= loadgfxfile(temapath,(char *) "images/",(char *) "setup");
    setuptvgraberback    	= loadgfxfile(temapath,(char *) "images/",(char *) "setuptvgraberback");
    _textureIdtv         	= loadgfxfile(temapath,(char *) "buttons/",(char *) "tv");
    _textureIdmusic     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "music");
    _textureIdfilm       	= loadgfxfile(temapath,(char *) "buttons/",(char *) "movie");
    _textureIdrecorded  	= loadgfxfile(temapath,(char *) "buttons/",(char *) "recorded");
    _texturemlast       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaylast");
    _texturemlast2      	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaylast");
    _texturemnext       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaynext");
    _texturemplay       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplay");
    _textureIdpup       	= loadgfxfile(temapath,(char *) "buttons/",(char *) "pup");
    _textureIdpdown     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "pdown");
    _texturemstop       	= loadgfxfile(temapath,(char *) "images/",(char *) "mplaystop");
    _textureIdrecorded_aktiv=loadgfxfile(temapath,(char *) "buttons/",(char *) "recorded_selected");
    _textureIdfilm_aktiv  = loadgfxfile(temapath,(char *) "buttons/",(char *) "movie1");
    _textureIdmusicsearch = loadgfxfile(temapath,(char *) "images/",(char *) "music_search");
    _textureIdradiosearch = loadgfxfile(temapath,(char *) "images/",(char *) "radio_search");

    _textureIdmusicsearch1= loadgfxfile(temapath,(char *) "images/",(char *) "music_search1");
    _textureIdloading   	= loadgfxfile(temapath,(char *) "images/",(char *) "loading");			// window
    _textureIdloading1  	= loadgfxfile(temapath,(char *) "images/",(char *) "loading1");			// window
    _textureIdplayinfo  	= loadgfxfile(temapath,(char *) "buttons/",(char *) "playinfo");
    _textureIdclose     	= loadgfxfile(temapath,(char *) "buttons/",(char *) "close");
    _textureIdclose1    	= loadgfxfile(temapath,(char *) "buttons/",(char *) "close1");
    _textureIdclosemask 	= loadgfxfile(temapath,(char *) "buttons/",(char *) "close_mask");
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
    _tvmaskprgrecordedbutton	= loadgfxfile(temapath,(char *) "images/",(char *) "recordedbutton_mask");


    // ****************** backgrounds **************************************
    setupsoundback      	= loadgfxfile(temapath,(char *) "images/",(char *) "setup-soundback");
    setupsqlback        	= loadgfxfile(temapath,(char *) "images/",(char *) "setupsqlback");
    setuptemaback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setuptemaback");
    setupnetworkback    	= loadgfxfile(temapath,(char *) "images/",(char *) "setupnetworkback");
    setupnetworkwlanback  = loadgfxfile(temapath,(char *) "images/",(char *) "setupnetworkwlanback");
    setupscreenback     	= loadgfxfile(temapath,(char *) "images/",(char *) "setupscreenback");
    setupfontback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setupfontback");
    setupkeysback       	= loadgfxfile(temapath,(char *) "images/",(char *) "setupkeysback");


// ************************* screen shot *******************************

    screenshot1=loadTexture ((char *) "images/screenshot1.png");
    screenshot2=loadTexture ((char *) "images/screenshot2.png");
    screenshot3=loadTexture ((char *) "images/screenshot3.png");
    screenshot4=loadTexture ((char *) "images/screenshot4.png");
    screenshot5=loadTexture ((char *) "images/screenshot5.png");
    screenshot6=loadTexture ((char *) "images/screenshot6.png");
    screenshot7=loadTexture ((char *) "images/screenshot7.png");
    screenshot8=loadTexture ((char *) "images/screenshot8.png");
    screenshot9=loadTexture ((char *) "images/screenshot9.png");
    screenshot10=loadTexture ((char *) "images/screenshot10.png");

    _tvbar1=loadgfxfile(temapath,(char *) "images/",(char *) "tvbar1");
    _tvoverskrift=loadgfxfile(temapath,(char *) "images/",(char *) "tvbar_top");
    _tvbar1_1=loadgfxfile(temapath,(char *) "images/",(char *) "tvbar1_1");
    _tvbar3=loadgfxfile(temapath,(char *) "images/",(char *) "tvbar3");

    _tvoldrecorded=loadgfxfile(temapath,(char *) "images/",(char *) "oldrecorded");
    //_tvoldrecordedmask=loadgfxfile(temapath,(char *) "images/",(char *) "oldrecorded_mask");
    _tv_prgtype=loadgfxfile(temapath,(char *) "images/",(char *) "tvprgtype");

    // icons buttons
    // radio buttons
    onlineradio_empty=loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio_empty");
    onlineradio      =loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio");
    onlineradio192   =loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio192");
    onlineradio320   =loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio320");
    onlineradiomask  =loadgfxfile(temapath,(char *) "images/",(char *) "onlineradio_mask");
    radiobutton      =loadgfxfile(temapath,(char *) "images/",(char *) "radio_button");
    musicbutton      =loadgfxfile(temapath,(char *) "images/",(char *) "music_button");
    // radio/music button mask
    radiomusicbuttonmask=loadgfxfile(temapath,(char *) "images/",(char *) "radiomusic_button_mask");
    // radio options (O) key in radio oversigt
    radiooptions=loadgfxfile(temapath,(char *) "images/",(char *) "radiooptions");
    // radio options mask (O) key in radio oversigt
    radiooptionsmask=loadgfxfile(temapath,(char *) "images/",(char *) "radiooptionsmask");
    onlinestreammask=loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream_mask");
    onlinestreammaskicon=loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream_mask");
    // stream stuf
    // stream/movie button mask
    streammoviebuttonmask=loadgfxfile(temapath,(char *) "images/",(char *) "streammovie_button_mask");
    streambutton=loadgfxfile(temapath,(char *) "images/",(char *) "stream_button");
    onlinestreammask=loadgfxfile(temapath,(char *) "images/",(char *) "onlinestream_mask");
    // movie
    moviebutton=loadgfxfile(temapath,(char *) "images/",(char *) "movie_button");
    // main logo
    _mainlogo=loadgfxfile(temapath,(char *) "images/",(char *) "logo");
    // mask for flags
    strcpy(fileload,(char *) "/usr/share/mythtv-controller/images/landemask.jpg");
    gfxlandemask=loadTexture (fileload);
    // screen saver boxes
    screensaverbox=loadgfxfile(temapath,(char *) "images/",(char *) "3d_brix");
    screensaverbox_mask=loadgfxfile(temapath,(char *) "images/",(char *) "3d_brix_mask");
    texturedot=loadgfxfile(temapath,(char *) "images/",(char *) "dot");
    _errorbox=loadgfxfile(temapath,(char *) "images/",(char *) "errorbox");

// *********************************************************************
/*
    i=0;
    while(i<11) {
        strcpy(tmpfilename,temapath);
        strcat(tmpfilename,(char *) "buttons/");
        strcat(tmpfilename,music_zoom_anim_files[i]);
        if (file_exists(tmpfilename)) {
           _textureIdmusic_mask_anim[i]=loadTexture ((char *) tmpfilename);
        } else {
            printf("Load error %s\n",music_zoom_anim_files[i]);
        }
        i++;
    }
*/

    strcpy(tmpfilename,temapath);
    strcat(tmpfilename,(char *) "buttons/music1.png");
    if (file_exists(tmpfilename)) {
        _textureIdmusic_aktiv=loadTexture ((char *) tmpfilename);
    } else _textureIdmusic_aktiv=0;
    printf ("Done loading init graphic.\n");
}


//
// free loaded gfx
//

void freegfx() {
    int i;
    glDeleteTextures( 1, &_textureutvbgmask);
    glDeleteTextures( 1, &_textureId1);				  // backside of roller windows in movie select func
    glDeleteTextures( 1, &_textureId2); 			  // backside of roller windows in movie select func
    glDeleteTextures( 1, &_defaultdvdcover);		// default dvd cover hvis der ikke er nogle at loade
    glDeleteTextures( 1, &_defaultdvdcover2);		// default dvd cover 2 hvis der ikke er nogle at loade
    glDeleteTextures( 1, &_texturemovieinfobox);		  // movie info box
    glDeleteTextures( 1, &_textureId7);				  // cd/dir icon in music oversigt (hvis ingen cd cover findes)
    glDeleteTextures( 1, &_textureId7_1);			  // cd/dir icon in music oversigt (hvis ingen cd cover findes)
    glDeleteTextures( 1, &_textureId7_2);			  // cd/dir icon in music oversigt mask (hvis cd cover findes)
    glDeleteTextures( 1, &_textureId7_4);			  // bruges til billed af cdcover i show music play
    glDeleteTextures( 1, &_texturemusicplayer); // show music info player
    glDeleteTextures( 1, &_textureId9_askbox);				  // ask box
    glDeleteTextures( 1, &_textureId9_2);			  // ask box
    glDeleteTextures( 1, &_textureId10);			  // play icon
    glDeleteTextures( 1, &_textureopen);        // open icon
    glDeleteTextures( 1, &_textureclose);			          // no dont play icon
    glDeleteTextures( 1, &_textureswap);			        // no dont play icon
    glDeleteTextures( 1, &_textureId11);			          // tv program oversigt logo
    glDeleteTextures( 1, &_textureIdback_main);       	// main background
    glDeleteTextures( 1, &_textureIdback_music);                // music background
    glDeleteTextures( 1, &_textureIdback_setup);                // setup background
    glDeleteTextures( 1, &_textureIdback_other);		    // other background
    glDeleteTextures( 1, &_textureId14);	           		//pause knap
    glDeleteTextures( 1, &_textureId15);					   		// bruges ikk
    glDeleteTextures( 1, &_textureId16);		          	// hvis ingen texture (music cover) set default (box2.bmp)
    glDeleteTextures( 1, &_dvdcovermask);	          		// dvd cover mask
    glDeleteTextures( 1, &_textureId20);		          	// bruges af 3d screen saver (lille logo)
    glDeleteTextures( 1, &_textureId21);		  					// bruges ikke
    glDeleteTextures( 1, &_textureId22);		          	// bruges ved recorded programs
    glDeleteTextures( 1, &_textureId23);	           		// bruges ved recorded programs
    glDeleteTextures( 1, &_textureId24);			          // bruges ved recorded programs
    glDeleteTextures( 1, &_textureId25);			          // recorded vis recorded border
    glDeleteTextures( 1, &_textureId26);			          // vol control
    glDeleteTextures( 1, &_textureId27);	           		// vol control
    glDeleteTextures( 1, &_textureId28);		           	// playlist default icon
    glDeleteTextures( 1, &_textureIdback);		        	// bruges ved music
    glDeleteTextures( 1, &_textureId29_1);							// bruges ikke
    glDeleteTextures( 1, &setuptexture);			          // bruges af setup
    glDeleteTextures( 1, &setuptvgraberback);           // bryges af setup tv graber
    glDeleteTextures( 1, &_textureIdtv);							  // bruges ikke
    glDeleteTextures( 1, &_textureIdmusic);			        // music
    glDeleteTextures( 1, &_textureIdfilm);			        // default film icon
    glDeleteTextures( 1, &_textureIdrecorded);			    // default recorded icon
    glDeleteTextures( 1, &_texturemlast);							  // bruges ikke
    glDeleteTextures( 1, &_texturemlast2);			        // bruges
    glDeleteTextures( 1, &_texturemnext);			          // next song
    glDeleteTextures( 1, &_texturemplay);		           	// play song
    glDeleteTextures( 1, &_textureIdpup);		           	//
    glDeleteTextures( 1, &_textureIdpdown);		         	//
    glDeleteTextures( 1, &_texturemstop);			          // stop
    glDeleteTextures( 1, &_textureIdrecorded_aktiv);		// film
    glDeleteTextures( 1, &_textureIdfilm_aktiv);	     	// film
    glDeleteTextures( 1, &_textureIdmusicsearch);	     	// search felt til music
    glDeleteTextures( 1, &_textureIdradiosearch);		    // sang search
    glDeleteTextures( 1, &_textureIdmusicsearch1);	   	// artist search
    glDeleteTextures( 1, &_textureIdloading);
    glDeleteTextures( 1, &_textureIdloading1);
    glDeleteTextures( 1, &_textureIdplayinfo);			// default show musicplay info
    glDeleteTextures( 1, &_textureIdclose);
    glDeleteTextures( 1, &_textureIdclose1);
    glDeleteTextures( 1, &_textureIdclosemask);
    glDeleteTextures( 1, &_texturelock);			// en lille hænge lås bruges i tvguide
    glDeleteTextures( 1, &_texturesetupmenu);			// icons
    glDeleteTextures( 1, &_texturesetupmenu_select);
    glDeleteTextures( 1, &_texturesoundsetup);        // setup
    glDeleteTextures( 1, &_texturesourcesetup);	  	 	// setup
    glDeleteTextures( 1, &_textureimagesetup);		  	// setup
    glDeleteTextures( 1, &_texturetemasetup);		    	// setup
    glDeleteTextures( 1, &_texturemythtvsql);		    	// setup
    glDeleteTextures( 1, &_texturesetupfont);			    // setup
    glDeleteTextures( 1, &_texturesetupclose);		  	//
    glDeleteTextures( 1, &_texturekeyssetup);		     	// setup
    glDeleteTextures( 1, &_texturevideoplayersetup);  // setup
    glDeleteTextures( 1, &_texturetvgrabersetup);     //
    glDeleteTextures( 1, &setupkeysbar1);			// bruges af myth_setup.cpp
    glDeleteTextures( 1, &setupkeysbar2);			// setupkeysbar1
    glDeleteTextures( 1, &tvprginfobig);			// bruges til tv oversigt kanal info
    glDeleteTextures( 1, &_tvoverskrift);     // tv oversigt top window
    glDeleteTextures( 1, &_tvprgrecorded);			// tv
    glDeleteTextures( 1, &_tvprgrecordedr);						// bruges ikke mere
    glDeleteTextures( 1, &_tvrecordbutton);			// tv
    glDeleteTextures( 1, &_tvrecordcancelbutton);
    glDeleteTextures( 1, &_tvoldprgrecordedbutton);
    glDeleteTextures( 1, &_tvnewprgrecordedbutton);
    glDeleteTextures( 1, &_tvmaskprgrecordedbutton);
    glDeleteTextures( 1, &setupsoundback);
    glDeleteTextures( 1, &setupsqlback);
    glDeleteTextures( 1, &setuptemaback);
    glDeleteTextures( 1, &setupnetworkback);
    glDeleteTextures( 1, &setupnetworkwlanback);
    glDeleteTextures( 1, &setupscreenback);
    glDeleteTextures( 1, &setupfontback);
    glDeleteTextures( 1, &setupkeysback);
    glDeleteTextures( 1, &screenshot1);		                   		// screen shots
    glDeleteTextures( 1, &screenshot2);			                  	// screen shots
    glDeleteTextures( 1, &screenshot3);			                  	// screen shots
    glDeleteTextures( 1, &screenshot4);				                  // screen shots
    glDeleteTextures( 1, &screenshot5);				                  // screen shots
    glDeleteTextures( 1, &screenshot6);                         // screen shots
    glDeleteTextures( 1, &screenshot7);                         // screen shots
    glDeleteTextures( 1, &screenshot8);                         // screen shots
    glDeleteTextures( 1, &screenshot9);                         // screen shots
    glDeleteTextures( 1, &screenshot10);                        // screen shots

    glDeleteTextures( 1, &_tvbar1);
    glDeleteTextures( 1, &_tvbar3);
    glDeleteTextures( 1, &_tvoldrecorded);
    glDeleteTextures( 1, &_tv_prgtype);
    glDeleteTextures( 1, &onlineradio_empty);
    glDeleteTextures( 1, &onlineradio);		                   		// radio icon
    glDeleteTextures( 1, &onlineradio192);			                // radio icon
    glDeleteTextures( 1, &onlineradio320);			                // radio icon
    glDeleteTextures( 1, &onlineradiomask);			                // radio icon mask
    glDeleteTextures( 1, &radiobutton);
    glDeleteTextures( 1, &musicbutton);
    glDeleteTextures( 1, &radiooptions);
    glDeleteTextures( 1, &radiooptionsmask);
    glDeleteTextures( 1, &_mainlogo);								             // Main logo not in use any more
    glDeleteTextures( 1, &gfxlandemask);			                   // lande mask
    glDeleteTextures( 1, &texturedot);
    glDeleteTextures( 1, &_textureuv1);                         // uv img
    glDeleteTextures( 1, &_textureuv1_top);                         // uv img
    glDeleteTextures( 1, &_errorbox);                           // error box

    // delete radio lande flags
    i=0;
    while(i<69) {
        if (gfxlande[i]) glDeleteTextures( 1, &gfxlande[i]);
        i++;
    }
    /*
    i=0;
    while(i<11) {
        if (_textureIdmusic_mask_anim[i]) glDeleteTextures( 1,&_textureIdmusic_mask_anim[i]);
        i++;
    }
    */
    glDeleteTextures( 1,&_textureIdmusic_aktiv);
    glDeleteTextures( 1,&screensaverbox);
    glDeleteTextures( 1,&screensaverbox_mask);


    //_textureIdmusic_anim[i].textureId;
    //film_oversigt.filmoversigt[i]
    //musicoversigt[i].				// clear selv

}


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
    char *lande[]={(char *) "",(char *) "",(char *) "",(char *) "yu",(char *) "fr.pmg",(char *) "luxembourg.jpg",(char *) "nl.jpg",(char *) "usa.jpg",(char *) "de.jpg",(char *) "uk.jpg", //
                   (char *) "ru.jpg",(char *) "israel.jpg",(char *) "Austria.jpg",(char *) "lebanon.jpg",(char *) "latvia.jpg",(char *) "",(char *) "Vietnam.jpg",(char *) "Saudi-Arabia.jpg",(char *) "as.jpg",(char *) "brazil.jpg", //
                   (char *) "Egypt.jpg",(char *) "no.jpg",(char *) "pl.jpg",(char *) "se.jpg",(char *) "sw.jpg",(char *) "mexico.jpg",(char *) "be.jpg",(char *) "ca.jpg",(char *) "as.jpg",(char *) "ru.jpg", //
                   (char *) "sp.jpg",(char *) "ae.jpg",(char *) "hu.jpg",(char *) "th.jpg",(char *) "gr.jpg",(char *) "bk.jpg",(char *) "nu.jpg",(char *) "in.jpg",(char *) "po.jpg",(char *) "ir.jpg", //
                   (char *) "ks.jpg",(char *) "romania.jpg",(char *) "it.jpg",(char *) "fi.jpg",(char *) "bahrain.jpg",(char *) "dk.jpg",(char *) "chile.jpg",(char *) "chile.jpg",(char *) "Slovakia.jpg",(char *) "Ukraine.jpg", //
                   (char *) "hu.jpg",(char *) "co.jpg",(char *) "do.jpg",(char *) "Azerbaijan.jpg",(char *) "Lithuania.jpg",(char *) "Andorra.jpg",(char *) "Estonia.jpg",(char *) "Tajikistan.jpg",(char *) "Turkey.jpg",(char *) "Mongolia.jpg", //
                   (char *) "Belarus.jpg",(char *) "Slovenia.jpg",(char *) "Cyprus.jpg",(char *) "China.jpg",(char *) "Cambodia.jpg",(char *) "Indonesia.jpg",(char *) "Singapore.jpg",(char *) "Croatia.jpg",(char *) "Czech Republic.jpg",(char *) ""};
    i=0;
    strcpy(path2,"");
    while(i<69) {
        printf("load flag %d %s \n",i,lande[i]);
        strcpy(tmpfilename,"/usr/share/mythtv-controller/images/");
        strcat(tmpfilename,lande[i]);							// add lande kode id fra table lande.
        strcpy(path,"/usr/share/mythtv-controller/images/");
        if (strcmp(lande[i],"")!=0) {
          gfxlande[i]=loadgfxfile(path2,path,(char *) lande[i]);
          //gfxlande[i]=loadTexture ((char *) tmpfilename);
        }
        i++;
    }
}



void InitGL()              // We call this right after our OpenGL window is created.
{
    unsigned int i;
    char tmpfilename[256];
    char fileload[256];
    char temapath[256];

    strcpy(temapath,"");
    if (tema==1) strcpy(temapath,"/usr/share/mythtv-controller/tema1/"); else
    if (tema==2) strcpy(temapath,"/usr/share/mythtv-controller/tema2/"); else
    if (tema==3) strcpy(temapath,"/usr/share/mythtv-controller/tema3/"); else
    if (tema==4) strcpy(temapath,"/usr/share/mythtv-controller/tema4/"); else
    if (tema==5) strcpy(temapath,"/usr/share/mythtv-controller/tema5/"); else
    if (tema==6) strcpy(temapath,"/usr/share/mythtv-controller/tema6/"); else
    if (tema==7) strcpy(temapath,"/usr/share/mythtv-controller/tema7/"); else
    if (tema==8) strcpy(temapath,"/usr/share/mythtv-controller/tema8/"); else
    if (tema==9) strcpy(temapath,"/usr/share/mythtv-controller/tema9/"); else
    if (tema==10) strcpy(temapath,"/usr/share/mythtv-controller/tema10/"); else
    if (tema==11) strcpy(temapath,"/usr/share/mythtv-controller/tema11/"); else
    if (tema==12) strcpy(temapath,"/usr/share/mythtv-controller/tema12/"); else
    if (tema==13) strcpy(temapath,"/usr/share/mythtv-controller/tema13/"); else
    if (tema==14) strcpy(temapath,"/usr/share/mythtv-controller/tema14/"); else
    if (tema==15) strcpy(temapath,"/usr/share/mythtv-controller/tema15/"); else
    if (tema==16) strcpy(temapath,"/usr/share/mythtv-controller/tema16/"); else
    if (tema==17) strcpy(temapath,"/usr/share/mythtv-controller/tema17/"); else
    if (tema==18) strcpy(temapath,"/usr/share/mythtv-controller/tema18/"); else
    if (tema==19) strcpy(temapath,"/usr/share/mythtv-controller/tema19/"); else
    if (tema==20) strcpy(temapath,"/usr/share/mythtv-controller/tema20/"); else {
        strcpy(temapath,"tema1/");
        tema=1;
    }
    //load_lande_flags();
// Load radio stations gfx **********************************************
// virker vis aktiveres
// bliver loaded første gang de bruges

//    radiooversigt.load_radio_stations_gfx();
// *********************************************************************
//    mybox.settexture(musicoversigt);
}





// bruges til at checke_copy radio icons som virker til nyt dir


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
}




int main(int argc, char** argv) {

    printf("Build date  : %u\n", (unsigned long) &__BUILD_DATE);
    printf("Build number: %u\n", (unsigned long) &__BUILD_NUMBER);

    if (argc>1) {
      //if (strcmp(argv[1],"-f")==0) full_screen=1;
      if (strcmp(argv[1],"-h")==0) {
          printf("\n\nMythtv-controller Version ");
//          printf("%s",SHOWVER);
          printf("\n\n");
          printf("-f For full screen mode\n");
          printf("-b For border less screen mode\n");
          printf("-p For program guide screen mode\n");
          printf("-h This help screen\n");
          exit(0);
      }
    }

    numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    printf("Numbers of cores :%d\n",numCPU);

    // Load config
    load_config((char *) "/etc/mythtv-controller.conf");				// load setup config
    if ((strncmp(configbackend,"mythtv",5)==0) || (strncmp(configbackend,"any",3)==0)) configmythtvver=hentmythtvver(); 		// get mythtv-backend version
    if (strncmp(configbackend,"mythtv",5)==0) {
        printf("mythtv - Backend\n");
        printf("Mythtv database version %d\n",configmythtvver);
        printf("configmysqluser   =%s \n",configmysqluser);
        //printf("configmysqlpass   =%s \n",configmysqlpass);
        printf("configmysqlhost   =%s \n",configmysqlhost);
        printf("config movie path =%s \n",configmoviepath);
        printf("config music path =%s \n",configmusicpath);
        printf("config record path=%s \n",configrecordpath);
        printf("config hostname   =%s \n",confighostname);
        printf("config fontname   =%s \n",configfontname);
        printf("Sound interface   =%s \n",configsoundoutport);
        printf("Default player    =%s \n",configdefaultplayer);
    }
    if (strncmp(configbackend,"xbmc",4)==0) {
        printf("XBMC - Backend\n");
        printf("sqluser           =%s \n",configmysqluser);
        //printf("sqlpass           =%s \n",configmysqlpass);
        printf("host              =%s \n",configmysqlhost);
        printf("config movie path =%s \n",configmoviepath);
        printf("config music path =%s \n",configmusicpath);
        printf("config record path=%s \n",configrecordpath);
        printf("config hostname   =%s \n",confighostname);
        printf("config fontname   =%s \n",configfontname);
        printf("Sound interface   =%s \n",configsoundoutport);
        printf("Default player    =%s \n",configdefaultplayer);
    }
    if (debugmode) {
      fprintf(stderr,"Debug mode selected %d",debugmode);
    }
    if ((full_screen) && (debugmode)) fprintf(stderr,"Enter full screen mode.\n");

    create_radio_oversigt();										                          // Create radio mysql database if not exist
    radiooversigt_antal=radiooversigt.opdatere_radio_oversigt(0);					// get numbers of radio stations

    strcpy(configbackend_tvgraber_old,"");

    if (strncmp(configbackend,"xbmc",4)==0) {
      // music loader
      pthread_t loaderthread;           // the load
      int rc=pthread_create(&loaderthread,NULL,xbmcdatainfoloader,NULL);
      if (rc) {
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
      }

      // movie loader
      pthread_t loaderthread1;           // the load
      int rc1=pthread_create(&loaderthread1,NULL,xbmcdatainfoloader_movie,NULL);
      if (rc1) {
        printf("ERROR; return code from pthread_create() is %d\n", rc1);
        exit(-1);
      }
    }

    if (strncmp(configbackend,"mythtv",5)==0) {
      // music loader
      if (configmythtvver>=0) {
        pthread_t loaderthread;
        int rc=pthread_create(&loaderthread,NULL,datainfoloader_music,NULL);
        if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
        }
      }
      // movie loader
      if (configmythtvver>0) {
        pthread_t loaderthread1;           // the load
        int rc1=pthread_create(&loaderthread1,NULL,datainfoloader_movie,NULL);
        if (rc1) {
          printf("ERROR; return code from pthread_create() is %d\n", rc1);
          exit(-1);
        }
      }
      // stram loader
      if (configmythtvver>0) {
        pthread_t loaderthread2;           // the load
        int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_stream,NULL);
        if (rc2) {
          printf("ERROR; return code from pthread_create() is %d\n", rc2);
      	  exit(-1);
        }
      }
    }



/*
    // xmltv loader
    if (true) {
      pthread_t loaderthread2;           // load tvguide xml file in to db
      int rc2=pthread_create(&loaderthread2,NULL,datainfoloader_xmltv,NULL);
      if (rc2) {
        printf("ERROR; return code from pthread_create() is %d\n", rc2);
        exit(-1);
      }
    }
*/


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

    initlirc();

    // bruges til at checke_copy radio icons som virker til nyt dir
    //check_radio_stations_icons();

    glutInit(&argc, argv);
    init_sound_system(soundsystem);                             // Init sound
    ilInit();                                                   // Initialization of DevIL
    // rember screeen size
    orgwinsizex=glutGet(GLUT_SCREEN_WIDTH);
    orgwinsizey=glutGet(GLUT_SCREEN_HEIGHT);

    if (orgwinsizex==1366) screen_size=4;
    if (orgwinsizex==1920) screen_size=3;

    if (orgwinsizex>1920) orgwinsizex=1920;
    if (orgwinsizey>1080) orgwinsizey=1080;

    printf("Screen size %dx%d\n",orgwinsizex,orgwinsizey);
    printf("Screen mode %d\n",screen_size);

    Display *dpy=NULL;
    Window rootxwindow;
    // get first monitor screen size (pixel)

    dpy = XOpenDisplay(":0");
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
    glutCreateWindow ("mythtv-controller");
    init();
    loadgfx();
    if (full_screen) glutFullScreen();                  // set full screen mode
    glutDisplayFunc(display);
    glutIdleFunc(NULL);                            // idle func
    glutKeyboardFunc(handleKeypress);                 // setup normal key handler
    glutSpecialFunc(handlespeckeypress);              // setup spacial key handler
    glutMouseFunc(handleMouse);                       // setup mousehandler
    glutTimerFunc(25, update, 0);                     // set start loop
    // init fonts
    init_ttf_fonts();
    // select start func if argc is this
    if ((argc>1) && (strcmp(argv[1],"-p")==0)) vis_tv_oversigt=true;
    if ((argc>1) && (strcmp(argv[1],"-r")==0)) vis_radio_oversigt=true;
    if ((argc>1) && (strcmp(argv[1],"-m")==0)) vis_music_oversigt=true;
    if ((argc>1) && (strcmp(argv[1],"-f")==0)) vis_film_oversigt=true;
    if ((argc>1) && (strcmp(argv[1],"-s")==0)) vis_stream_oversigt=true;

    //aktivfont.updatefontlist();
    //aktivfont.selectfont((char *) "Tlwg Mono");

    // select font from configfile (/etc/mythtv-controller.conf)
    aktivfont.selectfont(configfontname);

    printf("\nHardware           %s\n",(char *)glGetString(GL_RENDERER));                         // Display Renderer
    printf("OpenGL Render      %s\n",(char *)glGetString(GL_VENDOR));                           // Display Vendor Name
    printf("Version            %s\n",(char *)glGetString(GL_VERSION));
    // start main loop now
    glutMainLoop();

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
    return(EXIT_SUCCESS);
}
