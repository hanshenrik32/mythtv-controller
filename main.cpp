#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glc.h>                     // glc true type font system

#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

#include <dirent.h>                     // dir functions
#include <netinet/in.h>                 // hostname
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <lirc/lirc_client.h>           // lirc client lib
#include <mysql.h>                      // mysql stuf
#include <sys/socket.h>
#include <unistd.h>                     // get hostname
#include <vlc/vlc.h>
#include <fstream>
#include <fmt/format.h>
#include <jsoncpp/json/json.h>          // json parser
// torrent lib
#include <libtorrent/session.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <iostream>
#include <regex>
#include <vlc/vlc.h>
// time
#include <ctime>
#include <sys/timeb.h>

#include "main.h"
#include "renderer.h"
#include "readjpg.h"
#include "myctrl_stream.h"
#include "myth_config.h"
#include "utility.h"
#include "myctrl_readwebfile.h"
#include "myth_ttffont.h"
#include "myth_saver.h"
#include "myctrl_radio.h"
#include "myth_setup.h"
#include "myctrl_tidal2.h"
#include "myctrl_spotify.h"
#include "myctrl_music.h"
#include "myctrl_movie.h"
#include "myctrl_tvprg.h"
#include "myctrl_recorded.h"
#include "myctrl_mplaylist.h"
#include "checknet.h"
#include "myth_vlcplayer.h"
#include "myctrl_torrent.h"
#include "myth_setupsql.h"

// type used
using namespace std;

char systemcommand[8192];
bool vis_error = false;
int vis_error_timeout=0;

int configdefaultplayer_screenmode;

bool global_use_internal_music_loader_system = true;

int music_oversigt_loaded_nr;
int movie_oversigt_loaded_nr=0;                                                  //
int radio_oversigt_loaded_nr=0;

static int show_newmovietimeout=120*10;

int findtype=0;

unsigned int musicoversigt_antal=0;                       // antal aktive sange
char music_db_update_loader[256];                         //
int music_select_iconnr;                                  // selected icon

// json

Json::Value iRoot;
Json::Reader reader;

#define USE_FMOD_MIXER 1              // default fmod
#if defined USE_SDL_MIXER
#include <SDL_mixer.h>
#endif

// sound system include fmod
#if defined USE_FMOD_MIXER
#include "/opt/mythtv-controller/fmodstudioapi20311linux/api/core/inc/fmod.hpp"
#include "/opt/mythtv-controller/fmodstudioapi20311linux/api/core/inc/fmod_errors.h"
#endif

#ifdef USE_FMOD_MIXER
FMOD::DSP* dsp = 0;                   // fmod Sound device
#endif

// fmod stuf
#if defined USE_FMOD_MIXER
FMOD_OPENSTATE openstate;
int fmodbuffersize=16*1024;
// *************************************************************************************************
// setup fmod sound device and program vars
FMOD::System    *sndsystem;
FMOD::Sound     *sound=0;
FMOD::Channel   *channel=0;
FMOD_RESULT     result;
unsigned int    fmodversion;
bool            playing = 0;
int             snd=0;
#endif

int sampleSize = 64;

#if defined USE_SDL_MIXER
Mix_Music *sdlmusicplayer=0;
int audio_buffers=1024;
int audio_rate=44100;
Uint16 audio_format=MIX_DEFAULT_FORMAT;
int audio_channels;
int sdlmusic;
#endif


struct lirc_config *lircconfig;
int sock=0;                                     // lirc socket

// if defined the support will be enabled
#define ENABLE_TIDAL
#define ENABLE_SPOTIFY
//

// tidal music class
tidal_class tidal_oversigt;
static bool do_update_tidal_playlist = false;           // do it first time thread

// film oversigt type class
film_oversigt_typem film_oversigt(FILM_OVERSIGT_TYPE_SIZE+1);

Font myfont;
Font myfont2;
Font myfont_tv_guide_overskrift;
Font myfont_mini;
Font myfont_torrent_overskrift;
Font myfont_torrent_list;
Font myfont_search_bar;

fontctrl aktivfont;                                                             // font control (default aktiv font all over (if opencl))

Renderer renderer;
// fontctrl aktivfont;                                                             // font control (default aktiv font all over (if opencl))
wifinetdef wifinets;                            // wifi net class

extern configkeytype configkeyslayout[];	           		// functions keys startfunc

// stream
stream_class streamoversigt;                              // stream oversigt
radiostation_class radiooversigt;
musicoversigt_class musicoversigt;              // Music class
tv_graber_config aktiv_tv_graber;
// tv channel list
channel_list_type channel_list;

torrent_loader torrent_downloader;

extern mplaylist aktiv_playlist;                                                // music play list

recorded_overigt_class recorded_oversigt;

spotify_class spotify_oversigt;
static bool do_update_spotify_playlist = false;           // do it first time thread


extern rss_stream_class rssstreamoversigt;

musicmeter_class saver_musicmeter;                                    // screen saver class

// tv_oversigt aktiv_tv_oversigt;

int valgtrecordnr=0;                                    // valgte recorded program
int subvalgtrecordnr=0;                                 // valgte sub recorded program som skal vises
int tvvalgtrecordnr=0;                                  // valgte tv recorded program                                   (SIMPLE)
int tvsubvalgtrecordnr=0;                               // valgte tv sub recorded program som skal vises                (SIMPLE)
int tvvisvalgtnrtype=1;
int tvvisvalgttype=1;



char avalible_device[10][256];			                    // sound system devises list
int numbersofsoundsystems=0;                              // antal devices
char configdeviceid[256];                               // music device name
char configmythsoundsystem[256];	                   		// selected soundsystem output device 0 = default

bool saver_irq = false;

Font arial;
GLint ctx, myFont;
GLuint normal_icon=0;

// movie
GLuint _defaultdvdcover;
int film_select_iconnr=1;

// tv stuf
tv_oversigt aktiv_tv_oversigt;

GLuint _textureId_cursor;

GLuint screensaverbox;
GLuint pcplayer_icon;
GLuint mobileplayer_icon;
GLuint newstuf_icon;
GLuint unknownplayer_icon;
GLuint spotify_icon_border;                 // spotify border icon for spotify icon

GLuint playing_tidal_icon_texture;



GLuint textureId;
GLuint _texturetidalloading;              // show tidal loading image  

GLuint radiobutton;                       //
GLuint tidalbutton;                       //
GLuint tidalbutton1;                      //
GLuint spotifybutton;                     //
GLuint spotifybutton1;                    //
GLuint moviebutton;                       //
GLuint musicbutton;
GLuint streambutton;
GLuint spotify_search;
GLuint spotify_search_back;
GLuint _textureIdloading;

GLuint _textureId_song; 	                // folder image
GLuint _textureIdpup;
GLuint _textureIdpdown;
GLuint _textureIdrecorded_aktiv;
GLuint _textureIdrecorded;
GLuint _textureclosemain;
GLuint _texturesetupmenu;
GLuint _texturesetupmenu_select;
GLuint _textureexit;
GLuint _textureIdplayinfo;
GLuint _textureIdtv;
GLuint _textureIdfilm_aktiv;
GLuint moviebutton_2;
GLuint streambutton_2;
GLuint radiobutton1;
GLuint _textureIdmusic;
GLuint _textureIdmusic_aktiv;
GLuint _textureIdback; 	                  //
GLuint _textureId_dir; 	                  // folder image
GLuint onlineradio320;                    //
GLuint onlineradio_empty;
GLuint tidal_big_search_bar_artist;       // big search bar used by tidal search
GLuint tidal_big_search_bar_track;         // big search bar used by tidal search
GLuint tidal_big_search_bar_album;
GLuint playing_tidal_icon_textur;                                   // playing tidal icon texture
GLuint _texturemusicplayer; 	            // music image		// show player
GLuint _texturespotifyplayer; 	          // spotify image		// show player
GLuint _texturetidalplayer; 	            // tidal image		// show player
GLuint _textureradioplayer; 	            // radio image		// show player
GLuint _texturemplay;
GLuint _texturemstop;
GLuint _texturemlast;
GLuint _texturemlast2;
GLuint _texturemnext;
GLuint _texturempause;

GLuint _texturelock;
GLuint setuptexture;

GLuint _textureId28; 	                    // dir playlist_icon
GLuint music_big_search_bar_artist;       // big search bar used by tidal search
GLuint music_big_search_bar_track;         // big search bar used by tidal search
GLuint music_big_search_bar_album;

GLuint big_search_bar;
GLuint big_search_bar_playlist;
GLuint big_search_bar_albumm;
GLuint big_search_bar_artist;
GLuint big_search_bar_track;
GLuint volume_window;
// setup icons
GLuint _texturesoundsetup;
GLuint _texturesourcesetup;
GLuint _textureimagesetup;
GLuint _texturetemasetup;
GLuint _texturemythtvsql;
GLuint _texturesetupfont;
GLuint _texturekeyssetup;
GLuint _texturevideoplayersetup;
GLuint _texturetvgrabersetup;
GLuint _texturekeysrss;
GLuint _texturespotify;
GLuint _texturetidal;
GLuint _texturesetupclose;
GLuint screenshot1;
GLuint screenshot2;
GLuint screenshot3;
GLuint screenshot4;
GLuint screenshot5;
GLuint screenshot6;
GLuint screenshot7;
GLuint screenshot8;
GLuint screenshot9;
GLuint screenshot10;

GLuint texturedot;
GLuint gfxlande[80];                      // gfx lande array
GLuint _textureIdback_music;
GLuint _textureIdback_setup;
GLuint _textureIdback_main;
GLuint _textureIdback_other;

GLuint _texturemovieinfobox;	            //  movie image

GLuint _textureupdatetidalview; 	        // update icon tidal playlist in editor
GLuint _defaultdvdcover_mask;

GLuint _textureloadfile;
GLuint spotify_askplay;
GLuint spotify_askopen;

GLuint _textureId22;
GLuint _textureId23;
GLuint _textureId24;

GLuint analog_clock_background;

GLuint _textureId9_askbox;

GLuint _textureIdplayicon; 	              // play icon
GLuint _textureopen; 	                    // open icon
GLuint _textureclose; 	                  // close icon
GLuint _textureselect;
GLuint _texturetidalwait;

GLuint _textureswap;

GLuint _tvbar3;

GLuint _textureutvbgmask;
GLuint tvguide_last_hour_icon;
GLuint tvguide_next_hour_icon;

GLuint _tvoverskrift;             // overskrift window
GLuint _tvbar1_1;
GLuint _tvrecordbutton;
int tema=2;       					                            // aktiv tema (default is 3)
int screen_size=3;		                            			// default screen size
bool vis_uv_meter = false;

int aktiv_radio_station=0;                //
int tvchannel_startofset=0;                                 // ofset used in tv_graber config (line offset)
char aktivsongname[80];                                   	// song name
char aktivartistname[80];                      		// navn på aktiv artist (som spilles)

int configmythtvver=0;            			                  // mythtv config found version
int configxbmcver=1;

const int screenx=1920;                                         // default screen size
const int screeny=1080;                                         // default screen size


// Screen saver setup ******************************************************************************
int visur=0;
const int DIGITAL=1;
const int ANALOG=2;
const int SAVER3D=3;
const int SAVER3D2=4;
const int PICTURE3D=5;
const int MUSICMETER=6;
const int MUSICMETER2=7;
const int MUSICMETER3=10;
const int UV_METER=8;
const int PLASMA=9;

bool multi_editor = false;

bool do_shift_song = false;				                        // jump to next song to play
bool do_stop_music = false;                               // stop all play
bool do_stop_radio = false;                               // stop all play
bool do_stop_tidal = false;                               // stop all play
bool do_show_setup = false;
bool ask_tv_record = false;
bool show_status_update = false;
bool stream_jump = false;

bool show_stream_options = false;
bool reset_recorded_texture = false;

// show fps
bool showfps = false;
bool show_status = false;
static GLint T0     = 0;
static GLint Frames = 0;
char temptxt[2000];

static int tidal_start_delay=0;

char playlistfilename_cover_path[512];                  //
char playlistfileid[200];                               // playlistid
char playlistfileartistname[200];
int playlist_nr_of_songs=0;


bool show_uv=true;

bool hent_spotify_search=false;
bool hent_tidal_search=false;

bool loading_tv_guide=false;
bool tv_guide_firsttime_update=false;

bool do_play_radio = false;
bool startmovie = false;
bool stopmovie = false;

int film_key_selected=1;                                  // den valgte med keyboard i film oversigt
int music_key_selected=1;                                 // default music selected
int spotify_key_selected=1;                                 // default music selected
int tidal_key_selected=1;                                 // default music selected
int radio_key_selected=1;                                 // default

int do_zoom_film_aktiv_nr=0;

int do_play_tidal=0;

int orgwinsizex,orgwinsizey;

GLuint setupkeysbar1;                       // bruges af setup
GLuint setupkeysbar2;                       // bruges af setup


// aktiv stream play
int stream_playnr=0;                                      //
char stream_playing_name[80];                             //
char stream_playing_desc[80];                             //
GLuint stream_playing_icon=0;                             //


bool search_movie_string_changed=false;
bool do_show_movie_search_oversigt=false;                 // show music search oversigt

bool search_spotify_string_changed = false;
bool vis_nyefilm_oversigt = true;                         // start med at vise nye film
bool ask_open_dir_or_play_music = false;
bool vis_radio_or_music_oversigt = false;
bool vis_stream_or_movie_oversigt = false;
bool vis_tv_oversigt = false;
bool do_show_tv_kanal_info = false;
bool vis_music_oversigt = false;
bool vis_radio_oversigt = false;
bool vis_film_oversigt = false;
bool vis_spotify_oversigt = false;
bool vis_tidal_oversigt = false;
bool vis_recorded_oversigt = false;
bool firsttimespotifyupdate = false;

bool tidalok;

bool ask_open_dir_or_play_aopen = false;                  // auto open dir
bool ask_open_dir_or_play_spotify = false;                //
bool ask_open_dir_or_play_tidal = false;                //
bool do_swing_music_cover = true;                         // default swing music cover

bool ask_save_playlist = false;                                 // ask for name

bool do_show_spotify_search_oversigt=false;
bool do_show_tidal_search_oversigt=false;
bool do_show_music_search_oversigt=false;                 // show music search oversigt

bool music_oversigt_loaded_begin = false;                                      // true then music update is started
bool global_use_spotify_local_player;
bool spotify_oversigt_loaded_begin = false;                                      // true then spotify update is started

bool stream_loadergfx_started_break = false;

bool do_update_music_icons_now_in_thread=true;                    // do update music icons now in thread

bool save_ask_save_playlist = false;                            // do the save after ask

bool hent_music_search = false;
int stream_key_selected=0;
int stream_select_iconnr=0;
int antal_songs=0;

static int antal_i_tidal_playlist=0;

int visvalgtnrtype=1;
int radio_select_iconnr=1;

int do_music_icon_anim_icon_ofset=0;
int music_icon_anim_icon_ofset=0;

int _mangley;                                             //

int soundsystem=0;		      		                      	  // used sound system 1=FMOD 2=OPENSOUND

bool vis_tvrec_list = false;                              // show tv program need record

bool show_radio_options = false;                          // show radio options (land icon) in oversigt

int music_selected_startofset=0;                          // used by viewer

unsigned int do_show_play_open_select_line=0;             // bruges til at vælge hvilken music filer som skal spilles
unsigned int do_show_play_open_select_line_ofset=0;       // bruges til at vælge hvilken music filer som skal spilles (start ofset)

bool do_play_music_aktiv_play = false;                    //
int do_play_music_aktiv_nr=0;                             // den aktiv dirid som er trykket på
int do_play_music_aktiv_nr_select_array=0;

unsigned int setupfontselectofset=0;                                            // valgte font i oversigt
unsigned int setupwlanselectofset=0;                                            // valgte wlan nr i oversigt

// radio
unsigned int lenbytes, kbps;
unsigned int playtime,playtime_min,playtime_sec,playtime_hour;
unsigned int playtime_songlength,playtime_length_min,playtime_length_sec;
unsigned int radio_playtime,radio_playtime_min,radio_playtime_sec,radio_playtime_hour;
unsigned int radio_playtime_songlength,radio_playtime_length_min,radio_playtime_length_sec;
int radiooversigt_antal;

int numCPU;                                             // have the numbers of cpu cores
int urtype=MUSICMETER;

int PRGLIST_ANTAL=0;                                      // used in tvguide xml program selector
char aktivplay_music_path[1024];

char debuglogdata[4096];                                  // used by log system
char localuserhomedir[4096];
char movie_search_name[80];                             // name to use thewn search for movies
char playlistfilename[512];                             // name to use thewn save playlist
FILE *logfile=NULL;                   // global logfile

char confighostip[256];				                          // this host ip adress
char confighostwlanname[256];				                    // wlan netid name
char confighostname[256];                               // this hostname
char configdefaultplayer[256];				                  // default player
char configfontname[200];				                        // default ttf font name to load and use
char configuse3deffect[20];			                      	// use 3d effects
char configdefaultmusicpath[256];                       // internal db for music
char configdefaultmoviepath[256];                       // internal db for movie
char configbackend_tvgraber[256];                       // internal tv graber to use
char configbackend_tvgraber_old[256];                   // rember last select tvgraber (used in selector in keyboard controler)
char configbackend_tvgraberland[2048];                  // internal tv graber to use
// ************************************************************************************************
char configmysqluser[256];                              // /mythtv/mysql access info
char configmysqlpass[256];                              //
char configmysqlhost[256];                              //
// ************************************************************************************************
char configxbmcuser[256];                               // /mythtv/mysql access info
char configxbmcpass[256];                               //
char configxbmchost[256];                               //
// ************************************************************************************************
char configpicturepath[256];                            // path til pictures gallery source (den laver et dir under som hedder "mythc_gallery"
char configmusicpath[256];                              // path til music source
char configmusicmypath[256];                            // mythtv-controller internal path
char configmoviepath[256];                              // db path til movie source in db
char configrecordpath[256];                             //
char configmysqlip[256];				                        // mysql server ip
char configmythhost[256];			                        	// host of mythtv master
char configbackend[20];			                        		// backend system xbmc/mythtv
char configaktivescreensavername[256];			            // screen saver name
char configclosemythtvfrontend[256];		              	// close mythtvfront end on startup
char configscreensavertimeout[256];			                // default screensaver timeout
char configsoundoutport[256];			                    	// sound output port (hdmi/spdif/analog)
char configdvale[256];			                         		// kan vi gå i dvale (mythtv kontrol) gen lock file
char configvideoplayer[200];                            // default video player
char configbackend_starred_playlistname[1024];          // load spotify play on startup .
bool configbackend_openspotify_player=true;             // load spotify play on startup.
char configmouse[256];			                         		// mouse enable
unsigned long configtvguidelastupdate=0;                         // last date /unix time_t type) tvguide update
float configsoundvolume=1.0f;                           // default sound volume
int configland=0;
const int configlandantal=5;
char *configlandsprog[]={(char *) "English",(char *) "Dansk",(char *) "France",(char *) "Tysk",(char *) "Arabic"};
// ************************************************************************************************


GLuint setupupdatebutton;                 //
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
GLuint torrent_background;
GLuint setuptorrent_background;
GLuint setuptidalback;
GLuint setupbackend;
GLuint textureId_uv2;


long configrssguidelastupdate=0;                        // last date /unix time_t type) tvguide update
float configdefaulttvguidefontsize=12;                  // default font size tv tvguide
float configdefaultradiofontsize=12;                    // default font size radio
float configdefaultmusicfontsize=12;                    // default font size music
float configdefaultstreamfontsize=12;                   // default font size stream
float configdefaultmoviefontsize=12;                    // default font size movie
float configdefaulttidalfontsize=12;                    // default font size tidal
float configdefaultspotifyfontsize=12;                  // default font size spotify

bool do_show_setup_backend = false;                       //
bool do_show_setup_torrent = false;
bool do_show_setup_network = false;                       // Show network setup view
bool do_show_setup_font = false;                          // font setup view
bool do_show_setup_keys = false;                          // show keys setup view
bool do_show_setup_rss = false;                           // Setup rss posdcast view
bool do_show_setup_spotify = false;                       // setup spotify menu
bool do_show_setup_tidal = false;                         // setup tidal menu
bool do_save_setup_rss = false;                           // update db flag to do it (call func)
bool do_show_videoplayer = false;                         // show player config
bool do_show_tvgraber = false;                            // show tv graber/channel view config
bool do_show_rss = false;                                 // show rss config
bool use3deffect = false;                                 // use 3d scroll effect default no
bool do_zoom_music_cover = false;                         // show music conver
bool do_zoom_tidal = false;                               //
bool do_zoom_radio_cover = false;                               //
bool do_zoom_spotify_cover = false;                       // show spotify cover
bool do_zoom_tidal_cover = false;                         // show tidal play cover
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
bool do_find_playlist = false;
bool full_screen=false;                              // full screen mode


bool do_hent_music_search_online=false;                   // skal vi starte search 
bool do_hent_tidal_search_online=false;                   // skal vi starte search 
bool hent_radio_search=false;
bool hent_film_search=false;
bool hent_tv_channels=false;
bool hent_stream_search=false;
bool do_show_film_status_info=false;

bool do_hent_spotify_search_online=false;

int do_play_spotify=0;
int do_play_recorded_aktiv_nr=0;

int spotify_selected_startofset=0;                        // used by viewer
int tidal_selected_startofset=0;                          // used by viewer

int do_show_setup_select_linie=0;                       // bruges af setup
int do_show_editor_select_linie=0;                      // bruges af setup

int do_show_film_edit_select_linie=0;

int configrss_ofset=0;
int configdfont_ofset=1;

int config_font_select=1;


bool tidal_oversigt_loaded_begin = false;                                      // true then spotify update is started
int tidal_select_iconnr;                                  // selected icon
bool do_select_device_to_play = false;                       // select device to play on (spotify)
static bool do_play_music_cover = false;                         // start play music
bool ask_open_dir_or_play = false;                             // ask user to open dir or play file
bool do_stop_music_all = false;                           // stop all music play

int configuvmeter=1;

int spotify_select_iconnr=0;                              // selected icon

int do_play_music_aktiv_table_nr=0;                       // aktiv sang nr
int rknapnr=0;                                            // buttons vars
int sknapnr=0;                                            // stream button
int mknapnr=0;                                            // music
int tvknapnr=0;                                           // tv
int spotifyknapnr=1;                                      // spotify
int tidalknapnr=1;                                        // tidal support
int fknapnr=0;                                            // movie
int swknapnr=0;                                           //

bool do_show_setup_screen = false;                        // Show screen setuo view
bool do_show_setup_tema = false;                          // show tema setup view
bool do_show_setup_sound = false;                         // Show sound setup view

bool firsttime_xmltvupdate = true;                      // update tvguide xml files first start (force)
bool vis_old_recorded = false;                            //
int do_zoom_tvprg_aktiv_nr=0;                             //
bool do_save_config;
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



char keybuffer[512];                                    // keyboard buffer
std::string keybuffer1;
unsigned int keybufferindex=0;                          // keyboard buffer index
bool keybufferopenwin=false;
const int TEMA_ANTAL=10;                                                        // numbers of tema
bool do_show_torrent = false;
bool do_show_torrent_options = false;
bool do_show_torrent_options_move = false;
bool do_move_torrent_file = false;                        // do the move
bool do_move_torrent_file_now = false;                    // is it running now
bool do_show_load__torrent_file = false;
std::string do_show_load__torrent_file_string = "";
std::string rss_search_podcast_string = "";
float do_move_torrent_file_now_done = 0.0f;               // is it running now

static bool startwebbrowser=true;                                           // start web browser to enable login to spotify

int realrssrecordnr=0;                         //

static bool runwebserver=true;                                  // run spotify web server port 8080 default true else spotify do not work you need to login to be able to play



// screen saver uv stuf
float spectrum[5000];                                                           // used for spectium
float spectrum_left[4000];                                                      // used for spectium
float spectrum_right[4000];                                                     // used for spectium
float uvmax_values[1024];
int frequencyOctaves[15];                                                       // 15 octaver


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
static bool do_update_music_now = true;                            // start the process to update music db from global dir
static bool do_update_moviedb   = false;                            // set true to start thread on update movie db
static bool do_update_spotify   = true;                             // set true to start thread on update spotify + run web server

std::string config_playing_record_icon="images/playing_record.png";          // default playing record icon

// *************************************************************************************************
const char SOUNDUPKEY='+';
const char SOUNDDOWNKEY='-';
bool show_volume_info = false;                        	  // show vol info
int show_volume_info_timeout=0;                   		    // time out to close vol info again
int vis_volume_timeout=120;

class config_icons config_menu; // config icons used in menu

// Multi line editor **************************************************************************


/*
void mouse(int button, int state, int mouseX, int mouseY)
{
    if (button == GLUT_LEFT_BUTTON &&
        state == GLUT_DOWN)
    {
        textEditor.MouseClick(
            static_cast<float>(mouseX),
            static_cast<float>(mouseY)
        );
    }

    if (button == 3 && state == GLUT_DOWN)
        textEditor.ScrollUp();

    if (button == 4 && state == GLUT_DOWN)
        textEditor.ScrollDown();
}
*/


// #include <algorithm>
// #include <string>
// #include <vector>

#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
// #include <string>
// #include <vector>

class MultiLineEditor {
public:
    MultiLineEditor(Font* editorFont,float positionX,float positionY,float editorWidth,float editorHeight) : font(editorFont), x(positionX), y(positionY), width(editorWidth), height(editorHeight) {
        lines.emplace_back();
    }

    void SetActive(bool value) {
        active = value;
    }

    bool IsActive() const {
        return active;
    }

    void SetText(const std::string& text) {
      lines.clear();
      lines.emplace_back();
      cursorLine = 0;
      cursorColumn = 0;
      firstVisibleLine = 0;
      for (char character : text) {
        if (character == '\r')
          continue;
        if (character == '\n')
          NewLineInternal();
        else
          InsertCharacterInternal(character);
      }
      // Placér cursoren sidst i teksten
      if (!lines.empty()) {
        cursorLine = lines.size() - 1;
        cursorColumn = lines[cursorLine].size();
      } else {
        lines.emplace_back();
        cursorLine = 0;
        cursorColumn = 0;
      }
      KeepCursorVisible();
    }



    std::string GetText() const {
      std::string result;
      for (std::size_t i = 0; i < lines.size(); ++i) {
        if (i > 0)
          result += '\n';
        result += lines[i];
      }
      return result;
    }

    void Clear() {
      lines.clear();
      lines.emplace_back();
      cursorLine = 0;
      cursorColumn = 0;
      firstVisibleLine = 0;
    }

    void Draw(Renderer& renderer) {
      if (!font)
        return;
      DrawBackground(renderer);
      const std::size_t visibleLines = GetVisibleLineCount();
      for (std::size_t screenLine = 0; screenLine < visibleLines; ++screenLine) {
        const std::size_t documentLine = firstVisibleLine + screenLine;
        if (documentLine >= lines.size())
          break;
        const float drawX = x + padding;
        const float drawY = y + padding + static_cast<float>(screenLine) * lineHeight;
        if (drawY + lineHeight > y + height - padding)
          break;
        renderer.AddText(font,drawX,drawY+10,lines[documentLine],textR,textG,textB,textA);
      }
      DrawCursor(renderer);
    }

    void InsertCharacter(char character) {
      if (!active)
        return;
      if (character == '\r')
        return;
      if (character == '\n') {
        NewLine();
        return;
      }
      if (static_cast<unsigned char>(character) < 32)
        return;
      InsertCharacterInternal(character);
      KeepCursorVisible();
    }

    void InsertText(const std::string& text) {
      if (!active)
          return;
      for (char character : text) {
        if (character == '\r')
          continue;
        if (character == '\n')
          NewLineInternal();
        else
          InsertCharacterInternal(character);
      }
      KeepCursorVisible();
    }

    void NewLine() {
      if (!active)
        return;

      NewLineInternal();
      KeepCursorVisible();
    }

    void Backspace() {
      if (!active || lines.empty())
        return;
      if (cursorColumn > 0) {
        lines[cursorLine].erase(cursorColumn - 1, 1);
        --cursorColumn;
        RewrapLine(cursorLine);
      } else if (cursorLine > 0) {
        const std::size_t oldLine = cursorLine;
        const std::size_t previousLine = cursorLine - 1;
        const std::size_t previousLength =
            lines[previousLine].size();
        lines[previousLine] += lines[oldLine];
        lines.erase(lines.begin() + static_cast<std::ptrdiff_t>(oldLine));
        cursorLine = previousLine;
        cursorColumn = previousLength;
        RewrapLine(cursorLine);
      }
      EnsureAtLeastOneLine();
      KeepCursorVisible();
    }

    void DeleteCharacter() {
        if (!active || lines.empty())
            return;

        std::string& currentLine = lines[cursorLine];

        if (cursorColumn < currentLine.size()) {
            currentLine.erase(cursorColumn, 1);
            RewrapLine(cursorLine);
        }
        else if (cursorLine + 1 < lines.size()) {
            currentLine += lines[cursorLine + 1];

            lines.erase(
                lines.begin() +
                static_cast<std::ptrdiff_t>(cursorLine + 1)
            );

            RewrapLine(cursorLine);
        }

        EnsureAtLeastOneLine();
        KeepCursorVisible();
    }

    void MoveLeft() {
      std::printf("MoveLeft active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      if (!active)
          return;

      if (cursorColumn > 0) {
          --cursorColumn;
      }
      else if (cursorLine > 0) {
          --cursorLine;
          cursorColumn = lines[cursorLine].size();
      }
      std::printf("After MoveLeft active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      KeepCursorVisible();
    }

    void MoveRight() {
      std::printf("MoveRight active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      if (!active)
          return;

      if (cursorColumn < lines[cursorLine].size()) {
          ++cursorColumn;
      }
      else if (cursorLine + 1 < lines.size()) {
          ++cursorLine;
          cursorColumn = 0;
      }
      std::printf("After MoveRight active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      KeepCursorVisible();
    }

    void MoveUp() {
      std::printf("MoveUp active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      if (!active)
          return;
      if (cursorLine > 0) {
          --cursorLine;
          cursorColumn = std::min(
              cursorColumn,
              lines[cursorLine].size()
          );
      }
      std::printf("After MoveUp active=%d line=%zu column=%zu\n",active,cursorLine,cursorColumn);
      KeepCursorVisible();
    }

    void MoveDown() {
      std::printf("MoveDown active=%d line=%zu column=%zu lines=%zu\n",active,cursorLine,cursorColumn,lines.size());
      if (!active)
          return;
      if (cursorLine + 1 < lines.size()) {
          ++cursorLine;
          cursorColumn = std::min(
              cursorColumn,
              lines[cursorLine].size()
          );
      }
      std::printf("After MoveDown line=%zu column=%zu lines=%zu\n",cursorLine,cursorColumn,lines.size());
      KeepCursorVisible();
    }

    void MoveHome() {
        if (!active)
            return;

        cursorColumn = 0;
        KeepCursorVisible();
    }

    void MoveEnd() {
        if (!active)
            return;

        cursorColumn = lines[cursorLine].size();
        KeepCursorVisible();
    }

    void PageUp() {
        if (!active)
            return;
        const std::size_t count = GetVisibleLineCount();
        if (cursorLine > count)
            cursorLine -= count;
        else
            cursorLine = 0;
        cursorColumn = std::min(cursorColumn,lines[cursorLine].size()
        );

        KeepCursorVisible();
    }

    void PageDown() {
        if (!active)
            return;
        const std::size_t count = GetVisibleLineCount();
        cursorLine = std::min(cursorLine + count,lines.size() - 1);
        cursorColumn = std::min(cursorColumn, lines[cursorLine].size());

        KeepCursorVisible();
    }

    void MouseClick(float mouseX, float mouseY) {
        active = mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
        if (!active)
            return;
        const float localY = mouseY - y - padding;
        int clickedScreenLine =
            static_cast<int>(localY / lineHeight);
        if (clickedScreenLine < 0)
            clickedScreenLine = 0;
        std::size_t clickedLine =
            firstVisibleLine +
            static_cast<std::size_t>(clickedScreenLine);
        if (clickedLine >= lines.size())
            clickedLine = lines.size() - 1;
        cursorLine = clickedLine;
        cursorColumn = FindColumnFromPixel(mouseX - x - padding);
        KeepCursorVisible();
    }

    void ScrollUp() {
        if (firstVisibleLine > 0) --firstVisibleLine;
    }

    void ScrollDown() {
        const std::size_t visibleLines = GetVisibleLineCount();
        if (firstVisibleLine + visibleLines < lines.size()) ++firstVisibleLine;
    }

    void SetTextColor(float r, float g, float b, float a = 1.0f) {
        textR = r;
        textG = g;
        textB = b;
        textA = a;
    }

    void SetBackgroundColor(float r,float g,float b,float a = 1.0f) {
        backgroundR = r;
        backgroundG = g;
        backgroundB = b;
        backgroundA = a;
    }

    void SetPadding(float value) {
        padding = value;
        RewrapAllLines();
    }

    void SetLineHeight(float value) {
        lineHeight = value;
        KeepCursorVisible();
    }

    void update_movie_desc(int nr,std::string desc) {
      strcpy(film_oversigt.filmoversigt[nr].film_subtitle, desc.c_str());
    }

private:
    Font* font = nullptr;

    float x = 0.0f;
    float y = 0.0f;
    float width = 500.0f;
    float height = 200.0f;
    float padding = 10.0f;
    float lineHeight = 18.0f;
    float textR = 1.0f;
    float textG = 1.0f;
    float textB = 1.0f;
    float textA = 1.0f;
    float backgroundR = 0.04f;
    float backgroundG = 0.05f;
    float backgroundB = 0.08f;
    float backgroundA = 1.0f;
    float borderR = 0.20f;
    float borderG = 0.65f;
    float borderB = 1.00f;
    float borderA = 1.0f;
    std::vector<std::string> lines;
    std::size_t cursorLine = 0;
    std::size_t cursorColumn = 0;
    std::size_t firstVisibleLine = 0;
    bool active = false;

    void DrawBackground(Renderer& renderer) {
      renderer.AddTextureRect(0,setuprssback, x, y, width, height, backgroundR, backgroundG, backgroundB, backgroundA);
      const float borderWidth = 2.0f;
      renderer.AddThickLine(x, y, x + width, y, borderWidth, borderR, borderG, borderB, borderA);
      renderer.AddThickLine(x + width, y,x + width, y + height,borderWidth, borderR, borderG, borderB, borderA);
      renderer.AddThickLine(x + width, y + height,x, y + height, borderWidth,borderR, borderG, borderB, borderA);
      renderer.AddThickLine( x, y + height,x, y, borderWidth,borderR, borderG, borderB, borderA);
    }

    void DrawCursor(Renderer& renderer) {
      if (!active || lines.empty())
        return;
      using Clock = std::chrono::steady_clock;
      const auto now = Clock::now().time_since_epoch();
      const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
      // Cursoren vises 500 ms og skjules 500 ms
      if (((milliseconds / 500) % 2) == 0)
        return;
      if (cursorLine < firstVisibleLine)
        return;
      const std::size_t screenLine =
        cursorLine - firstVisibleLine;
      if (screenLine >= GetVisibleLineCount())
        return;
      const std::string textBeforeCursor =
        lines[cursorLine].substr(0, cursorColumn);
      const float cursorX =
        x + padding +
        GetTextWidth(textBeforeCursor);
      const float cursorY =
        y + padding +
        static_cast<float>(screenLine) * lineHeight;
      if (cursorX > x + width - padding)
        return;
      if (cursorY + lineHeight > y + height - padding)
        return;
      renderer.AddThickLine(cursorX,cursorY,cursorX, cursorY + lineHeight - 3.0f, 2.0f, 0.2f, 0.8f,1.0f, 1.0f);
    }



    void InsertCharacterInternal(char character) {
        EnsureAtLeastOneLine();
        std::string& currentLine = lines[cursorLine];
        currentLine.insert(currentLine.begin() +static_cast<std::ptrdiff_t>(cursorColumn),character);
        ++cursorColumn;
        RewrapLine(cursorLine);
    }

    void NewLineInternal() {
      EnsureAtLeastOneLine();
      std::string& currentLine = lines[cursorLine];
      std::string textAfterCursor = currentLine.substr(cursorColumn);
      currentLine.erase(cursorColumn);
      lines.insert(lines.begin() +static_cast<std::ptrdiff_t>(cursorLine + 1),textAfterCursor);
      ++cursorLine;
      cursorColumn = 0;
      RewrapLine(cursorLine);
    }

    void RewrapLine(std::size_t lineIndex) {
      if (!font || lineIndex >= lines.size())
        return;
      const float maxWidth = std::max(1.0f, width - padding * 2.0f);
      std::size_t currentIndex = lineIndex;
      while (currentIndex < lines.size()) {
        if (GetTextWidth(lines[currentIndex]) <= maxWidth)
            break;
        std::string& currentText = lines[currentIndex];
        std::size_t fitPosition =
            FindFittingCharacterCount(currentText, maxWidth);
        if (fitPosition == 0)
            fitPosition = 1;
        std::size_t splitPosition = fitPosition;
        std::size_t nextTextPosition = fitPosition;
        /*
          * Find sidste mellemrum inden for den tilladte bredde.
          * Hvis der ikke er et mellemrum, ombrydes det lange ord.
          */
        const std::size_t spacePosition =
            currentText.rfind(' ', fitPosition);
        if (spacePosition != std::string::npos && spacePosition > 0) {
            splitPosition = spacePosition;
            nextTextPosition = spacePosition + 1;
        }
        while (nextTextPosition < currentText.size() && currentText[nextTextPosition] == ' ') {
          ++nextTextPosition;
        }
        std::string nextLine = currentText.substr(nextTextPosition);
        currentText.erase(splitPosition);
        const bool cursorWasOnThisLine = cursorLine == currentIndex;
        if (cursorWasOnThisLine && cursorColumn > splitPosition) {
          if (cursorColumn >= nextTextPosition)
            cursorColumn -= nextTextPosition;
          else
            cursorColumn = 0;
          cursorLine = currentIndex + 1;
        }
        lines.insert(lines.begin() +static_cast<std::ptrdiff_t>(currentIndex + 1), nextLine);
        ++currentIndex;
      }
    }

    void RewrapAllLines() {
      if (!font)
        return;
      std::size_t lineIndex = 0;
      while (lineIndex < lines.size()) {
        RewrapLine(lineIndex);
        ++lineIndex;
      }
      cursorLine = std::min(
        cursorLine,
        lines.size() - 1
      );
      cursorColumn = std::min(
        cursorColumn,
        lines[cursorLine].size()
      );
      KeepCursorVisible();
    }

    std::size_t FindFittingCharacterCount(const std::string& text,float maxPixelWidth) const {
      float pixelWidth = 0.0f;
      for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char character =
          static_cast<unsigned char>(text[i]);
        if (character >= 240)
          continue;
        const Character& glyph = font->glyph[character];
        const float characterWidth = static_cast<float>(glyph.advance >> 6);
        if (pixelWidth + characterWidth > maxPixelWidth)
            return i;
        pixelWidth += characterWidth;
      }
      return text.size();
    }

    std::size_t FindColumnFromPixel(float pixelX) const {
      if (pixelX <= 0.0f)
          return 0;
      const std::string& currentText =
          lines[cursorLine];
      float currentX = 0.0f;
      for (std::size_t i = 0; i < currentText.size(); ++i) {
        const unsigned char character =
          static_cast<unsigned char>(currentText[i]);
        if (character >= 240)
            continue;
        const Character& glyph =
          font->glyph[character];
        const float characterWidth =
          static_cast<float>(glyph.advance >> 6);
        /*
          * Cursoren placeres før eller efter tegnet,
          * afhængigt af hvilken halvdel der klikkes på.
          */
        if (pixelX < currentX + characterWidth * 0.5f)
            return i;
        currentX += characterWidth;
        if (pixelX < currentX)
          return i + 1;
      }
      return currentText.size();
    }

    float GetTextWidth(const std::string& text) const {
      if (!font)
          return 0.0f;
      float pixelWidth = 0.0f;
      for (unsigned char character : text) {
        if (character >= 240)
            continue;
        const Character& glyph =
            font->glyph[character];
        pixelWidth += static_cast<float>(glyph.advance >> 6);
      }
      return pixelWidth;
    }

    std::size_t GetVisibleLineCount() const {
        const float availableHeight = std::max(1.0f, height - padding * 2.0f);
        return std::max<std::size_t>(1,static_cast<std::size_t>(availableHeight / lineHeight));
    }

    void KeepCursorVisible() {
        if (lines.empty())
            return;
        const std::size_t visibleLines = GetVisibleLineCount();
        if (cursorLine < firstVisibleLine) {
          firstVisibleLine = cursorLine;
        } else if (cursorLine >= firstVisibleLine + visibleLines) {
          firstVisibleLine = cursorLine - visibleLines + 1;
        }
        const std::size_t maximumFirstLine = lines.size() > visibleLines ? lines.size() - visibleLines : 0;
        firstVisibleLine = std::min(firstVisibleLine,maximumFirstLine);
    }

    void EnsureAtLeastOneLine() {
      if (lines.empty()) {
        lines.emplace_back();
        cursorLine = 0;
        cursorColumn = 0;
      }
    }
};


// ***********************************************************************************************



struct dirmusic_list_type {
    char name[200];
    std::string name1;
    unsigned int songlength;
    int aktiv;
};

struct dirmusic_dirs_type {
    char dirname[20];
    std::string dirname1;
    int dirid;
    GLuint _textureId;
};

const int dirliste_size=512;


// class to playlist *****************************************************************************

class dirmusictype {
    private:
        unsigned int listesize;			      		// antal elementer i liste
        int numbersofsongs;				           	// numbers of songs in songlist array
        int numbersofdirs;			          		// numbers of under directorys in songlist array
        int artist_id;				            		// artist id from mythtv mysql
    public:
        std::vector <dirmusic_dirs_type> dirliste = {};
        std::vector <dirmusic_list_type> songliste = {};
        GLuint textureId;		             			// directorys texture
        int emtydirmusic() {
            // for(unsigned int i=0;i<listesize;i++) {			// reset all music info
            songliste.clear();
            dirliste.clear();
            numbersofsongs=0;
            numbersofdirs=0;
            return(true);
        }
        // constructor
        dirmusictype(unsigned int antal) { 				// constructor
            listesize=antal;
            emtydirmusic();
        }

        // destructor
        ~dirmusictype() {
            listesize=0;
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
          if (nr<dirliste.size()) {
            dirliste[nr]._textureId=texture;
            return(1);
          } 
          return(0);
        }
        GLuint gettexture(int nr) {
            if (nr<dirliste.size()) return(dirliste[nr]._textureId); else return(0);
        }
        int numbersindirlist() {
             return(numbersofdirs);
        }
};

// ************************************************************************************************

dirmusictype dirmusic(dirliste_size);					// oversigt over dirs/songs som skal spilles

int dirmusictype::pushsong(char *name,char *artist) {
  // new vector ver
  dirmusic_list_type tmpsong;
  tmpsong.name1=name;
  artist_id=atoi(artist);
  songliste.push_back(tmpsong);
  numbersofsongs=songliste.size();
  return(numbersofsongs);
}


int dirmusictype::popsong(char *name,bool *aktiv,int nr) {
  if (nr<=numbersofsongs) {
    strcpy(name,songliste[nr].name1.c_str());
    *aktiv=songliste[nr].aktiv;
    return(1);
  } else return(0);
}


int dirmusictype::pushdir(char *name,char *dirid) {
  // new vector ver
  dirmusic_dirs_type tmpdir;
  tmpdir.dirname1=name;
  tmpdir.dirid=atoi(dirid);
  dirliste.push_back(tmpdir);
  // numbersofsongs=dirliste1.size();
  return(1);
}


int dirmusictype::popdir(char *name,int nr) {
  if (nr<dirliste.size()) strcpy(name,dirliste[nr].dirname1.c_str());
  else strcpy(name,"");
  return(1);
}


// ****************************************************************************************
//
// parameret til mysql er dirid = directory_id i database
// retunere path og parent_id som bliver udfyldt fra mysql databasen
//
// ****************************************************************************************

void hent_dir_id_info(char *path,char *parent_id,char *dirid) {
    const char *database = (char *) "mythtvcontroller";
    // mysql stuf
    char sqlselect[256];
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
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
    const char *database = (char *) "mythtvcontroller";
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
    // write to debug log
    sprintf(debuglogdata,"Loading info about playlist nr: %d ",playlistnr);
    write_logfile(logfile,(char *) debuglogdata);
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
              write_logfile(logfile,(char *) debuglogdata);

              strcpy(tmptxt,configmusicpath);		                           // start path
              sprintf(tmptxt2,"%s",row1[2]);			                         // hent dir id
              hent_dir_id_info(tmptxt1,parent_id,tmptxt2);		             // hent path af tmptxt2 som er = dir_id
              strcat(tmptxt,tmptxt1);				                               // add path
              strcat(tmptxt,"/");
              strcpy(tmptxt3,tmptxt);			                                 // er = path
              strcat(tmptxt3,"mythcfront.jpg");		                         // add filename til cover
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






// ************************************************************************************************



#if defined USE_FMOD_MIXER
// fmod error handler
void ERRCHECK(FMOD_RESULT result,unsigned int songnr) {
  char file_path[1024];
  if (result != FMOD_OK) {
    if (result!=46) {
      fprintf(stderr,"FMOD error! (%d): %s on songnr %d \n", result, FMOD_ErrorString(result),songnr);
      if (vis_music_oversigt) {
        // aktiv_playlist.m_play_playlist(file_path,songnr);
        fprintf(stderr,"File name %s\n",file_path);
      }
    }
    // vis_error_flag=result;
    // vis_error = true;			// set vis error flag
    // vis_error_songnr=songnr;		// gem fil navn som ikke kan spilles
    // vis_error_timeout=ERROR_TIMEOUT;
    // do_zoom_radio_cover = false;		// close play info
  }
}
#endif



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
    glcNewFontFromFamily(myFont, configfontname);                                       // Droid Serif,UbuntumFreeMono , FreeMono
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





// Simulerer FMOD FFT data
void updateSpectrum() {
    static float t = 0;
    t += 0.05f;
    for(int i=0;i<128;i++) {
        spectrum[i] = (sin(t + i*0.15f) * 0.5f + 0.5f);
        // lidt ekstra variation
        spectrum[i] *= 0.8f;
    }
}


float pixelToGLX(float x) {
    return (x / 1280.0f) * 2.0f - 1.0f;
}


float pixelToGLY(float y) {
    return 1.0f - (y / 720.0f) * 2.0f;
}




// *************************************************************************************
//
// Display
//
// *************************************************************************************

MultiLineEditor textEditor(&myfont,450.0f,700.0f,410.0f,230.0f);

void display() {
    std::string surl;
    char systemcommand[8192];
    static float sin_table[70*4];
    static float barRotation[70+1][51];
    int TABLE_SIZE = 50;
    float angle;
    float r,g,b;
    bool firsttime_sin_table=true;
    int iconsizex=200;
    int iconsizey=200;
    int xof;
    int yof;
    bool tidal_player_start_status;
    unsigned int ms = 0;
    unsigned int splaytime;
    unsigned int splaytime_songlength;
    float frequency;
    unsigned int rplaytime_songlength;
    float y;
    float ll;
    float xxx;
    static bool fmodcreatesound=false;
    static FMOD_OPENSTATE openstate;
    static int movie_play_status;
    int sounderrflag;
    char temptxt1[80];
    std::string temptxt2;
    bool do_play_music_aktiv_nr_select_array[1000];                             // array til at fortælle om sange i playlist askopendir er aktiv
    int numtags, numtagsupdated, count;                                         // bliver brugt til at vise stream tags
    static time_t rawtime,rawtime1=0;
    int min,tim;
    struct tm *timeinfo;
    struct tm* t;
    struct timeb tb;
    float clockVol=1000.0f, angle1min = M_PI / 30.0f,  minStart=4.9f,minEnd=5.0f, stepStart=4.8f,stepEnd=5.0f;
    static float angleHour = 0,angleMin  = 0,angleSec  = 0;
    static float last_angleSec=0.0f;  
    static time_t today=0;
    bool clock_udpate;
    char strhour[20];
    char strmin[20];
    int lastsec=0;

    // std::string temprgtxt;

    static float barHeights[45] = {0}; // persistent for smoothing
    // GLuint normal_icon=loadTexture((char *) "dvdcover.png");
    if (normal_icon==0) normal_icon=loadTexture((char *) "dvdcover.png");
    int savertimeout=0;
    rawtime=time(NULL);                                 // hent now time
    savertimeout=atoi(configscreensavertimeout);
    if ((rawtime1==0) || (saver_irq)) {                 // ur timer
      rawtime1=rawtime+(60*savertimeout);               // x minuter hentet i config
      visur = false;                                    // if (debug) printf("Start screen saver timer.\n");
      saver_irq = false;
    } else {
//      rawtime1=rawtime+(60*2);             // x minuter hentet i config
      visur = false;                        // if (debug) printf("Start screen saver timer.\n");
      saver_irq = false;                    // start screen saver
    }
    // update clock
    if (rawtime>rawtime1) {
      visur = true;
    }
    timeinfo = localtime(&rawtime);
    strftime(strhour,20,"%I",timeinfo);
    strftime(strmin,20,"%M",timeinfo);
    min=atoi(strmin);
    tim=atoi(strhour);
    // make xmltv update
    today=time(NULL);
    t=localtime(&today);                                                        // local time
    ftime(&tb);
    clock_udpate = true;
    // used by analog clock
    if (lastsec>=20) {
      clock_udpate = true;
      lastsec=0;
    }
    lastsec++;
    glClear(GL_COLOR_BUFFER_BIT);
    // update movie texture
    if (film_oversigt.film_is_playing) {
      film_oversigt.vlsupdateTexture();
      visur=false;
      saver_irq=true;                                     // stop screen saver
    }
    // updateSpectrum();
    renderer.Begin();
    // show screen saver
    if (visur) {
      if ((urtype == MUSICMETER) || (urtype==ANALOG)) {
        // show music meter if music is playing
        // No music is playing show analog clock
        // renderer.AddRect(10.0f, 10.0f, 20.0f, 100.0f, 1.0f,1.0f,1.0f,1.0f);
        // ok
        //renderer.AddTriangle(-100,-100,100,-100,0,100,1,1,1,1);
        // virker ikke
        // renderer.AddLine(1,1,400,400,1,1,1,1);
         // time
         // 
         float centerx=1920/2;
         float centery=1080/2;
        if ((streamoversigt.stream_is_playing==false) && (radiooversigt.playing==false) && (tidal_oversigt.get_tidal_playing_flag()== false) && (musicoversigt.play()==false) && (film_oversigt.film_is_playing==false)) {
          if (analog_clock_background) renderer.AddTextureRect(0,analog_clock_background, 1, 1, 1920, 1080,1,1,1,1);
          // Markeringerne rundt om uret
          float angle1min = M_PI / 30.0f;
          float cx = 1920.0f * 0.5f;
          float cy = 1080.0f * 0.5f;
          float clockRadius = 400.0f;
          float minStart  = clockRadius * 0.87f;
          float minEnd    = clockRadius * 0.95f;
          float stepStart = clockRadius * 0.85f;
          float stepEnd   = clockRadius * 0.95f;
          for (int i = 0; i < 60; i++) {
              float angle = i * angle1min - M_PI / 2.0f;
              float start;
              float end;
              float r;
              float g;
              float b;
              float width;
              if (i % 5 == 0) {
                  // 5-sekunders markering
                  start = stepStart;
                  end   = stepEnd;
                  r     = 1.0f;
                  g     = 0.0f;
                  b     = 0.0f;
                  width = 5.0f;
              } else {
                  // Sekundmarkering
                  start = minStart;
                  end   = minEnd;
                  r     = 1.0f;
                  g     = 1.0f;
                  b     = 1.0f;
                  width = 2.0f;
              }
              float x1 = cx + cosf(angle) * start;
              float y1 = cy + sinf(angle) * start;
              float x2 = cx + cosf(angle) * end;
              float y2 = cy + sinf(angle) * end;
              renderer.AddThickLine(x1, y1,x2, y2,width,r, g, b, 1.0f);
          }

          // show clock hands
          time_t nowt = time(nullptr);
          tm *t = localtime(&nowt);
          // minutter + sekunder giver glidende minutviser
          angleMin = ((t->tm_min + t->tm_sec / 60.0f) * 2.0f * M_PI) / 60.0f;
          // timer + minutter giver glidende timeviser
          angleHour = ((t->tm_hour % 12 + t->tm_min / 60.0f) * 2.0f * M_PI) / 12.0f;
          // timeviser
          renderer.DrawHand(cx,cy,clockRadius*0.65f,angleHour,6.0f,0,1,1,1);
          // minutviser
          renderer.DrawHand(cx,cy, clockRadius*0.80f, angleMin, 4.0f, 0,1,1,1);
          // Flydende sekundviser
          using Clock = std::chrono::system_clock;
          auto now = Clock::now();
          auto millisecondsSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
          // Position inden for det aktuelle minut: 0.0 til under 60.0
          double seconds = static_cast<double>(millisecondsSinceEpoch % 60000) / 1000.0;
          // 0 sekunder skal pege lige op
          float angleSec = static_cast<float>(seconds / 60.0 * 2.0 * M_PI - M_PI / 2.0);
          float handLength = clockRadius * 0.90f;
          float secX = cx + cosf(angleSec) * handLength;
          float secY = cy + sinf(angleSec) * handLength;
          renderer.AddThickLine(cx, cy,secX, secY,3.0f,1.0f, 1.0f, 1.0f, 1.0f);
        } else {
          // Music is playing
          // vis spectum as screen saver
          float uvxpos=100;
          float high;
          for(int qq=0;qq<55;qq++) {
            int yypos = 1050;
            float decay = 0.8f;        // 0.05f
            static float barHeights[45] = {0}; // persistent for smoothing
            float target = sqrtf(spectrum[qq] * 8.0f) * 2.0f;
            if (target > barHeights[qq]) {
              barHeights[qq] = target;
            } else {
              barHeights[qq] -= decay;
              if (barHeights[qq] < 0) barHeights[qq] = 0;
            }
            high = barHeights[qq]/2;
            if (vis_tidal_oversigt) 
            if (high>50) high=50;
            for(int i=0;i<high;i++) {
              renderer.AddTextureRect(0,texturedot, uvxpos, yypos, 28, 20,1,1,1,1);
              yypos = yypos - 16;  // 16
            }
            uvxpos += 28+1;
          }
        }
      }
      if (urtype == MUSICMETER3) {
        saver_musicmeter.DrawPlasmaVortex(
          960.0f,     // center X
          540.0f,     // center Y
          400.0f,     // radius
          0.5        // 0.0 - 1.0
        );
        saver_musicmeter.DrawAudioRing();
      }
      if (urtype == PLASMA) {
        drawPlasma(1920,1080);
      }
    }


    if (do_update_rss) {
      fprintf(stderr,"Start phread podcast.\n");
      streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");             // load all stream from rss files
      do_update_rss_show=false;
      do_update_rss = false;
    }
    // show background
    if ((!(visur)) && (_textureIdback_main) && (!(vis_radio_oversigt)) && (!(vis_stream_oversigt)) && (!(vis_spotify_oversigt)) && (!(vis_music_oversigt)) && (!(vis_tidal_oversigt)) && (!(vis_film_oversigt)) && (!(vis_tv_oversigt))) show_background();
    //
    // show menu **********************************************************************
    // main menu
    if ((!(visur)) && (!(vis_tv_oversigt)))  {
      // icon 1
      if (vis_radio_or_music_oversigt) {
        glLoadName(82);                                                           // Info icon nr 82 spotify
        renderer.AddTextureRect(82,spotifybutton,config_menu.config_spotifyx,config_menu.config_spotifyy,iconsizex,iconsizex,1,1,1,1);
      } else {
        // play info icon
        if ((vis_music_oversigt) || (vis_radio_oversigt) || (vis_film_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt) || (vis_tidal_oversigt)) {
            glLoadName(27);                                                           // Info icon nr 27
            renderer.AddTextureRect(27,_textureIdplayinfo,config_menu.config_playinfox,config_menu.config_playinfoy,iconsizex,iconsizex,1,1,1,1);
        } else {
            // tv
            glLoadName(1);                                                            // Tv guide icon nr 1
            renderer.AddTextureRect(1,_textureIdtv,config_menu.config_tvguidex,config_menu.config_tvguidey,iconsizex,iconsizex,1,1,1,1);
        }
      }
      // Icon 2
      if (vis_radio_oversigt) {
        renderer.AddTextureRect(80,radiobutton,config_menu.config_tidal1x,config_menu.config_tidal1y,iconsizex,iconsizex,1,1,1,1);
      }
      if (vis_radio_or_music_oversigt) {
        glLoadName(83);
        // printf("Show icon 2 1\n");                                                   // Info icon nr 83 tidal
        if (vis_radio_oversigt==false) renderer.AddTextureRect(83,tidalbutton,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
      } else if (vis_film_oversigt) {
        if (vis_film_oversigt) {
          if (vis_stream_or_movie_oversigt) {
            renderer.AddTextureRect(3,_textureIdfilm_aktiv,config_menu.config_musicx,config_menu.config_musicy,iconsizex,iconsizex,1,1,1,1);
          } else {
            renderer.AddTextureRect(84,streambutton,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
          }
        } else {
          glLoadName(80);           
          // printf("Show icon 2 2\n");                                                   // Info icon nr stream
          renderer.AddTextureRect(84,streambutton,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
        }
        if (vis_stream_or_movie_oversigt) {

        }
      } else if (vis_spotify_oversigt) {
        glLoadName(2);       
        // printf("Show icon 2 3\n");                                                   // Info icon nr 82 spotify
        renderer.AddTextureRect(2,spotifybutton1,config_menu.config_mediax,config_menu.config_mediay,iconsizex,iconsizex,1,1,1,1);
      } else if (vis_tidal_oversigt) {
        glLoadName(83);      
        // printf("Show icon 2 4\n");                                                   // Info icon nr 83 tidal
        renderer.AddTextureRect(83,tidalbutton1,config_menu.config_tidal1x,config_menu.config_tidal1y,iconsizex,iconsizex,1,1,1,1);
      } else {
        if (vis_film_oversigt) {
          glLoadName(3); 	
          // printf("Show icon 2 5\n");		                                                    // film icon name 3
          // renderer.AddTextureRect(3,_textureIdfilm_aktiv,config_menu.config_musicx,config_menu.config_musicy,iconsizex,iconsizex,1,1,1,1);
        } else if (vis_stream_oversigt) {
          glLoadName(3); 			 
          // printf("Show stream icon 2 6\n");                                                   // film icon name 3
          renderer.AddTextureRect(3,streambutton,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
        } else if (vis_music_oversigt) {
          // printf("Show icon 2 7\n");
          glLoadName(2); 			// Overwrite the first name in the buffer
          renderer.AddTextureRect(2,_textureIdmusic_aktiv,config_menu.config_music_activex,config_menu.config_music_activey,iconsizex,iconsizex,1,1,1,1);
        } else if (vis_radio_oversigt) {
          // printf("Show icon 2 8\n");
          glLoadName(2); 			// Overwrite the first name in the buffer
          renderer.AddTextureRect(2,radiobutton1,config_menu.config_radio1x,config_menu.config_radio1y,iconsizex,iconsizex,1,1,1,1);
        } else if (vis_spotify_oversigt) {          
          glLoadName(2); 			// Overwrite the first name in the buffer
          // printf("Show icon 2 9\n");
          renderer.AddTextureRect(2,_textureIdmusic,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
        } else if (vis_tidal_oversigt) {
          glLoadName(2); 			// Overwrite the first name in the buffer
          // printf("Show icon 2 10\n");
          renderer.AddTextureRect(2,_textureIdmusic,config_menu.config_tidalx,config_menu.config_tidaly,iconsizex,iconsizex,1,1,1,1);
        } else {
          if (vis_stream_or_movie_oversigt) {
            renderer.AddTextureRect(84,streambutton,config_menu.config_spotify1x,config_menu.config_spotify1y,iconsizex,iconsizex,1,1,1,1);
          } else {
            glLoadName(2); 			// Overwrite the first name in the buffer
            // printf("Show icon 2 11\n");
            renderer.AddTextureRect(2,_textureIdmusic,config_menu.config_musicx,config_menu.config_musicy,iconsizex,iconsizex,1,1,1,1);
          }
        }
      }
      
      // Icon 3
      if (vis_radio_or_music_oversigt) {
        // glBindTexture(GL_TEXTURE_2D,radiobutton);
        // printf("Show icon 3 1\n");
        glLoadName(80); 			                                                  // radio icon name 80
        if (vis_radio_oversigt==false) {
          renderer.AddTextureRect(80,radiobutton,config_menu.config_movie1x,config_menu.config_movie1y,iconsizex,iconsizex,1,1,1,1);
        } else {
          renderer.AddTextureRect(23,_textureIdpup,config_menu.config_upx,config_menu.config_upy,iconsizex,iconsizex,1,1,1,1);
        }

      } else if (vis_stream_or_movie_oversigt) {
        if (vis_film_oversigt) {
          renderer.AddTextureRect(23,_textureIdpup,config_menu.config_upx,config_menu.config_upy,iconsizex,iconsizex,1,1,1,1);
        } else {
          glLoadName(3);    
          // printf("Show icon 2\n");                                                       // Info icon nr movie
          renderer.AddTextureRect(3,moviebutton,config_menu.config_movie1x,config_menu.config_movie1y,iconsizex,iconsizex,1,1,1,1);
        }
      } else {
        //icon pil up
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt) || (vis_tidal_oversigt)) {
            glLoadName(23); 			// Overwrite the first name in the buffer
            // printf("Show icon 3\n");
            renderer.AddTextureRect(23,_textureIdpup,config_menu.config_upx,config_menu.config_upy,iconsizex,iconsizex,1,1,1,1);
        } else {
          if ((vis_film_oversigt) || (vis_stream_oversigt)) {
            // printf("Show icon 4\n");
            renderer.AddTextureRect(0,_textureIdfilm_aktiv,config_menu.config_moviex,config_menu.config_moviey,iconsizex,iconsizex,1,1,1,1);
          } else {

            glLoadName(13); 			// Overwrite the first name in the buffer
            // printf("Show icon 5\n");
            renderer.AddTextureRect(13,moviebutton_2,config_menu.config_moviex,config_menu.config_moviey,iconsizex,iconsizex,1,1,1,1);
          }
        }
      }
    
      // Icon 4
      if (vis_radio_or_music_oversigt) {
        if (vis_radio_oversigt==false) {
          glLoadName(81); 			                                                  // music icon name 81          
          renderer.AddTextureRect(81,musicbutton,config_menu.config_recordedx,config_menu.config_recordedy,iconsizex,iconsizex,1,1,1,1);
        } else {
          renderer.AddTextureRect(23,_textureIdpdown,config_menu.config_recordedx,config_menu.config_recordedy,iconsizex,iconsizex,1,1,1,1);
        }
      } else {
        // pil down
        if ((vis_music_oversigt) || (vis_film_oversigt) || (vis_radio_oversigt) || (vis_stream_oversigt) || (vis_spotify_oversigt) || (vis_tidal_oversigt)) {
          glLoadName(24); 			                                                // load film icon name
          renderer.AddTextureRect(24,_textureIdpdown,config_menu.config_downx,config_menu.config_downy,iconsizex,iconsizex,1,1,1,1);
        } else {
          // recorded icon
          if (vis_recorded_oversigt) {
            glLoadName(4);                                                        //
            renderer.AddTextureRect(4,_textureIdrecorded_aktiv,config_menu.config_recorded1x,config_menu.config_recorded1y,iconsizex,iconsizex,1,1,1,1);
          } else {
            glLoadName(4);                                                        //
            renderer.AddTextureRect(4,_textureIdrecorded,config_menu.config_recordedx,config_menu.config_recordedy,iconsizex,iconsizex,1,1,1,1);
            // moviebutton_2
            // _textureIdrecorded
          }
        }
      }
      // Icon 5
      if (vis_radio_or_music_oversigt) {
        glLoadName(29);
        renderer.AddTextureRect(29,_textureclosemain,config_menu.config_closex,config_menu.config_closey,iconsizex,iconsizex,1,1,1,1);
      }
      if (vis_stream_or_movie_oversigt) {
        glLoadName(29);
        renderer.AddTextureRect(29,_textureclosemain,config_menu.config_closex,config_menu.config_closey,iconsizex,iconsizex,1,1,1,1);
      }
      if (vis_stream_oversigt) {
        renderer.AddTextureRect(29,_textureclosemain,config_menu.config_closex,config_menu.config_closey,iconsizex,iconsizex,1,1,1,1);
      }


      if ((vis_film_oversigt) && (film_oversigt.get_search_view())) {
        // 28
        glLoadName(28);
        /*
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( config_menu.config_reset_searchx, config_menu.config_reset_searchy , 0.0);
        glTexCoord2f(0, 1); glVertex3f( config_menu.config_reset_searchx, config_menu.config_reset_searchy+iconsizex , 0.0);
        glTexCoord2f(1, 1); glVertex3f( config_menu.config_reset_searchx+iconsizex, config_menu.config_reset_searchy+iconsizex , 0.0);
        glTexCoord2f(1, 0); glVertex3f( config_menu.config_reset_searchx+iconsizex, config_menu.config_reset_searchy , 0.0);
        glEnd();
        */
      }
      if (vis_uv_meter==false) {
        if ((!(vis_stream_or_movie_oversigt)) && (!(vis_radio_or_music_oversigt)) && (!(vis_music_oversigt)) && (!(vis_film_oversigt))  && (!(vis_recorded_oversigt)) &&  (!(vis_stream_oversigt)) && (!(vis_radio_oversigt)) && (!(vis_spotify_oversigt))&& (!(vis_tidal_oversigt)) && (!(show_status_update))) {
          // setup icon
          glLoadName(5);
          if (do_show_setup) renderer.AddTextureRect(5,_texturesetupmenu_select,config_menu.config_setupx,config_menu.config_setupy,iconsizex,iconsizex,1,1,1,1);
            else renderer.AddTextureRect(5,_texturesetupmenu,config_menu.config_setupx,config_menu.config_setupy,iconsizex,iconsizex,1,1,1,1);
        }
      }
      if ((vis_spotify_oversigt) && (firsttimespotifyupdate==false)) {
          if (do_show_spotify_search_oversigt) {
              glLoadName(5);
              // renderer.AddTextureRect(5,spotify_search_back,config_search_activex.config_closex,config_menu.config_closey,config_search_activey,iconsizex,1,1,1,1);
          } else {
              glLoadName(5);
              // renderer.AddTextureRect(5,spotify_search,config_menu.config_search_activex,config_menu.config_closey,config_search_activey,iconsizex,1,1,1,1);
          }
      }
      if (vis_tidal_oversigt) {
          glLoadName(5);
          renderer.AddTextureRect(5,spotify_search,config_menu.config_search_activex,config_menu.config_search_activey,iconsizex,iconsizex,1,1,1,1);
      }
      if (vis_music_oversigt) {
          glLoadName(5);
          renderer.AddTextureRect(5,spotify_search,config_menu.config_search_activex,config_menu.config_search_activey,iconsizex,iconsizex,1,1,1,1);
      }
      // show exit button
      if ((!(vis_music_oversigt)) && (!(vis_music_oversigt)) && (!(vis_tv_oversigt)) && (!(vis_film_oversigt)) && (!(vis_stream_oversigt)) && (!(vis_spotify_oversigt)) && (!(vis_tidal_oversigt)) && (!(vis_radio_oversigt)) && (!((do_show_spotify_search_oversigt)))) {
          glLoadName(96);                                                        // exit button nr 6
          renderer.AddTextureRect(96,_textureexit,config_menu.config_exitx,config_menu.config_exity,iconsizex/3,iconsizex/3,1,1,1,1);
      }
    } // end icons

    // tv guide view
    if ((visur==false) && (vis_tv_oversigt)) {
      aktiv_tv_oversigt.show_tv_oversigt( tvvalgtrecordnr , tvsubvalgtrecordnr , do_update_xmltv_show);
      if (do_show_tv_kanal_info) {
        // do_zoom_tvprg_aktiv_nr=tvknapnr;
        if ((aktiv_tv_oversigt.vis_kanal_nr!=-1) && (aktiv_tv_oversigt.vis_program_nr!=-1)) {
          aktiv_tv_oversigt.showandsetprginfo();
        }
      }
    }

    // show all types views
    if (visur==false) {
      if (vis_stream_oversigt) {
        // printf("vis_stream_oversigt\n");
        streamoversigt.show_stream_oversigt(normal_icon, 0, 1);
      }
      if (vis_radio_oversigt) {
        radiooversigt.show_radio_oversigt( _textureId_dir , 0 , _textureIdback , onlineradio320 , 0);
      }
      // tidal stuf
      if (vis_tidal_oversigt) {
        if (do_show_tidal_search_oversigt == false) {
          tidal_oversigt.set_textureloaded(false);
          tidal_oversigt.show_tidal_oversigt( normal_icon , _textureId_song , _textureIdback , _textureIdback , tidal_selected_startofset , tidalknapnr );
        } else {
          tidal_oversigt.show_tidal_search_oversigt(_textureId_dir, _textureId_song, _textureIdback, _textureIdback, tidal_selected_startofset, tidalknapnr, keybuffer1);
        }
      }
      if (vis_music_oversigt) {    
        if (do_show_music_search_oversigt == false) {
          musicoversigt.show_music_oversigt(_textureId_dir,_textureIdback,_textureId28,_mangley,music_key_selected);
          // if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
        } else {
          musicoversigt.show_search_music_oversigt1(_textureId_dir,_textureIdback,_textureId28,_mangley,music_key_selected);
          // if (debugmode & 1) cout << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
        }
      } else if (vis_film_oversigt) {
        if (do_show_movie_search_oversigt==false) {
          film_oversigt.show_film_oversigt(0,film_select_iconnr);
        } else {
          film_oversigt.show_film_search_oversigt(0,film_select_iconnr);
        }
      }

      if (vis_recorded_oversigt) {
        recorded_oversigt.show_recorded_oversigt(valgtrecordnr,subvalgtrecordnr);
      }

      // music select window for play or swap (id 20 play/22 swap)
      if ((vis_music_oversigt) && (!(visur)) && (ask_open_dir_or_play_music) && (mknapnr>0)) {
        if (dirmusic.numbersinlist()>0) {
          // window ask play ?
          renderer.AddTextureRect(0,_textureId9_askbox, 500 , 200, 800, 600,1,1,1,1);
          // play button
          renderer.AddTextureRect(20,_textureIdplayicon, 520 , 680, 100, 100,1,1,1,1);
          // 21 = close
          renderer.AddTextureRect(22,_textureswap, 520+110 , 680, 100, 100,1,1,1,1);
          std::string temprgtxt;
          temprgtxt = fmt::format("Nr of songs : {:<8} {:<20}",dirmusic.numbersinlist(),musicoversigt.get_album_name(mknapnr-1));
          renderer.AddText(&myfont, 500 + 20 , 200 + 40 ,temprgtxt,1,1,1,1);
          // music cover
          GLuint cc_cover=musicoversigt.get_textureId(mknapnr-1);
          if (cc_cover==0) cc_cover=0;
          renderer.AddTextureRect(20,cc_cover , 1040 , 250, 200, 200,1,1,1,1);
          // show song list
          int i=0;
          int dirmusiclistemax=20;
          while (((unsigned int) i<(unsigned int) dirmusic.numbersinlist()) && ((unsigned int) i<(unsigned int) dirmusiclistemax)) {	// er der nogle sange navne som skal vises
            char songname_path[256];
            std::string filepath1;
            bool aktiv;
            dirmusic.popsong(songname_path,&aktiv,i+do_show_play_open_select_line_ofset);
            filepath1=songname_path;
            std::string show_filename = filepath1.substr(filepath1.find_last_of("/\\") + 1);
            if (aktiv==true) 
              renderer.AddText(&myfont, 500 + 30 , 300 + 20 + (i*18) ,"[X]",1,1,1,1);
            else 
              renderer.AddText(&myfont, 500 + 30 , 300 + 20 + (i*18) ,"[ ]",1,1,1,1);
            if (i==do_show_play_open_select_line+do_show_play_open_select_line_ofset) renderer.AddText(&myfont, 500 + 60 , 300 + 20 + (i*18) ,show_filename,0,1,0,1);
            else renderer.AddText(&myfont, 500 + 60 , 300 + 20 + (i*18) ,show_filename,1,1,1,1);
            i++;
          }
        }
      }


      //
      // music ask play or open playlist
      //
      // if ((vis_music_oversigt) && (!(visur)) && (do_zoom_music_cover==false) && (ask_open_dir_or_play_music) && (mknapnr>0)) {
        // renderer.AddTextureRect(20,spotify_askplay, 550, 400, 551, 328,1,1,1,1);
      // }



      // ******************************************************************************************************************
      //
      // show movie playing. (play to texture)
      //
      // ******************************************************************************************************************

      
      if (film_oversigt.film_is_playing) {
        film_oversigt.show_vlc_frame();
        if (aktiv_tv_oversigt.vis_tv_guide) {
          aktiv_tv_oversigt.fade=0.5f;
          aktiv_tv_oversigt.show_tv_oversigt( tvvalgtrecordnr , tvsubvalgtrecordnr , do_update_xmltv_show);
        } else aktiv_tv_oversigt.fade=0.8f;
      }
      

      // ********************** Stream stuf *******************************************************************************
      //
      // show stream player control
      //
      // ******************************************************************************************************************
      if ((vis_stream_oversigt) && (do_zoom_stream_cover)) {
        // renderer.AddTextureRect(0,_texturemusicplayer, config_menu.config_mediaplayer_infox,config_menu.config_mediaplayer_infoy, config_menu.config_mediaplayer_sizx, config_menu.config_mediaplayer_sizy,1,1,1,1);
        renderer.AddTextureRect(0,_texturemovieinfobox, config_menu.config_movieplayer_infox,config_menu.config_movieplayer_infoy, config_menu.config_movieplayer_sizx, config_menu.config_movieplayer_sizy,1,1,1,1);
        // play button
        renderer.AddTextureRect(PLAYBUTTON,_texturemplay, 400+20     , 830, 120, 120,1,1,1,1);
        // stop button
        renderer.AddTextureRect(STOPBUTTON,_texturemstop, 400+20+100 , 830, 120, 120,1,1,1,1);
        renderer.AddText(&myfont,450,26*18,"Name ",1,1,1,1);
        std::string tmptext;
        // show name
        tmptext=streamoversigt.get_stream_name(sknapnr-1);
        renderer.AddText(&myfont,450+120,26*18,tmptext,1,1,1,1);
        // show desc
        renderer.AddText(&myfont,450,26*19,"Desc ",1,1,1,1);
        tmptext=streamoversigt.FeedCatalog[sknapnr-1].feed_desc;
        renderer.AddText(&myfont,450+120,26*19,tmptext,1,1,1,1);
        // show texture
        if (streamoversigt.FeedCatalog[sknapnr-1].textureId) {
          renderer.AddTextureRect(0,streamoversigt.FeedCatalog[sknapnr-1].textureId, config_menu.config_movieplayer_infox+540,config_menu.config_movieplayer_infoy+300, 220, 220,1,1,1,1);
        }
        // show status
        int playstatsus=streamoversigt.get_play_status();
        std::string txtplaystatus;
        switch (playstatsus) {
          case 1: txtplaystatus="Open stream";
                  break;
          case 2:txtplaystatus="Buffing";
                  break;
          case 3:txtplaystatus="Playing";
                  break;
          case 4:txtplaystatus="Pause";
                  break;
          case 5:txtplaystatus="Stop";
                  break;
          case 6:txtplaystatus="Stream ended";
                  break;
          default:
                  txtplaystatus="Other error";
                  break;
        }
        renderer.AddText(&myfont,450,26*20,"Status",1,1,1,1);
        renderer.AddText(&myfont,450+120,26*20,txtplaystatus,1,1,1,1);
        // if status playing
        if (playstatsus==3) {
          long length_in_ms;
          int intotalsec;
          int inhour;
          int inmin;
          int insec;
          long play_length_in_ms;
          int play_intotalsec;
          int play_inhour;
          int play_inmin;
          int play_insec;
          static time_t lastUpdate = 0;
          time_t now = time(nullptr);
          if (now != lastUpdate) {
            lastUpdate=now;
            length_in_ms=streamoversigt.get_length_in_ms();
            intotalsec=(length_in_ms/1000);
            inhour=(intotalsec/3600);
            inmin=(intotalsec % 3600) / 60;
            insec=(intotalsec % 60);

            play_length_in_ms=streamoversigt.get_position_in_ms();
            play_intotalsec=(play_length_in_ms/1000);
            play_inhour=(play_intotalsec/3600);
            play_inmin=(play_intotalsec % 3600) / 60;
            play_insec=(play_intotalsec % 60);
          }
          renderer.AddText(&myfont,450,26*21,"Playlength",1,1,1,1);
          std::string play_intime=fmt::format("{:02}:{:02}:{:02}/{:02}:{:02}:{:02}",play_inhour,play_inmin,play_insec,inhour,inmin,insec);
          renderer.AddText(&myfont,450+120,26*21,play_intime,1,1,1,1);
        }
      }
    }


    if (vis_tv_oversigt) {
      // if (do_zoom_tv_cover) {
        // renderer.AddTextureRect(0,_texturemovieinfobox, config_menu.config_movieplayer_infox,config_menu.config_movieplayer_infoy, config_menu.config_movieplayer_sizx, config_menu.config
      // }
    }


    if ((vis_music_oversigt) || (vis_stream_oversigt) || (vis_tidal_oversigt) || (vis_spotify_oversigt) || (vis_film_oversigt) || (vis_recorded_oversigt) || (vis_tv_oversigt) || (vis_radio_or_music_oversigt) || (vis_stream_or_movie_oversigt)) {
      show_newmovietimeout = 0;
      vis_nyefilm_oversigt = false;
    }
    //
    // show oversigt over new movies before timeout if not other stuf is started.
    //
    if ((vis_nyefilm_oversigt) && (film_oversigt.film_is_playing==false) && (do_show_setup == false) && (vis_spotify_oversigt == false) && (vis_tidal_oversigt == false) && (vis_music_oversigt == false) && (vis_stream_oversigt == false) && (vis_film_oversigt == false) && (vis_recorded_oversigt == false) && (vis_tv_oversigt == false) && (vis_radio_oversigt == false) && (vis_stream_oversigt == false)) {
      if (show_newmovietimeout == 0) vis_nyefilm_oversigt = false;
      if (fknapnr == 0) show_newmovietimeout--;
      film_oversigt.show_minifilm_oversigt(0,0);
    }



    // ******************************************************************************************************************
    //
    // ** show movie info **
    //
    // ******************************************************************************************************************
    
    if (((vis_film_oversigt) || (vis_nyefilm_oversigt)) && (do_zoom_film_cover) && (fknapnr>0) && (!(visur))) {
      int configland=0;
      float strlength;
      do_zoom_film_aktiv_nr=fknapnr-1;
      // draw window
      renderer.AddTextureRect(0,_texturemovieinfobox, config_menu.config_movieplayer_infox,config_menu.config_movieplayer_infoy, config_menu.config_movieplayer_sizx, config_menu.config_movieplayer_sizy,1,1,1,1);
      // play button
      renderer.AddTextureRect(PLAYBUTTON,_texturemplay, 400+20     , 830, 120, 120,1,1,1,1);
      // stop button
      renderer.AddTextureRect(STOPBUTTON,_texturemstop, 400+20+100 , 830, 120, 120,1,1,1,1);
      // back button
      renderer.AddTextureRect(10,_texturemlast,         400+20+200 , 830, 120, 120,1,1,1,1);
      // ff button
      renderer.AddTextureRect(11,_texturemnext,         400+20+300 , 830, 120, 120,1,1,1,1);
      // cover
      if (film_oversigt.editmode==2) {
        renderer.AddTextureRect(31,_defaultdvdcover, 896 , 600, 220+40, 320,1,1,1,1);
      } else {
        renderer.AddTextureRect(0,_defaultdvdcover, 896 , 600, 220+40, 320,1,1,1,1);
      }
      // update button
      if (film_oversigt.editmode==2) {
        renderer.AddTextureRect(32,_textureupdatetidalview, 980 , 430, 188, 81,1,1,1,1);
      }
      // Show movie cover
      textureId=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfronttextureid();
      if (textureId==0) textureId=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].gettextureid();
      if (textureId) {
        if (film_oversigt.editmode==2) {
          // cover
          renderer.AddTextureRect(31,textureId,config_menu.config_movieplayer_coverx,config_menu.config_movieplayer_covery,config_menu.config_movieplayer_cover_sizx,config_menu.config_movieplayer_cover_sizy-2,1,1,1,1);
          // mask
          renderer.AddTextureRect(31,_defaultdvdcover_mask,config_menu.config_movieplayer_coverx,config_menu.config_movieplayer_covery,config_menu.config_movieplayer_cover_sizx,config_menu.config_movieplayer_cover_sizy-2,1,1,1,1);
        } else {
          // cover
          renderer.AddTextureRect(0,textureId,config_menu.config_movieplayer_coverx,config_menu.config_movieplayer_covery,config_menu.config_movieplayer_cover_sizx,config_menu.config_movieplayer_cover_sizy-2,1,1,1,1);
          // mask
          renderer.AddTextureRect(0,_defaultdvdcover_mask,config_menu.config_movieplayer_coverx,config_menu.config_movieplayer_covery,config_menu.config_movieplayer_cover_sizx,config_menu.config_movieplayer_cover_sizy-2,1,1,1,1);
        }
      }

      renderer.AddText(&myfont,450,26*18,movie_genre[configland],1,1,1,1);
      if (film_oversigt.editmode==2) {
        renderer.AddText(&myfont,450 + 120 ,26*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].genre,1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].genre);
        if (do_show_film_edit_select_linie==0) showcoursornow(450 + 120 + strlength,25*18,0);
      } else {
        renderer.AddText(&myfont,450 + 120 ,26*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].genre,1,1,1,1);
      }


      renderer.AddText(&myfont,450,27*18,"Title",1,1,1,1);
      if (film_oversigt.editmode==2) {
        renderer.AddText(&myfont,450 + 120 ,27*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle(),1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle());
        if (do_show_film_edit_select_linie==1) showcoursornow(450 + 120 + strlength,26*18,0);
      } else {
        renderer.AddText(&myfont,450 + 120 ,27*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle(),1,1,1,1);
      }

      renderer.AddText(&myfont,450,28*18,"Length",1,1,1,1);
      if (film_oversigt.editmode==2) {
        temptxt2 = fmt::format("{}",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmlength()));
        renderer.AddText(&myfont,450 + 120 ,28*18,temptxt2,1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, temptxt2);
        if (do_show_film_edit_select_linie==2) showcoursornow(450 + 120 + strlength,27*18,0);
      } else {
        temptxt2 = fmt::format("{}",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmlength()));
        renderer.AddText(&myfont,450 + 120 ,28*18,temptxt2,1,1,1,1);
      }

      renderer.AddText(&myfont,450,29*18,"Year",1,1,1,1);
      if (film_oversigt.editmode==2) {
        temptxt2 = fmt::format("{}",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmaar()));
        renderer.AddText(&myfont,450 + 120 ,29*18,temptxt2,1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, temptxt2);
        if (do_show_film_edit_select_linie==3) showcoursornow(450 + 120 + strlength,28*18,0);
      } else {
        if (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmaar()) sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmaar());
          else strcpy(temptxt,"NONE");
        renderer.AddText(&myfont,450 + 120 ,29*18,temptxt,1,1,1,1);
      }

      renderer.AddText(&myfont,450,30*18,"Rating",1,1,1,1);
      if (film_oversigt.editmode==2) {
        if (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating()) sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating());
          else strcpy(temptxt,"NONE");
        renderer.AddText(&myfont,450 + 120 ,30*18,temptxt,1,1,1,1);
        strlength=strlen(temptxt);
        if (do_show_film_edit_select_linie==4) showcoursornow(450 + 120 + strlength,29*18,0);
      } else {
        if (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating()) sprintf(temptxt,"%d ",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating());
          else strcpy(temptxt,"NONE");
        renderer.AddText(&myfont,450 + 120 ,30*18,temptxt,1,1,1,1);
      }

      renderer.AddText(&myfont,450,31*18,"Format",1,1,1,1);
      if (film_oversigt.editmode==2) {
        renderer.AddText(&myfont,450 + 120 ,31*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getFormat(),1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getFormat());
        if (do_show_film_edit_select_linie==5) showcoursornow(450 + 120 + strlength,30*18,0);
      } else {
        renderer.AddText(&myfont,450 + 120 ,31*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getFormat(),1,1,1,1);
      }


      renderer.AddText(&myfont,450,32*18,"W/H",1,1,1,1);
      if (film_oversigt.editmode==2) {
        temptxt2 = fmt::format("{}/{}",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getWidth(),film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getHigh());
        renderer.AddText(&myfont,450 + 120 ,32*18,temptxt2,1,1,1,1);
        if (do_show_film_edit_select_linie==6) showcoursornow(450 + 120 + strlength,31*18,0);
      } else {
        temptxt2 = fmt::format("{}/{}",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getWidth(),film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getHigh());
        renderer.AddText(&myfont,450 + 120 ,32*18,temptxt2,1,1,1,1);
      }

      renderer.AddText(&myfont,450,33*18,"Size",1,1,1,1);
      if (film_oversigt.editmode==2) {
        if ((film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024/1024)>1) 
          temptxt2 = fmt::format("{} Gb",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024/1024));
        else
          temptxt2 = fmt::format("{} Mb",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024));
        renderer.AddText(&myfont,450 + 120 ,33*18,temptxt2,1,1,1,1);
        strlength=renderer.GetTextWidth(&myfont, temptxt2);
        if (do_show_film_edit_select_linie==7) showcoursornow(450 + 120 + strlength,32*18,0);
      } else {
        if ((film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024/1024)>1) 
          temptxt2 = fmt::format("{} Gb",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024/1024));
        else
          temptxt2 = fmt::format("{} Mb",(int ) (film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getSize()/1024/1024));
        renderer.AddText(&myfont,450 + 120 ,33*18,temptxt2,1,1,1,1);
      }

      renderer.AddText(&myfont,450,34*18,"IMDB",1,1,1,1);
      if (film_oversigt.editmode==2) {
        strlength=0;
        if (do_show_film_edit_select_linie==8) showcoursornow(450 + 120 + strlength,33*18,0);
      } else {

      }
      renderer.AddText(&myfont,450,35*18,"Cast",1,1,1,1);
      renderer.AddText(&myfont,450 + 120 ,35*18,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].cast[0],1,1,1,1);
      renderer.AddText(&myfont2,450,38*18,"Description",1,1,1,1);

      if (film_oversigt.editmode==0) {
         renderer.AddText(&myfont,950, 580,"CTRL-E for edit mode.",1,1,1,1);
      }

      int sted=0;
      float linof=0.0f;
      int maxWidth=56;
      float subtitlelength=strlen(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].film_subtitle); // get description length
      bool show_desc_no_editor=false;
      if (film_oversigt.editmode==2) {
        if (do_show_film_edit_select_linie>=9) {
          multi_editor=true;
          textEditor.SetActive(true);
          static bool set_edit_text=false;
          // first time set text to editor
          if (set_edit_text==false) {
            textEditor.SetText(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].film_subtitle);
            set_edit_text=true;
          }
          textEditor.Draw(renderer);
        } else show_desc_no_editor=true;
      } else show_desc_no_editor=true;
      if (show_desc_no_editor) {
        // textEditor.update_movie_desc(do_zoom_film_aktiv_nr,textEditor.GetText());
        const std::string& subtitle = film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].film_subtitle;
        std::istringstream stream(subtitle);
        std::string word;
        std::string line;
        float x = 450.0f;
        float y = ((39) * 18.0f)+10.0f;
        float lineHeight = 18.0f;
        float linof = 0.0f;
        while ((stream >> word) && (linof < (lineHeight * 7))) {
          std::string testLine = line;
          if (!testLine.empty())
            testLine += ' ';
          testLine += word;
          if (!line.empty() && testLine.length() > maxWidth) {
            renderer.AddText(&myfont, x,y + linof, line, 1, 1, 1, 1);
            linof += lineHeight;
            line = word;
          } else {
            line = testLine;
          }
        }
        // Tegn den sidste linje
        if (!line.empty() && linof < 60.0f) {
            renderer.AddText(&myfont,x,y + linof,line,1, 1, 1, 1);
        }
      }  
    }
    
    // show movie status info over playing movie if use of pil keys
    static GLuint menubartexture=0;
    float fader=film_oversigt.film_fader;
    if ((film_oversigt.film_is_playing) && (do_show_film_status_info)) {
      static bool firsttime=true;
      static bool startfader=false;
      static std::chrono::steady_clock::time_point start,nextTime,now;
      if (firsttime) {
          start = std::chrono::steady_clock::now() + std::chrono::seconds(3);
          nextTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
          firsttime = false;
      }
      now = std::chrono::steady_clock::now();
      if (now >= start) {
          startfader = true;
      }
      if (startfader) {
        if (now >= nextTime) {
          nextTime = now + std::chrono::milliseconds(20);
          if (fader>0.0f) film_oversigt.film_fader=film_oversigt.film_fader-0.02f;
        }
        if (fader<=0.0f) {
          do_show_film_status_info=false;
          startfader=false;
          firsttime=true;
        }
      }
      // show movie status info over playing movie
      // cover icon in corner of playing movie
      renderer.AddTextureRect(0,textureId, 10, 10, 200, 200,fader,fader,fader,fader);
      int length_sec=film_oversigt.get_movie_length_ms()/1000;
      int hlength_sec=length_sec;
      int fpos=film_oversigt.get_movie_pos()/1000;
      if (menubartexture==0) menubartexture=loadTexture((char *) "/opt/mythtv-controller/tema2/images/menubar.png");
      renderer.AddTextureRect(0,menubartexture, 312, 986, 1296, 92,fader,fader,fader,fader);
      // movie title
      std::string mtitle=fmt::format("Title {}", film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle());
      renderer.AddText(&myfont,312+4, 986+(18*1), mtitle,fader,fader,fader,fader);

      // # of sub titles
      std::string antal_sub_titles=fmt::format("Sub Titles {}", film_oversigt.antal_sub_tracks());
      renderer.AddText(&myfont,312+4, 986+(18*2), antal_sub_titles,fader,fader,fader,fader);

      // name of audio track
      std::string audiotrack_name=fmt::format("Audio track {}", film_oversigt.active_audiotrack_name());
      renderer.AddText(&myfont,812+4, 986+(18*1), audiotrack_name,fader,fader,fader,fader);

      // time info
      int hours   = fpos / 3600;
      int minutes = (fpos % 3600) / 60;
      int seconds = fpos % 60;
      std::string time_running=fmt::format("{:02}:{:02}:{:02}", hours, minutes, seconds);
      renderer.AddText(&myfont,478, 986+(18*4), time_running,fader,fader,fader,fader);
      length_sec=length_sec-fpos;
      int lhours   = length_sec / 3600;
      int lminutes = (length_sec % 3600) / 60;
      int lseconds = length_sec % 60;
      int barwidth=0;
      float procent=0.0f;
      time_running=fmt::format(" -{:02}:{:02}:{:02} ", lhours, lminutes, lseconds);
      if (length_sec > 0.0f) {
        procent = std::clamp((static_cast<float>(fpos) / hlength_sec) * 100.0f, 0.0f,100.0f);
        barwidth = (float) (procent * 6.40f); // 12.96 pixels per percent
      }
      renderer.AddText(&myfont,1190, 986+(18*4), time_running,fader,fader,fader,fader);
      renderer.AddTextureRect(0,0, 548, 1042, barwidth, 26,fader,fader,fader,fader);
    }
    
    // start play movie
    if ((startmovie) && (do_zoom_film_cover)) {
      if ((sound) && (snd)) {
        // stop any sound playing
        result=channel->stop();                         // stop fmod player
        // release any sound system again
        result=sound->release();
        // stop uv meters
        dsp=0;
        snd=0;
        sound = 0;
      }
      do_zoom_film_cover=true;
      do_zoom_music_cover=false;
      do_zoom_radio_cover=false;
      do_zoom_tidal_cover=false;
      do_zoom_spotify_cover=false; 
      // start play movie
      movie_play_status=film_oversigt.playmovie(fknapnr-1);
      if (movie_play_status==1) {
        do_zoom_film_cover=false;
      } else {
        if (!(film_oversigt.libvlc_player_play())) {
          movie_play_status=-1;
        }
      }
      startmovie=false;                       //  set play flag done
    }

    static time_t status_time_out_last=0;
    static time_t status_time_out=0;
    static bool show_play_status_info=false;
    if (movie_play_status==-1) {
      status_time_out = time(nullptr);
      if (status_time_out_last==0) {
        status_time_out_last=status_time_out+3;
      }
      printf("time %d \n",status_time_out);
      if ((status_time_out_last>0) && (status_time_out_last>status_time_out)) {
        show_play_status_info=true;
      } else {
        show_play_status_info=false;
      }
      if (show_play_status_info) {
        printf("Error start play movie \n");
        renderer.AddTextureRect(20,torrent_background, 600, 400, 551, 328,1,1,1,1);
        renderer.AddText(&myfont2, 750, 550  ,"Error start movie",1,1,1,1);
      }
    }
    
    // get subtitles after movie start (check if playing)
    if (film_oversigt.libvlc_player_play()) {
      if ((film_oversigt.film_is_playing) && (film_oversigt.getsubs_timer==200)) {
        film_oversigt.GetSubtitleTracks();
        film_oversigt.GetAudioTracks();
        film_oversigt.getsubs_timer=0;
        // film_oversigt.SelectSubtitle("Danish");
      } else if (film_oversigt.getsubs_timer>0) film_oversigt.getsubs_timer++;
    }

    // stop movie
    if (stopmovie) {
      // stop movie playing
      film_oversigt.stopmovie();
      do_zoom_film_cover=false;
      stopmovie = false;    
    }


    // ******************************************************************************************************************
    //
    // Spotify show play info
    //
    // ******************************************************************************************************************
    if (vis_spotify_oversigt) {
      if (do_show_spotify_search_oversigt==false) {
        spotify_oversigt.show_spotify_oversigt( _textureId_dir , _textureId_song , _textureIdback , _textureIdback , spotify_selected_startofset , spotifyknapnr );
      } else {
        // spotify_oversigt.show_spotify_search_oversigt( onlineradio , _textureId_song , _textureId_dir , _textureIdback , spotify_selected_startofset , spotifyknapnr ,keybuffer);
      }
      if (strcmp(spotify_oversigt.spotify_get_token(),"")==0) {        
        if (startwebbrowser) {
          write_logfile(logfile,(char *) "start webbrowser to login on spotify.");
          // start webbroser to login on spotify
          do_system_call("firefox localhost:8000");
          startwebbrowser=false;
        }
      }
      // 7 ms is my timer
      // select play device
      if (do_select_device_to_play) {
        spotify_oversigt.select_device_to_play();
      }
    }

    // ******************************************************************************************************************
    //
    // ** show music/radio play status window **
    //
    // show player info for all music types (fmod files)
    //
    // ******************************************************************************************************************
    if ((visur==false) && ((do_zoom_tidal_cover) || (do_zoom_spotify_cover) || (do_zoom_music_cover) || (do_zoom_radio_cover))) {
      if (snd) {
        // background
        if (do_zoom_tidal_cover) {
          renderer.AddTextureRect(0,_texturetidalplayer, config_menu.config_tidalplayer_infox, config_menu.config_tidalplayer_infoy, config_menu.config_tidalplayer_sizx, config_menu.config_tidalplayer_sizy,1,1,1,1);
        } else if (do_zoom_spotify_cover) {
          renderer.AddTextureRect(0,_texturetidalplayer, config_menu.config_radioplayer_infox, config_menu.config_radioplayer_infoy, config_menu.config_radioplayer_sizx, config_menu.config_radioplayer_sizy,1,1,1,1);
        } else if (do_zoom_radio_cover) {
          renderer.AddTextureRect(0,_texturetidalplayer, config_menu.config_radioplayer_infox, config_menu.config_radioplayer_infoy, config_menu.config_radioplayer_sizx, config_menu.config_radioplayer_sizy,1,1,1,1);
        } else if (do_zoom_music_cover) {
          renderer.AddTextureRect(0,_texturetidalplayer, config_menu.config_radioplayer_infox, config_menu.config_radioplayer_infoy, config_menu.config_radioplayer_sizx, config_menu.config_radioplayer_sizy,1,1,1,1);
        } else {
          renderer.AddTextureRect(0,_texturetidalplayer, config_menu.config_radioplayer_infox, config_menu.config_radioplayer_infoy, config_menu.config_radioplayer_sizx, config_menu.config_radioplayer_sizy,1,1,1,1);
        }
        // buttons
        // play
        if (do_zoom_tidal_cover) {
          renderer.AddTextureRect(8,_texturemplay, config_menu.config_tidalplayer_infox+config_menu.config_tidalplayer_play_button_posx,config_menu.config_tidalplayer_play_button_posy, config_menu.config_tidalplayer_play_button_sizx, config_menu.config_tidalplayer_play_button_sizy,1,1,1,1);
          renderer.AddTextureRect(9,_texturemstop, config_menu.config_tidalplayer_infox+config_menu.config_tidalplayer_stop_button_posx,config_menu.config_tidalplayer_stop_button_posy, config_menu.config_tidalplayer_stop_button_sizx, config_menu.config_tidalplayer_stop_button_sizy,1,1,1,1);
          renderer.AddTextureRect(10,_texturemlast, config_menu.config_tidalplayer_infox+config_menu.config_tidalplayer_ff_button_posx,config_menu.config_tidalplayer_ff_button_posy, config_menu.config_tidalplayer_ff_button_sizx, config_menu.config_tidalplayer_ff_button_sizy,1,1,1,1);               
          renderer.AddTextureRect(11,_texturemnext, config_menu.config_tidalplayer_infox+config_menu.config_tidalplayer_bw_button_posx,config_menu.config_tidalplayer_bw_button_posy, config_menu.config_tidalplayer_bw_button_sizx, config_menu.config_tidalplayer_bw_button_sizy,1,1,1,1);
        } else if (do_zoom_radio_cover) {
          // play button + stop button
          renderer.AddTextureRect(8,_texturemplay, config_menu.config_radioplayer_infox+config_menu.config_radioplayer_play_button_posx,config_menu.config_radioplayer_play_button_posy, config_menu.config_radioplayer_play_button_sizx, config_menu.config_radioplayer_play_button_sizy,1,1,1,1);
          renderer.AddTextureRect(9,_texturemstop, config_menu.config_radioplayer_infox+config_menu.config_radioplayer_stop_button_posx,config_menu.config_radioplayer_stop_button_posy, config_menu.config_radioplayer_stop_button_sizx, config_menu.config_radioplayer_stop_button_sizy,1,1,1,1);
        } else if (do_zoom_music_cover) {
          // play button + stop + fw + back button
          renderer.AddTextureRect(8,_texturemplay, config_menu.config_musicplayer_infox+config_menu.config_musicplayer_play_button_posx,config_menu.config_musicplayer_play_button_posy, config_menu.config_musicplayer_play_button_sizx, config_menu.config_musicplayer_play_button_sizy,1,1,1,1);
          renderer.AddTextureRect(9,_texturemstop, config_menu.config_musicplayer_infox+config_menu.config_musicplayer_stop_button_posx,config_menu.config_musicplayer_stop_button_posy, config_menu.config_musicplayer_stop_button_sizx, config_menu.config_musicplayer_stop_button_sizy,1,1,1,1);
          renderer.AddTextureRect(10,_texturemlast, config_menu.config_musicplayer_infox+config_menu.config_musicplayer_ff_button_posx,config_menu.config_musicplayer_ff_button_posy, config_menu.config_musicplayer_ff_button_sizx, config_menu.config_musicplayer_ff_button_sizy,1,1,1,1);        
          renderer.AddTextureRect(11,_texturemnext, config_menu.config_musicplayer_infox+config_menu.config_musicplayer_bw_button_posx,config_menu.config_musicplayer_bw_button_posy, config_menu.config_musicplayer_bw_button_sizx, config_menu.config_musicplayer_bw_button_sizy,1,1,1,1);
          // music icon
          GLuint tmptexture=aktiv_playlist.get_textureid(1);
          renderer.AddTextureRect(0,tmptexture,config_menu.config_musicplayer_infox+346, config_menu.config_musicplayer_infoy+57,168,120,1,1,1,1);
        } else if (do_zoom_spotify_cover) {
          renderer.AddTextureRect(8,_texturemplay, config_menu.config_spotifyplayer_infox+config_menu.config_radioplayer_play_button_posx,config_menu.config_spotifyplayer_play_button_posy, config_menu.config_spotifyplayer_play_button_sizx, config_menu.config_spotifyplayer_play_button_sizy,1,1,1,1);
          renderer.AddTextureRect(9,_texturemstop, config_menu.config_spotifyplayer_infox+config_menu.config_radioplayer_stop_button_posx,config_menu.config_spotifyplayer_stop_button_posy, config_menu.config_spotifyplayer_stop_button_sizx, config_menu.config_spotifyplayer_stop_button_sizy,1,1,1,1);
          renderer.AddTextureRect(10,_texturemlast, config_menu.config_spotifyplayer_infox+config_menu.config_radioplayer_ff_button_posx,config_menu.config_spotifyplayer_ff_button_posy, config_menu.config_spotifyplayer_ff_button_sizx, config_menu.config_spotifyplayer_ff_button_sizy,1,1,1,1);               
          renderer.AddTextureRect(11,_texturemnext, config_menu.config_spotifyplayer_infox+config_menu.config_radioplayer_bw_button_posx,config_menu.config_spotifyplayer_bw_button_posy, config_menu.config_spotifyplayer_bw_button_sizx, config_menu.config_spotifyplayer_bw_button_sizy,1,1,1,1);
        }
      }
      // show radio info
      if (radiooversigt.playing) {
        std::string temptxt;
        if (radiooversigt.playingstationnr>0) {
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(4*18) ,"Station ",1,1,1,1);
          temptxt=radiooversigt.get_station_name(radiooversigt.playingstationnr);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(4*18) ,temptxt,1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(5*18) ,"Song Name ",1,1,1,1);
          if ((channel) && (sound) && (snd)) {
            result = sound->getNumTags(&numtags, &numtagsupdated); 
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
                }
              }
            }
          }
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(5*18) ,aktivsongname,1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(6*18) ,"Artist ",1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(6*18) ,aktivartistname,1,1,1,1);
          
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(8*18) ,"Bit rate ",1,1,1,1);
          int bbs=radiooversigt.get_kbps(radiooversigt.playingstationnr);
          std::string showbb=fmt::format("{} Kps.",bbs);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(8*18) ,showbb,1,1,1,1);

          // Radio cover
          GLuint radiotexture_2;
          radiotexture_2=radiooversigt.get_texture(radiooversigt.playingstationnr-1);
          if (radiotexture_2) {
            renderer.AddTextureRect(0,radiotexture_2, config_menu.config_radioplayer_infox+346,config_menu.config_radioplayer_play_button_posy-114, 168, 120,1,1,1,1);
          } else {
            renderer.AddTextureRect(0,normal_icon, config_menu.config_radioplayer_infox+346,config_menu.config_radioplayer_play_button_posy-114, 168, 120,1,1,1,1);
          }
          result=channel->getPosition(&ms, FMOD_TIMEUNIT_MS);		// get fmod audio info
          if ((result == FMOD_OK) && (ms>0)) {
            result=sound->getLength(&lenbytes,FMOD_TIMEUNIT_RAWBYTES);
            if ((result == FMOD_OK) && (ms>1000)) {
              radio_playtime=ms/1000;
              radio_playtime_hour=(radio_playtime/60)/60;
              radio_playtime_min=(radio_playtime/60);
              radio_playtime_sec=radio_playtime-(radio_playtime_min*60);
              radio_playtime_min=radio_playtime_min-(radio_playtime_hour*60);
              if (radio_playtime>0) {
                temptxt=fmt::format("{:02}:{:02}:{:02} ",radio_playtime_hour,radio_playtime_min,radio_playtime_sec);
                renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(9*18) ,"Play time ",1,1,1,1);
                renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(9*18) ,temptxt,1,1,1,1);
              }
            }
          }
        }
      }

      // Tidal show play stuf
      if (do_zoom_tidal_cover) {
        if (tidal_oversigt.antal_in_playlist()>0) {
          renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+20, config_menu.config_tidalplayer_infoy+(4*18) ,"Artist",1,1,1,1);
          if (tidal_oversigt.get_tidal_artistname(tidalknapnr)) renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+120, config_menu.config_tidalplayer_infoy+(4*18) ,tidal_oversigt.get_tidal_artistname(tidalknapnr),1,1,1,1);

          renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+20, config_menu.config_tidalplayer_infoy+(5*18) ,"Songname",1,1,1,1);
          if (tidal_oversigt.tidal_aktiv_song_name()) renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+120, config_menu.config_tidalplayer_infoy+(5*18) ,tidal_oversigt.tidal_aktiv_song_name(),1,1,1,1);

          sprintf(temptxt,"%d/%d",tidal_oversigt.get_aktiv_played_song()+1,tidal_oversigt.total_aktiv_songs()+1);
          renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+20, config_menu.config_tidalplayer_infoy+(6*18) ,"song ",1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+120, config_menu.config_tidalplayer_infoy+(6*18) ,temptxt,1,1,1,1);

          result=channel->getPosition(&ms, FMOD_TIMEUNIT_MS);		// get fmod audio info
          if ((result == FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
            result=sound->getLength(&playtime_songlength,FMOD_TIMEUNIT_MS);
            result=sound->getLength(&lenbytes,FMOD_TIMEUNIT_RAWBYTES);
            if ((playtime_songlength>0) && (result==FMOD_OK)) {
             kbps = (lenbytes/(playtime_songlength/1000)*8)/1000;			// calc bit rate
            }
            y=ms/1000;
            ll=playtime_songlength/1000;
            // draw length metter
          }
          renderer.AddText(&myfont,config_menu.config_tidalplayer_infox+20, config_menu.config_tidalplayer_infoy+(7*18) ,"playtime ",1,1,1,1);
          if (y>0) {
            xxx = ((y/ll)*12);
          } else xxx=0;
          for(int x=0;x<xxx;x++) {
            renderer.AddTextureRect(0,setuptorrent_background, config_menu.config_tidalplayer_infox+120+(x*12), config_menu.config_tidalplayer_infoy+6+(6*18),16,16,1,1,1,1);
          }
          // show bitrate
          sprintf(temptxt,"%5.0f/%d Kbits",frequency,kbps);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+20, config_menu.config_radioplayer_infoy+(8*18) ,"Samplerate",1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_radioplayer_infox+120, config_menu.config_radioplayer_infoy+(8*18) ,temptxt,1,1,1,1);
        }
      }
      
      // music stuf
      if ((snd) && (musicoversigt.get_music_is_playing()) && (do_zoom_music_cover)) {
        // play position
        result=channel->getPosition(&ms, FMOD_TIMEUNIT_MS);		// get fmod audio info
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
          ERRCHECK(result,0);
        }
        // get play length new version
        result=sound->getLength(&rplaytime_songlength,FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
          ERRCHECK(result,do_play_music_aktiv_table_nr);
        }
        result=sound->getLength(&lenbytes,FMOD_TIMEUNIT_RAWBYTES);
        if (result!=FMOD_OK) {
          ERRCHECK(result,0);
        }
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(4*18) ,"Artist",1,1,1,1);
        aktiv_playlist.get_artistname(temptxt,do_play_music_aktiv_table_nr-1);
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(4*18) ,temptxt,1,1,1,1);

        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(5*18) ,"Song name",1,1,1,1);
        aktiv_playlist.get_songname(temptxt,do_play_music_aktiv_table_nr-1);
        // remove path
        char *pos;
        pos=strrchr(temptxt,'/');
        if (strrchr(temptxt,'/')) {
          strcpy(temptxt1,pos+1);
          strcpy(temptxt,temptxt1);
        }
        pos=strrchr(temptxt,'.');
        if (strrchr(temptxt,'.')) {
          temptxt[pos-temptxt]='\0';
        }
        temptxt[40]=0;
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(5*18) ,temptxt,1,1,1,1);
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(6*18) ,"Album",1,1,1,1);
        aktiv_playlist.get_albumname(temptxt,do_play_music_aktiv_table_nr-1);     
        // remove path
        pos=strrchr(temptxt,'/');
        if (strrchr(temptxt,'/')) {
          strcpy(temptxt1,pos+1);
          strcpy(temptxt,temptxt1);
        }
        pos=strrchr(temptxt,'.');
        if (strrchr(temptxt,'.')) {
          temptxt[pos-temptxt]='\0';
        }
        temptxt[40]=0;
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(6*18) ,temptxt,1,1,1,1);
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(7*18) ,"Bit rate",1,1,1,1);
        // show playlist info # of tracks
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(9*18) ,"Songs  ",1,1,1,1);
        sprintf(temptxt,"%d/%d", do_play_music_aktiv_table_nr  , aktiv_playlist.numbers_in_playlist());
        renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(9*18) ,temptxt,1,1,1,1);
      }
      if (snd) {
        // play position
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
        unsigned int splaytime;
        unsigned int splaytime_songlength;
        // do the calc
        if (result==FMOD_OK) {
          splaytime=ms/1000;
        } else {
          splaytime_songlength=0;
          // radio_playtime
          splaytime=0;
        }
        if ((playtime_songlength>0) && (result==FMOD_OK)) {
          if ((lenbytes>0) && ((playtime_songlength/1000)>0))
            kbps = (lenbytes/(playtime_songlength/1000)*8)/1000;			// calc bit rate
          else
            kbps=0;
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
        playtime_hour = (playtime/60)/60;
        playtime_min = (playtime/60);
        playtime_sec = playtime-(playtime_min*60);
        playtime_min = playtime_min-(playtime_hour*60);
        channel->getFrequency(&frequency);
        // music
        if (do_zoom_music_cover) {
          std::string tt=std::to_string((int) frequency);
          renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(7*18) ,tt,1,1,1,1);
          renderer.AddText(&myfont,config_menu.config_musicplayer_infox+20, config_menu.config_musicplayer_infoy+(8*18) ,"Time",1,1,1,1);
          std::string tt2=fmt::format("{:02}:{:02}:{:02} ",playtime_hour,playtime_min,playtime_sec);
          renderer.AddText(&myfont,config_menu.config_musicplayer_infox+120, config_menu.config_musicplayer_infoy+(8*18) ,tt2,1,1,1,1);
        }
      }
    }
    // show loading
    if ((vis_tidal_oversigt) && (do_show_tidal_search_oversigt)) {
      if (tidal_oversigt_loaded_begin) {
        renderer.AddTextureRect(0,_texturetidalloading, config_menu.config_tidalplayer_infox+146,config_menu.config_tidalplayer_play_button_posy-124, 785/2, 486/2,1,1,1,1);
        // std::cout << " Loading tidal stuf" << "\n";
      }
    }


    // ******************************************************************************************************************
    //
    // ** show torrent status **
    //
    // ******************************************************************************************************************

    if (do_show_torrent) {
      torrent_downloader.show_torrent_oversigt(0,0);
      if (do_show_torrent_options) {
        torrent_downloader.show_torrent_options();
      }
      if (do_show_torrent_options_move) {
        torrent_downloader.show_move_options();
      }
      // show we move the file
      if (do_move_torrent_file_now) {
        torrent_downloader.show_file_move();
      }
      if (do_show_load__torrent_file) {
        torrent_downloader.select_file_name();
        do_show_load__torrent_file = false;
      }
    }


    // start play music
    if (do_play_music_cover) {
      musicoversigt.play_songs(true);                                   // set play flag in class
      radiooversigt.playing=false;                                      // stop radio playing flag
      tidal_oversigt.set_tidal_playing_flag(false);                     // stop tidal playing flag
      if (do_find_playlist) {
        do_find_playlist=false;                                     // sluk func igen
        do_play_music_cover=false;
        if (((do_zoom_music_cover==false) || (do_play_music_aktiv_play==0)) && (mknapnr!=0)) {
          // playliste funktion set start play
          fprintf(stderr,"Type af sange nr %d som skal loades %d\n ",mknapnr-1,musicoversigt.get_album_type(mknapnr-1));
          if (musicoversigt.get_album_type(mknapnr-1)==-1) {
            fprintf(stderr,"Loading songs from mythtv playlist: %4d %d\n",do_play_music_aktiv_nr,mknapnr);          
            write_logfile(logfile,(char *) "Loading songs from mythtv playlist.");
            if (hent_mythtv_playlist(musicoversigt.get_directory_id(mknapnr-1))==0) {		// tilføj musik valgte til playliste + load af covers
              fprintf(stderr,"**** PLAYLIST LOAD ERROR **** No songs. mythtv playlist id =%d\n",musicoversigt.get_directory_id(mknapnr-1));
              exit(2);        // STOP program
            }
            do_play_music_aktiv_table_nr = 1;						// sæt start play aktiv nr
          // normal cd cover browser funktion set start play
          } else if (musicoversigt.get_album_type(mknapnr-1)==0) {
            fprintf(stderr,"Loading songs from id:%4d \n",do_play_music_aktiv_nr);
            // reset valgt liste
            bool eraktiv;
            // clear last play list
            aktiv_playlist.clean_playlist();
            //
            // fill playlist array med song to play
            //
            for(int t=0;t<dirmusic.numbersinlist();t++) {
              dirmusic.popsong(temptxt1,&eraktiv,t);
              do_play_music_aktiv_nr_select_array[t]=eraktiv;
            }
            aktiv_playlist.m_add_playlist(temptxt1,temptxt1,(char *) "artist",(char *) "alb name",temptxt1,(char *) "artist name",(char *) "120",0,0);
            get_music_pick_playlist(do_play_music_aktiv_nr,do_play_music_aktiv_nr_select_array);	// tilføj musik valgt til aktiv play liste + load af cover

            if (do_play_music_aktiv_table_nr==0) {
              do_play_music_aktiv_table_nr = 1;			// sæt start play aktiv nr
            }
            fprintf(stderr,"Set aktiv playlist nr to start ved nr 1\n");
          }
          strcpy(configsoundoutport,"int");
          // startplaying
          if (do_play_music_aktiv_table_nr>0) {
            musicoversigt.set_music_is_playing(true);
            musicoversigt.playingmusicnr = musicoversigt.get_album_id(mknapnr-1);
            if (sound) result = sound->release();          		                            // stop last played sound on soundsystem fmod
            if (snd==0) {
                // aktiv_playlist = class to control music to play
                // (indenholder array til playliste samt hvor mange der er i playliste) 
                // aktivplay_music_path = string til den sang som skal spilles nu
                aktiv_playlist.m_play_playlist((char *) aktivplay_music_path,0);			// hent første sang i playlist
                printf("aktivplay_music_path = %s \n",(char *) aktivplay_music_path);
                if (strcmp(configsoundoutport,"STREAM")!=0) {
                  result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                  ERRCHECK(result,do_play_music_aktiv_table_nr);
                } else if (strcmp(configsoundoutport,"int")!=0) {
                  // load playlist
                  result = sndsystem->createSound(aktivplay_music_path, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);                
                } else {
                  sprintf(aktivplay_music_path,"http://%s/mythweb/music/stream?i=%d",configmysqlhost,aktiv_playlist.get_songid(do_play_music_aktiv_table_nr-1));
                  result = sndsystem->createSound(aktivplay_music_path,FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
                  ERRCHECK(result,do_play_music_aktiv_table_nr);
                }
                if (result==FMOD_OK) {
                  result = sndsystem->playSound( sound,NULL,false, &channel);
                  musicoversigt.set_music_is_playing(true);
                  ERRCHECK(result,do_play_music_aktiv_table_nr);
                  if (sndsystem) channel->setVolume(configsoundvolume);

                  do_zoom_music_cover=true;
                  ask_open_dir_or_play=false;
                  ask_open_dir_or_play_music=false;


                } else {
                  write_logfile(logfile,(char *) "Error loading song.");
                  printf("Error loading song %s \n",aktivplay_music_path);
                }
                do_stop_music_all=false;						// fjern stop musik bremse
                if (result==FMOD_OK) snd=1;
                dsp=0;                             // reset uv to start
            }
          }
        }
      }
    }
    
    
    // Tidal save playlist to db
    if (((vis_tidal_oversigt) || (do_show_tidal_search_oversigt)) && (save_ask_save_playlist)) {
      tidal_oversigt.save_tidal_oversigt_playlists(playlistfilename,tidalknapnr-1);
      save_ask_save_playlist=false;
      ask_save_playlist=false;
      // reset keyboard buffer
      // strcpy(keybuffer,"");
      strcpy(playlistfilename,"");
      keybufferindex=0;
    }
    

    //
    // tidal ask play or open playlist
    //
    if ((vis_tidal_oversigt) && (!(visur)) && (do_zoom_tidal_cover==false) && (ask_open_dir_or_play_tidal) && (tidalknapnr>0)) {
      renderer.AddTextureRect(20,spotify_askplay, 550, 400, 551, 328,1,1,1,1);
    }

    // first show status window
    if ((tidal_oversigt.tidal_stop_loader_thread==false) && (tidal_oversigt.startplay) && (tidal_start_delay<5) && (!(visur))) {
        renderer.AddTextureRect(0,_texturetidalloading, 550, 400, 551, 328,1,1,1,1);
    }
    if (tidal_oversigt.startplay) {
      tidal_start_delay++;
    }

    if ((tidal_oversigt.tidal_stop_loader_thread) && (tidal_oversigt.search_loaded==false)) {
      renderer.AddTextureRect(0,_texturetidalwait, 550, 400, 551, 328,1,1,1,1);
    }
     
    // start play
    // after 4 frames. Do play
    if ((tidal_oversigt.startplay) && (tidal_start_delay==4) && (!(visur))) {
      // start tidal play by fmod
      tidal_start_delay=0;
      ask_open_dir_or_play_tidal=false;
      tidal_oversigt.startplay=false;                                                   // stop starting more that one in next run
      // playlist play
      if (do_show_tidal_search_oversigt==false) {
        if (tidal_oversigt.streamantal()>0) {
          if (tidal_oversigt.type==0) {
            if (strcmp(tidal_oversigt.get_tidal_playlistid(tidalknapnr-1),"")!=0) {                                   // check playlistid exists
              // try load and start playing playlist
              if (tidal_oversigt.get_tidal_type(tidalknapnr-1)==0) {
                // stop last song playing
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play play album");
                // int antal_i_oversigt = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);        
                
                // rember name of playlist to save it later
                strcpy( playlistfilename , tidal_oversigt.get_tidal_feed_showtxt(tidalknapnr-1) );          // get name of playlist
                if ( strlen(tidal_oversigt.get_tidal_textureurl(tidalknapnr-1))>0 ) strcpy( playlistfilename_cover_path,tidal_oversigt.get_tidal_textureurl(tidalknapnr-1) );
                else strcpy( playlistfilename_cover_path , "" );
                strcpy( playlistfileid , tidal_oversigt.get_tidal_playlistid(tidalknapnr-1));
                strcpy( playlistfileartistname , tidal_oversigt.get_tidal_feed_artistname(tidalknapnr-1));
                playlist_nr_of_songs=tidal_oversigt.get_tidal_feed_nr_of_songs(tidalknapnr-1);
                // keybufferindex=strlen(playlistfilename);        
                //
                // play album then selected in tidal view.
                //
                do_zoom_tidal_cover=true;                                       // show we play
                musicoversigt.set_music_is_playing(false);
                antal_i_tidal_playlist = tidal_oversigt.tidal_play_now_album((char *) tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                snd = 1;                                // set play new flag
                if (antal_i_tidal_playlist) {
                  tidal_oversigt.tidal_set_aktiv_song(0);
                } else {
                  tidal_oversigt.tidal_set_aktiv_song(-1);
                }
                tidal_oversigt.update_playlist_playcount(tidal_oversigt.get_tidal_playlistid(tidalknapnr-1));
              }

              // debug code
              // printf("antal_i_tidal_playlist %d func get_aktiv_played_song return %d \n",antal_i_tidal_playlist,tidal_oversigt.get_aktiv_played_song());

              // try load and play song
              if ((tidal_oversigt.get_tidal_type(tidalknapnr-1)==1) && (antal_i_tidal_playlist)) {
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play song");
                tidal_player_start_status = tidal_oversigt.tidal_play_now_song((char *) tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
              }
              // try play search result
              if (tidal_oversigt.get_tidal_type(tidalknapnr-1)==2) {
                do_play_tidal=false;
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play search result");
                strcpy(playlistfilename,tidal_oversigt.get_tidal_feed_showtxt(tidalknapnr-1));          // get name of playlist
                if (strlen(tidal_oversigt.get_tidal_textureurl(tidalknapnr-1))>0) strcpy(playlistfilename_cover_path,tidal_oversigt.get_tidal_textureurl(tidalknapnr-1));
                else strcpy(playlistfilename_cover_path,"");
                // keybufferindex=strlen(playlistfilename);
                strcpy( playlistfileid , tidal_oversigt.get_tidal_playlistid(tidalknapnr-1));
                strcpy( playlistfileartistname , tidal_oversigt.get_tidal_feed_artistname(tidalknapnr-1));
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_song( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_album( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);

                // tidal_player_start_status = tidal_oversigt.tidal_play_now_album( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);

                antal_i_tidal_playlist = tidal_oversigt.tidal_play_now_album((char *) tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);

                // save playlist flag to save it later 
                save_ask_save_playlist=true;

                musicoversigt.set_music_is_playing(false);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                
                // do_zoom_tidal_cover=true;                                       // show we play        
                snd=1;
                show_uv=true;
                vis_uv_meter=true;
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
              }
              if (tidal_player_start_status == 0 ) {
                fprintf(stderr,"tidal start play return ok.\n");
                snd=1;
                show_uv=true;
                vis_uv_meter=true;
                do_zoom_tidal_cover=true;                                       // show we play
                keybufferindex=0;
              } else {
                // error start playing
                // do_play_tidal_cover=false;                                          // do not show we play.
                // do_zoom_tidal_cover=false;                                       // show we play
                //write_logfile(logfile,(char *) "Error loading tidal song");
                // snd=0;                                                                       // 1=1
              }
            } else {
              printf("Error tidal playid is missing %s.\n",playlistfileid);
              write_logfile(logfile,(char *) "Error tidal playid is missing");
            }
          }
          // play song
          if (tidal_oversigt.type==1) {
            
              // aktiv_playlist.clean_playlist();		// clear old playlist        

            if (strcmp(tidal_oversigt.get_tidal_playlistid(tidalknapnr-1),"")!=1) {
              // try load and start playing playlist
              if (tidal_oversigt.get_tidal_type(tidalknapnr-1)==1) {
                // stop last song playing
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                tidal_oversigt.clear_tidal_aktiv_songlist();
                tidal_oversigt.tidal_set_aktiv_song(0);                                                  // set first song in list as active
                write_logfile(logfile,(char *) "Tidal start play play album");
                strcpy(playlistfilename,tidal_oversigt.get_tidal_feed_showtxt(tidalknapnr-1));          // get name of playlist
                if (strlen(tidal_oversigt.get_tidal_textureurl(tidalknapnr-1))>0) strcpy(playlistfilename_cover_path,tidal_oversigt.get_tidal_textureurl(tidalknapnr-1));
                else strcpy(playlistfilename_cover_path,"");
                //keybufferindex=strlen(playlistfilename);
                strcpy( playlistfileid , tidal_oversigt.get_tidal_playlistid(tidalknapnr-1));
                strcpy( playlistfileartistname , tidal_oversigt.get_tidal_feed_artistname(tidalknapnr-1));
                
                playlist_nr_of_songs=tidal_oversigt.get_tidal_feed_nr_of_songs(tidalknapnr-1);

                // tidal_player_start_status = tidal_oversigt.tidal_play_now_song( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_album( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                tidal_player_start_status = tidal_oversigt.tidal_play_now_song((char *) tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                musicoversigt.set_music_is_playing(false);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                keybufferindex=0;
                do_zoom_tidal_cover=true;                                       // show we play        
                snd=1;
                show_uv=true;
                vis_uv_meter=true;
              }
            }       
          }
        }
      }
      // tidal search view
      if (do_show_tidal_search_oversigt==true) {
        if (tidal_oversigt.search_streamantal()>0) {
          if (tidal_oversigt.type==0) {
            if (strcmp(tidal_oversigt.get_tidal_search_playlistid(tidalknapnr-1),"")!=0) {                                   // check playlistid exists
              // try load and start playing playlist
              if (tidal_oversigt.search_get_tidal_type(tidalknapnr-1)==0) {
                // stop last song playing
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play play album");
                // int antal_i_oversigt = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);        
                
                // rember name of playlist to save it later
                strcpy( playlistfilename , tidal_oversigt.search_get_tidal_feed_showtxt(tidalknapnr-1) );          // get name of playlist
                if ( strlen(tidal_oversigt.search_get_tidal_textureurl(tidalknapnr-1))>0 ) strcpy( playlistfilename_cover_path,tidal_oversigt.search_get_tidal_textureurl(tidalknapnr-1) );
                else strcpy( playlistfilename_cover_path , "" );
                strcpy( playlistfileid , tidal_oversigt.get_tidal_search_playlistid(tidalknapnr-1));
                strcpy( playlistfileartistname , tidal_oversigt.search_get_tidal_feed_artistname(tidalknapnr-1));
                playlist_nr_of_songs=tidal_oversigt.search_get_tidal_feed_nr_of_songs(tidalknapnr-1);
                // keybufferindex=strlen(playlistfilename);        
                //
                // play album then selected in tidal view.
                //
                do_zoom_tidal_cover=true;                                       // show we play
                musicoversigt.set_music_is_playing(false);
                antal_i_tidal_playlist = tidal_oversigt.tidal_play_now_search_album((char *) tidal_oversigt.get_tidal_search_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);
                // tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                snd = 1;                                // set play new flag
                if (antal_i_tidal_playlist) {
                  tidal_oversigt.tidal_set_aktiv_song(0);
                } else {
                  tidal_oversigt.tidal_set_aktiv_song(-1);
                }
                // tidal_oversigt.update_playlist_playcount(tidal_oversigt.get_tidal_playlistid(tidalknapnr-1));
              }
              // debug code
              // printf("antal_i_tidal_playlist %d func get_aktiv_played_song return %d \n",antal_i_tidal_playlist,tidal_oversigt.get_aktiv_played_song());
              // try load and play song
              if ((tidal_oversigt.search_get_tidal_type(tidalknapnr-1)==1) && (antal_i_tidal_playlist)) {
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play song");
                tidal_player_start_status = tidal_oversigt.tidal_play_now_song((char *) tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
              }
              // try play search result
              if (tidal_oversigt.search_get_tidal_type(tidalknapnr-1)==2) {
                do_play_tidal=false;
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                write_logfile(logfile,(char *) "Tidal start play search result");
                strcpy(playlistfilename,tidal_oversigt.search_get_tidal_feed_showtxt(tidalknapnr-1));          // get name of playlist
                if (strlen(tidal_oversigt.get_tidal_textureurl(tidalknapnr-1))>0) strcpy(playlistfilename_cover_path,tidal_oversigt.get_tidal_textureurl(tidalknapnr-1));
                else strcpy(playlistfilename_cover_path,"");
                // keybufferindex=strlen(playlistfilename);
                strcpy( playlistfileid , tidal_oversigt.get_tidal_search_playlistid(tidalknapnr-1));
                strcpy( playlistfileartistname , tidal_oversigt.search_get_tidal_feed_artistname(tidalknapnr-1));
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_song( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_album( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_album( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                antal_i_tidal_playlist = tidal_oversigt.tidal_play_now_album((char *) tidal_oversigt.get_tidal_search_playlistid( tidalknapnr-1 ), tidalknapnr-1 , 1);
                // save playlist flag to save it later 
                save_ask_save_playlist=true;
                musicoversigt.set_music_is_playing(false);
                tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                // do_zoom_tidal_cover=true;                                       // show we play        
                snd=1;
                show_uv=true;
                vis_uv_meter=true;
                // tidal_player_start_status = tidal_oversigt.tidal_play_now_playlist( tidal_oversigt.get_tidal_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
              }
              if (tidal_player_start_status == 0 ) {
                fprintf(stderr,"tidal start play return ok.\n");
                snd=1;
                show_uv=true;
                vis_uv_meter=true;
                do_zoom_tidal_cover=true;                                       // show we play
                keybufferindex=0;
              } else {
                // error start playing
                // do_play_tidal_cover=false;                                          // do not show we play.
                // do_zoom_tidal_cover=false;                                       // show we play
                write_logfile(logfile,(char *) "Error loading tidal song");
                snd=0;                                                                       // 1=1
              }
            } else {
              printf("Error tidal playid is missing %s.\n",playlistfileid);
              write_logfile(logfile,(char *) "Error tidal playid is missing");
            }
          }
          // play song
          if (tidal_oversigt.type==1) {
            // aktiv_playlist.clean_playlist();		// clear old playlist        
            if (strcmp(tidal_oversigt.get_tidal_search_playlistid(tidalknapnr-1),"")!=0) {
              // try load and start playing playlist
              if (tidal_oversigt.search_get_tidal_type(tidalknapnr-1)==0) {
                // stop last song playing
                if (snd) {
                  // yes stop play
                  // stop old playing
                  sound->release();                                                                       // stop last playing song
                  dsp = 0;                                                                                  // reset uv
                  ERRCHECK(result,0);
                  snd=0;                                // set play new flag
                }
                tidal_oversigt.clear_tidal_aktiv_songlist();
                tidal_oversigt.tidal_set_aktiv_song(0);                                                  // set first song in list as active
                write_logfile(logfile,(char *) "Tidal start play play album");
                if (strlen(tidal_oversigt.search_get_tidal_feed_showtxt(tidalknapnr-1))>0) {
                  strcpy(playlistfilename,tidal_oversigt.search_get_tidal_feed_showtxt(tidalknapnr-1));          // get name of playlist
                  // if (strlen(tidal_oversigt.search_get_tidal_textureurl(tidalknapnr-1))>0) strcpy(playlistfilename_cover_path,tidal_oversigt.search_get_tidal_textureurl(tidalknapnr-1));
                  strcpy(playlistfilename_cover_path,"");
                  // strcpy( playlistfileid , tidal_oversigt.get_tidal_search_playlistid(tidalknapnr-1));
                  // strcpy( playlistfileartistname , tidal_oversigt.search_get_tidal_feed_artistname(tidalknapnr-1));
                  playlist_nr_of_songs=tidal_oversigt.search_get_tidal_feed_nr_of_songs(tidalknapnr-1);
                  tidal_player_start_status = tidal_oversigt.tidal_play_now_search_album((char *) tidal_oversigt.get_tidal_search_playlistid( tidalknapnr-1 ),tidalknapnr-1, 1);
                  musicoversigt.set_music_is_playing(false);
                  tidal_oversigt.set_tidal_playing_flag(true);                          // set playing flag
                  keybufferindex=0;
                  do_zoom_tidal_cover=true;                                       // show we play        
                  snd=1;
                  show_uv=true;
                  vis_uv_meter=true;
                }
              }
            }
          }
        }
      }
    }



    //
    // start play tidal song
    //
    if (do_play_tidal==1) {
      do_play_tidal=0;      
      // set play flag
      tidal_oversigt.startplay=true;
    }

    static bool radio_startplaying=false;

    // Start Play radio
    if (vis_radio_oversigt) {
      if ((do_play_radio) && (rknapnr>0) && (rknapnr<=radiooversigt.radioantal())) {
        // do_play_radio=false;
        // if playing stop play
        if (snd) {
          // yes stop old radio player
          // stop old playing
          if (channel) {
            result=channel->stop();
            dsp = 0;
            ERRCHECK(result,0);
            snd=0;
            channel = nullptr;
          }
        }
        if (radio_startplaying==false) {
          strcpy(aktivplay_music_path,radiooversigt.get_stream_url(rknapnr-1));
          printf("play radio path = %s \n",aktivplay_music_path);
          result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);
          result = sndsystem->createSound(aktivplay_music_path, FMOD_NONBLOCKING | FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
          ERRCHECK(result,rknapnr);
          radio_startplaying=true;
        }
        sound->getOpenState(&openstate, 0, 0, 0);
        if (openstate==FMOD_OPENSTATE_ERROR) printf("Check state openstate = FMOD_OPENSTATE_ERROR \n");
        else if (openstate==FMOD_OPENSTATE_LOADING) printf("Check state openstate = FMOD_OPENSTATE_LOADING \n");
        else if (openstate==FMOD_OPENSTATE_PLAYING) printf("Check state openstate = FMOD_OPENSTATE_PLAYING \n");
        else if (openstate==FMOD_OPENSTATE_CONNECTING) printf("Check state openstate = FMOD_OPENSTATE_CONNECTING \n");
        else if (openstate==FMOD_OPENSTATE_BUFFERING) printf("Check state openstate = FMOD_OPENSTATE_BUFFERING \n");
        else if (openstate==FMOD_OPENSTATE_SEEKING) printf("Check state openstate = FMOD_OPENSTATE_SEEKING \n");
        else printf("Check state  = %d \n",openstate);

        if (openstate == FMOD_OPENSTATE_ERROR) {
          do_play_radio=false;
          radio_startplaying=false;
          snd=0;
        }
        if ((do_play_radio) && (openstate == FMOD_OPENSTATE_READY)) {
          if (openstate==FMOD_OPENSTATE_READY) {
            printf("Star play sound \n");
            if (sound) result = sndsystem->playSound(sound,NULL, false, &channel);
            ERRCHECK(result,do_play_music_aktiv_table_nr);
            if (sndsystem) channel->setVolume(configsoundvolume); // set play volume from configfile
            radiooversigt.set_radio_popular(rknapnr-1);				    // set afspillings antal
            radiooversigt.set_radio_online(rknapnr-1,true);				// station virker fint ok status igen
            // radiooversigt.set_radio_intonline(rknapnr-1);         // set online in class.
            snd=1;
            radiooversigt.playingstationnr=rknapnr;
            radiooversigt.playing=true;                                 // set playing flag
            do_play_radio=false;
          } else {
            // radiooversigt.set_radio_online(rknapnr-1,false);			// set radio til ofline (vis som rød)
            // radiooversigt.set_radio_aktiv(rknapnr-1,false);			  //
            snd=0;
          }
          if (result==FMOD_OK) {
            do_zoom_radio_cover=true;
          }
          do_play_radio=false;
        }

        // check om vi spiller radio
        // if get song tag
        
        if ((channel) && (sound) && (snd)) {
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
      }
    }

    // play stream
    if (vis_stream_oversigt) {
      if ((do_play_stream) && (startstream) && (sknapnr>0) && (sknapnr<=streamoversigt.streamantal())) {
        // do_play_stream=false;
        // printf("Start play stream here\n");
        if (snd) {
          // stop old playing
          // sound->release();                                                                       // stop last playing song
          result=channel->stop();
          ERRCHECK(result,0);
          snd=0;
        }
        stream_playnr=sknapnr;                                                  // rember the stream we play
        strcpy(stream_playing_name,streamoversigt.get_stream_name(stream_playnr-1));
        // strcpy(stream_playing_desc,streamoversigt.get_stream_desc(stream_playnr-1));
        stream_playing_icon=streamoversigt.get_texture(stream_playnr-1);
        if (streamoversigt.get_stream_url(sknapnr-1)) {
        surl = streamoversigt.get_stream_url(sknapnr-1);
        } else surl = "";        
        if (surl.find("youtube") != std::string::npos) {
          strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
          strcat(systemcommand,"'");
          if (sknapnr>0) {
            if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash:",10)==0) {
              strcat(systemcommand,"http://");
              strcat(systemcommand,streamoversigt.get_stream_url(sknapnr)+10);
            } else strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1));
            strcat(systemcommand,"' &");
            if (do_system_call(systemcommand)==0) {
              printf("Error running system command ");
              // vis_error=true;
              // vis_error_timeout=60;
            }
          }
        } else {
          if ((sknapnr>0) && (streamoversigt.toplevel==0)) {
            if (fmodcreatesound==false) {
              printf("Start stream vlc player sknapnr = %d URL = %s \n",sknapnr-1,streamoversigt.get_stream_url(sknapnr-1));
              std::string uurl = streamoversigt.get_stream_url(sknapnr-1);
              // play by vlc
              streamoversigt.playstream((char *) uurl.c_str());
              startstream=false;
              printf("call createsound fmod result = %d \n",result);
              fmodcreatesound=true;
            }
          }
        }
      }
    }

    
    //
    // *********************** play recorded program ********************************************************************
    // 
    if (do_play_recorded_aktiv_nr) {
      // write debug log
      sprintf(debuglogdata,"Start playing recorded program");
      write_logfile(logfile,(char *) debuglogdata);

      if (strcmp("internal",configdefaultplayer)!=0) {
        strcpy(systemcommand,"./startrecorded.sh ");
        recorded_oversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
        //strcat(systemcommand,configrecordpath);
        strcat(systemcommand,temptxt);
        if (debugmode & 64) {
          sprintf(debuglogdata,"Start command :%s ",systemcommand);
          write_logfile(logfile,(char *) debuglogdata);
        }
        do_system_call(systemcommand);
        do_play_recorded_aktiv_nr=0;                                                                // start kun 1 player
      } else {
        // write debug log
        write_logfile(logfile,(char *) "Start default playing recorded program");
        strcpy(systemcommand,"./startrecorded.sh ");
        recorded_oversigt.get_recorded_filepath(temptxt,valgtrecordnr,subvalgtrecordnr);               // hent filepath
        //strcat(systemcommand,configrecordpath);
        strcat(systemcommand,temptxt);
        // write debug log
        sprintf(debuglogdata,"Start command :%s ",systemcommand);
        write_logfile(logfile,(char *) debuglogdata);
        do_system_call(systemcommand);
        do_play_recorded_aktiv_nr=0;                                                                // start kun 1 player
      }
    }

    // do setup stuf
    if (do_show_setup) {
      do_zoom_tidal_cover = false;
      do_zoom_spotify_cover = false;
      do_zoom_film_cover = false;
      do_zoom_music_cover = false;
      do_zoom_radio_cover = false;
      do_zoom_stream_cover = false;
      show_setup_interface();                                             // show setup interface
      if (do_show_setup_sound) show_setup_sound();                        // sound device
      if (do_show_setup_screen) show_setup_screen();                      //
      if (do_show_videoplayer) show_setup_video();                        //
      // if (do_show_setup_sql) show_setup_sql();                            //
      if (do_show_setup_backend) show_setup_sql();                            //
      if (do_show_setup_tidal) tidal_oversigt.show_setup_tidal();         // tidal
      if (do_show_setup_spotify) spotify_oversigt.show_setup_spotify();
      if (do_show_setup_torrent) show_setup_torrent();                    // torrent setup
      if (do_show_setup_tema) show_setup_tema();                          // tema
      if (do_show_setup_rss) show_setup_rss(configrss_ofset);
      if (do_show_setup_keys) show_setup_keys();
      if (do_show_tvgraber) show_setup_tv_graber(tvchannel_startofset);   // tv graber
      if (do_show_setup_network) {
        show_setup_network();
        if (show_wlan_select) {
          show_wlan_networks((int) setupwlanselectofset);                         // show wlan list in opengl
        }
      }
      if (do_show_setup_font) {
        // select font
        show_setup_font(config_font_select);        
      }
    }



    if (do_stop_tidal) {
      do_stop_tidal=false;
      #if defined USE_FMOD_MIXER
      if (sound) result=sound->release();			            // stop all music if user press show playlist stop button
      ERRCHECK(result,do_play_music_aktiv_table_nr);
      #endif
      #if defined USE_SDL_MIXER
      if (sdlmusicplayer) Mix_FreeMusic(sdlmusicplayer);	// stop music and free music
      #endif
      snd=0;                                            // clear sound device
      show_uv=false;
      vis_uv_meter=false;
      tidal_oversigt.tidal_playingnr=-1;
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
      snd=0;
      do_stop_radio = false;
      do_stop_music_all = true;
      radiooversigt.playingstationnr=-1;
    }

    // check if we play. and set playing var to 1
    result = channel->isPlaying(&playing);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
      ERRCHECK(result,do_play_music_aktiv_table_nr);
    }
    //
    // Tidal auto next aktiv song
    //
    if (playing==false) {
      //
      // Tidal auto next aktiv song
      //
      if (((vis_tidal_oversigt) || (tidal_oversigt.get_tidal_playing_flag())) && (snd)) {
        sound->release();           // stop last playing song
        if (tidal_oversigt.tidal_next_play()==0) {
          write_logfile(logfile,(char *) "Error in tidal next song to play");
        }
      }

      // enable when music is working
      
      // check music next
      if ((do_stop_music==false) && (vis_music_oversigt) || (musicoversigt.get_music_is_playing()==true)) {
        musicoversigt.update_afspillinger_music_song(aktivplay_music_path);				// Set aktive sang antal played +1 og set dagsdato til lastplayed mysql felt
        if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {			// er der flere sange i playliste
          do_play_music_aktiv_table_nr++;								// auto next song
          aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);	// hent ny aktiv sang aktivplay_music_path=PATH
          sound->release();           // stop last playing song
          musicoversigt.set_music_is_playing(false);
          ERRCHECK(result,do_play_music_aktiv_table_nr);
          fprintf(stderr,"Auto1 Next song %s \n",aktivplay_music_path);
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
            musicoversigt.set_music_is_playing(true);
            ERRCHECK(result,do_play_music_aktiv_table_nr);
            if (sndsystem) channel->setVolume(configsoundvolume);
            dsp = 0;
          } else {
            // play next song error (set skip song flag) jump to next song
            if (do_play_music_aktiv_table_nr<aktiv_playlist.numbers_in_playlist()) {
              do_play_music_aktiv_table_nr++;
              do_shift_song = true;
              musicoversigt.set_music_is_playing(false);
            } else {
              do_stop_music_all = true;            // stop play music
            }
          }
          // do_zoom_music_cover_remove_timeout=showtimeout;			                // set close info window timeout
          do_zoom_music_cover = true;				                                  // show music cover info til timeout showtimeout
        }
      }
    }


    // test for show uv
    if ((snd) && (do_show_torrent==false) && (do_show_torrent_options_move==false) && (vis_tv_oversigt==false)) vis_uv_meter=true; else vis_uv_meter=false;

    //calc uv
    if (playing) {
      // getSpectrum() performs the frequency analysis, see explanation below
      // sampleSize = 64;                // nr of samples default 64
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
        }
        for (int i=0;i<13;i++) frequencyOctaves[i]=(int) (44100/2)/(float) pow(2,12-i);
      }
      FMOD::ChannelGroup *mastergroup;
      if (!(dsp)) {
        sndsystem->getMasterChannelGroup(&mastergroup);
        sndsystem->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
        // dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE,FMOD_DSP_FFT_WINDOW_TRIANGLE);
        // dsp->setParameterInt(FMOD_DSP_FFT_WINDOW_TYPE,FMOD_DSP_FFT_WINDOW_TRIANGLE);
        dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, sampleSize);
        mastergroup->addDSP(0, dsp);
        //channel->addDSP(FMOD_DSP_PARAMETER_DATA_TYPE_FFT, dsp);
        dsp->setActive(true);
      }
      result=dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0);
      if (result!=FMOD_OK) fprintf(stderr,"Error DSP %s\n",FMOD_ErrorString(result));
      int length = fft->length/2;
      int numChannels = fft->numchannels;
      int gangefaktor=16;
      // crash if only 1 channel
      if ((fft) && (result==FMOD_OK)) {
        // new ver 5
        for (int x = 0; x < 128; x++) {
          int startBin = (x * fft->length) / 128;
          int endBin   = ((x + 1) * fft->length) / 128;
          float sum = 0.0f;
          float sum_left=0.0f;
          float sum_right=0.0f;
          int count = 0;
          for (int i = startBin; i < endBin; i++) {
            float spectum_value;
            float spectum_left;
            float spectum_right;
            if (numChannels == 1)
              spectum_value = fft->spectrum[0][i];
            else
              spectum_value = (fft->spectrum[0][i] + fft->spectrum[1][i]) * 0.5f;
            spectum_left=fft->spectrum[0][i]*0.5f;
            spectum_right=fft->spectrum[1][i]*0.5f;
            sum += spectum_value;
            sum_left += spectum_left;
            sum_right += spectum_right;
            count++;
          }
          float spec_value=(count > 0) ? (sum / count) * gangefaktor : 0.0f;
          spectrum[x] = (count > 0) ? (sum / count) * 200.0f : 0.0f;
          spectrum_left[x]=(count > 0) ? (sum_left / count) * 200.0f : 0.0f;
          spectrum_right[x]=(count > 0) ? (sum_right / count) * 200.0f : 0.0f;
          // saver_musicmeter.music_spectrum[x] = spec_value;
        }
      }
    }

    if (visur==false) {
      // vis uv in corner
      int high;
      int uvxpos=1650;
      if (vis_uv_meter) {
        if ((configuvmeter==1) || (configuvmeter==4)) {
          for(int qq=0;qq<32;qq++) {
            int yypos = 1050;
            float decay = 0.8f;        // 0.05f
            static float barHeights[45] = {0}; // persistent for smoothing
            float target = sqrtf(spectrum[qq] * 8.0f) * 2.0f;
            if (target > barHeights[qq]) {
              barHeights[qq] = target;
            } else {
              barHeights[qq] -= decay;
              if (barHeights[qq] < 0) barHeights[qq] = 0;
            }
            high = barHeights[qq]/3;
            if (vis_tidal_oversigt) 
            if (high>10) high=10;
            for(int i=0;i<high;i++) {
              renderer.AddTextureRect(0,texturedot, uvxpos, yypos, 20, 20,1,1,1,1);
              yypos = yypos - 16;  // 16
            }
            uvxpos += 14/2+1;
          }
        }
        if ((configuvmeter==2) || (configuvmeter==3)) {
          for(int qq=0;qq<32;qq++) {
            int yypos = 1050;
            float decay = 0.8f;        // 0.05f
            static float barHeights[45] = {0}; // persistent for smoothing
            float target = sqrtf(spectrum[qq] * 8.0f) * 2.0f;
            if (target > barHeights[qq]) {
              barHeights[qq] = target;
            } else {
              barHeights[qq] -= decay;
              if (barHeights[qq] < 0) barHeights[qq] = 0;
            }
            high = barHeights[qq]/3;
            if (high>10) high=10;
            for(int i=0;i<high;i++) {
              renderer.AddTextureRect(0,texturedot, uvxpos, yypos, 20, 20,1,1,1,1);
              yypos = yypos - 16;  // 16
            }
            uvxpos += 14/2+1;
          }
        }
      }
    }
    
    // reset tidal (reload playlists)
    if (tidal_oversigt.do_setup_tidal_start_entry) {
      tidal_oversigt.setup_tidal_start_entry();
    }

  
    //
    // show search box and text for radio and music and movie and spotify and tidal offline search NOT spotify online search
    //
    if ((vis_radio_oversigt) || (vis_music_oversigt) || (vis_film_oversigt) || (do_show_spotify_search_oversigt) || (do_show_tidal_search_oversigt) || ((vis_spotify_oversigt))) {
      if ((vis_spotify_oversigt) && (strcmp(keybuffer,"")!=0)) keybufferopenwin=true;
      if ((vis_tidal_oversigt) && (strcmp(keybuffer,"")!=0)) keybufferopenwin=true;
      if ((vis_tidal_oversigt) && (ask_save_playlist)) {
        keybufferopenwin=true;                                                    // open input window
      }   
      if ((do_show_tidal_search_oversigt) && (ask_save_playlist)) {
        keybufferopenwin=true;                                                    // open input window
      }
    }
    // show volume info
    if (show_volume_info) {
      if (vis_volume_timeout>0) vis_volume_timeout--;
      if (vis_volume_timeout>0) {
        float xof = (1920.0f/2.0)-(248.0f/2.0f);
        float yof = 900.0f;
        float buttonsizex = 248.0f;
        float buttonsizey = 58.0f;
        renderer.AddTextureRect(0,volume_window, xof, yof, buttonsizex, buttonsizey,1,1,1,1);
        if (configsoundvolume>0) {
          xof = (1920.0f/2.0)-(248.0f/2.0f) + 60.0f;
          yof = 922.0f;
          buttonsizex = (configsoundvolume*168.0f);
          buttonsizey = 14.0f;
          renderer.AddTextureRect(0,volume_window, xof, yof, buttonsizex, buttonsizey,1,1,1,1);
        }
      } else show_volume_info=false;
    }

    // save config
    if (do_save_config) {
      do_save_config = false;
      if (save_config((char *) "/etc/mythtv-controller.conf")!=0) {
        fprintf(stderr,"Error saving config file mythtv-controller.conf\n");
      } else fprintf(stderr,"Saving config ok.\n");
      freegfx();                                                                // free gfx loaded
      team_settings_load();                                         // load new team settings
      loadgfx();                                                                // reload all menu + icon gfx
    }

    // show pfs
    // debug mode 1
    showfps=false;
    show_status=false;
    if ((showfps) && (show_status==false)) {
      // Gather our frames per second
      Frames++;
      GLuint t = SDL_GetTicks();
      GLfloat fps;
      GLfloat seconds = (t - T0) / 1000.0;
      fps = Frames / seconds;
      T0 = t;
      Frames = 0;
      sprintf(temptxt,"FPS: %-4.0f \n", fps);
      renderer.AddText(&myfont,1, 1070 ,temptxt,1,1,1,1);
    }
    renderer.End();
    glutSwapBuffers();
} // end display





// ****************************************************************************************
//
// lirc + udpate music player
//
// ****************************************************************************************

void update2(int value) {
  char *code=0;
  char cmd[200];
  int ret;
  int n;
  int numbers_cd_covers_on_line=8;        // 9
  int numbers_film_covers_on_line=9;
  int numbers_radio_covers_on_line=8;
  int numbers_stream_covers_on_line=8;
  int snumbersoficonline=8;
  // lirc
  if ((sock!=0) && (sock!=-1)) {
    while (true) {
      code = nullptr;
      int ret = lirc_nextcode(&code);
      if (ret < 0) {
        // Ingen data på non-blocking socket
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          break;
        std::perror("lirc_nextcode");
        break;
      }
      if (code == nullptr)
          break;
      char cmd[128] = {};
      // Eksempel:
      // 0000000080010069 00 KEY_LEFT devinput-32
      if (std::sscanf(code, "%*s %*s %127s", cmd) == 1) {
        std::fprintf(stderr, "LIRC siger: %s\n", cmd);
        if (std::strcmp(cmd, "KEY_AUDIO") == 0) {
          vis_tv_oversigt       = false;
          vis_film_oversigt     = false;
          do_zoom_film_cover    = false;
          do_zoom_stream_cover  = false;
          vis_music_oversigt    = !vis_music_oversigt;
          vis_recorded_oversigt = false;
          vis_radio_oversigt    = false;
          vis_nyefilm_oversigt  = false;
          vis_stream_oversigt   = false;
          vis_spotify_oversigt  = false;
        }
        // lirc vis_radio_oversigt
        if (strcmp(cmd,"KEY_RADIO")==0) {					        // show radio oversigt
          vis_tv_oversigt       = false;									// sluk tv oversigt
          vis_film_oversigt     = false;									// sluk film oversigt
          do_zoom_film_cover    = false;									// sluk zoom film oversigt
          do_zoom_music_cover   = false;					        // sluk zoom cd cover
          do_zoom_stream_cover  = false;									// sluk stream rss oversigt
          vis_music_oversigt    = false;									// sluk music oversigt
          vis_radio_oversigt    = !vis_radio_oversigt;    // show/hide radio oversigt
          vis_recorded_oversigt = false;									// sluk recorded oversigt
          vis_nyefilm_oversigt  = false;									// sluk nye film oversigt
          vis_stream_oversigt   = false;                  // sluk rss stream oversigt
          vis_spotify_oversigt  = false;									// sluk spotify oversigt
        }
        // lirc vis_film_oversigt
        if (strcmp(cmd,"KEY_VIDEO")==0) {						      // Show video overview
          vis_tv_oversigt       = false;									// sluk tv oversigt
          vis_film_oversigt     = !vis_film_oversigt;			// show/hide film oversigt
          vis_music_oversigt    = false;									// sluk music oversigt
          vis_recorded_oversigt = false;									// sluk recorded oversigt
          vis_radio_oversigt    = false;				   			  // sluk radio oversigt
          do_zoom_music_cover   = false;							    // sluk zoom cd cover
          do_zoom_stream_cover  = false;									// sluk zoom stresm
          vis_nyefilm_oversigt  = false;									// sluk nyefilm oversigt (i startup)
          vis_stream_oversigt   = false;                  // sluk rss stream oversigt
          vis_spotify_oversigt  = false;									// sluk spotify oversigt
        }
        // lirc vis_recorded_oversigt
        if (strcmp(cmd,"KEY_PVR")==0) {                   // show recorded overview
          vis_tv_oversigt      = false;								    // sluk tv oversigt
          vis_film_oversigt    = false; 						   	  // sluk film oversigt
          vis_music_oversigt   = false;  							    // sluk music oversigt
          do_zoom_music_cover  = false;							      // sluk zoom cd cover
          do_zoom_stream_cover = false;									  // sluk stream rss oversigt
          vis_recorded_oversigt= !vis_recorded_oversigt;	// show/hide recorded program oversigt
          vis_radio_oversigt   = false;						   	    // sluk radio oversigt
          vis_nyefilm_oversigt = false;									  // sluk nyefilm oversigt (i startup)
          vis_stream_oversigt  = false;                   // show rss stream oversigt
          vis_spotify_oversigt = false;									  // sluk spotify oversigt
        }
        // lirc vis_tv_oversigt
        if (strcmp(cmd,"KEY_EPG")==0) {                   // show tv guide
          vis_tv_oversigt = !vis_tv_oversigt;  		        // show/hide tv oversigt
          vis_film_oversigt    = false; 						   	  // sluk film oversigt
          vis_music_oversigt   = false;  							    // sluk music oversigt
          do_zoom_film_cover   = false;									  // sluk zoom film
          do_zoom_music_cover  = false;							      // sluk zoom cd cover
          do_zoom_stream_cover = false;									  // sluk zoom stream
          vis_recorded_oversigt= false;                		// sluk recorded program oversigt
          vis_radio_oversigt   = false;						   	    // sluk radio oversigt
          vis_nyefilm_oversigt = false;									  // sluk nyefilm oversigt (i startup)
          vis_stream_oversigt  = false; 					        // show rss stream oversigt
          vis_spotify_oversigt = false;									  // sluk spotify oversigt
        }
        // lirc show rss stream oversigt
        if (strcmp(cmd,"KEY_TUNER")==0) {                 // show stream overview
          vis_tv_oversigt      = false;             		  // sluk tv oversigt
          vis_film_oversigt    = false; 						   	  // sluk film oversigt
          vis_music_oversigt   = false;  							    // sluk music oversigt
          do_zoom_film_cover   = false;									  // sluk zoom film
          do_zoom_music_cover  = false;							      // sluk zoom cd cover
          do_zoom_stream_cover = false;								   	// sluk recorded view
          vis_recorded_oversigt= false;                		// sluk recorded program oversigt
          vis_radio_oversigt   = false;						   	    // sluk radio oversigt
          vis_nyefilm_oversigt = false;									  // sluk nyefilm oversigt (i startup)
          vis_stream_oversigt  = !vis_stream_oversigt;		// show/hide rss stream oversigt
          vis_spotify_oversigt = false;									  // sluk spotify oversigt
        }
        // lirc none enable
        if ((strcmp(cmd,"KEY_HOME")==0) || (strcmp(cmd,"KEY_MEDIA")==0)) {
          vis_tv_oversigt      = false;								    // sluk tv oversigt
          vis_film_oversigt    = false; 						   	  // sluk film oversigt
          vis_music_oversigt   = false;  							    // sluk music oversigt
          do_zoom_music_cover  = false;							      // sluk zoom cd cover
          do_zoom_stream_cover = false;									  // sluk
          do_zoom_film_cover   = false;									  // sluk
          vis_recorded_oversigt= false;						        // sluk recorded program oversigt
          vis_radio_oversigt   = false;				   			    // sluk radio oversigt
          vis_nyefilm_oversigt = false;									  // sluk nyefilm oversigt (i startup)
          vis_stream_oversigt  = false;                   // sluk rss stream oversigt
          vis_spotify_oversigt = false;
        }
        // enable spotify
        if (strcmp("KEY_CAMERA",cmd)==0) {
          vis_spotify_oversigt=!vis_spotify_oversigt;     // show/hide spotify oversigt
          vis_tv_oversigt      = false;								    // sluk tv oversigt
          vis_film_oversigt    = false; 						   	  // sluk film oversigt
          vis_music_oversigt   = false;  							    // sluk music oversigt
          do_zoom_music_cover  = false;							      // sluk zoom cd cover
          do_zoom_stream_cover = false;                   // sluk stream oversigt
          vis_recorded_oversigt= false;                   // sluk recorded program oversigt
          vis_radio_oversigt   = false;						   	    // sluk radio oversigt
          vis_nyefilm_oversigt = false;									  // sluk nyefilm oversigt (i startup)
          vis_stream_oversigt  = false;   								// sluk rss stream oversigt
        }
     
        if (strcmp(cmd,"KEY_UP")==0) {
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            if (do_show_play_open_select_line>0) do_show_play_open_select_line--; 
              else if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
          }
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
            if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1)) ) {
              if (((int) music_key_selected<=numbers_cd_covers_on_line) && ((int) music_select_iconnr>((int) numbers_cd_covers_on_line-1))) {
                  music_select_iconnr-=numbers_cd_covers_on_line;
                  do_music_icon_anim_icon_ofset=-1;
              } else music_select_iconnr-=numbers_cd_covers_on_line;
              if ((int) music_key_selected>(int) numbers_cd_covers_on_line) {
                music_key_selected-=numbers_cd_covers_on_line;
              }
            }
          }
          if (vis_film_oversigt) {
            if ((vis_film_oversigt) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1)) ) {
              if (((int) film_key_selected<=numbers_film_covers_on_line) && ((int) film_select_iconnr>((int) numbers_film_covers_on_line-1))) {
                  film_select_iconnr-=numbers_film_covers_on_line;
              } else film_select_iconnr-=numbers_film_covers_on_line;
              if ((int) film_key_selected>(int) numbers_film_covers_on_line) film_key_selected-=numbers_film_covers_on_line;
            }         
          }
          if ((vis_radio_oversigt) && (show_radio_options==false)) {
            if ((vis_radio_oversigt) && (radio_select_iconnr>(numbers_radio_covers_on_line-1)) ) {
              if ((radio_key_selected<=numbers_radio_covers_on_line) && (radio_select_iconnr>(numbers_radio_covers_on_line-1))) {
                radio_select_iconnr-=numbers_radio_covers_on_line;
              } else radio_select_iconnr-=numbers_radio_covers_on_line;
              if (radio_key_selected>numbers_radio_covers_on_line) radio_key_selected-=numbers_radio_covers_on_line;
            }
          }
          // up key
          if ((vis_radio_oversigt) && (show_radio_options)) {
            // radiooversigt.lastradiooptselect();
          }
          // stream
          if ((vis_stream_oversigt) && (show_stream_options==false)) {
            if ((vis_stream_oversigt) && (stream_select_iconnr>(numbers_stream_covers_on_line-1)) ) {
              if ((stream_key_selected<=numbers_stream_covers_on_line) && (stream_select_iconnr>(numbers_stream_covers_on_line-1))) {
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

          if ((vis_spotify_oversigt) &&  (spotify_select_iconnr+9<spotify_oversigt.antal_spotify_streams())) {
            printf("LIRC-DOWN  ask_open_dir_or_play = %d stream antal %d \n",ask_open_dir_or_play,spotify_oversigt.antal_spotify_streams());
            if (!(ask_open_dir_or_play_spotify)) {
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
                if (spotifyknapnr+spotify_selected_startofset+snumbersoficonline<spotify_oversigt.antal_spotify_streams()+1) {
                  if ((((spotifyknapnr+snumbersoficonline)>40) && (do_show_spotify_search_oversigt==false)) || (((spotifyknapnr+snumbersoficonline)>32))) {
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
          }

          if (vis_film_oversigt) {
            if ((int) film_select_iconnr<(int) film_oversigt.film_antal()-5) {
              if (film_select_iconnr+numbers_film_covers_on_line>13) {		// skal vi scroll liste up
                //film_select_iconnr-=numbers_film_covers_on_line;		// husk at trække fra da vi står samme sted
                if (film_key_selected>0) film_key_selected-=numbers_film_covers_on_line;
              }
              film_key_selected+=numbers_film_covers_on_line;
              film_select_iconnr+=numbers_film_covers_on_line;
            }
          }
          if ((vis_radio_oversigt) && (show_radio_options==false) && ((radio_select_iconnr+numbers_radio_covers_on_line)<radiooversigt.radioantal())) {
            if (radio_key_selected>=20) {
              radio_select_iconnr+=numbers_radio_covers_on_line;
            } else {
              radio_key_selected+=numbers_radio_covers_on_line;
              radio_select_iconnr+=numbers_radio_covers_on_line;
            }
          }

          // lirc
          // down key
          if (vis_recorded_oversigt) {
              if (visvalgtnrtype==1) {
                if ((int) valgtrecordnr<(int) recorded_oversigt.top_antal()) {
                  valgtrecordnr++;
                  subvalgtrecordnr=0;
                }
              } else if (visvalgtnrtype==2) {
                if ((int) subvalgtrecordnr<(int) recorded_oversigt.programs_type_antal(valgtrecordnr)-1) {
                  subvalgtrecordnr++;
                }
              }
              reset_recorded_texture = true;
          }
          if (vis_tv_oversigt) {
            /*
            if (tvsubvalgtrecordnr+1<aktiv_tv_oversigt.kanal_prg_antal(tvvalgtrecordnr)) {
              tvsubvalgtrecordnr++;
            }
            */
            // check hvor vi er
            if (aktiv_tv_oversigt.getprogram_endunixtume(tvvalgtrecordnr,tvsubvalgtrecordnr)>hourtounixtime(aktiv_tv_oversigt.vistvguidekl+3)) {
              if (aktiv_tv_oversigt.vistvguidekl<24*2) aktiv_tv_oversigt.vistvguidekl++;
            }
          }
        }

        if (strcmp(cmd,"P")==0) {
          if (channel) {
            //channel->setMute(true);
            channel->setVolume(0.0);
            ERRCHECK(result,0);
          }
        }


        if (std::strcmp(cmd, "KEY_LEFT") == 0) {
          // Flyt til venstre

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
                     music_key_selected+=numbers_cd_covers_on_line-1;  // den viste på skærm af 1 til 20
                     music_select_iconnr--;                  	// den rigtige valgte af 1 til cd antal
                  }
                }
              }
            }
          }
          // lirc
          if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify))) {
            printf("LIRC-LEFT  ask_open_dir_or_play = %d stream antal %d \n",ask_open_dir_or_play,spotify_oversigt.antal_spotify_streams());
            // spotify left
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
            spotifyknapnr=spotify_select_iconnr;
          }

          // lirc
          // left key
          if (vis_film_oversigt) {							// ved film oversigt
            if (film_key_selected>1) {
            }
          }


        } else if (std::strcmp(cmd, "KEY_RIGHT") == 0) {
          // Flyt til højre
          if ((vis_music_oversigt) && (ask_open_dir_or_play)) {
            dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
          }
          // lirc
          // right key
          if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
            if (((int) music_key_selected<(int) musicoversigt_antal) && (music_icon_anim_icon_ofset==0)) {
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr<musicoversigt_antal)) {
                if (((int) music_key_selected % (numbers_cd_covers_on_line*4)==0) || (((int) music_select_iconnr==((numbers_cd_covers_on_line*4)-1)) && ((int) music_key_selected % numbers_cd_covers_on_line==0))) {
                  music_key_selected-=numbers_cd_covers_on_line;			// den viste på skærm af 1 til 20
                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                } else {
                  music_select_iconnr++;						// den rigtige valgte af 1 til cd antal
                }
                music_key_selected++;
              }
            }
          }

        } else if (std::strcmp(cmd, "KEY_OK") == 0 ||
          std::strcmp(cmd, "KEY_ENTER") == 0) {
          // Vælg
        }

      }
    }
    if (code) std::free(code);
  }
  sndsystem->update();				        // run update on fmod sound system
  glutTimerFunc(25, update2, 0);      // call again
  glutPostRedisplay();
}


// ****************************************************************************************
//
// in use in main (startup)
// dataload Music from empty db/update
//
// ****************************************************************************************

void datainfoloader_music_v2() {
  //pthread_mutex_lock(&count_mutex);
  // write debug log
  write_logfile(logfile,(char *) "loader thread starting - Loading music info.");
  if (strcmp(configbackend,"mythtv")==0) {
    // opdatere music oversigt
    // hent alt music info fra database
    // check if internal music db exist if yes do set global use it
    if (global_use_internal_music_loader_system_exist() == true) {
      fprintf(stderr,"******** Use global music database ********\n");
      // set use global loader
      global_use_internal_music_loader_system = true;
    } else {
      fprintf(stderr,"Search for music in :%s\n",configdefaultmusicpath);
      // build new db (internal db loader)
      // mew ver
      musicoversigt.opdatere_music_oversigt_nodb();
      if (debugmode & 2) fprintf(stderr,"Done update db from datasource.\n");
      write_logfile(logfile,(char *) "Done update db from datasource.");
      global_use_internal_music_loader_system = true;
    }
    // update music db from disk
    if ((do_update_music) || (do_update_music_now)) {
      // update the music db
      musicoversigt.opdatere_music_oversigt_nodb();
      // new test
      musicoversigt.opdatere_music_oversigt_icons();                                  // load icons
      do_update_music_now = false;                                              // do not call update any more
      do_update_music = false;                                                  // stop show music update
    }
    // load music db created by opdatere_music_oversigt_nodb function
    // first time
    // New ver
    if (musicoversigt.opdatere_music_oversigt(0)>0) {
     musicoversigt.opdatere_music_oversigt_icons();                                  // load icons
      write_logfile(logfile,(char *) "Music db+icons loaded..");
    }
  } else {
    if (debugmode % 2) fprintf(stderr,"Search for music in :%s\n",configdefaultmusicpath);
    // New ver
   musicoversigt.opdatere_music_oversigt_nodb();
    // new ver
   musicoversigt.opdatere_music_oversigt(0);
  }
  do_update_music=false;
  // write debug log
  write_logfile(logfile,(char *) "loader thread done loaded music info.");
}





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
    if (global_use_internal_music_loader_system) strcpy(database,"mythtvcontroller"); else strcpy(database,"mythconverg");
    if (debugmode & 2) {
      fprintf(stderr,"Henter info om playlistnr = %d \n",playlistnr);
      sprintf(debuglogdata,"Henter info om playlistnr = %d ",playlistnr);
      write_logfile(logfile,(char *) debuglogdata);
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
    std::string sqlselect1;
    std::string imgpath;
    std::string diridpath;
    std::string checkpath;
    char tmptxt[200];
    vector<string> pathlist;
    unsigned int i,ii;
    int parentdir_id;
    int iii;
    GLuint textureId;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    char database[256];
    if (global_use_internal_music_loader_system) strcpy(database,"mythtvcontroller"); else strcpy(database,"mythconverg");
    // write debug log
    sprintf(debuglogdata,"Hent info om directory_id = %d ",dirid);
    write_logfile(logfile,(char *) debuglogdata);
    dirmusic.emtydirmusic();
    strcpy(sqlselect,"SELECT song_id,name,artist_id FROM music_songs where directory_id=");
    sprintf(tmptxt,"%d order by name limit %d",dirid,dirliste_size);
    strcat(sqlselect,tmptxt);
    if (dirid>0) {
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
      // hent aktiv dir id info in music db
      strcpy(sqlselect,"SELECT path,parent_id FROM music_directories where directory_id=");
      sprintf(tmptxt,"%d limit 1",dirid);
      strcat(sqlselect,tmptxt);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          pathlist.push_back(row[0]);
          parentdir_id = atoi(row[1]);
          diridpath = row[0];
        }
        // er det et subdir ? if yes make full path
        if (parentdir_id>0) {
          do {
            sqlselect1 = fmt::format("SELECT path,parent_id FROM music_directories where directory_id={} limit 1",parentdir_id);
            mysql_query(conn,sqlselect1.c_str());
            res = mysql_store_result(conn);
            while ((row = mysql_fetch_row(res)) != NULL) {
              pathlist.push_back(row[0]);
              parentdir_id = atoi(row[1]);
            }
          } while(parentdir_id>0);
          iii=pathlist.size();
          if (global_use_internal_music_loader_system) strcpy(tmpfilename,configdefaultmusicpath); else strcpy(tmpfilename,configmusicpath);
          imgpath=tmpfilename; // start path
          while(iii) { 
            imgpath = imgpath + pathlist[iii-1];
            imgpath = imgpath + "/";
            iii--;
          }
          std::string huskpath;
          huskpath = imgpath;
          imgpath = imgpath + "cover.jpg";
          cout << "Path " << imgpath << "\n";
          if (!(file_exists(imgpath.c_str()))) {
            imgpath=tmpfilename; // start path
            while(iii) { 
              imgpath = imgpath + pathlist[iii-1];
              imgpath = imgpath + "/";
              iii--;
            }
            imgpath = imgpath + "cover.png";
            cout << "Path " << imgpath << "\n";
          }
          if (file_exists(imgpath.c_str())) {
            dirmusic.textureId=loadTexture((char *) imgpath.c_str());
          } else {
            // check if Front.jpg exist from old config
            huskpath = huskpath + "front.jpg";
            if (file_exists(huskpath.c_str())) {
               dirmusic.textureId=loadTexture((char *) huskpath.c_str());
            } else {
              dirmusic.textureId=0;
            }
          }
        }
      }
      // hent dirs info til visning samt covers data
      strcpy(sqlselect,"SELECT directory_id,path FROM music_directories where parent_id=");
      sprintf(tmptxt,"%d limit 100",dirid);
      strcat(sqlselect,tmptxt);
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      ii=0;
      if (res) {
        pathlist.clear();                                 // clear array again
        // rowl over dirs in dir
        while ((row = mysql_fetch_row(res)) != NULL) {
          pathlist.push_back(row[1]);                     // gem dir
          dirmusic.pushdir(row[1],row[0]);
          ii++;
        }
        iii=pathlist.size();
        ii=0;
        if (global_use_internal_music_loader_system) strcpy(tmpfilename,configdefaultmusicpath); else strcpy(tmpfilename,configmusicpath);
        if (dirmusic.gettexture(ii)==0) {
          while(iii) { 
            checkpath = tmpfilename;
            checkpath = checkpath + diridpath;
            checkpath = checkpath + "/";
            checkpath = checkpath + pathlist[iii-1];
            checkpath = checkpath + "/";
            checkpath = checkpath + "cover.jpg";
            if (file_exists(checkpath.c_str())) {
              textureId = 0;
              // textureId = loadTexture((char *) checkpath.c_str());		// load texture to opengl
            } else {
              checkpath = tmpfilename;
              checkpath = checkpath + diridpath;
              checkpath = checkpath + "/";
              checkpath = checkpath + pathlist[iii-1];
              checkpath = checkpath + "/";
              checkpath = checkpath + "front.jpg";
              if (file_exists(checkpath.c_str())) {
                textureId = 0;
                // textureId = loadTexture((char *) checkpath.c_str());		// load texture to opengl
              } else textureId=0;
            }
            dirmusic.settexture(textureId,ii);				                  // set directory texture
            ii++;
            iii--;        
          }
        }
      }
      mysql_close(conn);
    }
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
      write_logfile(logfile,(char *) "No lirc mythtv-controller config file found.");
      lirc_deinit();
      sock=-1;                                              				// lirc error code
    } else {
      // fjern vente tid fra lirc getchar
      fcntl(sock,F_SETOWN,getpid());
      flags=fcntl(sock,F_GETFL,0);
      if(flags!=-1) {
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
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



// ***********************************************************************
//
// Mouse handler
//
// ***********************************************************************

void handleMouse(int button,int state,int mousex,int mousey) {
  saver_irq=true;                                     // stop screen saver
  visur=0;
  int id=0;
  char temprgtxt[8192];
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    id = renderer.HitTest(mousex, mousey);
    if (id != -1) {
      printf("Klik på id=%d\n", id);
      saver_irq=true;                                                         // stop screen saver
    }
  }
  switch(button) {                                                            // check for mouse button
    case GLUT_LEFT_BUTTON:
      if (state==GLUT_DOWN) {
        if (vis_spotify_oversigt) {
          // touch screen scroll
          spotify_oversigt.dragging = true;
          spotify_oversigt.lastX = mousex;
          spotify_oversigt.lastY = mousey;
          spotify_oversigt.velocityX = spotify_oversigt.velocityY = 0; // nulstil
          spotify_oversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          spotify_oversigt.moved=false;
        }
        if (vis_tidal_oversigt) {
          // touch screen scroll
          tidal_oversigt.dragging = true;
          tidal_oversigt.lastX = mousex;
          tidal_oversigt.lastY = mousey;
          tidal_oversigt.velocityX = tidal_oversigt.velocityY = 0; // nulstil
          tidal_oversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          tidal_oversigt.moved=false;
        } 
                   
        if (vis_music_oversigt) {
          // touch screen scroll
          musicoversigt.dragging = true;
          musicoversigt.lastX = mousex;
          musicoversigt.lastY = mousey;
          musicoversigt.velocityX = musicoversigt.velocityY = 0; // nulstil
          musicoversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          musicoversigt.moved=false;
        }
        if (vis_radio_oversigt) {
          // touch screen scroll
          radiooversigt.dragging = true;
          radiooversigt.lastX = mousex;
          radiooversigt.lastY = mousey;
          radiooversigt.velocityX = radiooversigt.velocityY = 0; // nulstil
          radiooversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          radiooversigt.moved=false;
        }
        if (vis_stream_oversigt) {
          // touch screen scroll
          streamoversigt.dragging = true;
          streamoversigt.lastX = mousex;
          streamoversigt.lastY = mousey;
          streamoversigt.velocityX = streamoversigt.velocityY = 0; // nulstil
          streamoversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          streamoversigt.moved=false;
        }
        if (vis_film_oversigt) {
          // touch screen scroll
          film_oversigt.dragging = true;
          film_oversigt.lastX = mousex;
          film_oversigt.lastY = mousey;
          film_oversigt.velocityX = film_oversigt.velocityY = 0; // nulstil
          film_oversigt.downTimeMs = glutGet(GLUT_ELAPSED_TIME);
          film_oversigt.moved=false;
        }

      }
      if (state == GLUT_UP) {         
          if (vis_spotify_oversigt) {
            spotify_oversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            spotify_oversigt.velocityX = spotify_oversigt.lastDX * 3.0f;
            spotify_oversigt.velocityY = -spotify_oversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - spotify_oversigt.downTimeMs;
            if ((!spotify_oversigt.moved) && (dt<200)) spotify_oversigt.gettouchbutton=true;
          }
          if (vis_tidal_oversigt) {
            tidal_oversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            tidal_oversigt.velocityX = tidal_oversigt.lastDX * 3.0f;
            tidal_oversigt.velocityY = -tidal_oversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - tidal_oversigt.downTimeMs;
            if ((!tidal_oversigt.moved) && (dt<200)) tidal_oversigt.gettouchbutton=true;
          }
          if (vis_music_oversigt) {
            musicoversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            musicoversigt.velocityX = musicoversigt.lastDX * 3.0f;
            musicoversigt.velocityY = -musicoversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - musicoversigt.downTimeMs;
            if ((!musicoversigt.moved) && (dt<200)) musicoversigt.gettouchbutton=true;
          }
          if (vis_radio_oversigt) {
            radiooversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            radiooversigt.velocityX = radiooversigt.lastDX * 3.0f;
            radiooversigt.velocityY = -radiooversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - radiooversigt.downTimeMs;
            if ((!radiooversigt.moved) && (dt<200)) radiooversigt.gettouchbutton=true;
          }
          if (vis_stream_oversigt) {
            streamoversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            streamoversigt.velocityX = streamoversigt.lastDX * 3.0f;
            streamoversigt.velocityY = -streamoversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - streamoversigt.downTimeMs;
            if ((!streamoversigt.moved) && (dt<200)) streamoversigt.gettouchbutton=true;
          }
          if (vis_film_oversigt) {
            film_oversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            film_oversigt.velocityX = film_oversigt.lastDX * 3.0f;
            film_oversigt.velocityY = -film_oversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - film_oversigt.downTimeMs;
            if ((!film_oversigt.moved) && (dt<200)) film_oversigt.gettouchbutton=true;
          }
          id = renderer.HitTest(mousex, mousey);
          // in setup menu
          if (do_show_setup) {
            // test for setup menu sound
            if ((id==SOUND_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              do_show_setup_sound = true;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_setup_rss = false;
              do_show_setup_spotify = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
              strcpy(keybuffer,"");
            }
            // test NETWORK setup
            if ((id==NETWORK_SETUP_OVERVIEW)  && (do_show_setup_backend==false) && (do_show_setup_sound==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = true;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }
            // test for gfx opløsning
            if ((id==GFX_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_font = false;
              do_show_setup_tema = true;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }
            // test for screen setup
            if ((id==SCREEN_SETUP_OVERVIEW)  && (do_show_setup_backend==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_sound==false) && (do_show_setup_keys==false)) {
              do_show_setup_sound = false;
              do_show_setup_screen = true;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }
            // test for torrent setup/info
            if ((id==TORRENT_SETUP_OVERVIEW) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              do_show_setup_backend = false;
              do_show_setup_torrent = true;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;

            }
            // test for ttffont setup/info
            if ((id==TTF_SETUP_OVERVIEW) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_setup_font = true;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;

            }
            // test for F keys setup/info
            if ((id==FKEYS_SETUP_OVERVIEW) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false))  {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_videoplayer = false;
              do_show_setup_keys = true;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;

            }
            // test for setupclose
            if ((id==CLOSE_SETUP_OVERVIEW) && (do_show_setup_rss==false) && (do_show_setup_spotify==false) && (do_show_setup_tidal==false) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_keys==false)) {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_setup = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
              do_save_config = true;             // save setup now
            }
            if ((id==VIDEO_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              do_show_videoplayer = true;
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_tvgraber = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;

            }
            // do_show_tvgraber
            if ((id==TVGRABER_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = true;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;

            }
            // test for close windows again icon for all other windows in setup glLoadName(40)
            if ((id==40) && ((do_show_setup_tidal) || (do_show_setup_spotify) || (do_show_setup_sound) || (do_show_setup_screen) || (do_show_setup_backend) || (do_show_setup_torrent) || (do_show_setup_network) || (do_show_setup_tema) || (do_show_setup_font) || (do_show_setup_keys) || (do_show_videoplayer) || (do_show_setup_rss) || (do_show_tvgraber))) {
              do_show_setup_sound = false;
              do_show_setup_screen = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_tema = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_setup_spotify = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
              if (do_show_setup_rss) {
                rssstreamoversigt.save_rss_data();   // save rss data to mysql db
                do_show_setup_rss = false;
              }
              // font select
              if (do_show_setup_font) {
                do_show_setup_font = false;
              }
              if (do_show_tvgraber) {
                aktiv_tv_oversigt.update_guide();
                // need enable
                aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass);
                do_show_tvgraber = false;
              }

              if (do_show_setup_tema) {
                if (id==41) {
                  tema++;
                  if (tema>10) tema=0;
                }
                if (id==42) {
                  do_show_setup_tema=false;
                }
              }
            }

            if (do_show_setup_font) {
              if (id==41) {
                fprintf(stderr,"Set aktiv font to '%s' \n",aktivfont.typeinfo[aktivfont.selected_font_nr].fontname);
                strcpy(configfontname,aktivfont.typeinfo[setupfontselectofset].fontpath.c_str());
                myfont.Load((char *) configfontname,14);
                do_show_setup_font = false;
              }
            }

            if (id==TTF_SETUP_OVERVIEW) {
              do_show_setup_font=true;              
            }

            // test for tema setup/info
            if ((id==TEMA_SETUP_OVERVIEW) && (do_show_setup_sound==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_font==false) && (do_show_setup_tema))  {
              // next tema
              tema++;
              if (tema>TEMA_ANTAL) tema = 1;
            }

            

            // test for select rss setup
            if ((id==RSS_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              // close  all show setup windows
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
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
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }

            // test for select rss setup
            if ((id==SETUP_BACKEND) && (do_show_setup_backend==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              // close  all show setup windows
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_rss = false;
              do_show_setup_rss = false;
              do_show_setup_spotify = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
              do_show_setup_backend = true;
            }

            // test for select spotify setup
            if ((id==SPOTIFY_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_rss==false)) {
              // close  all show setup windows
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
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
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }
            // test for tidal select setup
            if ((id==TIDAL_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false) && (do_show_setup_rss==false && (do_show_setup_spotify==false))) {
              // close  all show setup windows
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = false;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_rss = false;
              do_show_setup_rss = false;
              do_show_setup_spotify = false;
              do_show_setup_tidal = true;
              do_show_setup_torrent = false;
            }

            // test for select network setup
            if ((id==NETWORK_SETUP_OVERVIEW) && (do_show_setup_backend==false) && (do_show_tvgraber==false) && (do_show_setup_network==false) && (do_show_setup_screen==false) && (do_show_setup_tema==false)) {
              // close  all show setup windows
              do_show_setup_sound = false;
              // do_show_setup_sql = false;
              do_show_setup_backend = false;
              do_show_setup_network = true;
              do_show_setup_screen = false;
              do_show_setup_tema = false;
              do_show_setup_font = false;
              do_show_setup_keys = false;
              do_show_videoplayer = false;
              do_show_tvgraber = false;
              do_show_rss = false;
              do_show_setup_rss = false;
              do_show_setup_spotify = false;
              do_show_setup_tidal = false;
              do_show_setup_torrent = false;
            }

            if ((id==40) && (do_show_setup_network)) do_show_setup_network=false;

            //
            if (id==45) {
              fprintf(stderr,"45 Button pressed \n");
            }
          } 
        

          // main menu
          if (id==TV) {
            vis_tv_oversigt = true;
          }
          

          if (vis_radio_oversigt) {
            radiooversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            radiooversigt.velocityX = radiooversigt.lastDX * 3.0f;
            radiooversigt.velocityY = -radiooversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - radiooversigt.downTimeMs;
            if ((!radiooversigt.moved) && (dt<200)) radiooversigt.gettouchbutton=true;
          }


          // menu entry (81)
          if (id==SHOW_MUSIC_OVERVIEW) {
            printf("Music selected \n");
            vis_radio_oversigt = false;
            vis_music_oversigt = !vis_music_oversigt;
            vis_spotify_oversigt = false;
            vis_radio_or_music_oversigt=false;
            printf("Enable music view\n ");
          }

          if (vis_film_oversigt) {
            film_oversigt.dragging=false;
            // giv slip → brug sidste bevægelse som start-hastighed
            film_oversigt.velocityX = film_oversigt.lastDX * 3.0f;
            film_oversigt.velocityY = -film_oversigt.lastDY * 3.0f;
            int upTime=glutGet(GLUT_ELAPSED_TIME);
            int dt=upTime - film_oversigt.downTimeMs;
            if ((!film_oversigt.moved) && (dt<200)) film_oversigt.gettouchbutton=true;
          }

          if (vis_radio_oversigt) {
            rknapnr=id-99;
            radio_key_selected = rknapnr-1;
            radiooversigt.selected_icon_in_view=rknapnr-1;
          }
          if (id==SHOW_RADIO_OVERVIEW) {  // 80
            vis_radio_oversigt =! vis_radio_oversigt;
          }

          // menu entry (2)
          if (id==MUSIC) {
            printf("Radio og Music selected \n");
            vis_radio_or_music_oversigt=!vis_radio_or_music_oversigt;
            vis_radio_oversigt = false;
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_setup_torrent = false;
            do_show_tvgraber = false;
            strcpy(keybuffer,"");
          }
          // menu entry (13)
          if (id==MOVIE_STREAM) {
            printf("Movie Stream selected \n");
            vis_stream_or_movie_oversigt =! vis_stream_or_movie_oversigt;
            // close all other setup windows
            vis_radio_oversigt = false;
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_radio_or_music_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
            strcpy(keybuffer,"");
          }
          // menu entry (3)
          if ((id==MOVIE) && (vis_stream_or_movie_oversigt==true)) {
            printf("Movie selected \n");
            vis_stream_or_movie_oversigt = false;
            // close all other setup windows
            vis_radio_oversigt = false;
            vis_music_oversigt = false;
            vis_film_oversigt = !vis_film_oversigt;
            vis_tv_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_radio_or_music_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
            strcpy(keybuffer,"");
          }
          // menu entry
          if (id == RECORDED) {
            printf("Recorded selected \n");
            // close all other setup windows
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_radio_or_music_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_recorded_oversigt =! vis_recorded_oversigt;
            vis_stream_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
            strcpy(keybuffer,"");
          }

          if (id == SHOW_TIDAL_OVERVIEW) {
            printf("Tidal selected \n");
            // close all other setup windows
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_radio_or_music_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = !vis_tidal_oversigt;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
            strcpy(keybuffer,"");
          }

          if (id == SHOW_SPOTIFY_OVERVIEW) {
            printf("Spotify selected \n");
            // close all other setup windows
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_radio_or_music_oversigt = false;
            vis_spotify_oversigt=!vis_spotify_oversigt;
            vis_tidal_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
          }

          // menu entry
          if ((id == SETUP) && (!(do_show_tidal_search_oversigt)) && (!(vis_tidal_oversigt)) && (!(vis_music_oversigt)) && (!(vis_spotify_oversigt))) {
            do_show_setup =! do_show_setup;
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_stream_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            vis_radio_or_music_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_rss = false;
            do_show_setup_spotify = false;
            do_show_setup_torrent = false;
          }


          
          // radio music tidal spotiy menu is enabled
          // vis_radio_or_music_oversigt
          if (vis_radio_or_music_oversigt) {
            if (id==29) {
              vis_radio_or_music_oversigt = ! vis_radio_or_music_oversigt;
            }
          }

          if (vis_radio_oversigt) {
            // stop play
            if (id==9) {
              // radiooversigt.
              if (channel)
                channel->stop();
              snd=0;
            }            
            if (id==23) {
              radiooversigt.onScroll(-11.25);
            }
            if (id==24) {
              radiooversigt.onScroll(+11.25);
            }
          }
          
          // stream / movie v
          if (vis_stream_or_movie_oversigt) {
            if (id==29) {
              if (vis_film_oversigt) vis_film_oversigt=false;
              vis_stream_or_movie_oversigt =! vis_stream_or_movie_oversigt;
            }
            // stream view
            if ((id==84) && (vis_stream_or_movie_oversigt)) {
              vis_stream_oversigt = true;
              vis_stream_or_movie_oversigt = false;
            }
            // movie view
            if ((id==81) && (vis_stream_or_movie_oversigt)) {
              vis_film_oversigt = true;
              vis_stream_or_movie_oversigt = false;
            }
          }
          //
          // show movie overview
          //
          if (vis_film_oversigt) {
            if ((film_oversigt.editmode==0) && (film_oversigt.dragging==false) && (film_oversigt.moved==false) && (film_oversigt.gettouchbutton==true)) {
              // har vi valgt en film.
              if (id>99) {
                printf("Vælg film \n");
                fknapnr=id-99;
                film_key_selected=fknapnr;
                // do_zoom_film_aktiv_nr=fknapnr;
                film_oversigt.selected_icon_in_view=fknapnr;
                do_zoom_film_cover=true;
              }
              // start movie
              if (id==8) {
                strcpy(configdefaultplayer,"Internal");	                 	// default sound player (fmod) (default) movie player
                printf("set flag start film \n");
                startmovie=true;
              }
              // stop movie
              if ((id==9) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                stopmovie=true;
                do_zoom_film_cover=false;
              }
              // close windows again
              if ((id==3) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                vis_music_oversigt=false;
                vis_stream_or_movie_oversigt=true;
              }
              
              if ((id==23) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                printf("Scroll \n");
                film_oversigt.onScroll(-11.25);
              }
              if ((id==24) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                printf("Scroll \n");
                film_oversigt.onScroll(+11.25);
              }
              
              if ((film_oversigt.film_is_playing) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                // back movie vlc
                if (id==10) {
                  film_oversigt.tilbage10sec();
                  do_show_film_status_info=true;
                }
                // ff movie vlc
                if (id==11) {
                  film_oversigt.frem10sec();
                  do_show_film_status_info=true;
                }
              }
            }
            // edit movie
            if ((film_oversigt.editmode==2) && (aktiv_tv_oversigt.vis_tv_guide==false) && (film_oversigt.dragging==false) && (film_oversigt.moved==false) && (film_oversigt.gettouchbutton==true)) {
              if (id==31) {
                printf("Get cover file \n ");
                std::string cover_filename=film_oversigt.select_file_name("/data2/Movie/");
                cover_filename.erase(std::remove(cover_filename.begin(), cover_filename.end(), '\n'), cover_filename.cend());
                if (cover_filename.length()>0) {
                  film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmcoverfile((char *) cover_filename.c_str());
                  film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].settextureidfile((char *) cover_filename.c_str());
                  film_oversigt.update_movierec_in_db(film_oversigt.selected_icon_in_view);
                  write_logfile(logfile,(char *) "Load new cover file." );
                }
                id=0;
              }
              // update movie record in db
              if (id==32) {
                printf("update movie record in db.\n ");
                // update edited movie to db again
                std::string textbuffer=textEditor.GetText();
                textEditor.Clear();
                film_oversigt.filmoversigt[film_oversigt.selected_icon_in_view-1].setfilmsubtitle((char *) textbuffer.c_str());
                film_oversigt.update_movierec_in_db_all(film_oversigt.selected_icon_in_view-1);
                film_oversigt.editmode=0;
                write_logfile(logfile,(char *) "Update movie info to db." );
                id=0;
              }
            }
          }
          // used in new movie overview
          if (vis_nyefilm_oversigt) {
            if (id>99) {
              printf("Vælg film \n");
              fknapnr=id-99;
              film_key_selected=fknapnr;
              film_oversigt.selected_icon_in_view=fknapnr;
              do_zoom_film_cover=true;
            }
            if (id==8) {
              strcpy(configdefaultplayer,"Internal");	                 	// default sound player (fmod) (default) movie player
              printf("set flag start film \n");
              startmovie=true;
              // vis_nyefilm_oversigt=false;
            }
            // stop movie
            if ((id==9) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
              stopmovie=true;
              do_zoom_film_cover=false;
            }
          }

          /*
          // DET er vist CLOSE
          // vis_stream_or_movie_oversigt
          if (id==3) {
            vis_stream_or_movie_oversigt=true;
            do_zoom_film_cover=false;
          }
          */

          // music select play stuf
          if ((vis_music_oversigt) && (do_zoom_music_cover==false)) {
            if (id>99) {
              mknapnr=id-99;
              ask_open_dir_or_play_music=true;
            }
            
            // back button
            if ((id==100) && (strcmp(musicoversigt.get_album_name(mknapnr-1),"   BACK")==0)) {
              ask_open_dir_or_play_music=false;
              mknapnr=0;
              musicoversigt.opdatere_music_oversigt(0);  // hent list over mythtv playlistes
            }

            if (ask_open_dir_or_play_music) {
              // play music            
              if (id==20) {
                do_play_music_aktiv_table_nr=1;
                ask_open_dir_or_play_music=false;
                do_play_music_cover=true;
                do_find_playlist=true;
              }
              // swap
              if (id==22) {
                printf("swap\n");
                for(int x=0;x<dirmusic.numbersinlist();x++) {
                  dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(x)),x);
                }
                id=0;
              }
            }
          }
          
          if ((vis_music_oversigt) && (do_zoom_music_cover==false)) {
            if (id==23) {
              musicoversigt.onScroll(-11.25);
            }
            if (id==24) {
              musicoversigt.onScroll(+11.25);
            }
          }

          // stop music
          if ((vis_music_oversigt) && (do_zoom_music_cover)) {
            if (id==9) {
              do_stop_music=true;
              do_zoom_music_cover=false;
              musicoversigt.set_music_is_playing(false);
              snd=0;
              id=0;
              dirmusic.emtydirmusic();      // clear last list
              sound->release();							// stop last playing song
              mknapnr = 0;
            }
          }


          // music control
          if ((musicoversigt.get_music_is_playing()) && (do_zoom_music_cover)) {
            // play next
            if ((id==11) && (do_play_music_aktiv_table_nr<dirmusic.numbersinlist()+1)) {
              if (sound) sound->release();
              printf("Play next \n");
              id=0;
              do_shift_song=true;
              do_play_music_aktiv_table_nr++;
            }
            // play last
            if (id==10) {
              printf("Play last \n");
              if (do_play_music_aktiv_table_nr>0) {
                if (sound) sound->release();
                id=0;
                do_shift_song=true;
                do_play_music_aktiv_table_nr--;
              }
            }
          }

          //
          // hent music dir if subdir.
          //
          // old ver if ((mknapnr>0) && (vis_music_oversigt) && (ask_open_dir_or_play_aopen==false) && (dirmusic.numbersinlist()==0)) {

          if ((vis_music_oversigt) && (do_shift_song==false)) {
            if ((mknapnr>0) && (vis_music_oversigt) && (ask_open_dir_or_play_aopen==false)) {
              if (((mknapnr-1==0) || (musicoversigt.get_directory_id(mknapnr-1)!=0)) && (!(do_zoom_music_cover))) {
                // normal song list
                if (musicoversigt.get_album_type(mknapnr-1)==0) {
                  fprintf(stderr,"Normal dir id load.\n");
                  do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1); 	// set det aktiv dir id
                  antal_songs=hent_antal_dir_songs(musicoversigt.get_directory_id(mknapnr-1));    // loader antal dir/song i dir id
                  fprintf(stderr,"Found numbers of songs 1 :%2d name %s \n",antal_songs,musicoversigt.get_album_name(mknapnr-1));
                  if ((antal_songs==0) || (musicoversigt.get_directory_id(mknapnr-1)==0)) {
                    ask_open_dir_or_play_aopen = true;
                  } else {
                    ask_open_dir_or_play_aopen = false;
                  }
                } else {
                  // here playlist load
                  fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                  // playlist loader
                  do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                  fprintf(stderr,"playlist nr %d  ",do_play_music_aktiv_nr);
                  if (do_play_music_aktiv_nr>0) {
                    antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                  } else antal_songs=0;
                  // write debug log
                  sprintf(debuglogdata,"Found numbers of songs in playlist :%2d",antal_songs);
                  write_logfile(logfile,(char *) debuglogdata);
                  if (antal_songs==0) {
                    ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                  } else {
                    ask_open_dir_or_play_aopen = false;
                  }
                }
                if (do_play_music_aktiv_nr) {						// er der et dirid/playlistid

                  printf("Set ask_open_dir_or_play = true \n ");

                  ask_open_dir_or_play = true;						// yes ask om de skal spilles
                } else ask_open_dir_or_play = true;
              } else {
                printf("********************** Playlist selected \n");

                fprintf(stderr,"mknapnr=%d Playlist loader af playlist id %d \n",mknapnr,musicoversigt.get_directory_id(mknapnr-1));
                // playlist loader
                // 
                // do_play_music_aktiv_nr=musicoversigt.get_directory_id(mknapnr-1);
                //
                do_play_music_aktiv_nr=mknapnr-1;

                fprintf(stderr,"playlist nr %d  \n",do_play_music_aktiv_nr);
                if (do_play_music_aktiv_nr>0) {
                  antal_songs=hent_antal_dir_songs_playlist(do_play_music_aktiv_nr);
                } else antal_songs=0;
                // write debug log
                sprintf(debuglogdata,"Found numbers of songs 2:%2d",antal_songs);
                write_logfile(logfile,(char *) debuglogdata);
                if (antal_songs==0) {
                  ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
                } else {
                  ask_open_dir_or_play_aopen = false;
                }
                if (do_play_music_aktiv_nr) {                               // er der et dirid/playlistid
                  ask_open_dir_or_play = true;                              // yes ask om de skal spilles
                } else ask_open_dir_or_play = true;
              }
            }
          }



          // auto open dir if dirid
          if ((vis_music_oversigt) && (do_shift_song==false)) {
            // auto open dir
            if ((ask_open_dir_or_play_aopen && (musicoversigt.get_directory_id(mknapnr-1) > 0))) {
              ask_open_dir_or_play=false;
              ask_open_dir_or_play_aopen=false;
              // load playlist
              if (musicoversigt.get_album_type(mknapnr-1)==-1) {
                // write debug log
                sprintf(debuglogdata,"Open/read playlist id %d ",musicoversigt.get_directory_id(mknapnr-1));
                write_logfile(logfile,(char *) debuglogdata);
                sprintf(debuglogdata,"Opdatere musicarray henter playlist oversigt");
                // write debug log
                write_logfile(logfile,(char *) debuglogdata);

                // hent playlist oversigt
                // new ver
                musicoversigt.opdatere_music_oversigt_playlists();	// hent list over mythtv playlistes

              } else {
                // write debug log
                sprintf(debuglogdata,"Opdate musicdb Geting overview for directory id = %d ",musicoversigt.get_directory_id(mknapnr-1));
                write_logfile(logfile,(char *) debuglogdata);
                // opdate fra mythtv-backend if avable

                // New ver
                // hent fra db
                if (do_show_music_search_oversigt==false) {
                  if (musicoversigt.opdatere_music_oversigt(musicoversigt.get_directory_id(mknapnr-1))>0) {
                    do_update_music_icons_now_in_thread=true;                                      // opdatere icons nu
                    // musicoversigt.opdatere_music_oversigt_icons();                                  // load icons
                  } else {
                    // opdatere music oversigt fra intern path
                    fprintf(stderr,"nr %d path=%s\n",mknapnr-1,musicoversigt.get_album_path(mknapnr-1));

                    // New ver
                    if (musicoversigt.opdatere_music_oversigt_nodb()==0) {
                      // no update posible
                      fprintf(stderr,"No Music loaded/found by internal loader.\n");
                      write_logfile(logfile,(char *) "No Music loaded/found by internal loader.");
                    }
                  }
                }
              }
              /// reset mouse/key pos in vis_music_oversigt
              mknapnr = 0;
              music_key_selected = 1;
              music_select_iconnr = 0;
            }
          }

          if (do_shift_song) {
            aktiv_playlist.m_play_playlist(aktivplay_music_path,do_play_music_aktiv_table_nr-1);			// hent aktive sang i playliste
            sound->release();							// stop last playing song
            musicoversigt.set_music_is_playing(false);
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
              musicoversigt.set_music_is_playing(true);
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
            // write debug log
            sprintf(debuglogdata,"User Next song %s ",aktivplay_music_path);
            write_logfile(logfile,(char *) debuglogdata);
          }



          

          // tidal select play stuf + up / down buttons
          if ((vis_tidal_oversigt) || (do_show_tidal_search_oversigt)) {
            // ask play tidal.
            if (id>99) {
              tidalknapnr=id-99;
              ask_open_dir_or_play_tidal=true;
            }
            // play tidal (button id 20)
            if (id==TIDAL_PLAY_BUTTON) {
              tidal_oversigt.startplay=true;
            }
            // tidal search button
            if (id==5) {
              do_show_tidal_search_oversigt=!do_show_tidal_search_oversigt;
            }
            // down button
            if (id==DOWNKEY) {
              tidal_oversigt.onScroll(-11.25);
            }
            // up button
            if (id==UPKEY) {
              tidal_oversigt.onScroll(+11.25);
            }
          }
          // do we play tidal stuf
          if ((tidal_oversigt.get_tidal_playing_flag()) && (!(vis_tv_oversigt))) {
            // play next
            if (id==11) {
              if (sound) sound->release();
              tidal_oversigt.tidal_next_play();
            }
            // play last
            if (id==10) {
              if (sound) sound->release();
              tidal_oversigt.tidal_last_play();
            }
            // stop tidal music
            if (id==9) {
              do_stop_tidal=true;
              do_zoom_tidal_cover=false;
              tidal_oversigt.set_tidal_playing_flag(false);
            }
            // hide/show info window
            if (id==CLOSE) {
              do_zoom_tidal_cover=!do_zoom_tidal_cover;
            }
          }

          // tv guide
          if ((vis_tv_oversigt) || (aktiv_tv_oversigt.vis_tv_guide)) {
            if ((id==CLOSE) && (do_show_tv_kanal_info==false)) {
              vis_tv_oversigt=false;
            }
            // luk tv guide program info window
            if ((id==CLOSE) && (do_show_tv_kanal_info==true)) {
             do_show_tv_kanal_info=false; 
            }

            // goto tidligere tid
            if (id==BACKINTIME) {
              printf("tvguide back in time\n");
              if (aktiv_tv_oversigt.vistvguidekl>1) {
                aktiv_tv_oversigt.visdato_unixtime=aktiv_tv_oversigt.visdato_unixtime - 3600;
                aktiv_tv_oversigt.vistvguidekl=aktiv_tv_oversigt.vistvguidekl-1;
                aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass);
              }
            }
            if (id==FORWARDINTIME) {
              printf("tvguide back in time\n");
              if (aktiv_tv_oversigt.vistvguidekl<24) {
                aktiv_tv_oversigt.visdato_unixtime=aktiv_tv_oversigt.visdato_unixtime + 3600;
                aktiv_tv_oversigt.vistvguidekl=aktiv_tv_oversigt.vistvguidekl+1;
                aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass);                
              }
            }

            if (id>100) {
              tvknapnr=id-99;
              int kanaler=0;
              int programmer=0;
              bool program_fundet=false;
              aktiv_tv_oversigt.vis_kanal_nr=-1;
              aktiv_tv_oversigt.vis_program_nr=-1;

              while (((kanaler<aktiv_tv_oversigt.tvkanaler.size()) && (program_fundet==false))) {
                programmer=0;
                while((programmer<aktiv_tv_oversigt.tvkanaler[kanaler].programs.size() && (program_fundet==false))) {
                  if (aktiv_tv_oversigt.tvkanaler[kanaler].programs[programmer].program_idnr==id) {
                    program_fundet = true;
                  }
                  if (program_fundet) {
                    // set i class den som skal vises
                    aktiv_tv_oversigt.vis_kanal_nr=kanaler;
                    aktiv_tv_oversigt.vis_program_nr=programmer;
                    do_show_tv_kanal_info=true; 
                  }
                  programmer++;
                }
                kanaler++;
              }
            }
          }




          
          //
          // stream control
          //
          // if ((vis_stream_oversigt) && (streamoversigt.dragging==false)  && (streamoversigt.moved==false) && (streamoversigt.gettouchbutton==true)) {

          if (vis_stream_oversigt) {
            // set flags for play stream (check url)
            if (id>=100) printf("toplevel %d \n ",streamoversigt.toplevel);
            if ((id-100>=0) && (do_zoom_stream_cover==false) && (startstream==false) && (streamoversigt.toplevel==false)) {
              printf("start stream");
              startstream=true;                                                                   // flag for starting the stream
              do_play_stream=true;
              sknapnr=id-99;
              streamoversigt.selected_icon_in_view=sknapnr;
            }
          }


          if ((vis_stream_oversigt) && (streamoversigt.moved==false)) {
            if (id>=100) {
              sknapnr=id-99;
              streamoversigt.selected_icon_in_view=sknapnr;
              
              if (streamoversigt.type==0) {
                strncpy(temptxt,streamoversigt.get_stream_name(id-100),200);
                fprintf(stderr,"stream nr %d name %s \n ",id-1,temptxt);

                if ((streamoversigt.rss_search_podcast_string!="") && (strcmp(temptxt,"BACK")==0)) {
                  // streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");
                  // streamoversigt.update_search_podcast_stream_view();
                  streamoversigt.rss_search_podcast_string="";
                  keybufferindex=0;
                  keybuffer[0]='\0';
                  streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                }
                streamoversigt.clean_stream_oversigt();
                streamoversigt.opdatere_stream_oversigt(temptxt,(char *)"");
                printf("Antal streams i oversigt streamoversigt %d \n",streamoversigt.streamantal());
              } else if (streamoversigt.type!=1) {
                // update
                streamoversigt.clean_stream_oversigt();
                streamoversigt.opdatere_stream_oversigt((char *) streamoversigt.get_stream_name(sknapnr-1),(char *) streamoversigt.get_stream_path(id-100));
                do_play_stream=false;
                sknapnr=0;
              } else {
                // back button
                if ((id-100)==0) {
                  if (streamoversigt.type==2) {
                    // one level up
                    streamoversigt.clean_stream_oversigt();
                    streamoversigt.opdatere_stream_oversigt((char *) streamoversigt.get_stream_name(id-100),(char *)"");
                    do_play_stream=false;
                    sknapnr=0;
                  } else {
                    // jump to top
                    streamoversigt.clean_stream_oversigt();
                    streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");
                    do_play_stream=false;
                    sknapnr=0;
                  }
                } else {
                  // do play stream
                  if (startstream) {
                    if ((do_play_stream) && (stream_playnr==0)) {
                      do_zoom_stream_cover=true;
                      stream_playnr=sknapnr;                                                  // rember the stream we play
                      strcpy(stream_playing_name,streamoversigt.get_stream_name(stream_playnr-1));
                      strcpy(stream_playing_desc,streamoversigt.get_stream_desc(stream_playnr-1));
                      stream_playing_icon=streamoversigt.get_texture(stream_playnr-1);
                    }
                    
                    strcpy(configdefaultplayer,"Internal");

                    if (strcmp("Internal",configdefaultplayer)!=0) {
                      fprintf(stderr,"Start stream nr %d Player is firefox if needed.\n",sknapnr);
                      if (channel) result = channel->stop();
                      std::string surl;
                      if (streamoversigt.get_stream_url(sknapnr-1)) {
                        surl = streamoversigt.get_stream_url(sknapnr-1);
                      } else surl = "";
                      if (surl.find("youtube") != std::string::npos) {
                        strcpy(systemcommand,"/bin/sh /usr/bin/firefox ");
                        strcat(systemcommand,"'");
                        if (sknapnr>0) {
                          if (strncmp(streamoversigt.get_stream_url(sknapnr-1),"mythflash:",10)==0) {
                            strcat(systemcommand,"http://");
                            strcat(systemcommand,streamoversigt.get_stream_url(sknapnr)+10);
                          } else strcat(systemcommand,streamoversigt.get_stream_url(sknapnr-1));
                          strcat(systemcommand,"' &");
                          if (do_system_call(systemcommand)==0) {
                            // vis_error=true;
                            // vis_error_timeout=60;
                          }
                        }
                      } else {
                        result = sndsystem->createSound(streamoversigt.get_stream_url(sknapnr-1), FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
                        result = sndsystem->playSound(sound,NULL, false, &channel);
                      }
                    } else {
                      // start play stream or show rss page
                      // write to log file
                      sprintf(debuglogdata,"Stream to play %s ",streamoversigt.get_stream_url(sknapnr-1));
                      write_logfile(logfile,(char *) debuglogdata);
                      sprintf(debuglogdata,"Start stream. Player is internal ");
                      write_logfile(logfile,(char *) debuglogdata);
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
                          if (do_system_call(systemcommand)==0) {
                            // vis_error=true;
                            // vis_error_timeout=60;
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
                }
              }
            }
            // bliver IKKE kaldt
            // close icon pressed - close open steam window
            if (id==3) {
              vis_stream_or_movie_oversigt=true;
              vis_stream_oversigt=false;
              id=0;
              sknapnr=0;
            }
            if ((id==PLAYBUTTON) && (do_zoom_stream_cover)) {

            }
            if ((id==PAUSEBUTTON) && (do_zoom_stream_cover)) {
              if (do_pause_stream) do_pause_stream=false; else do_pause_stream=true;
              id=0;
            }
            if (id==STOPBUTTON) {
              if (streamoversigt.stream_is_playing) {
                do_zoom_stream_cover = false;
                do_stop_stream = true;                                            // flag to stop play
                stopstream = true;                                                // flag to stop play
                do_play_stream = false;                                           // we are not play any more
                stream_jump = false;                                              // we can not jump in stream any more
                streamoversigt.stopstream();
                id=0;
              }
            }
          }

          // Stop play streams
          if ((id==9) && (vis_stream_oversigt) && (do_zoom_stream_cover)) {
            streamoversigt.stopstream();
            id=0;
          }

          // start play radio
          if ((vis_radio_oversigt) && (rknapnr>0)) {
            do_play_radio=!do_play_radio;
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_radio_or_music_oversigt = false;
            vis_spotify_oversigt=false;
            vis_tidal_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
          }

          if ((do_show_setup) && (id == 37)) {
            do_show_setup = false;
            vis_music_oversigt = false;
            vis_film_oversigt = false;
            vis_tv_oversigt = false;
            vis_radio_or_music_oversigt = false;
            vis_spotify_oversigt = false;
            vis_tidal_oversigt = false;
            vis_recorded_oversigt = false;
            vis_stream_oversigt = false;
            vis_stream_or_movie_oversigt = false;
            do_show_tvgraber = false;
            do_show_setup_torrent = false;
          }

          if (do_show_torrent) {
            if (id==40) {
              do_show_torrent = false;
            }
            if (id==41) {
              do_show_load__torrent_file =! do_show_load__torrent_file;
            }
            if (do_show_torrent_options) {
              torrent_downloader.show_torrent_options();
            }
            if (do_show_torrent_options_move) {
              torrent_downloader.show_move_options();
            }
            // show we move the file
            if (do_move_torrent_file_now) {
              torrent_downloader.show_file_move();
            }
            // select file
            if (do_show_load__torrent_file) {
              torrent_downloader.select_file_name();
              do_show_load__torrent_file = false;
            }
          }

          if ((id == EXIT_PROGRAM) && (do_show_torrent==false)) {
            exit(0);
          }

      }
      break;
    case GLUT_RIGHT_BUTTON:
      // close ask film again
      if ((vis_film_oversigt) && (state==GLUT_UP) && (film_oversigt.editmode==0)) {
        do_zoom_film_cover=!do_zoom_film_cover;
        do_zoom_spotify_cover=false;
        do_zoom_tidal_cover=false;
        do_zoom_music_cover=false;
        do_zoom_radio_cover=false;
        do_zoom_stream_cover=false;
      }
      if ((vis_tidal_oversigt) && (ask_open_dir_or_play_tidal==false) && (state==GLUT_UP)) {
        if (snd) do_zoom_tidal_cover=!do_zoom_tidal_cover;
      } else if ((vis_spotify_oversigt) && (state==GLUT_UP)) {
        do_zoom_spotify_cover=!do_zoom_spotify_cover;
      } else if ((vis_radio_oversigt) && (state==GLUT_UP)) {
        if (snd) do_zoom_radio_cover=!do_zoom_radio_cover;
      } else if ((vis_music_oversigt) && (state==GLUT_UP)) {
        if (ask_open_dir_or_play_music==false) {
          if (snd) do_zoom_music_cover=!do_zoom_music_cover;
        } else ask_open_dir_or_play_music=false;
        // mknapnr=0;
      } else if ((vis_stream_oversigt) && (state==GLUT_UP)) {
        do_zoom_stream_cover=!do_zoom_stream_cover;
      } else if ((tidal_oversigt.tidal_do_we_play()==true) && (vis_film_oversigt==false) && (vis_tv_oversigt==false) && (state==GLUT_UP)) {
        do_zoom_tidal_cover=!do_zoom_tidal_cover;
        printf("tidal_oversigt.tidal_do_we_play() = true \n");
      }
      // new movie view
      if (vis_nyefilm_oversigt) {
        do_zoom_film_cover=false;
        fknapnr=0;
      }
      /*
      if (ask_open_dir_or_play_music) {
        ask_open_dir_or_play_music=false;
        mknapnr=0;
      }
      */
      break;
  }

  int retfunc=0;
  // All scroll calls
  if (vis_music_oversigt) {
    // scroll down
    if ((retfunc==0) && (button==4)) {
      musicoversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc==0) && (button==3)) {
      musicoversigt.onScroll(-1.0);
    }
    // scroll down
    if ((retfunc==5) && (button==0)) {
      musicoversigt.onScroll(+11.22);
    }
    // scroll up
    if ((retfunc==4) && (button==0)) {
      musicoversigt.onScroll(-11.22);
    }
  }
  if (vis_tidal_oversigt) {
    if ((retfunc == 0 ) && (button == 4 )) {
      tidal_oversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc == 0 ) && (button == 3 )) {
      tidal_oversigt.onScroll(-1.0);
    }
  }

  if (vis_radio_oversigt) {
    if ((retfunc == 0 ) && (button == 4 )) {
      radiooversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc == 0 ) && (button == 3 )) {
      radiooversigt.onScroll(-1.0);
    }
  }

  if (vis_film_oversigt) {
    if ((retfunc == 0 ) && (button == 4 )) {
      film_oversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc == 0 ) && (button == 3 )) {
      film_oversigt.onScroll(-1.0);
    }
  }
  if (vis_stream_oversigt) {
    if ((retfunc == 0 ) && (button == 4 )) {
      streamoversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc == 0 ) && (button == 3 )) {
      streamoversigt.onScroll(-1.0);
    }
  }
  if ((vis_tv_oversigt) || (aktiv_tv_oversigt.vis_tv_guide)) {
    if ((retfunc == 0 ) && (button == 4 )) {
      aktiv_tv_oversigt.onScroll(+1.0);
    }
    // scroll up
    if ((retfunc == 0 ) && (button == 3 )) {
      aktiv_tv_oversigt.onScroll(-1.0);
    }
  }
}





// ****************************************************************************************
//
// Keyboard handler
//
// ****************************************************************************************


void handleKeypress(unsigned char key, int x, int y) {
  std::string tmptxt;
  const char optionmenukey='O';
  saver_irq=true;                                     // stop screen saver
  stream_loadergfx_started_break=true;		// break tread stream gfx loader
  if (key==SOUNDUPKEY) {
    #ifdef ENABLE_SPOTIFY
    if (!(vis_spotify_oversigt)) {
      if ((configsoundvolume+0.05)<1.0f) configsoundvolume+=0.05f;
      #if defined USE_FMOD_MIXER
      if (sndsystem) channel->setVolume(configsoundvolume);
      #endif
      //save_config((char *) "/etc/mythtv-controller.conf");
      show_volume_info=true;					// show volume info window
      vis_volume_timeout=120;
    }
    #endif
  }
  if (key==SOUNDDOWNKEY) {                               // volume down
    if (!(vis_spotify_oversigt)) {
      if ((configsoundvolume-0.00)>0) configsoundvolume-=0.05f;
      #if defined USE_FMOD_MIXER
      if (sndsystem) channel->setVolume(configsoundvolume);
      #endif
      //save_config((char *) "/etc/mythtv-controller.conf");
      show_volume_info=true;					// show volume info window
      vis_volume_timeout=120;
    }
  }

  if (multi_editor) {
    switch (key) {
        case 8:
            textEditor.Backspace();
            break;

        case 13:
            textEditor.NewLine();
            break;

        case 127:
            textEditor.DeleteCharacter();
            break;

        case 27:
            textEditor.SetActive(false);
            break;

        case 5:
            if (film_oversigt.editmode==0) film_oversigt.editmode=2;          // ctrl e pressed
            else if (film_oversigt.editmode==2) film_oversigt.editmode=0;
            break;

        default:
            textEditor.InsertCharacter(
                static_cast<char>(key)
            );
            break;
    }
    if (key==27) multi_editor=false;
    if (key!=27) key=0;
  }
  // show tv guide as overlay over movie playing
  if (film_oversigt.film_is_playing) {
    if ((key==13) && (aktiv_tv_oversigt.vis_tv_guide)) {
      // show selected tvguide program info.
      
      aktiv_tv_oversigt.showandsetprginfo();
    }
    if (key=='t') aktiv_tv_oversigt.vis_tv_guide=!aktiv_tv_oversigt.vis_tv_guide;
  }

  if (((do_show_setup_keys) && (key!=27)) || (key!=SOUNDUPKEY) && (key!=SOUNDDOWNKEY) && (key!=127) && (key!='S') && (key!='*') && (key!='U') && (key!=117) && (key!=optionmenukey) && (key!=13) && (key!=27) || ((vis_spotify_oversigt) && (key!='*') && (key!=13) && (key!=27)) || ((vis_tidal_oversigt) && (key!='*') && (key!=13) && (key!=27)) || ((vis_film_oversigt) && (key!=13) && (key!=27) && (key!=117)) || ((tidal_oversigt.do_setup_tidal_start_entry) && (key!=13) && (key!=27) && (key!=117)) || ((vis_radio_oversigt) && (key!='u') && (key!=optionmenukey) && (key!=27 && (key!=13)))) {

    if (do_show_setup_rss) {
      switch(rssstreamoversigt.setup_select_linie) {
        case 0: keybuffer1=rssstreamoversigt.get_stream_name_std(0+configrss_ofset);
                break;
        case 1: keybuffer1=rssstreamoversigt.get_stream_url_std(0+configrss_ofset);
                break;
        case 2: keybuffer1=rssstreamoversigt.get_stream_name_std(1+configrss_ofset);
                break;
        case 3: keybuffer1=rssstreamoversigt.get_stream_url_std(1+configrss_ofset);
                break;
        case 4: keybuffer1=rssstreamoversigt.get_stream_name_std(2+configrss_ofset);
                break;
        case 5: keybuffer1=rssstreamoversigt.get_stream_url_std(2+configrss_ofset);
                break;
        case 6: keybuffer1=rssstreamoversigt.get_stream_name_std(3+configrss_ofset);
                break;
        case 7: keybuffer1=rssstreamoversigt.get_stream_url_std(3+configrss_ofset);
                break;
        case 8: keybuffer1=rssstreamoversigt.get_stream_name_std(4+configrss_ofset);
                break;
        case 9: keybuffer1=rssstreamoversigt.get_stream_url_std(4+configrss_ofset);
                break;
        case 10:keybuffer1=rssstreamoversigt.get_stream_name_std(5+configrss_ofset);
                break;
        case 11:keybuffer1=rssstreamoversigt.get_stream_url_std(5+configrss_ofset);
                break;
        case 12:keybuffer1=rssstreamoversigt.get_stream_name_std(6+configrss_ofset);
                break;
        case 13:keybuffer1=rssstreamoversigt.get_stream_url_std(6+configrss_ofset);
                break;
        case 14:keybuffer1=rssstreamoversigt.get_stream_name_std(7+configrss_ofset);
                break;
        case 15:keybuffer1=rssstreamoversigt.get_stream_url_std(7+configrss_ofset);
                break;
        case 16:keybuffer1=rssstreamoversigt.get_stream_name_std(8+configrss_ofset);
                break;
        case 17:keybuffer1=rssstreamoversigt.get_stream_url_std(8+configrss_ofset);
                break;
        case 18:keybuffer1=rssstreamoversigt.get_stream_name_std(9+configrss_ofset);
                break;
        case 19:keybuffer1=rssstreamoversigt.get_stream_url_std(9+configrss_ofset);
                break;
        case 20:keybuffer1=rssstreamoversigt.get_stream_name_std(10+configrss_ofset);
                break;
        case 21:keybuffer1=rssstreamoversigt.get_stream_url_std(10+configrss_ofset);
                break;
        case 22:keybuffer1=rssstreamoversigt.get_stream_name_std(11+configrss_ofset);
                break;
        case 23:keybuffer1=rssstreamoversigt.get_stream_url_std(11+configrss_ofset);
                break;
        case 24:keybuffer1=rssstreamoversigt.get_stream_name_std(12+configrss_ofset);
                break;
        case 25:keybuffer1=rssstreamoversigt.get_stream_url_std(12+configrss_ofset);
                break;
        case 26:keybuffer1=rssstreamoversigt.get_stream_name_std(13+configrss_ofset);
                break;
        case 27:keybuffer1=rssstreamoversigt.get_stream_url_std(13+configrss_ofset);
                break;
        case 28:keybuffer1=rssstreamoversigt.get_stream_name_std(14+configrss_ofset);
                break;
        case 29:keybuffer1=rssstreamoversigt.get_stream_url_std(14+configrss_ofset);
                break;
        case 30:keybuffer1=rssstreamoversigt.get_stream_name_std(15+configrss_ofset);
                break;
        case 31:keybuffer1=rssstreamoversigt.get_stream_url_std(15+configrss_ofset);
                break;
        case 32:keybuffer1=rssstreamoversigt.get_stream_name_std(16+configrss_ofset);
                break;
        case 33:keybuffer1=rssstreamoversigt.get_stream_url_std(16+configrss_ofset);
                break;
        case 34:keybuffer1=rssstreamoversigt.get_stream_name_std(17+configrss_ofset);
                break;
        case 35:keybuffer1=rssstreamoversigt.get_stream_url_std(17+configrss_ofset);
                break;
        default:keybuffer1="";
                break;
      }
      keybufferindex=strlen(keybuffer);
    }
    if (do_show_setup_spotify) {
      switch (do_show_setup_select_linie) {
        case 0: strcpy(keybuffer,spotify_oversigt.spotify_client_id);
                keybuffer1=spotify_oversigt.spotify_client_id;
                keybufferindex=strlen(keybuffer);
                break;
        case 1: strcpy(keybuffer,spotify_oversigt.spotify_secret_id);
                keybuffer1=spotify_oversigt.spotify_secret_id;
                keybufferindex=strlen(keybuffer);
                break;
      }
    }
    if (do_show_setup_tidal) {
      switch (do_show_setup_select_linie) {
        case 0: strcpy(keybuffer,tidal_oversigt.tidal_client_id.c_str());
                keybuffer1=tidal_oversigt.tidal_client_id;
                keybufferindex=strlen(keybuffer);
                break;
        case 1: strcpy(keybuffer,tidal_oversigt.tidal_secret_id.c_str());
                keybuffer1=tidal_oversigt.tidal_secret_id;
                keybufferindex=strlen(keybuffer);
                break;
      }
    }

    if (do_show_setup_torrent) {
      switch (do_show_setup_select_linie) {
        case 0: if (torrent_downloader.trash_torrent) {
                  keybuffer1="Y";
                } else {
                  keybuffer1="N";
                }
                keybufferindex=keybuffer1.length();
                break;
        case 1: if (torrent_downloader.automove_to_movie_path) {
                  keybuffer1="Y";
                } else {
                  keybuffer1="N";
                }
                keybufferindex=keybuffer1.length();
                break;
        case 2:keybuffer1=torrent_downloader.downloadpath;
                keybufferindex=keybuffer1.length();
                break;
        default:keybuffer1="";
                keybufferindex=0;
                break;
      }
    }

    if ((tidal_oversigt.do_setup_tidal_start_entry)) {
      if (do_show_editor_select_linie<tidal_oversigt.tidal_start_playlist_array.size()) {
        strcpy(keybuffer,tidal_oversigt.tidal_start_playlist_array[do_show_editor_select_linie].c_str());
        keybuffer1=tidal_oversigt.tidal_start_playlist_array[do_show_editor_select_linie];
        keybufferindex=keybuffer1.length();
      }
    }

    // pause movie if playing (space key pressed)
    if (film_oversigt.film_is_playing) {
      if (key==32) {
        film_oversigt.pausemovie();
      }
    }

    if (vis_film_oversigt) {
      if ((key==5) && (film_oversigt.editmode==0)) film_oversigt.editmode=2;          // ctrl e pressed
      else if ((key==5) && (film_oversigt.editmode==2)) film_oversigt.editmode=0;
      if (film_oversigt.editmode==2) {
        // in edit mode       
        switch (do_show_film_edit_select_linie) {
          case 0: strcpy(keybuffer,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmgenre());
                  keybuffer1=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmgenre();
                  keybufferindex=keybuffer1.length();
                  break;
          case 1: strcpy(keybuffer,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle());
                  keybuffer1=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmtitle();
                  keybufferindex=keybuffer1.length();
                  break;
          case 2: sprintf(keybuffer,"%d",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmlength());
                  keybuffer1=keybuffer;
                  keybufferindex=keybuffer1.length();
                  break;
          case 3: tmptxt=std::to_string(film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmaar());    // year
                  strcpy(keybuffer,tmptxt.c_str());
                  keybuffer1=tmptxt;
                  keybufferindex=keybuffer1.length();
                  break;
          case 4: sprintf(keybuffer,"%d",film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmrating());
                  keybuffer1=keybuffer;
                  keybufferindex=keybuffer1.length();
                  break;
          case 5: break;
          case 6: break;
          case 7: break;
          case 8: strcpy(keybuffer,film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmimdbnummer()); // imdb
                  keybuffer1=film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].getfilmimdbnummer();
                  keybufferindex=keybuffer1.length();
                  break;
          // case 5: // strcpy(keybuffer,film_oversigt.film_get_director((film_key_selected-1)+film_selected_startofset)); // descriptor
                  // keybufferindex=keybuffer1.length();
                  /// break;
          // case 6: // strcpy(keybuffer,film_oversigt.film_get_cast((film_key_selected-1)+film_selected_startofset));
                  // keybufferindex=keybuffer1.length();
                  // break;
          // case 7: // strcpy(keybuffer,film_oversigt.film_get_runtime((film_key_selected-1)+film_selected_startofset));
                  // keybufferindex=keybuffer1.length();
                  // break;
          // case 8: // strcpy(keybuffer,film_oversigt.film_get_plot((film_key_selected-1)+film_selected_startofset));
                  // keybufferindex=keybuffer1.length();
                  // break;
          default:strcpy(keybuffer,"");
                  keybuffer1="";
                  keybufferindex=keybuffer1.length();
        }
      }
    }

    if (vis_stream_oversigt) {
      if (keybufferindex<80) {
        if ((key!=13) && (key!=8) && (key!=127)) {
          keybuffer[keybufferindex]=key;
          keybufferindex++;
          keybuffer[keybufferindex]='\0';
          keybuffer1.push_back(key);
          streamoversigt.rss_search_podcast_string=keybuffer1;
          streamoversigt.update_search_podcast_stream_view();
        }
      }
    }

    if (do_show_setup_backend) {
      switch (do_show_setup_select_linie) {
        case 0: keybuffer1=configbackend;
                break;
        case 1: keybuffer1=configmysqlhost;
                break;
        case 2: keybuffer1=configmysqluser;
                break;
        case 3: keybuffer1=configmysqlpass;
                break;
        case 4: keybuffer1=configmusicpath;
                break;
        case 5: keybuffer1=configmoviepath;
                break;
        case 6: keybuffer1=configpicturepath;
                break;
        case 7: keybuffer1=configrecordpath;
                break;
      }
      keybufferindex=keybuffer1.length();
    }

    if (do_show_setup_screen) {        
      switch (do_show_setup_select_linie) {
        case 0: break;
        case 1: if (urtype==DIGITAL) keybuffer1="digital";
                else if (urtype==ANALOG) keybuffer1="analog";
                else if (urtype==SAVER3D) keybuffer1="3D";                  
                else if (urtype==SAVER3D2) keybuffer1="3D2";
                else if (urtype==PICTURE3D) keybuffer1="PICTURE3D";
                else if (urtype==MUSICMETER) keybuffer1="MUSICMETER";
                else if (urtype==MUSICMETER2) keybuffer1="MUSICMETER2";
                else if (urtype==MUSICMETER3) keybuffer1="MUSICMETER3";
                else if (urtype==UV_METER) keybuffer1="UV-METER";
                else if (urtype==PLASMA) keybuffer1="PLASMA";
                else keybuffer1="analog"; 
                break;
        case 2: keybuffer1=configscreensavertimeout;
                break;
        default:keybuffer1="";
                break;
      }
    }

    if (do_show_setup_keys) {
      switch (do_show_setup_select_linie) {
        case 0: keybufferindex=strlen(configkeyslayout[0].cmdname);
                strcpy(keybuffer,configkeyslayout[0].cmdname);
                keybuffer1=configkeyslayout[0].cmdname;
                break;
        case 1: break;
        case 2: keybufferindex=strlen(configkeyslayout[1].cmdname);
                strcpy(keybuffer,configkeyslayout[1].cmdname);
                keybuffer1=configkeyslayout[1].cmdname;
                break;
        case 3: break;                  
        case 4: keybufferindex=strlen(configkeyslayout[2].cmdname);
                strcpy(keybuffer,configkeyslayout[2].cmdname);                  
                keybuffer1=configkeyslayout[2].cmdname;
                break;
        case 5: break;
        case 6: keybufferindex=strlen(configkeyslayout[3].cmdname);
                strcpy(keybuffer,configkeyslayout[3].cmdname);
                keybuffer1=configkeyslayout[3].cmdname;
                break;
        case 7: break;
        case 8: keybufferindex=strlen(configkeyslayout[4].cmdname);        
                strcpy(keybuffer,configkeyslayout[4].cmdname);
                keybuffer1=configkeyslayout[4].cmdname;
                break;
        case 9: break;                  
        case 10:keybufferindex=strlen(configkeyslayout[5].cmdname);
                strcpy(keybuffer,configkeyslayout[5].cmdname);
                keybuffer1=configkeyslayout[5].cmdname;
                break;
        case 11:break;
        case 12:keybufferindex=strlen(configkeyslayout[6].cmdname);
                strcpy(keybuffer,configkeyslayout[6].cmdname);
                keybuffer1=configkeyslayout[6].cmdname;
                break;
        case 13:break;
        case 14:keybufferindex=strlen(configkeyslayout[7].cmdname);
                strcpy(keybuffer,configkeyslayout[7].cmdname);
                keybuffer1=configkeyslayout[7].cmdname;
        case 15:break;                  
        case 16:keybufferindex=strlen(configkeyslayout[8].cmdname);
                strcpy(keybuffer,configkeyslayout[8].cmdname);
                keybuffer1=configkeyslayout[8].cmdname;
                break;
        case 17: break;                   
      }
    }

    if (do_show_tvgraber) {
      // space bar
      if (key==32) {
        channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=!channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
      }
    }

    // gem key pressed in buffer
    if (keybufferindex<80) {
      // delete key
      if (key==127) {
        if (vis_tidal_oversigt) {
          // slet element i stack
          if (do_show_tidal_search_oversigt==false) {
            tidal_oversigt.delete_record_in_view(tidalknapnr-1);
            if ((tidalknapnr)+tidal_selected_startofset>tidal_oversigt.streamantal()) {
              tidalknapnr--;
            }
          }
        }
      }
      // backspace key
      if (key==8) {
        if (keybufferindex>0) {
          keybufferindex--;
          keybuffer[keybufferindex]=0;
          if (!keybuffer1.empty()) keybuffer1.erase(keybuffer1.size()-1);
        }
        if (vis_stream_oversigt)  {
          streamoversigt.rss_search_podcast_string=keybuffer;
          streamoversigt.update_search_podcast_stream_view();
        }
        if (vis_tidal_oversigt) {
          if (keybufferindex>=0) playlistfilename[keybufferindex]=0;
        }
        if (tidal_oversigt.do_setup_tidal_start_entry) {
          if (do_show_editor_select_linie<tidal_oversigt.tidal_start_playlist_array.size()) tidal_oversigt.tidal_start_playlist_array[do_show_editor_select_linie]=keybuffer;
        }
        
        
        // test
        if (vis_film_oversigt) {
          if (film_oversigt.editmode==2) {
            switch (do_show_film_edit_select_linie) {
              case 0: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmgenre((char *) keybuffer1.c_str());
                      break;
              case 1: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmtitle((char *) keybuffer1.c_str());
                      break;
              case 2: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmlength(atoi((char *) keybuffer1.c_str()));   
                      break;
              case 3: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmaar(atoi((char *) keybuffer1.c_str()));
                      break;
              case 4: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmrating(atoi((char *) keybuffer1.c_str()));
                      break;
              case 5: break;
              case 6: break;
              case 7: break;
              case 8: break;
              case 9: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmimdbnummer((char *) keybuffer1.c_str());
                      break;
              // case 6: // film_oversigt.film_set_director((film_key_selected-1)+film_selected_startofset,keybuffer);
              //         break;
              // case 7: // film_oversigt.film_set_cast((film_key_selected-1)+film_selected_startofset,keybuffer);
              //        break;
              // case 8: // film_oversigt.film_set_runtime((film_key_selected-1)+film_selected_startofset,keybuffer);
              //         break;
              // case 9: // film_oversigt.film_set_plot((film_key_selected-1)+film_selected_startofset,keybuffer);
              //         break;
            }
          }
        }
        
      }
      // stream
      if (vis_stream_oversigt) {
        if (keybufferindex<80) {
          if ((key!=13) && (key!=127)) {
            if (key!=8) {  
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';
              keybuffer1.push_back(key);
              streamoversigt.rss_search_podcast_string=keybuffer;
              streamoversigt.update_search_podcast_stream_view();
            }
          }
        }
      }

      if (do_show_setup_rss) {
        if ((key!=13) && (key!=127)) {
          if (key!=8) {
            // keybuffer[keybufferindex]=key;
            keybufferindex++;
            // keybuffer[keybufferindex]='\0';
            keybuffer1.push_back(key);
          }
          if (key==8) {
            if (keybuffer1.size()>0) keybuffer1.erase(keybuffer1.length()-1);
          }
        }
        switch(rssstreamoversigt.setup_select_linie) {
          case 0: rssstreamoversigt.set_stream_name(0+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 1: rssstreamoversigt.set_stream_url(0+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 2: rssstreamoversigt.set_stream_name(1+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 3: rssstreamoversigt.set_stream_url(1+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 4: rssstreamoversigt.set_stream_name(2+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 5: rssstreamoversigt.set_stream_url(2+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 6: rssstreamoversigt.set_stream_name(3+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 7: rssstreamoversigt.set_stream_url(3+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 8: rssstreamoversigt.set_stream_name(4+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 9: rssstreamoversigt.set_stream_url(4+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 10:rssstreamoversigt.set_stream_name(5+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 11:rssstreamoversigt.set_stream_url(5+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 12:rssstreamoversigt.set_stream_name(6+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 13:rssstreamoversigt.set_stream_url(6+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 14:rssstreamoversigt.set_stream_name(7+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 15:rssstreamoversigt.set_stream_url(7+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 16:rssstreamoversigt.set_stream_name(8+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 17:rssstreamoversigt.set_stream_url(8+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 18: rssstreamoversigt.set_stream_name(9+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 19: rssstreamoversigt.set_stream_url(9+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 20: rssstreamoversigt.set_stream_name(10+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 21: rssstreamoversigt.set_stream_url(10+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 22: rssstreamoversigt.set_stream_name(11+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 23: rssstreamoversigt.set_stream_url(11+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 24: rssstreamoversigt.set_stream_name(12+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 25: rssstreamoversigt.set_stream_url(12+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 26: rssstreamoversigt.set_stream_name(13+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 27: rssstreamoversigt.set_stream_url(13+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 28:rssstreamoversigt.set_stream_name(14+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 29:rssstreamoversigt.set_stream_url(14+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 30:rssstreamoversigt.set_stream_name(15+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 31:rssstreamoversigt.set_stream_url(15+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 32:rssstreamoversigt.set_stream_name(16+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 33:rssstreamoversigt.set_stream_url(16+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 34:rssstreamoversigt.set_stream_name(17+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          case 35:rssstreamoversigt.set_stream_url(17+configrss_ofset,(char *) keybuffer1.c_str());
                  break;
          }
      }
      
      
      if (do_show_setup_backend) {
        if ((key!=13) && (key!=127)) {
          if (key!=8) {
            keybuffer[keybufferindex]=key;
            keybufferindex++;
            keybuffer[keybufferindex]='\0';
            keybuffer1.push_back(key);
          }
          switch(do_show_setup_select_linie) {
            case 0: strcpy(configbackend,keybuffer1.c_str());
                    break;
            case 1: strcpy(configmysqlhost,keybuffer1.c_str());
                    break;
            case 2: strcpy(configmysqluser,keybuffer1.c_str());
                    break;
            case 3: strcpy(configmysqlpass,keybuffer1.c_str());
                    break;
            case 4: strcpy(configmusicpath,keybuffer1.c_str());
                    break;
            case 5: strcpy(configmoviepath,keybuffer1.c_str());
                    break;
            case 6: strcpy(configpicturepath,keybuffer1.c_str());
                    break;
            case 7: strcpy(configrecordpath,keybuffer1.c_str());
                    break;
          }
        }
      }

      //
      // search tidal oversigt
      //
      if (( vis_tidal_oversigt ) && ( ask_open_dir_or_play==false ) && ( do_show_tidal_search_oversigt)) {
        if ((key!=13) && (key!=8) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) && (key!=127)) {
          keybuffer[keybufferindex]=key;
          keybufferindex++;
          keybuffer[keybufferindex]='\0';       // else input key text in buffer
          keybuffer1.push_back(key);
          keybufferopenwin=true;
        }
      }



      if (tidal_oversigt.do_setup_tidal_start_entry) {
        if ((key!=13) && (key!=8) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) && (key!=127)) {
          keybuffer[keybufferindex]=key;
          keybufferindex++;
          keybuffer[keybufferindex]='\0';       // else input key text in buffer
          keybuffer1.push_back(key);
        }
        tidal_oversigt.tidal_start_playlist_array[do_show_editor_select_linie]=keybuffer1;
      }

      if (vis_music_oversigt) {
        if (ask_open_dir_or_play) {
          if (key==32) {
            dirmusic.set_songaktiv(!(dirmusic.get_songaktiv(do_show_play_open_select_line+do_show_play_open_select_line_ofset)),do_show_play_open_select_line+do_show_play_open_select_line_ofset);
          }
        }
      }


      // printf("Keyboard buffer = %s\n",keybuffer);
    } else {
      // delete key 127
      if ((vis_film_oversigt) && (key==127)) {
        // del key
      }

      // hvis vi ikke gør andre ting
      if ((ask_save_playlist==false) || (save_ask_save_playlist==false)) {

        if ((vis_music_oversigt) && (!(ask_open_dir_or_play))) {
          if (key!=13) {
            keybuffer[keybufferindex]=key;
            keybufferindex++;
            keybuffer[keybufferindex]='\0';       // else input key text in buffer
            keybuffer1.push_back(key);
          }
          if (keybuffer1.length()>0) {
            // do not enable search view. if we are in save playlist mode
            if (ask_save_playlist==false) {
              do_show_music_search_oversigt=true;
              write_logfile(logfile,(char *) "Enable Music search.");
            }
          }
        }

        if (( vis_tidal_oversigt ) && ( ask_open_dir_or_play==false ) && ( do_show_tidal_search_oversigt==false ) && (keybufferindex<search_string_max_length)) {
          if ((key!=13) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) && (key!=127)) {
            keybuffer[keybufferindex]=key;
            keybufferindex++;
            keybuffer[keybufferindex]='\0';       // else input key text in buffer
            keybuffer1.push_back(key);
            keybufferopenwin=true;
          }
        }

        if ((vis_spotify_oversigt) && (keybufferindex==0)) {
          if (key=='D') {
            do_select_device_to_play=true;                                                                  // enable select dvice to play on
          }
        }
        
        // enable når spotify virker
        if ((firsttimespotifyupdate==false) && (strcmp(spotify_oversigt.spotify_get_token(),"")!=0)) {
          // søg efter spotify not online fill buffer from keyboard
          if ((vis_spotify_oversigt) && (!(do_show_spotify_search_oversigt))) {
            if ((do_select_device_to_play==false) && (do_zoom_spotify_cover==false)) {
              //do_zoom_spotify_cover=!do_zoom_spotify_cover;                                             // close/open window
              if ((key!=13) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY)) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';       // else input key text in buffer
                keybuffer1.push_back(key);
                // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
              }
            }
          }
          // do show search spodify oversigt online
          if ((vis_spotify_oversigt) && (do_show_spotify_search_oversigt)) {
            if ((key!=13) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) &&  (keybufferindex<search_string_max_length)) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';       // else input key text in buffer
              keybuffer1.push_back(key);
              // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
              search_spotify_string_changed=true;
            }
          }
        }
        

        // tidal stuf
        if (vis_tidal_oversigt) {
          // do show search tidal oversigt online
          if (do_show_tidal_search_oversigt) {
            if ((key!=13) && (key!='S') && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) &&  (keybufferindex<search_string_max_length)) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';       // else input key text in buffer
              keybuffer1.push_back(key);
              // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
              // search_tidal_string_changed=true;
            }
          }
        }

        // movie search fill buffer from keyboard
        if (vis_film_oversigt) {
          if ((key!=13) && (key!='*') && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY) &&  (keybufferindex<search_string_max_length)) {
            if (keybufferindex==0) do_show_movie_search_oversigt=true;
            keybuffer[keybufferindex]=key;
            keybufferindex++;
            keybuffer[keybufferindex]='\0';       // else input key text in buffer
            keybuffer1.push_back(key);
            search_movie_string_changed=true;
            // show_search_music_oversigt=true;
            // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
          }
        }

        /*
        // enable når tidal virker (Skal ikke brugere mere)
        if (key=='S') {
          // (disable save as askbox) if 'S' is pressed again.
          if ((do_show_tidal_search_oversigt) || (vis_tidal_oversigt)) {
            // the playlist is saved then start play after search in tidal search overview we do not need save_ask_save_playlist = true
            // save_ask_save_playlist=true;
            ask_save_playlist=false;
          } else {

            // if (vis_music_oversigt) {
              // save playlist
            //  fprintf(stderr,"Ask save playlist\n");
            //  ask_save_playlist = true;                                         // set save playlist flag
            }
          }
        }
        */


        // søg efter radio station navn fill buffer from keyboard
        if ((vis_radio_oversigt) && (!(show_radio_options))) {
          if ((key!=13) && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY)) {
            if ((key>31) && (key<127)) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';       // else input key text in buffer
              keybuffer1.push_back(key);
              // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
            }
          }
        }

        // used by tidal and spotify (playlistfilename)
        // is ask for playlist file name use keybuffer to get filename
        if (ask_save_playlist) {
          if ((key!=13) && (key!=SOUNDUPKEY)  && (key!=SOUNDDOWNKEY)) {
            // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
            if ((key!='S') && (key!=13)) {
              if ((key>31) && (key<127)) {
                strcpy(playlistfilename,keybuffer);
                playlistfilename[keybufferindex]='\0';       // else input key text in buffer
              }
            }
          }
        }

        if ((vis_film_oversigt) && (do_show_movie_search_oversigt==false)) {
          if (key!=13) {
            if ((key>31) && (key<127)) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';       // else input key text in buffer
              keybuffer1.push_back(key);
            }
            // fprintf(stderr,"Keybuffer=%s\n",keybuffer);
          }
        }



        // bliver aldrig kaldt.
        if (vis_film_oversigt) {
          if (film_oversigt.editmode==2) {
            printf("keybuffer1 %s \n",keybuffer1.c_str());
            switch (do_show_film_edit_select_linie) {
              case 0: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmgenre((char *) keybuffer1.c_str());
                      break;
              case 1: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmtitle((char *) keybuffer1.c_str());
                      break;
              case 2: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmlength(atoi((char *) keybuffer1.c_str()));   
                      break;
              case 3: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmaar(atoi((char *) keybuffer1.c_str()));
                      break;
              case 4: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmrating(atoi((char *) keybuffer1.c_str()));
                      break;
              case 5: break;
              case 6: break;
              case 7: break;
              case 8: break;
              case 9: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmimdbnummer((char *) keybuffer1.c_str());
                      break;
              /* 
              case 0: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmgenre((char *) keybuffer1.c_str());
                      break;
              case 1: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmtitle((char *) keybuffer1.c_str());
                      break;
              case 2: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmaar(atoi((char *) keybuffer1.c_str()));
                      break;
              case 3: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmrating(atoi((char *) keybuffer1.c_str()));
                      break;
              case 4: film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmimdbnummer((char *) keybuffer1.c_str());
                      break;
              case 5: // film_oversigt.film_set_director((film_key_selected-1)+film_selected_startofset,keybuffer);
                      break;
              case 6: // film_oversigt.film_set_cast((film_key_selected-1)+film_selected_startofset,keybuffer);
                      break;
              case 7: // film_oversigt.film_set_runtime((film_key_selected-1)+film_selected_startofset,keybuffer);
                      break;
              case 8: // film_oversigt.film_set_plot((film_key_selected-1)+film_selected_startofset,keybuffer);
                      break;
              */
            }
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
                  if (strcmp(keybuffer,"HDMI")==0) {
                    strcpy(keybuffer,"SPDIF");
                    keybuffer1="SPDIF";
                  } else if (strcmp(keybuffer,"SPDIF")==0) {
                    strcpy(keybuffer,"STREAM");
                    keybuffer1="STREAM";
                  } else if (strcmp(keybuffer,"STREAM")==0) {
                    strcpy(keybuffer,"HDMI");
                    keybuffer1="HDMI";
                  } else {
                    strcpy(keybuffer,"STREAM");
                    keybuffer1="STREAM";
                  }
                } else {
                  strcpy(keybuffer,"DEFAULT");
                  keybuffer1="DEFAULT";
                }
              } else if (do_show_setup_select_linie==2) {
                if (key==32) {
                  if (strcmp(keybuffer,"FMOD")==0) {
                    strcpy(keybuffer,"SDL");
                    keybuffer1="SDL";
                  } else if (strcmp(keybuffer,"SDL")==0) {
                    strcpy(keybuffer,"FMOD");
                    keybuffer1="FMOD";
                  } else {
                    strcpy(keybuffer,"FMOD");
                    keybuffer1="FMOD";
                  }
                } else {
                  strcpy(keybuffer,"FMOD");
                  keybuffer1="FMOD";
                }
              } else {
                if (key!=13) {
                  keybuffer[keybufferindex]=key;
                  keybufferindex++;
                  keybuffer[keybufferindex]='\0';	// else input key text in buffer
                  keybuffer1.push_back(key);
                }
              }
          } else if (do_show_setup_screen) {
            if (do_show_setup_select_linie==0) {
              if (key==32) {		// space key
                if (screen_size==1) {
                  strcpy(keybuffer,"1280 x 1024 (720p)");
                  keybuffer1="1280 x 1024 (720p)";
                  screen_size=2;
                } else if (screen_size==2) {
                  strcpy(keybuffer,"1920 x 1080 (1080p)");
                  keybuffer1="1920 x 1080 (1080p)";
                  screen_size=3;
                } else if (screen_size==3) {
                  strcpy(keybuffer,"1360 x 768         ");
                  keybuffer1="1360 x 768         ";
                  screen_size=4;
                } else if (screen_size==4) {
                  strcpy(keybuffer,"1024 x 768 (720p)  ");
                  keybuffer1="1024 x 768 (720p) ";
                  screen_size=1;
                }
              }
            // screen saver
            } else if (do_show_setup_select_linie==1) {
              if (key==32) {		// space key
                if (strncmp(keybuffer,"analog",7)==0) {
                  strcpy(keybuffer,"digital");
                  keybuffer1="digital";
                  urtype=DIGITAL;
                } else if (strcmp(keybuffer,"digital")==0) {
                  strcpy(keybuffer,"3D");
                  keybuffer1="3D";
                  urtype=SAVER3D;
                } else if (strcmp(keybuffer,"3D")==0) {
                  strcpy(keybuffer,"3D2");
                  keybuffer1="3D2";
                  urtype=SAVER3D2;
                } else if (strcmp(keybuffer,"3D2")==0) {
                  strcpy(keybuffer,"PICTURE3D");
                  keybuffer1="PICTURE3D";
                  urtype=PICTURE3D;
                } else if (strcmp(keybuffer,"PICTURE3D")==0) {
                  strcpy(keybuffer,"UV-METER");
                  keybuffer1="UV-METER";
                  urtype=UV_METER;
                } else if (strcmp(keybuffer,"UV-METER")==0) {
                  strcpy(keybuffer,"PLASMA");
                  keybuffer1="PLASMA";
                  urtype=PLASMA;
                } else if (strcmp(keybuffer,"PLASMA")==0) {
                  strcpy(keybuffer,"analog");
                  keybuffer1="analog";
                  urtype=ANALOG;
                } else {
                  strcpy(keybuffer,"analog");
                  keybuffer1="analog";
                }
              }
              // screen saver timeout
            } else if (do_show_setup_select_linie==2) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
              } // use3d
            } else if (do_show_setup_select_linie==3) {
              if (key==32) {
                if (use3deffect==true) {
                  use3deffect=false;
                  strcpy(configuse3deffect,"no");
                  strcpy(keybuffer,"no");
                  keybuffer1="no";
                } else if (use3deffect==false) {
                  strcpy(configuse3deffect,"yes");
                  strcpy(keybuffer,"yes");
                  keybuffer1="yes";
                  use3deffect=true;
                }
                keybuffer1=keybuffer;
              } else {
                strcpy(keybuffer,configuse3deffect);
                keybuffer1=configuse3deffect;
              }
            // language
            } else if (do_show_setup_select_linie==4) {
              if (key==32) {
                if (configland<(configlandantal-1)) configland++;
                else configland=0;
              }
              strcpy(keybuffer,configlandsprog[configland]);
            } else if (do_show_setup_select_linie==5) {
              if (key==32) full_screen=!full_screen;
              if (full_screen) {
                strcpy(keybuffer,"yes"); 
                keybuffer1="yes";
              } else {
                strcpy(keybuffer,"no");
                keybuffer1="no";
              }
            }
          } else if (do_show_setup_network) {
            // hostname
            if (do_show_setup_select_linie==0) {
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
              }
              // network link type
            } else if (do_show_setup_select_linie==1) {
              if (key==32) {				// space key
                if (strcmp(keybuffer,"DHCP")==0) {
                  strcpy(keybuffer,"MANUAL");
                  keybuffer1="MANUAL";
                } else if (strcmp(keybuffer,"MANUAL")==0) {
                  strcpy(keybuffer,"DHCP");
                  keybuffer1="DHCP";
                }
              }
            } else if (do_show_setup_select_linie==2) {
              // mulighed for at manuelt indtaste wlan essid to use
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
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
                keybuffer1.push_back(key);
              }
            }
            // backend/sql setup
          } else if (do_show_setup_backend) {
            if (do_show_setup_select_linie==0) {
              if (key==32) {
                if (strcmp(keybuffer,"xbmc")==0) {
                  strcpy(keybuffer,"mythtv");
                  keybuffer1="mythtv";
                } else if (strcmp(keybuffer,"mythtv")==0) {
                  strcpy(keybuffer,"xbmc");
                  keybuffer1="xbmc";
                }
              }
            } else {              
              if (key!=13) {
                keybuffer[keybufferindex]=key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
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
              keybuffer1.push_back(key);
            }
            // rss editor
          } else if (do_show_setup_rss) {
            if (key!=13) {
              // keybuffer[keybufferindex]=key;
              keybufferindex++;
              // keybuffer[keybufferindex]='\0';	// else input key text in buffer
              keybuffer1.push_back(key);
            }
          } else if (do_show_setup_spotify) {
            if (key!=13) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';	// else input key text in buffer
              keybuffer1.push_back(key);
            }
          } else if (do_show_setup_tidal) {
            if (key!=13) {
              keybuffer[keybufferindex]=key;
              keybufferindex++;
              keybuffer[keybufferindex]='\0';	// else input key text in buffer
              keybuffer1.push_back(key);
            }
          } else if (do_show_setup_torrent) {
            if (key!=13) {
              keybuffer[keybufferindex]=key;
              if (do_show_setup_select_linie==0) {
                keybuffer[0]=key;
                keybuffer[1]='\0';
                keybuffer1.push_back(key);
              }
              if (do_show_setup_select_linie==1) {
                keybuffer[0]=key;
                keybuffer[1]='\0';
                keybuffer1.push_back(key);
              }
              if (do_show_setup_select_linie==2) {
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
              }
            }
          } else if (do_show_videoplayer) {
            // video player setting
            if (do_show_setup_select_linie==0) {
              if (key!=13) {
                keybuffer[keybufferindex] = key;
                keybufferindex++;
                keybuffer[keybufferindex]='\0';	// else input key text in buffer
                keybuffer1.push_back(key);
                // if (debugmode) fprintf(stderr,"Keybuffer=%s\n",keybuffer);
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
              if (configuvmeter==0) {
                strcpy(keybuffer,"none");
                keybuffer1="none";
              }
              if (configuvmeter==1) {
                strcpy(keybuffer,"Simple");
                keybuffer1="Simple";
              }
              if (configuvmeter==2) {
                strcpy(keybuffer,"Dual");
                keybuffer1="Dual";
              }
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
            // if (debugmode) fprintf(stderr,"do_show_setup_select_linie %d tvchannel_startofset %d \n",do_show_setup_select_linie,tvchannel_startofset);
            if (do_show_setup_select_linie>=1) {
              // set tvguide channel activate or inactive
              channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=!channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
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
                  if (screen_size==1) {
                    strcpy(keybuffer,"1024 x 768 ");
                    keybuffer1="1024 x 768";
                  } else if (screen_size==2) {
                    strcpy(keybuffer,"1280 x 1024");
                    keybuffer1="1280 x 1024";
                  } else if (screen_size==3) {
                    strcpy(keybuffer,"1920 x 1080");
                    keybuffer1="1920 x 1080";
                  } else if (screen_size==4) {
                    strcpy(keybuffer,"1360 x 768 ");
                    keybuffer1="1360 x 768 ";
                  }
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
          } else if (do_show_setup_backend) {
            switch(do_show_setup_select_linie) {
                 case 0: strcpy(configbackend,keybuffer);
                         break;
                 case 1: strcpy(configmysqlhost,keybuffer);
                         break;
                 case 2: strcpy(configmysqluser,keybuffer);
                         break;
                 case 3: strcpy(configmysqlpass,keybuffer);
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
            switch(rssstreamoversigt.setup_select_linie) {
              case 0: rssstreamoversigt.set_stream_name(0+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 1: rssstreamoversigt.set_stream_url(0+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 2: rssstreamoversigt.set_stream_name(1+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 3: rssstreamoversigt.set_stream_url(1+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 4: rssstreamoversigt.set_stream_name(2+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 5: rssstreamoversigt.set_stream_url(2+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 6: rssstreamoversigt.set_stream_name(3+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 7: rssstreamoversigt.set_stream_url(3+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 8: rssstreamoversigt.set_stream_name(4+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 9: rssstreamoversigt.set_stream_url(4+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 10:rssstreamoversigt.set_stream_name(5+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 11:rssstreamoversigt.set_stream_url(5+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 12:rssstreamoversigt.set_stream_name(6+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 13:rssstreamoversigt.set_stream_url(6+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 14:rssstreamoversigt.set_stream_name(7+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 15:rssstreamoversigt.set_stream_url(7+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 16:rssstreamoversigt.set_stream_name(8+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 17:rssstreamoversigt.set_stream_url(8+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 18: rssstreamoversigt.set_stream_name(9+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 19: rssstreamoversigt.set_stream_url(9+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 20: rssstreamoversigt.set_stream_name(10+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 21: rssstreamoversigt.set_stream_url(10+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 22: rssstreamoversigt.set_stream_name(11+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 23: rssstreamoversigt.set_stream_url(11+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 24: rssstreamoversigt.set_stream_name(12+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 25: rssstreamoversigt.set_stream_url(12+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 26: rssstreamoversigt.set_stream_name(13+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 27: rssstreamoversigt.set_stream_url(13+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 28:rssstreamoversigt.set_stream_name(14+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 29:rssstreamoversigt.set_stream_url(14+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 30:rssstreamoversigt.set_stream_name(15+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 31:rssstreamoversigt.set_stream_url(15+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 32:rssstreamoversigt.set_stream_name(16+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 33:rssstreamoversigt.set_stream_url(16+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 34:rssstreamoversigt.set_stream_name(17+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
              case 35:rssstreamoversigt.set_stream_url(17+configrss_ofset,(char *) keybuffer1.c_str());
                      break;
             }
          }
        }
        if (do_show_setup_spotify) {
            switch(do_show_setup_select_linie) {
              case 0: strcpy(spotify_oversigt.spotify_client_id,keybuffer);
                      break;
              case 1: strcpy(spotify_oversigt.spotify_secret_id,keybuffer);
                      break;
            }
        }

        
        if (do_show_setup_tidal) {
            switch(do_show_setup_select_linie) {
              case 0: strcpy(tidal_oversigt.client_id,keybuffer);
                      break;
              case 1: tidal_oversigt.client_secret=keybuffer;
                      break;
            }
        }


         if (do_show_setup_torrent) {
             switch(do_show_setup_select_linie) {
               case 0: if (strcmp(keybuffer,"Y")) torrent_downloader.trash_torrent=true; else torrent_downloader.trash_torrent=false;
                       break;
               case 1: if (strcmp(keybuffer,"Y")) torrent_downloader.automove_to_movie_path=true; else torrent_downloader.automove_to_movie_path=false;
                       break;
               case 2: torrent_downloader.downloadpath = keybuffer;
                       break;
             }
         }
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
    }
  } else {
    switch(key) {
      case 27:
        if (ask_save_playlist) {
          save_ask_save_playlist=false;
          ask_save_playlist=false;
        }
        if ((do_show_tidal_search_oversigt) && (tidal_oversigt.tidal_stop_loader_thread==false)) {
          tidal_oversigt.tidal_stop_loader_thread=true;
          key=0;
        }

        if (vis_film_oversigt) {
          if (film_oversigt.film_is_playing) {
            film_oversigt.stopmovie();
            // vis_film_oversigt=false;
            key=0;
            film_oversigt.film_is_playing=false;
          }
        }

        if (vis_nyefilm_oversigt) {
          if (film_oversigt.film_is_playing==false) vis_nyefilm_oversigt=false;
          if (film_oversigt.film_is_playing) {
            film_oversigt.stopmovie();
            film_oversigt.film_is_playing=false;
          }
          key=0;
        }

        // close setup windows again or close proram window.
        if (do_show_setup) {
          if (do_show_tvgraber) {
            // make new tv overview
            // kill running graber
            // NOT ESC key
            // killrunninggraber();
            // clear old tvguide in db
            // aktiv_tv_oversigt.cleartvguide();                             // clear old db
            // aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
            // hent/update tv guide from db
            // aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
            // order_channel_list();                                         // ordre struct
            // save chennel list info to internal datafile
            // save_channel_list();                                          // save to db file
            // set update flag in display() func
            firsttime_xmltvupdate = true;                                 // if true reset xml config file
            // close tv graber windows again
            do_show_tvgraber=false;

            aktiv_tv_oversigt.edit_mode=false;
            aktiv_tv_oversigt.update_guide();

            key=0;
          } else if (do_show_videoplayer) {
            do_show_videoplayer=false;
            key=0;
          } else if (do_show_setup_backend) {
            do_show_setup_backend=false;
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
            do_show_setup_rss=false;
            key=0;
          } else if (do_show_setup_spotify) {
            do_show_setup_spotify=false;
            key=0;
          } else if (do_show_setup_tidal) {
            do_show_setup_tidal=false;
            key=0;
          } else if (do_show_setup_torrent) {
            do_show_setup_torrent=false;
            key=0;
          } else if (do_show_setup_network) {
            key=0;
          } else do_show_setup=false;
          key=0;
        }
        // vis tv overview and show tv_guide setup and press ESC key
        // called by esc  key
        if ((do_show_torrent==false) && (vis_tv_oversigt) && (do_show_setup==false) && (do_show_tvgraber)) {
          // make new tv overview
          // kill running graber
          // ESC KEY
          // killrunninggraber();
          // clear old tvguide in db
          // aktiv_tv_oversigt.cleartvguide();                             // clear old db
          // aktiv_tv_oversigt.parsexmltv("tvguide.xml");                  // parse all channels xml file again
          // hent/update tv guide from db
          // aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass,0);
          // order_channel_list();                                         // ordre struct
          // save chennel list info to internal datafile
          // save_channel_list();                                          // save to db file
          // set update flag in display() func
          firsttime_xmltvupdate = true;                                 // if true reset xml config file
          // close tv graber windows again
          do_show_tvgraber=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_music_oversigt)) {
          if (do_zoom_music_cover) do_zoom_music_cover=false;
          else if (ask_open_dir_or_play_music) ask_open_dir_or_play_music=false;
          else vis_music_oversigt=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_radio_oversigt)) {
          if (do_zoom_radio_cover) {
            do_zoom_radio_cover=false;
          } else {
            vis_radio_or_music_oversigt=true;
            vis_radio_oversigt=false;
          }
          key=0;
        } else if ((do_show_torrent==false) && (vis_film_oversigt)) {
          if (film_oversigt.editmode==2) {
            film_oversigt.editmode=0;
          } else {
            if ((film_oversigt.film_is_playing==false) && (key!=0)) {
              if (do_zoom_film_cover) do_zoom_film_cover=false;
              else vis_film_oversigt=false;
            }
          }
          key=0;
        } else if ((do_show_torrent==false) && (vis_stream_oversigt)) {
          if (do_zoom_stream_cover) do_zoom_stream_cover=false;
          else if (vis_stream_oversigt) {
            vis_stream_or_movie_oversigt=true;
            vis_stream_oversigt=false;
          }
          key=0;
        } else if ((do_show_torrent==false) && (vis_tv_oversigt)) {
          vis_tv_oversigt=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_recorded_oversigt)) {
          vis_recorded_oversigt=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_radio_or_music_oversigt)) {
          vis_radio_or_music_oversigt=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_stream_or_movie_oversigt)) {
          vis_stream_or_movie_oversigt=false;
          key=0;
        } else if ((do_show_torrent==false) && (vis_spotify_oversigt)) {
          ask_save_playlist=false;
          hent_spotify_search=false;
          vis_spotify_oversigt=false;
          keybufferopenwin=false;
          //spotify_oversigt.opdatere_spotify_oversigt(0);                  // reset spotify overview
          //spotify_oversigt.load_spotify_iconoversigt();
          //spotify_oversigt.set_search_loaded();                           // triger icon loader
          key=0;
        } else if ((do_show_torrent==false) && (vis_tidal_oversigt) && (tidal_oversigt.tidal_stop_loader_thread==false)) {
          ask_save_playlist=false;
          hent_tidal_search=false;
          vis_tidal_oversigt=false;
          keybufferopenwin=false;
          key=0;
        } else if ((!(do_show_setup)) && (do_show_torrent==false) && (key==CLOSE) && (tidal_oversigt.do_setup_tidal_start_entry==false) && (ask_save_playlist==false)) {      // exit program
          remove("mythtv-controller.lock");
          runwebserver=false;
          // order_channel_list();
          // save_channel_list();
          write_logfile(logfile,(char *) "Exit program.");
          exit(0);                                                        //  exit program
        } else if ((vis_tv_oversigt) && (do_show_torrent==false) && (do_show_tvgraber) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
          // Close tv_graber view from tv_oversigt
          do_show_tvgraber=false;
          key=0;
        } else if ((do_show_torrent) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
          // torrent stuf
          if (do_show_torrent_options_move) {
            do_show_torrent_options_move = false;
            key=0;
          } else if (do_show_torrent_options) {
            do_show_torrent_options = false;
            key=0;
          } else if (do_show_torrent) {
            do_show_torrent = false;
            key=0;
          }
        } else if (tidal_oversigt.do_setup_tidal_start_entry) {
          tidal_oversigt.do_setup_tidal_start_entry=false;
          key=0;
        } else key=0;
        break;
      case '*':
        if (vis_spotify_oversigt) {
          // Do we show search view if yes do
          // select search type (artist/album/song)
          if (do_show_spotify_search_oversigt == true ) {
            spotify_oversigt.searchtype++;
            if (spotify_oversigt.searchtype>3) spotify_oversigt.searchtype=0;
          }
          if (do_show_spotify_search_oversigt == false) {
            if (do_update_spotify_playlist == false) do_update_spotify_playlist=true;       // set update flag til true og start background update
          } else if (vis_music_oversigt) do_zoom_music_cover=!do_zoom_music_cover;        // show/hide music info
        }
        // if show tidal overview then '*' do update
        if ((vis_tidal_oversigt) && (do_show_tidal_search_oversigt==false)) {
          if (do_update_tidal_playlist==false) do_update_tidal_playlist=true;       // set update flag til true og start background update
        }
        // if show tidal search overview
        if ((vis_tidal_oversigt) && (do_show_tidal_search_oversigt)) {                
          tidal_oversigt.searchtype++;
          if (tidal_oversigt.searchtype>3) tidal_oversigt.searchtype=0;
        }

        if ((!(vis_spotify_oversigt)) && (!(vis_tidal_oversigt)) && (!(vis_radio_oversigt)) && (!(vis_tv_oversigt))) {
          do_update_spotify_playlist=true;
        }

        if (vis_radio_oversigt) {
          if (do_zoom_radio_cover) do_zoom_radio_cover=false;                           // show/hide music player info
          else {
            // opdate radiooverview (reset view)
            write_logfile(logfile,(char *) "Load radioo overview.");
            radiooversigt.clean_radio_oversigt();
            radiooversigt.opdatere_radio_oversigt(0);
            radiooversigt.load_radio_stations_gfx();
          }                
        }
        if (vis_film_oversigt) do_zoom_film_cover=!do_zoom_film_cover;             // film info
        if ((vis_stream_oversigt) && (sknapnr>0)) do_zoom_stream_cover=!do_zoom_stream_cover;  // stream info
        if ((vis_tv_oversigt) && (do_zoom_tvprg_aktiv_nr>0)) {                     // tv oversigt zoom
          do_zoom_tvprg_aktiv_nr=0;
        }
        if (vis_tv_oversigt) {
          // spørg kan/skal vi optage den ?
          ask_tv_record = true;
          tvknapnr=tvsubvalgtrecordnr;                                                   // set program nr
          do_zoom_tvprg_aktiv_nr=tvknapnr;					                                     // husk den valgte aktiv tv prg
        }
        break;
      case optionmenukey:                                                       // default o
          if (vis_film_oversigt) {
            vis_movie_options=!vis_movie_options;
          } else if ((vis_tv_oversigt) && (!(vis_tvrec_list))) {
            vis_old_recorded=!vis_old_recorded;	                        	    // show old recorded programs
          } else if (vis_radio_oversigt) {
            show_radio_options=!show_radio_options;			                    	// show radio options
            if (do_zoom_radio_cover) do_zoom_radio_cover=false;
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
          write_logfile(logfile,(char *) "Loading tvguidedb file.");
          // aktiv_tv_oversigt.loadparsexmltvdb();
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
        if (vis_tidal_oversigt) {
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
        write_logfile(logfile,(char *) "Key y Not in use.");
        break;
      case 'u':
        if ((vis_tv_oversigt) && (loading_tv_guide==false)) {
          // u key
          // Update tv guide
          write_logfile(logfile,(char *) "Update tv guide.");
          // set flag for show update
          do_update_xmltv_show = true;
          loading_tv_guide = true;
          do_update_xmltv = true;                                      // set update flag
          aktiv_tv_oversigt.get_dr_proguide(0);                                                                // get tv guide
          tv_guide_firsttime_update = true;                                 // set update flag
        } else write_logfile(logfile,(char *) "Already updating tv guide.(running)");
        // Update rss stuf
        if (vis_stream_oversigt) {
          do_update_rss_show = true;                                     // set show update flag
          do_update_rss = true;                                          // set update flag
        }
        //
        // Movie
        //
        
        if ((vis_film_oversigt) && (!(do_update_moviedb))) {
          do_update_moviedb = true;                                           // set update flag
                                                                              // bliver sikket cleared in thread
          // pthread_t loaderthread1;                                          // loader thread
          // start multi thread and update movie overview
          // movie loader
          // write debug log
          write_logfile(logfile,(char *) "Update movie db.");
          /*
          if ((strncmp(configbackend,"xbmc",4)==0) || (strncmp(configbackend,"kodi",4)==0)) {
            int rc1=pthread_create(&loaderthread1,NULL,xbmcdatainfoloader_movie,NULL);
            if (rc1) {
              fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
              exit(-1);
            }
          } else {
            if (configmythtvver>=0) {
              // datainfoloader_movie_v2();                                // load movie info
              int rc1=pthread_create(&loaderthread1,NULL,datainfoloader_movie_v2_force,NULL);
              if (rc1) {
                fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
                exit(-1);
              }
            }
          }
          */
        }
        break;
      case 'U':
        //
        // music
        //
        if ((vis_music_oversigt) && (ask_open_dir_or_play==false)) {
          write_logfile(logfile,(char *) "Update music db.");
          do_update_music = true;                                               // show update
          do_update_music_now = true;                                           // Set update flag
        }
        // spotify
        #ifdef ENABLE_SPOTIFY
        if (vis_spotify_oversigt) {
          write_logfile(logfile,(char *) "Update Spotify.");
          do_update_spotify = true;                                             // set flag to update spotify
        }
        #endif
        #ifdef ENABLE_TIDAL
        if (vis_tidal_oversigt) {
          write_logfile(logfile,(char *) "Update Tidal.");
          do_update_tidal_playlist=true;                                        // set update flag til true og start background update
          // do_hent_tidal_search_online=true;                                     //
        }
        if (vis_tv_oversigt) {
          write_logfile(logfile,(char *) "Update tv guide.");
          aktiv_tv_oversigt.get_dr_proguide(0);
        }
        #endif
        //
        // Movie
        //
        if ((vis_film_oversigt) && (!(do_update_moviedb))) {
          do_update_moviedb = true;                                           // set update flag
                                                                              // bliver sikket cleared in thread
          pthread_t loaderthread1;                                          // loader thread
          // start multi thread and update movie overview
          // movie loader
          // write debug log
          write_logfile(logfile,(char *) "Update movie db.");
          /*
          if ((strncmp(configbackend,"xbmc",4)==0) || (strncmp(configbackend,"kodi",4)==0)) {
            int rc1=pthread_create(&loaderthread1,NULL,xbmcdatainfoloader_movie,NULL);
            if (rc1) {
              fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
              exit(-1);
            }
          } else {
            if (configmythtvver>=0) {
              // datainfoloader_movie_v2();                                // load movie info
              int rc1=pthread_create(&loaderthread1,NULL,datainfoloader_movie_v2_force,NULL);
              if (rc1) {
                fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc1);
                exit(-1);
              }
            }
          }
          */
        }
        if (vis_stream_oversigt) {
          do_update_rss_show = true;                                     // set show update flag
          do_update_rss = true; 
          // streamoversigt.cleanup_rss_db();                                          // set update flag
          streamoversigt.opdatere_stream_oversigt((char *)"",(char *)"");             // load all stream from rss files
          do_update_rss_show = true;                                     // set show update flag
          do_update_rss = false;                                          // set update flag
        }
        break;
      case 13:
        // ENTER key
        
        if (do_show_torrent == false) {
          
          if (vis_music_oversigt) {
            if (do_show_music_search_oversigt) {
              do_hent_music_search_online=true;
              printf("do_hent_music_search_online=%d\n",do_hent_music_search_online);  
            }
            if (ask_save_playlist) write_logfile(logfile,(char *) "Save playlist key pressed, update music list.");
            else write_logfile(logfile,(char *) "Enter key pressed, update music list.");
          }
          

          // set save flag of playlist
          if (vis_tidal_oversigt) {
            if (ask_save_playlist) {
              save_ask_save_playlist = true;        // set save flag
            }
            if ((do_show_tidal_search_oversigt) && (ask_save_playlist==false)) {
              do_hent_tidal_search_online=true;
              printf("do_hent_tidal_search_online=%d\n",do_hent_tidal_search_online);  
            }
            if ((tidalknapnr>0) && (do_show_tidal_search_oversigt==false)) {
              // set play playlist flag
              // printf("tidal_selected_startofset = %d  tidalknapnr = %d ",tidal_selected_startofset,tidalknapnr);
              do_play_tidal=tidalknapnr;
              tidal_oversigt.startplay=true;
            }
            if (do_show_tidal_search_oversigt==false) {
              hent_tidal_search=true;
            }
          }              
          if (vis_spotify_oversigt) {
            if (ask_save_playlist) {
              save_ask_save_playlist = true;
            }
          }
          if (vis_radio_oversigt) {
            rknapnr=0;
            hent_radio_search = true;			  	// start radio station search
            radio_key_selected=1;
            
          }
          if (vis_film_oversigt) {
            if (film_oversigt.editmode==2) {
              std::string filmtype=film_oversigt.select_movie_type(); 
              if (filmtype.length()>0) {
                if (debugmode & 2) fprintf(stderr,"Set movie type to %s\n",filmtype.c_str());
                filmtype.erase(std::remove(filmtype.begin(), filmtype.end(), '\n'), filmtype.cend());
                film_oversigt.filmoversigt[do_zoom_film_aktiv_nr].setfilmgenre((char *) filmtype.c_str());
                strcpy(keybuffer,filmtype.c_str());
                keybuffer1=filmtype;
                keybufferindex=strlen(keybuffer);

              }
            } else {
              hent_film_search = true;			  	// start movie title search
              // strcpy(keybuffer,"");
              // fknapnr=0;
            }
          }
          // search podcast
          if (vis_stream_oversigt) {
            hent_stream_search = true;				   // start stream station search
            sknapnr=stream_select_iconnr;        // selected
            stream_key_selected=1;
            stream_select_iconnr=0;
            
            do_play_stream=0;
          }
          // search spotify
          #ifdef ENABLE_SPOTIFY
          if (vis_spotify_oversigt) {
            hent_spotify_search = true;				              // start spotify search
            spotifyknapnr=spotify_select_iconnr;            // selected
            spotify_key_selected=1;
            spotify_select_iconnr=0;
            
            do_play_spotify=0;
            //spotifyknapnr = the one to play
          }
          #endif
          #ifdef ENABLE_TIDAL
          // start search tidal
          if (vis_tidal_oversigt) {
            hent_tidal_search = true;				                // start tidal search
            tidalknapnr=tidal_select_iconnr;                // selected
            tidal_key_selected=1;
            tidal_select_iconnr=0;
            
            do_play_tidal=0;
            //tidalknapnr = the one to play
          }
          #endif
          // start music search
          if ((vis_music_oversigt) && (keybufferopenwin) && (strcmp(keybuffer,"")!=0)) {
            mknapnr=0;				                            	// reset mouse/key pos in vis_music_overs
            _mangley=0.0f;
            hent_music_search = true;                       // start music search (set flag)
            music_key_selected=1;                           // reset to first icon
            music_select_iconnr=0;                          // reset to first icon
            // music_icon_anim_icon_ofset=0;                   // reset to first icon
            // music_icon_anim_icon_ofsety=0;
          }
          // start music 
          if ((vis_music_oversigt) && (!(do_zoom_music_cover)) && ((ask_save_playlist==false)) && (hent_music_search==false)) {
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
              write_logfile(logfile,(char *) debuglogdata);
              if (antal_songs==0) {
                ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
              } else {
                ask_open_dir_or_play_aopen=false;
              }
              ask_open_dir_or_play = true;							// yes ask om vi skal spille den (play playlist)
              //do_zoom_music_cover=true;
            } else if (strcmp(keybuffer,"")!=0) {
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
              write_logfile(logfile,(char *) debuglogdata);
              if (antal_songs==0) {
                ask_open_dir_or_play_aopen = true;					// ask om de skal spilles
              } else {
                ask_open_dir_or_play_aopen=false;
              }
              ask_open_dir_or_play = true;							// yes ask om vi skal spille den
            }
          }
          // enter key pressed og ask open dir or play er åben så start play

          // save playlist
          if ((vis_music_oversigt) && (ask_save_playlist)) {
            fprintf(stderr,"Enter key pressed start play music \n");
            musicoversigt.save_music_oversigt_playlists(keybuffer);   // save playlist
            ask_save_playlist=false;                                   // luk vindue igen
          }

          if ((vis_music_oversigt) && (ask_open_dir_or_play) && (ask_save_playlist==false)) {
            ask_open_dir_or_play=false;                 // flag luk vindue igen
            do_play_music_cover=1;                      // der er trykket på cover play det
            do_zoom_music_cover=false;                  // ja den skal spilles lav zoom cover info window
            do_find_playlist = true;                      // find de sange som skal indsættes til playlist (og load playlist andet sted)
          }
          if ((vis_radio_oversigt) && (show_radio_options==false)) {
            rknapnr=radio_select_iconnr+1;					// hent button
            fprintf(stderr,"Set do_play_radio flag rknapnr=%d \n",rknapnr);
            if (rknapnr>0) do_play_radio=1;					// start play
          }
          // opdatere radio oversigt igen efter vis radio options
          if ((vis_radio_oversigt) && (show_radio_options)) {
            // write debug log
            write_logfile(logfile,(char *) "Update radio overview.");
            radiooversigt.clean_radio_oversigt();				// clean old liste
            radiooversigt.opdatere_radio_oversigt(radiooversigt.getradiooptionsselect());
            radiooversigt.load_radio_stations_gfx();
            show_radio_options=false;
            
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
              aktiv_tv_oversigt.opdatere_tv_oversigt(configmysqlhost,configmysqluser,configmysqlpass);
            } else {
              // spørg kan/skal vi optage den ?
              ask_tv_record = true;
              tvknapnr=tvsubvalgtrecordnr;
              do_zoom_tvprg_aktiv_nr=tvknapnr;					// husk den valgte aktiv tv prg
            }
          }
          // stream oversigt (icon valgt)
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
                streamoversigt.opdatere_stream_oversigt((char *) "",(char *) "");
                //streamoversigt.opdatere_stream_oversigt(streamoversigt.get_stream_name(sknapnr),streamoversigt.get_stream_path(sknapnr));
                do_play_stream=false;
                stream_key_selected=1;
                stream_select_iconnr=0;
                
              }
            } else {
              // back button
              if (debugmode & 4) fprintf(stderr,"stream nr %d \n ",sknapnr-1);
              if ((sknapnr)==0) {
                if (streamoversigt.type==2) {
                  // one level up
                  streamoversigt.clean_stream_oversigt();
                  streamoversigt.opdatere_stream_oversigt((char *) streamoversigt.get_stream_name(sknapnr),(char *) "");
                  do_play_stream=false;
                  do_play_stream=false;
                  stream_key_selected=1;
                  stream_select_iconnr=0;
                  
                } else {
                  // jump to top (root)
                  streamoversigt.clean_stream_oversigt();
                  streamoversigt.opdatere_stream_oversigt((char *) "",(char *) "");
                  do_play_stream=false;
                  do_play_stream=false;
                  stream_key_selected=1;
                  stream_select_iconnr=0;
                  
                }
              }
            }
            // play stream
            if ((sknapnr>=0) && (do_play_stream)) {
              if (strncmp(streamoversigt.get_stream_url(sknapnr),"mythflash",9)==0) {
                startstream = true;                                                           // set stream play flag
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
              write_logfile(logfile,(char *) "Send play command to spotify device");
              spotify_oversigt.spotify_play_now_playlist( spotify_oversigt.get_spotify_playlistid( (spotifyknapnr+spotify_selected_startofset)-1 ) ,1);
              // close window again
              do_select_device_to_play=false;
            }
            // search func
            if (!(do_select_device_to_play)) {
              // write debug log
              write_logfile(logfile,(char *) "Start search spotify");
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
              // strcpy(path,localuserhomedir);
              //getuserhomedir(path);
              // strcat(path,"/tvguide_channels.dat");
              // unlink(path);                                                 // delete file
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
              write_logfile(logfile,(char *) "Open wifi network scan");
              // wifinets.get_networkid(setupwlanselectofset,id);
              // strcpy(confighostwlanname,id);
              // sprintf(tmptxt,"sudo /sbin/iwconfig wlan0 essid %s",id);
              // fprintf(stderr,"Charge network by %s \n",tmptxt);
              // do_system_call(tmptxt);
              sprintf(debuglogdata,"Set wifi to %s ",confighostwlanname);
              write_logfile(logfile,(char *) debuglogdata);
            }
            if (do_show_setup_network) {
              if (do_show_setup_select_linie<4) do_show_setup_select_linie++;
              fprintf(stderr,"next line %d \n",do_show_setup_select_linie);
            }
            if (do_show_videoplayer) {
              /*
              std::string pdevice=select_play_device();
              if (pdevice.length()>0) {
                fprintf(stderr,"Set play device to %s\n",pdevice.c_str());
                pdevice.erase(std::remove(pdevice.begin(), pdevice.end(), '\n'), pdevice.cend());
                strcpy(configvideoplayer,pdevice.c_str());
                strcpy(configdefaultplayer,pdevice.c_str());
                sprintf(debuglogdata,"Set video output to %s ",configvideoplayer);
                write_logfile(logfile,(char *) debuglogdata);
              }
              */
            }
          }
        }
        


        // enter pressed in tv guide view
        if (vis_tv_oversigt) {
          int kanaler=0;
          int programmer=0;
          bool program_fundet=false;
          while (((kanaler<aktiv_tv_oversigt.tvkanaler.size()) && (program_fundet==false))) {
            programmer=0;
            while((programmer<aktiv_tv_oversigt.tvkanaler[kanaler].programs.size() && (program_fundet==false))) {
              if (aktiv_tv_oversigt.tvkanaler[kanaler].programs[programmer].aktive) {
                program_fundet = true;
              }
              if (program_fundet == false) programmer++;
            }
            if (program_fundet == false) kanaler++;
          }
          if (program_fundet) {
            aktiv_tv_oversigt.vis_kanal_nr=kanaler;
            aktiv_tv_oversigt.vis_program_nr=programmer;
            do_show_tv_kanal_info=true;
          }
        }


        if (do_show_setup_rss) {
          rssstreamoversigt.setup_select_linie++;
          /*
          if (((do_show_setup_select_linie+configrss_ofset) % 2)==0) {
            if ((realrssrecordnr)<43) realrssrecordnr++;
          } else configrss_ofset++;
          */
        }


        if (do_show_torrent) {
          printf("enter pressed\n ");
          if ((do_show_torrent_options) && (do_show_torrent_options_move==false)) {
            // pause
            if (torrent_downloader.get_torrent_info_line_nr()==0) {
              torrent_downloader.pause_torrent(torrent_downloader.get_edit_line());
              do_show_torrent_options = false;
            }
            // move
            if (torrent_downloader.get_torrent_info_line_nr()==1) {
              torrent_downloader.move_torrent(torrent_downloader.get_edit_line());    // move torrent file to selected dir in thread. This function call set the flag
            }
            // delete select torrent
            if ((torrent_downloader.get_torrent_info_line_nr()==2) && (do_show_torrent_options)) {
              torrent_downloader.delete_torrent();
              do_show_torrent_options = false;
              
            }
            // do_show_torrent_options = false;
          } else {
            if ((do_show_torrent_options) && (do_show_torrent_options_move==false)) {
              do_show_torrent_options_move = true;
            } else {
              if (do_show_torrent_options==false) {
                do_show_torrent_options = true;
              } else {
                printf("Move file \n");
                do_move_torrent_file = true; // set flag for do the move in thread in datainfoloader_webserver_v2
              }
            }
          }
        }
        break;
    }
  }
  std::cout << "keybuffer = " << keybuffer << " keybuffer1 = " << keybuffer1 << std::endl;
  std::flush(std::cout);
}




// ****************************************************************************************
//
// Handle specials keys
// like functions keys
//
// ****************************************************************************************


void handlespeckeypress(int key,int x,int y) {
    unsigned int mnumbersoficonline;
    int numbers_cd_covers_on_line=8;
    int fnumbersoficonline;
    int rnumbersoficonline;
    int snumbersoficonline;
    int tnumbersoficonline;
    std::string output;
    saver_irq=true;                                     // stop screen saver
    mnumbersoficonline=8;		                            // antal i music oversigt
    fnumbersoficonline=8;	                              // antal i film oversigt
    rnumbersoficonline=8;                               // antal i radio oversigt
    snumbersoficonline=8;                               // antal i stream/spotify oversigt
    tnumbersoficonline=8;                               // antal i tidal oversigt
    time_t nut;
  

    if ((multi_editor) && (textEditor.IsActive()==true)) {
      switch (key) {
        case GLUT_KEY_LEFT:
            textEditor.MoveLeft();
            break;
        case GLUT_KEY_RIGHT:
            textEditor.MoveRight();
            break;
        case GLUT_KEY_UP:
            textEditor.MoveUp();
            break;
        case GLUT_KEY_DOWN:
            textEditor.MoveDown();
            break;
        case GLUT_KEY_HOME:
            textEditor.MoveHome();
            break;
        case GLUT_KEY_END:
            textEditor.MoveEnd();
            break;
        case GLUT_KEY_PAGE_UP:
            textEditor.PageUp();
            break;
        case GLUT_KEY_PAGE_DOWN:
            textEditor.PageDown();
            break;        
      }
      key=0;
    }
    
    switch(key) {
      case 1: // F1 key
              if (vis_tv_oversigt) {
                do_show_tvgraber=!do_show_tvgraber;
              }
              if (vis_music_oversigt) {
                if (findtype==0) findtype=1;
                else if (findtype==1) findtype=0;
              }
              if ((!(do_show_setup_keys)) && (!(vis_radio_oversigt)) && (!(vis_radio_or_music_oversigt)) && (!(vis_tidal_oversigt)) && (strlen(configkeyslayout[1].cmdname)==0)) {
                // if (do_show_setup) do_save_config = true;		                   // set save config file flag
                do_save_config = true;		                   // set save config file flag
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

              if ((do_show_setup) && (do_show_setup_keys)) {
                printf("do_show_setup_select_linie = %d \n",do_show_setup_select_linie);
                // select_exe_functions_keys_name();
              } else {
                if ((vis_tidal_oversigt) && (do_show_tidal_search_oversigt)) {
                  tidal_oversigt.searchtype++;
                  if (tidal_oversigt.searchtype>2) tidal_oversigt.searchtype=0;
                } else {
                  // saveexitcommand(configkeyslayout[0]);
                  // doexitcommand();
                }
              }
              break;
      case 2: // F2 key
              exit(2);
              break;
      case 3: // F3 key
              do_show_torrent =! do_show_torrent;
              break;
      case 4: // F4 key
              /*
              if (strlen(configkeyslayout[1].cmdname) > 0) {
                // saveexitcommand(configkeyslayout[1]);
                // doexitcommand();
                do_system_call_with_timeout(configkeyslayout[1].cmdname, output, 2);
              } else {
                tidal_oversigt.do_setup_tidal_start_entry = ! tidal_oversigt.do_setup_tidal_start_entry;
                do_show_torrent = false;
              }
              */
              tidal_oversigt.do_setup_tidal_start_entry = ! tidal_oversigt.do_setup_tidal_start_entry;
              break;
      case 5: // F5 key
              break;
      case 6: // F6 key
              break;
      case 7: // F7 key
              break;
      case 8: // F8 key
              break;
      case 9: // F9 key
              break;
      case 10: // F10 key
              break;
      case 11: // F11 key
              break;
      case 12:  // F12 key
              break;
      case 100:  // Left key
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (music_key_selected>1) {
                  music_key_selected--;
                  music_select_iconnr--;
                  musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view-1;
                  if (musicoversigt.selected_icon_in_view<1) musicoversigt.selected_icon_in_view=1;
                } else {
                  if ((music_select_iconnr>0) && (_mangley>0)) {
                      // _mangley-=MUSIC_CS;
                      music_key_selected+=mnumbersoficonline-1;  // den viste på skærm af 1 til 20
                      // musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view-7;
                      music_select_iconnr--;                  	// den rigtige valgte af 1 til cd antal
                      musicoversigt.onScroll(-11.25);
                  }
                  musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view-1;
                  if (musicoversigt.selected_icon_in_view<1) musicoversigt.selected_icon_in_view=1;
                }
                // printf("music_select_iconnr=%d music_key_selected=%d selected_icon_in_view=%d \n",music_select_iconnr,music_key_selected,musicoversigt.selected_icon_in_view);
              }


              if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (spotifyknapnr>1) {
                  if (spotifyknapnr==1) {
                    if (spotify_selected_startofset>0) {
                      spotify_selected_startofset-=8;
                      spotifyknapnr+=7;
                    }
                  }
                  spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view-1;
                  spotifyknapnr--;
                  spotify_key_selected--;
                  spotify_select_iconnr--;
                } else {
                  if (spotify_selected_startofset>0) {
                    spotify_selected_startofset-=8;
                    spotifyknapnr+=7;
                    spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view-7;
                  }
                }
                spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view-1;
                if (spotify_oversigt.selected_icon_in_view<=0) spotify_oversigt.selected_icon_in_view=1;
                // printf("selected_icon_in_view=%d\n",spotify_oversigt.selected_icon_in_view);
              }
              // tidal left
              if ((vis_tidal_oversigt) && (!(ask_open_dir_or_play_tidal))) {
                if (tidalknapnr>1) {
                  if (tidalknapnr==1) {
                    if (tidal_selected_startofset>0) {
                      tidal_selected_startofset-=8;                                   // last line
                      tidalknapnr+=8;
                    }
                  }
                  tidalknapnr--;
                  tidal_key_selected--;
                  tidal_select_iconnr--;
                } else {
                  if (tidal_selected_startofset>0) {
                    tidal_selected_startofset-=8;                                     // next line
                    tidalknapnr+=8;
                  }                    
                }
                tidal_oversigt.selected_icon_in_view=tidal_oversigt.selected_icon_in_view-1;
                if (tidal_oversigt.selected_icon_in_view<=0) tidal_oversigt.selected_icon_in_view=1;
                // printf("selected_icon_in_view=%d\n",tidal_oversigt.selected_icon_in_view);
              }

              if ((vis_film_oversigt) && (film_oversigt.film_is_playing) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                // tilbage 10 sekunder
                film_oversigt.tilbage10sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }

              if ((vis_nyefilm_oversigt) && (film_oversigt.film_is_playing)) {
                film_oversigt.tilbage10sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }


              // film/movie left

              if ((vis_film_oversigt) && (film_oversigt.film_is_playing==false) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (film_key_selected>1) {
                  film_key_selected--;
                  film_select_iconnr--;
                } else {
                  if (film_select_iconnr>0) {
                    // _fangley-=MOVIE_CS;
                    film_key_selected+=fnumbersoficonline-1;	// den viste på skærm af 1 til 20
                    film_select_iconnr--;			// den rigtige valgte af 1 til cd antal
                    film_oversigt.onScroll(-11.25);
                  }
                }
                
                film_oversigt.selected_icon_in_view=film_oversigt.selected_icon_in_view-1;
                if (film_oversigt.selected_icon_in_view<=0) film_oversigt.selected_icon_in_view=1;
                // printf("selected_icon_in_view=%d\n",film_oversigt.selected_icon_in_view);
              }


              if ((vis_recorded_oversigt) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (visvalgtnrtype==1) visvalgtnrtype=2;
                else if (visvalgtnrtype==2) visvalgtnrtype=1;
              }
              if ((vis_radio_oversigt) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (radio_key_selected>1) {
                  radio_key_selected--;
                  radio_select_iconnr--;
                  radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view-1;
                } else {
                  if (radio_select_iconnr>0) {
                    
                    radio_key_selected+=rnumbersoficonline-1;	// den viste på skærm af 1 til 20
                    radio_select_iconnr--;			// den rigtige valgte af 1 til cd antal
                    radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view-7;
                    radiooversigt.onScroll(-11.25);
                  }
                }
                // printf("radio_select_iconnr=%d radio_key_selected=%d selected_icon_in_view=%d \n",radio_select_iconnr,radio_key_selected,radiooversigt.selected_icon_in_view);
              }
              // left key.
              if ((vis_stream_oversigt) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (stream_key_selected>1) {
                  stream_key_selected--;
                    stream_select_iconnr--;
                } else {
                  if (stream_select_iconnr>0) {
                    
                    stream_key_selected+=snumbersoficonline-1;	// den viste på skærm af 1 til 20
                      stream_select_iconnr--;			                // den rigtige valgte af 1 til cd antal
                  }
                }
                if (stream_select_iconnr>0) stream_select_iconnr--;
                streamoversigt.selected_icon_in_view=streamoversigt.selected_icon_in_view-1;
                if (streamoversigt.selected_icon_in_view<=0) streamoversigt.selected_icon_in_view=1;
              }
              // left key
              // if indside tv overoview
              // normal tv overview select last channel
              if (((vis_tv_oversigt) && (tidal_oversigt.do_setup_tidal_start_entry==false)) || (aktiv_tv_oversigt.vis_tv_guide)) {
                // Normal view
                if (!(do_show_tvgraber)) {
                  if ((tvvisvalgtnrtype==1) && (tvsubvalgtrecordnr>0)) {
                    tvsubvalgtrecordnr--;
                    // tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr+1,tvsubvalgtrecordnr));
                  }
                }
              }
              if (do_show_tvgraber) {
                aktiv_tv_oversigt.edit_mode = false;
                channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=false;
                // first reset all other
                // for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                // channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=false;
              }
              /*
              if ((do_show_setup) && (do_show_tvgraber)) {
                channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected=! channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].selected;
              }
              */
              break;  
      case 102: // Right key
              if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr<musicoversigt_antal) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if ((music_key_selected % (mnumbersoficonline*4)==0) || ((music_select_iconnr==((mnumbersoficonline*4)-1)) && (music_key_selected % mnumbersoficonline==0))) {
                  // _spangley+=MUSIC_CS;
                  music_key_selected-=mnumbersoficonline;			// den viste på skærm af 1 til 20
                  musicoversigt.onScroll(+11.25);
                  musicoversigt.selected_icon_in_view=40-7;
                  music_select_iconnr++;	                 		// den rigtige valgte af 1 til cd antal
                  musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view+1;
                } else {
                  music_select_iconnr++;			                // den rigtige valgte af 1 til cd antal
                  musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view+1;
                }
                music_key_selected++;
              }

              if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (do_show_spotify_search_oversigt==false) {
                  if ((spotifyknapnr+spotify_selected_startofset)<spotify_oversigt.antal_spotify_streams()+1) {
                    if (spotifyknapnr+1>40) {

                      if (spotify_oversigt.selected_icon_in_view==40) {
                        spotify_oversigt.onScroll(+11.25);
                        spotify_oversigt.selected_icon_in_view=40-7;
                      }

                      spotify_selected_startofset+=8;
                      spotifyknapnr-=(spotify_selected_startofset-1);
                    } else {
                      spotifyknapnr++;
                      spotify_key_selected++;
                      spotify_select_iconnr++;
                      spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view+1;
                    }
                  }
                }
                // spotify online search
                // key right
                if (do_show_spotify_search_oversigt) {
                  if ((spotifyknapnr+spotify_selected_startofset)<spotify_oversigt.antal_spotify_streams()+1) {
                    if (spotifyknapnr+1>32) {
                      spotify_selected_startofset+=8;
                      spotifyknapnr-=(spotify_selected_startofset-1);
                    } else {
                      spotifyknapnr++;
                      spotify_key_selected++;
                      spotify_select_iconnr++;
                      spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view+1;
                    }
                  }
                }
              }

              if ((vis_tidal_oversigt) && (!(ask_open_dir_or_play_tidal)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (do_show_tidal_search_oversigt==false) {
                  if ((tidalknapnr+tidal_selected_startofset)<tidal_oversigt.streamantal()) {
                    /*
                    if (tidal_select_iconnr+1>40) {
                      tidal_selected_startofset+=8;                                   // last line
                      tidalknapnr+=1;
                      // tidalknapnr-=(tidal_selected_startofset-1);
                    } else {
                      tidalknapnr++;
                      tidal_key_selected++;
                      tidal_select_iconnr++;
                    }
                    */

                    // scroll view if needed
                    if (tidal_oversigt.selected_icon_in_view==40) {
                      tidal_oversigt.onScroll(+11.25);
                      tidal_oversigt.selected_icon_in_view=40-7;
                      tidal_selected_startofset+=8;                                   // last line
                      tidalknapnr+=1;
                    } else {
                      tidalknapnr++;
                      tidal_key_selected++;
                      tidal_select_iconnr++;
                      tidal_oversigt.selected_icon_in_view=tidal_oversigt.selected_icon_in_view+1;
                    }
                  }
                }
              }


              if (do_show_tidal_search_oversigt) {
                if ((tidalknapnr+tidal_selected_startofset)<tidal_oversigt.streamantal()+1) {
                  if (tidalknapnr+1>32) {
                    tidal_selected_startofset+=8;
                    tidalknapnr-=(tidal_selected_startofset-1);
                    if (tidal_oversigt.selected_icon_in_view==32) {
                      tidal_oversigt.onScroll(+11.25);
                      // tidal_oversigt.selected_icon_in_view=32-7;
                      tidal_selected_startofset+=8;                                   // last line
                      tidalknapnr+=1;
                    }
                  } else {
                    tidalknapnr++;
                    tidal_key_selected++;
                    tidal_select_iconnr++;
                    tidal_oversigt.selected_icon_in_view=tidal_oversigt.selected_icon_in_view+1;
                  }
                }
              }
            
              if ((vis_film_oversigt) && (film_oversigt.film_is_playing) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                // frem 10 sekunder
                film_oversigt.frem10sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }

              if ((vis_nyefilm_oversigt) && (film_oversigt.film_is_playing)) {
                film_oversigt.frem10sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }


              if ((vis_film_oversigt) && (film_oversigt.film_is_playing==false) && (aktiv_tv_oversigt.vis_tv_guide==false) && ((int unsigned) (film_select_iconnr+1)<film_oversigt.filmoversigt.size()-1) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if ((film_key_selected % (mnumbersoficonline*4)==0) || ((film_select_iconnr==14) && (film_key_selected % mnumbersoficonline==0))) {
                  int aaaa=film_oversigt.filmoversigt.size();
                  // _fangley+=MOVIE_CS;
                  film_key_selected-=mnumbersoficonline;	// den viste på skærm af 1 til 20
                  film_select_iconnr++;			              // den rigtige valgte af 1 til film antal
                } else {
                  film_select_iconnr++;			              // den rigtige valgte af 1 til film antal
                }
                film_key_selected++;
                film_oversigt.selected_icon_in_view=film_oversigt.selected_icon_in_view+1;
                if (film_oversigt.selected_icon_in_view>=40) {
                  film_oversigt.onScroll(+11.25);
                }
              }


              if ((vis_recorded_oversigt) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (visvalgtnrtype==1) visvalgtnrtype=2;
                else if (visvalgtnrtype==2) visvalgtnrtype=1;
              }


              // radio
              // key right              
              if ((vis_radio_oversigt)  && (radio_select_iconnr<radiooversigt.radioantal()) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if ((radio_key_selected % (rnumbersoficonline*3)==0) || ((radio_select_iconnr==19) && (radio_key_selected % rnumbersoficonline==0))) {
                  
                  radio_key_selected-=rnumbersoficonline;	// den viste på skærm af 1 til 20
                  radio_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                } else {
                  radio_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                }
                if (radiooversigt.selected_icon_in_view>=40) {
                  radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view+1;
                  radiooversigt.onScroll(+11.25);
                } else {
                  radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view+1;
                }
                radio_key_selected++;
              }
              
              // Podcast
              // key right
              if (vis_stream_oversigt) {
                // normal
                if (rss_search_podcast_string=="") {
                  if ((stream_select_iconnr<streamoversigt.antalstreams()) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                    // er vi på første skærm ingen scroll
                    // så scroll
                    if ((stream_key_selected % (snumbersoficonline*5)==0) || ((stream_select_iconnr==19) && (stream_key_selected % snumbersoficonline==0))) {
                      // ikke max
                      if ((stream_select_iconnr+1)<streamoversigt.antal_rss_streams()) {
                        
                        stream_key_selected-=snumbersoficonline;	// den viste på skærm af 1 til 20
                        stream_select_iconnr++;			// den rigtige valgte af 1 til stream antal
                        streamoversigt.onScroll(+11.25);
                      }
                    } else {
                      int ant=streamoversigt.antal_rss_streams();
                      int ant2=streamoversigt.antalstreams();
                      if (stream_select_iconnr+1<ant2) {
                        stream_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                        stream_key_selected++;
                      }
                    }
                    streamoversigt.selected_icon_in_view=streamoversigt.selected_icon_in_view+1;
                    // printf("stream_select_iconnr=%d stream_key_selected=%d selected_icon_in_view=%d \n",stream_select_iconnr,stream_key_selected,streamoversigt.selected_icon_in_view);

                  }
                } else {
                  // search podcast list
                  if ((stream_select_iconnr<streamoversigt.FeedCatalog_search_antalstreams()) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                    // er vi på første skærm ingen scroll
                    // så scroll
                    if ((stream_key_selected % (snumbersoficonline*5)==0) || ((stream_select_iconnr==19) && (stream_key_selected % snumbersoficonline==0))) {
                      // ikke max
                      if ((stream_select_iconnr+1)<streamoversigt.FeedCatalog_search_antalstreams()) {
                        
                        stream_key_selected-=snumbersoficonline;	// den viste på skærm af 1 til 20
                        stream_select_iconnr++;			// den rigtige valgte af 1 til stream antal
                      }
                    } else {
                      int ant=streamoversigt.FeedCatalog_search_antalstreams();
                      if (stream_select_iconnr+1<ant) {
                        stream_select_iconnr++;			// den rigtige valgte af 1 til cd antal
                        stream_key_selected++;
                      }
                    }
                  }
                }
              }

              if ((vis_tv_oversigt) || (aktiv_tv_oversigt.vis_tv_guide)) {
                // Normal view
                if (!(do_show_tvgraber)) {
                  if ((tvvisvalgtnrtype==1) && (tvsubvalgtrecordnr<aktiv_tv_oversigt.tvkanaler[tvvalgtrecordnr].programs.size())) {
                    // tvvalgtrecordnr++;
                    tvsubvalgtrecordnr++;
                    
                    // tvsubvalgtrecordnr=aktiv_tv_oversigt.findguidetvtidspunkt(tvvalgtrecordnr,aktiv_tv_oversigt.hentprgstartklint(tvvalgtrecordnr+1,tvsubvalgtrecordnr));
                  }
                  // Show tv graber setup vibreak;ew over tv_oversigt
                }
              }
              if (do_show_tvgraber) {
                aktiv_tv_oversigt.edit_mode = true;
                channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=true;
                // first reset all other
                // for(int i=0;i<MAXCHANNEL_ANTAL-1;i++) channel_list[i].changeordernr=false;
                // channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset].changeordernr=false;
              }

              break;
      case 103: // key down
              if (do_show_torrent==false) {
                // bruges af ask_open_dir_or_play
                if ((vis_music_oversigt) && (ask_open_dir_or_play) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((int) (do_show_play_open_select_line+do_show_play_open_select_line_ofset)<dirmusic.numbersinlist()-1) {
                    if (do_show_play_open_select_line<15) do_show_play_open_select_line++; else do_show_play_open_select_line_ofset++;
                  }
                }
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr+mnumbersoficonline<=musicoversigt_antal) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((unsigned int) music_key_selected>=((mnumbersoficonline*3)+1)) {
                    if (music_key_selected<(music_select_iconnr+mnumbersoficonline)) {
                      // _mangley+=MUSIC_CS;								//scroll gfx down
                      music_select_iconnr+=mnumbersoficonline;
                      // do_music_icon_anim_icon_ofset=1;                       // set scroll
                    }
                  } else {
                    music_key_selected+=mnumbersoficonline;
                    music_select_iconnr+=mnumbersoficonline;
                  }
                  if (musicoversigt.selected_icon_in_view+8>40) {
                    musicoversigt.onScroll(+11.25);
                    // music_selected_startofset+=8;
                    musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view+8;
                  } else {
                    musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view+8;
                  }
                }
              }

              if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                // select device to play on
                if (do_select_device_to_play) {
                  // select play device
                  spotify_oversigt.select_device_to_play();
                  // if ((spotify_oversigt.active_spotify_device!=-1) && (spotify_oversigt.active_spotify_device<9)) {
                  // if (strcmp(spotify_oversigt.get_device_name(spotify_oversigt.active_spotify_device+1),"")!=0) {
                  // if (spotify_oversigt.active_spotify_device<9) spotify_oversigt.active_spotify_device+=1;
                  // }
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
                  if (spotify_oversigt.selected_icon_in_view+8>40) {
                    spotify_oversigt.onScroll(+11.25);
                    // spotify_selected_startofset+=8;
                  } else {
                    spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view+8;
                  }
                }
              }


              if ((vis_tidal_oversigt) && (!(ask_open_dir_or_play_tidal)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                // select device to play on
                if (do_select_device_to_play) {
                  // select play device
                  if ((tidal_oversigt.active_tidal_device!=-1) && (tidal_oversigt.active_tidal_device<9)) {
                    if (strcmp(tidal_oversigt.get_device_name(tidal_oversigt.active_tidal_device+1),"")!=0) {
                      if (tidal_oversigt.active_tidal_device<9) tidal_oversigt.active_tidal_device+=1;
                    }
                  }
                } else {
                  // move coursor
                  if ((tidalknapnr+tidal_selected_startofset+tnumbersoficonline)<tidal_oversigt.streamantal()+1) {
                    /*
                    if ((((tidalknapnr+tnumbersoficonline)>40) && (do_show_tidal_search_oversigt==false)) || (((tidalknapnr+tnumbersoficonline)>32) && (do_show_tidal_search_oversigt==true))) {
                      if ((tidalknapnr+tnumbersoficonline)<tidal_oversigt.streamantal()) {
                        tidal_selected_startofset+=8;
                      } else {
                        if ((tidalknapnr-1)<tidal_oversigt.streamantal()) {
                          tidalknapnr++;
                          tidal_key_selected+=1;
                          tidal_select_iconnr+=1;
                        }
                      }
                    } else {
                      tidalknapnr+=tnumbersoficonline;
                      tidal_key_selected+=tnumbersoficonline;
                      tidal_select_iconnr+=tnumbersoficonline;

                    }
                      */                      
                    // scroll view if needed
                    tidalknapnr+=tnumbersoficonline;
                    tidal_key_selected+=tnumbersoficonline;
                    tidal_select_iconnr+=tnumbersoficonline;

                    if (tidal_oversigt.selected_icon_in_view+8>40) {
                      tidal_oversigt.onScroll(+11.25);
                      // tidal_selected_startofset+=8;
                    } else {
                      tidal_oversigt.selected_icon_in_view=tidal_oversigt.selected_icon_in_view+8;
                    }
                  }
                }
              }
              // movie
              // key down
              if ((vis_film_oversigt) && (film_oversigt.film_is_playing==false) && (aktiv_tv_oversigt.vis_tv_guide==false) && (film_oversigt.editmode==false) && ((int) (film_select_iconnr+fnumbersoficonline)<(int) film_oversigt.filmoversigt.size()-1)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (film_key_selected>=18) {
                  // _fangley+=MOVIE_CS;
                  film_select_iconnr+=fnumbersoficonline;
                } else {
                  film_key_selected+=fnumbersoficonline;
                  film_select_iconnr+=fnumbersoficonline;
                }
                film_oversigt.selected_icon_in_view=film_oversigt.selected_icon_in_view+8;
                if (film_oversigt.selected_icon_in_view>32) {
                  film_oversigt.onScroll(+15.25);
                }
              }

              if ((vis_film_oversigt) && (film_oversigt.film_is_playing) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                // tilbage 60 sekunder
                film_oversigt.tilbage60sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }

              if ((vis_nyefilm_oversigt) && (film_oversigt.film_is_playing)) {
                film_oversigt.tilbage60sec();
                do_show_film_status_info=true;
                film_oversigt.film_fader=1.0f;
              }

              // radio
              // key down
              if ((vis_radio_oversigt) && (show_radio_options==false) && ((radio_select_iconnr+rnumbersoficonline)<radiooversigt.radioantal()-1)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (radio_key_selected>=20) {
                  
                  radio_select_iconnr+=rnumbersoficonline;
                } else {
                  radio_key_selected+=rnumbersoficonline;
                  radio_select_iconnr+=rnumbersoficonline;
                }
                radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view+8;
                if (radiooversigt.selected_icon_in_view>40-(1*8)) {
                  radiooversigt.onScroll(+11.25);
                }
              }

              if (film_oversigt.editmode) {
                do_show_film_edit_select_linie++;               // editor add new lines if needed
              }
              
              if (vis_stream_oversigt) {
                if ((streamoversigt.selected_icon_in_view+8)<=streamoversigt.streamantal()) {
                  streamoversigt.selected_icon_in_view=streamoversigt.selected_icon_in_view+8;
                }
                
                if (streamoversigt.selected_icon_in_view>40-(1*8)) {
                  streamoversigt.onScroll(+11.25);
                }
              }

              // key down
              if ((vis_recorded_oversigt)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (visvalgtnrtype==1) {
                  if ((int) valgtrecordnr<(int) recorded_oversigt.top_antal()) {
                    valgtrecordnr++;
                    subvalgtrecordnr=0;
                  }
                } else if (visvalgtnrtype==2) {
                  if ((int) subvalgtrecordnr<(int) recorded_oversigt.programs_type_antal(valgtrecordnr)-1) {
                    subvalgtrecordnr++;
                  }
                }
                reset_recorded_texture = true;                                //
              }

              if ((vis_tv_oversigt) || (aktiv_tv_oversigt.vis_tv_guide)) {
                tvvalgtrecordnr++;
              }

              if (do_show_tvgraber) {
                if (aktiv_tv_oversigt.edit_mode == true) {
                  if (do_show_setup_select_linie+tvchannel_startofset < channel_list.channel_list.size()) {
                    int i = 0;
                    bool fundet=false;                    
                    while ((!fundet) && (i<channel_list.channel_list.size())) {
                      if (channel_list.channel_list[i].changeordernr==true) {
                        fundet=true;
                      } else i++;
                    }
                    if (fundet) {
                      std::swap(channel_list.channel_list[i] , channel_list.channel_list[i+1]);
                    }
                    do_show_setup_select_linie++;
                    // if (channel_list.channel_list[(do_show_setup_select_linie)+tvchannel_startofset].changeordernr) {
                    std::cout << "here" << std::endl;
                    //std::swap(channel_list.channel_list[(do_show_setup_select_linie-1)+tvchannel_startofset] , channel_list.channel_list[(do_show_setup_select_linie)+tvchannel_startofset]);
                    // }
                    // do_show_setup_select_linie++;
                  }
                } else {
                  if (do_show_setup_select_linie+tvchannel_startofset < channel_list.channel_list.size()) do_show_setup_select_linie++;
                }
              }


              if (do_show_setup_backend) {
                if (do_show_setup_select_linie<7) do_show_setup_select_linie++;
              }

              // tidal setup menu
              if (do_show_setup_tidal) {
                if (do_show_setup_select_linie<2) do_show_setup_select_linie++;
              }
              // tidal editor
              if (tidal_oversigt.do_setup_tidal_start_entry) {
                if (do_show_setup_select_linie<18) do_show_setup_select_linie++;
              }

              if (do_show_setup_font) {
                if (config_font_select<aktivfont.mastercount) config_font_select++;
              }

              if (do_show_setup_keys) {
                if (do_show_setup_select_linie<11) do_show_setup_select_linie++;
              }

              // rss setup
              // setup rss source window
              if (do_show_setup_rss) {
                if ((rssstreamoversigt.setup_select_linie<35) && ((rssstreamoversigt.setup_select_linie/2)+configrss_ofset<rssstreamoversigt.streamantal())) {
                  rssstreamoversigt.setup_select_linie++;
                } else {
                  if ((((rssstreamoversigt.setup_select_linie/2)+1+configrss_ofset))<=rssstreamoversigt.streamantal()) {
                    configrss_ofset++;
                    // rssstreamoversigt.rss_source_feed_vector.insert(streamoversigt.antalstreams());
                  } else {
                    if ((((rssstreamoversigt.setup_select_linie/2)+1+configrss_ofset))<=rssstreamoversigt.streamantal()) {
                      realrssrecordnr++;
                      configrss_ofset++;
                    }
                  }
                }
                if (((rssstreamoversigt.setup_select_linie+configrss_ofset) % 2)==0) {
                  if ((realrssrecordnr)<43) realrssrecordnr++;
                }
              }
              // torrent
              if ((do_show_torrent)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (do_show_torrent_options == false) {
                  torrent_downloader.next_edit_line();
                } else {
                  if (do_show_torrent_options_move == false) {
                    torrent_downloader.next_edit_line_info();  
                  } else {
                    torrent_downloader.next_edit_line_move_info();
                  }
                }
              }


              break;
      case 101: // key up
              if (do_show_torrent==false) {
                if ((vis_music_oversigt) && (ask_open_dir_or_play)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if (do_show_play_open_select_line>0) do_show_play_open_select_line--; else
                    if (do_show_play_open_select_line_ofset>0) do_show_play_open_select_line_ofset--;
                }
                // music stuf
                if ((vis_music_oversigt) && (!(ask_open_dir_or_play)) && (music_select_iconnr>(mnumbersoficonline-1))  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((_mangley>0) && ((unsigned int) music_key_selected<=mnumbersoficonline) && (music_select_iconnr>(mnumbersoficonline-1))) {
                    // _mangley-=MUSIC_CS;
                    // do_music_icon_anim_icon_ofset = -1;			// set scroll
                    music_select_iconnr -= mnumbersoficonline;
                  } else if ((music_select_iconnr-mnumbersoficonline)>0) {
                    music_select_iconnr -= mnumbersoficonline;
                  }
                  if (music_key_selected>(int ) mnumbersoficonline) music_key_selected-=mnumbersoficonline;
                  
                  musicoversigt.selected_icon_in_view=musicoversigt.selected_icon_in_view-8;
                  if (music_key_selected-8<1) {
                    musicoversigt.onScroll(-11.25);
                  }
                  if (musicoversigt.selected_icon_in_view<1) musicoversigt.selected_icon_in_view=1;
                }


                if ((vis_spotify_oversigt) && (!(ask_open_dir_or_play_spotify)) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  // select play device
                  if (do_select_device_to_play) {
                    if (spotify_oversigt.active_spotify_device>0) spotify_oversigt.active_spotify_device-=1;
                  } else {
                    // move coursor
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
                    if (spotify_oversigt.selected_icon_in_view-8<1) {
                      spotify_oversigt.onScroll(-11.25);
                    }
                    spotify_oversigt.selected_icon_in_view=spotify_oversigt.selected_icon_in_view-8;
                    if (spotify_oversigt.selected_icon_in_view<=0) spotify_oversigt.selected_icon_in_view=1;
                  }
                }


                // tidal stuf
                if ((vis_tidal_oversigt) && (!(ask_open_dir_or_play_tidal))  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  // select play device
                  if (do_select_device_to_play) {
                    if (tidal_oversigt.active_tidal_device>0) tidal_oversigt.active_tidal_device-=1;
                  } else {
                    // move coursor
                    if (((tidalknapnr+tidal_selected_startofset)-tnumbersoficonline)>=1) {
                      if (tidal_selected_startofset>0) {
                        if ((tidalknapnr>=1) && (tidalknapnr<=9)) {
                          tidal_selected_startofset-=8;
                        } else {
                          if (tidalknapnr>1) {
                            tidalknapnr-=tnumbersoficonline;
                            tidal_key_selected-=tnumbersoficonline;
                            tidal_select_iconnr-=tnumbersoficonline;
                          }
                        }
                      } else {
                        if (tidalknapnr>1) {
                          tidalknapnr-=tnumbersoficonline;
                          tidal_key_selected-=tnumbersoficonline;
                          tidal_select_iconnr-=tnumbersoficonline;
                        }
                      }
                      tidal_oversigt.selected_icon_in_view=tidal_oversigt.selected_icon_in_view-8;
                      if (tidal_oversigt.selected_icon_in_view<=0) tidal_oversigt.selected_icon_in_view=1;
                      if (tidal_oversigt.selected_icon_in_view-8<1) {
                        tidal_oversigt.onScroll(-11.25);
                      }
                    }
                  }
                }

                if ((vis_film_oversigt) && (film_oversigt.film_is_playing==false) && (aktiv_tv_oversigt.vis_tv_guide==false) && (film_oversigt.editmode==false) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((vis_film_oversigt) && (film_select_iconnr>(fnumbersoficonline-1))) {
                    if ((film_key_selected<=fnumbersoficonline) && (film_select_iconnr>(fnumbersoficonline-1))) {
                      // _fangley -= MOVIE_CS;
                      film_select_iconnr-=fnumbersoficonline;
                    } else {
                      film_select_iconnr-=fnumbersoficonline;
                    }
                    if (film_key_selected>fnumbersoficonline) {
                      film_key_selected-=fnumbersoficonline;
                    }
                  }
                  film_oversigt.selected_icon_in_view=film_oversigt.selected_icon_in_view-8;
                  if (film_oversigt.selected_icon_in_view-(2*8)<1) {
                    film_oversigt.onScroll(-15.25);
                  }
                  if (film_oversigt.selected_icon_in_view<0) film_oversigt.selected_icon_in_view=1;
                }

                if ((vis_film_oversigt) && (film_oversigt.film_is_playing) && (aktiv_tv_oversigt.vis_tv_guide==false)) {
                  film_oversigt.frem60sec();
                  do_show_film_status_info=true;
                  film_oversigt.film_fader=1.0f;
                }

                if ((vis_nyefilm_oversigt) && (film_oversigt.film_is_playing)) {
                    film_oversigt.frem60sec();
                  do_show_film_status_info=true;
                  film_oversigt.film_fader=1.0f;
                }

                if (film_oversigt.editmode) {
                  if (do_show_film_edit_select_linie>0) do_show_film_edit_select_linie--;               // editor add new lines if needed
                }

                
                if ((vis_radio_oversigt) && (show_radio_options==false) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((vis_radio_oversigt) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                    if ((radio_key_selected<=fnumbersoficonline) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                      
                      radio_select_iconnr-=rnumbersoficonline;
                      radiooversigt.onScroll(-11.25);
                    } else radio_select_iconnr-=rnumbersoficonline;
                    if (radio_key_selected>rnumbersoficonline) radio_key_selected-=rnumbersoficonline;
                  }
                  radiooversigt.selected_icon_in_view=radiooversigt.selected_icon_in_view-8;
                  if (radiooversigt.selected_icon_in_view<=0) radiooversigt.selected_icon_in_view=1;
                  if (radiooversigt.selected_icon_in_view-8<1) {
                    radiooversigt.onScroll(-11.25);
                  }
                  if (radiooversigt.selected_icon_in_view<0) radiooversigt.selected_icon_in_view=1;
                }
                // if ((vis_radio_oversigt) && (show_radio_options)) radiooversigt.lastradiooptselect();

                //
                // stream stuf (podcast)
                //
                if ((vis_stream_oversigt) && (show_stream_options==false)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  //if ((vis_stream_oversigt) && (stream_select_iconnr>(snumbersoficonline-1))) {
                  if ((vis_stream_oversigt) && (stream_select_iconnr>(snumbersoficonline-1))) {
                    /*
                    if ((stream_key_record_oversigtselected<=snumbersoficonline) && (stream_select_iconnr>(snumbersoficonline-1))) {
                      stream_select_iconnr-=snumbersoficonline;
                    } else {
                      stream_select_iconnr-=snumbersoficonline;
                    }
                    */
                  } else {
                    if (stream_key_selected>snumbersoficonline) stream_key_selected-=snumbersoficonline;
                    if (snumbersoficonline<0) snumbersoficonline=0;
                  }
                  if (streamoversigt.selected_icon_in_view-8<1) {
                    streamoversigt.onScroll(-11.25);
                  }
                  streamoversigt.selected_icon_in_view=streamoversigt.selected_icon_in_view-8;
                  if (streamoversigt.selected_icon_in_view<=0) streamoversigt.selected_icon_in_view=1;
                }


                // recorded 
                // 
                if ((vis_recorded_oversigt)  && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  if ((visvalgtnrtype==1) && (valgtrecordnr>0)) {
                    valgtrecordnr--;
                    subvalgtrecordnr=0;
                  } else if ((visvalgtnrtype==2) && (subvalgtrecordnr>0)) {
                    subvalgtrecordnr--;	// bruges til visning af optaget programmer
                  }
                  reset_recorded_texture=true;		// load optaget programs texture gen by mythtv
                }

                // tidal seup menu
                if (do_show_setup_tidal) {
                  if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                }
                // tidal editor
                if (tidal_oversigt.do_setup_tidal_start_entry) {
                  if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                }


                if ((vis_tv_oversigt)  || (aktiv_tv_oversigt.vis_tv_guide)) {
                  if (tvvalgtrecordnr>0) tvvalgtrecordnr--;
                }

                if (do_show_tvgraber) {
                  if (aktiv_tv_oversigt.edit_mode == true) {
                    if ((do_show_setup_select_linie+tvchannel_startofset>0) && (do_show_setup_select_linie+tvchannel_startofset < channel_list.channel_list.size()-1)) {
                      int i = 0;
                      bool fundet=false;                    
                      while ((!fundet) && (i<channel_list.channel_list.size())) {
                        if (channel_list.channel_list[i].changeordernr==true) {
                          fundet=true;
                        } else i++;
                      }
                      if (fundet) {
                        std::swap(channel_list.channel_list[i-1] , channel_list.channel_list[i]);
                      }
                      do_show_setup_select_linie--;
                      std::cout << "here" << std::endl;
                    }
                  } else {
                    if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                  }
                }

                if (do_show_setup_backend) {
                  if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                }

                if (do_show_setup_font) {
                  if (config_font_select>0) config_font_select--;
                }
                if (do_show_setup_keys) {
                  if (do_show_setup_select_linie>0) do_show_setup_select_linie--;
                }

                // setup rss
                if (do_show_setup_rss) {
                  if (rssstreamoversigt.setup_select_linie>0) rssstreamoversigt.setup_select_linie--;
                  else if (configrss_ofset>0) configrss_ofset--;
                  if ((((rssstreamoversigt.setup_select_linie+configrss_ofset) % 2)==0) && ((rssstreamoversigt.setup_select_linie+configrss_ofset)>0)) realrssrecordnr--;
                 }
              }
              // torrent view
              if ((do_show_torrent) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                if (do_show_torrent_options==false) {
                  torrent_downloader.last_edit_line();
                } else {
                  if (do_show_torrent_options_move == false) {
                    torrent_downloader.last_edit_line_info();  
                  } else {
                    torrent_downloader.last_edit_line_move_info();
                  }
                }
              }
              break;
      case GLUT_KEY_PAGE_UP:

              if ((vis_music_oversigt) && (music_select_iconnr>numbers_cd_covers_on_line)) {
                musicoversigt.onScroll(-11.25);
                // do_music_icon_anim_icon_ofset=-1;			              // direction -1 = up 1 = down
                // _mangley-=(41.0f);			                       			// scroll window up
                music_select_iconnr-=numbers_cd_covers_on_line;			// add to next line
              }

              if (vis_spotify_oversigt) {
                spotify_oversigt.onScroll(-11.25);
                spotify_oversigt.search_playlist_song=!spotify_oversigt.search_playlist_song;
              }
              if (vis_tidal_oversigt) {                
                if ((do_show_setup) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  tidal_oversigt.search_playlist_song=!tidal_oversigt.search_playlist_song;
                } else {
                  tidal_oversigt.onScroll(-11.25);
                }
              }

              if (vis_radio_oversigt) {                  
                radiooversigt.onScroll(-11.25);
              }
              // update tv guide
              if (vis_tv_oversigt) {
                aktiv_tv_oversigt.dags_ofset++;
                aktiv_tv_oversigt.visdato_unixtime=std::time(nullptr); // Hent nu tid
                aktiv_tv_oversigt.visdato_unixtime = aktiv_tv_oversigt.visdato_unixtime + (60*60*24);
                aktiv_tv_oversigt.get_dr_proguide(aktiv_tv_oversigt.dags_ofset); // 0 = idag 1 = imorgen osv.
                aktiv_tv_oversigt.opdatere_tv_oversigt((char *) "localhost",(char *) "mythtv",(char *) "bimmer");
              }

              break;
      case GLUT_KEY_PAGE_DOWN:
              if (vis_music_oversigt) {
                musicoversigt.onScroll(+11.25);
                // do_music_icon_anim_icon_ofset=1;			             	// direction -1 = up 1 = down
                // _mangley+=(41.0f);				                      		// scroll window down one icon
                music_select_iconnr+=numbers_cd_covers_on_line;			// add to next line
              }
              if (vis_spotify_oversigt) {
                spotify_oversigt.onScroll(+11.25);
                spotify_oversigt.search_playlist_song=!spotify_oversigt.search_playlist_song;
              }
              if (vis_tidal_oversigt) {
                if ((do_show_setup) && (tidal_oversigt.do_setup_tidal_start_entry==false)) {
                  tidal_oversigt.search_playlist_song=!tidal_oversigt.search_playlist_song;
                } else {
                  tidal_oversigt.onScroll(+11.25);
                }
              }
              if (vis_radio_oversigt) {
                radiooversigt.onScroll(+11.25);
              }
              // update tv guide
              if (vis_tv_oversigt) {
                aktiv_tv_oversigt.dags_ofset--;
                aktiv_tv_oversigt.visdato_unixtime=std::time(nullptr); // Hent nu tid
                aktiv_tv_oversigt.visdato_unixtime = aktiv_tv_oversigt.visdato_unixtime - (60*60*24);
                aktiv_tv_oversigt.get_dr_proguide(aktiv_tv_oversigt.dags_ofset); // 0 = idag 1 = imorgen osv.
                aktiv_tv_oversigt.opdatere_tv_oversigt((char *) "localhost",(char *) "mythtv",(char *) "bimmer");
              }

              break;
      case GLUT_KEY_HOME:
              if (vis_music_oversigt) {
                _mangley = 0;     	                      							  // scroll start ofset reset to start
                music_select_iconnr = 1;                                  // first icon in view left top conner
                // do_music_icon_anim_icon_ofset = 1;                        // set scroll
                music_key_selected = 1;
              }
              if ((vis_radio_oversigt) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                if ((radio_key_selected<=fnumbersoficonline) && (radio_select_iconnr>(rnumbersoficonline-1))) {
                  radio_select_iconnr = 1;
                }
                radio_key_selected = 1;
                radio_select_iconnr = 1;
              }

              break;
      case GLUT_KEY_END:
              break;
      case 114: // CTRL key
              if (vis_film_oversigt) {
                  // film_oversigt.editmode=1;
              }
              break;
      case 115:
              break;
    }
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
                   settema, setfont, setmouse, setuse3d, setland, sethostname, setdebugmode, setbackend, setscreenmode, setvideoplayer,setconfigdefaultmusicpath, \
                   setconfigdefaultmoviepath,setuvmetertype,setvolume,settvgraber,tvgraberupdate,tvguidercolor,tvguidefontsize,radiofontsize,musicfontsize, \
                   streamfontsize,moviefontsize,tidalfontsize,spotifyfontsize,spotifydefaultdevice,starred_playlistname,startspotifyonboot,rssgraberupdate,trash_torrent_files,torrent_automove_file,torrent_download_path};
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
            } else if ((strncmp(buffer+n,"tvgraber",7)==0) && (strncmp(buffer+n,"rssgraberupdate",11)!=0)) {
              command = true;
              command_nr=rssgraberupdate;
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
            } else if (strncmp(buffer+n,"tidalfontsize",12)==0) {
              command = true;
              command_nr=tidalfontsize;
              commandlength=12;
            } else if (strncmp(buffer+n,"spotifyfontsize",14)==0) {
              command = true;
              command_nr=spotifyfontsize;
              commandlength=12;
            } else if (strncmp(buffer+n,"spotifydefaultdevice",19)==0) {
              printf("Set command to set default spotify play device\n");
              command = true;
              command_nr=spotifydefaultdevice;
              commandlength=19;
            } else if (strncmp(buffer+n,"starred_playlistname",20)==0) {
              command = true;
              command_nr=starred_playlistname;
              commandlength=19;
            } else if (strncmp(buffer+n,"startspotifyonboot",18)==0) {
              command = true;
              command_nr=startspotifyonboot;
              commandlength=17;
            } else if (strncmp(buffer+n,"trash_torrent_files",19)==0) {
              command = true;
              command_nr=trash_torrent_files;
              commandlength=18;
            } else if (strncmp(buffer+n,"automove_file",13)==0) {
              command = true;
              command_nr=torrent_automove_file;
              commandlength=12;
            } else if (strncmp(buffer+n,"torrent_download_path",21)==0) {
              command = true;
              command_nr=torrent_download_path;
              commandlength=20;
            } else {
              command = false;
            }
          }
          strcpy(value,"");
          if (command) {
            long bufferlength=strlen(buffer);
            while((n<bufferlength) && (!(valueok))) {
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
            else if (command_nr==rssgraberupdate) configrssguidelastupdate=atol(value);
            // set tvguide color or no color
            else if (command_nr==tvguidercolor) {
             // if (strcmp(value,"yes")==0) aktiv_tv_oversigt.vistvguidecolors = true;
             // else aktiv_tv_oversigt.vistvguidecolors = false;
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
            // set videt player program name (if default use vlc)
            else if (command_nr==setvideoplayer) {
              if (strcmp(value,"")==0) strcpy(value,"internal");                               // set default player (internal vlc)
              strcpy(configvideoplayer,value);
              strcpy(configdefaultplayer,value);
            }
            // sound port
            else if (command_nr==setsoundoutport) {
              strcpy(configsoundoutport,value);
            }
            else if (command_nr==setscreensaver) {
              // screen saver timeout
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
              } else if (strncmp(configaktivescreensavername,"MUSICMETER2",11)==0) {
                  urtype=MUSICMETER2;
              } else if (strncmp(configaktivescreensavername,"MUSICMETER3",11)==0) {
                  urtype=MUSICMETER3;
              } else if (strncmp(configaktivescreensavername,"MUSICMETER",10)==0) {
                  urtype=MUSICMETER;
              } else if (strncmp(configaktivescreensavername,"PLASMA",6)==0) {
                  urtype=PLASMA;
              } else if (strncmp(configaktivescreensavername,"UV-METER",8)==0) {
                  urtype=UV_METER;
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
            } else if (command_nr==tidalfontsize) {
              configdefaulttidalfontsize=atof(value);                           // set movie font size
            } else if (command_nr==spotifyfontsize) {
              configdefaultspotifyfontsize=atof(value);                           // set movie font size
            } else if (command_nr==spotifydefaultdevice) {                      // do now work for now
              printf("Set default spotify play device to %s\n",value);
              #ifdef ENABLE_SPOTIFY
              // strcpy(spotify_oversigt.active_default_play_device_name,value);   //
              #endif
            } else if (command_nr==starred_playlistname) {
              strcpy(configbackend_starred_playlistname,value);                 // star playlist in spotify
            } else if (command_nr==startspotifyonboot) {
              // check if command is enable by yes in config
              if (strcmp(value,"yes")==0) {
                configbackend_openspotify_player=true;
              } else {
                configbackend_openspotify_player=false;
              }
            } else if (command_nr==trash_torrent_files) {
              if (strcmp(value,"yes")==0) torrent_downloader.trash_torrent=true; else torrent_downloader.trash_torrent=false;
            } else if (command_nr==torrent_automove_file) {
              if (strcmp(value,"yes")==0) torrent_downloader.automove_to_movie_path=true; else torrent_downloader.automove_to_movie_path=false;
            } else if (command_nr==torrent_download_path) {
              torrent_downloader.torrent_download_path=value;          // set torrent download path
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
  char temp[4096];
  FILE *file;
  if ((file = fopen(filename, "w"))) {
    fputs("#\n# Config file for mythtv-controller.\n#\n",file);
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
    // show types in config file.
    fputs("#Types of screen saver\n# digital\n# analog\n# 3D\n# 3D2\n# PICTURE3D\n# MUSICMETER\n# MUSICMETER2\n# MUSICMETER3\n# UV-METER\n",file);
    fputs("screensavername=",file);
    if (urtype==DIGITAL) fputs("digital\n",file);
    else if (urtype==ANALOG) fputs("analog\n",file);
    else if (urtype==SAVER3D) fputs("3D\n",file);
    else if (urtype==SAVER3D2) fputs("3D2\n",file);
    else if (urtype==PICTURE3D) fputs("PICTURE3D\n",file);
    else if (urtype==MUSICMETER) fputs("MUSICMETER\n",file);
    else if (urtype==MUSICMETER2) fputs("MUSICMETER2\n",file);
    else if (urtype==MUSICMETER3) fputs("MUSICMETER3\n",file);
    else if (urtype==UV_METER) fputs("UV-METER\n",file);
    else if (urtype==PLASMA) fputs("PLASMA\n",file);
    else fputs("None\n",file);
    snprintf(temp,sizeof(temp),"screensize=%d\n",screen_size);
    fputs(temp,file);
    sprintf(temp,"tema=%d\n",tema);
    fputs(temp,file);
    sprintf(temp,"font=%s\n",configfontname);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"mouse=%s\n",configmouse);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"use3d=%s\n",configuse3deffect);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"land=%d\n",configland);
    fputs(temp,file);
    if (full_screen) snprintf(temp,sizeof(temp),"fullscreen=true\n"); else sprintf(temp,"fullscreen=false\n");
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"debug=%d\n",debugmode);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"videoplayer=%s\n",configdefaultplayer);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"configdefaultmusicpath=%s\n",configdefaultmusicpath);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"configdefaultmoviepath=%s\n",configdefaultmoviepath);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"uvmetertype=%d\n",configuvmeter);                               // uv meter type
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"defaultvolume=%2.2f\n",configsoundvolume);                      // sound volume
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"tvgraber=%s\n",configbackend_tvgraber);                         // tv graber to use
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"tvgraberupdate=%ld\n",configtvguidelastupdate);
    fputs(temp,file);      
    snprintf(temp,sizeof(temp),"rssgraberupdate=%ld\n",configrssguidelastupdate);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"tvguidefontsize=%0.0f\n",configdefaulttvguidefontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"radiofontsize=%0.0f\n",configdefaultradiofontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"musicfontsize=%0.0f\n",configdefaultmusicfontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"streamfontsize=%0.0f\n",configdefaultstreamfontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"moviefontsize=%0.0f\n",configdefaultmoviefontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"tidalfontsize=%0.0f\n",configdefaulttidalfontsize);
    fputs(temp,file);
    snprintf(temp,sizeof(temp),"spotifyfontsize=%0.0f\n",configdefaultspotifyfontsize);
    fputs(temp,file);
    //aktiv_tv_oversigt.vistvguidecolors=true;
    // if (aktiv_tv_oversigt.vistvguidecolors) sprintf(temp,"tvguidercolor=yes\n");
    // else sprintf(temp,"tvguidercolor=no\n");
    // fputs(temp,file);
    #ifdef ENABLE_SPOTIFY
    //sprintf(temp,"spotifydefaultdevice=%s\n",spotify_oversigt.get_device_name(spotify_oversigt.active_default_play_device));
    //fputs(temp,file);
    #endif
    sprintf(temp,"starred_playlistname=%s\n",configbackend_starred_playlistname);
    fputs(temp,file);
    if (configbackend_openspotify_player) {
      sprintf(temp,"startspotifyonboot=yes\n");
      fputs(temp,file);
    } else {
      sprintf(temp,"startspotifyonboot=no\n");
      fputs(temp,file);
    }
    if (torrent_downloader.trash_torrent) 
      sprintf(temp,"trash_torrent_files=yes\n"); 
    else 
      sprintf(temp,"trash_torrent_files=no\n");
    fputs(temp,file);
    if (torrent_downloader.automove_to_movie_path) {
      sprintf(temp,"automove_file=yes\n");
    } else {
      sprintf(temp,"automove_file=no\n");
    }
    fputs(temp,file);
    sprintf(temp,"torrent_download_path=%s\n",torrent_downloader.torrent_download_path.c_str());
    fputs(temp,file);
    fclose(file);
  } else error = true;
  std::ofstream keyfile("mythtv-controller.keys", std::ios::binary);
  if (keyfile) {
    keyfile.write(reinterpret_cast<const char*>(configkeyslayout),sizeof(configkeytype)*12);
    keyfile.close();
  }
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
  /*
  for(int i = 0; i < storagegroupantal; i++) {
    strcpy(configstoragerecord[i].path,"");
    strcpy(configstoragerecord[i].name,"");
  }
  */
  strcpy(configdefaultplayer,"internal");	                 	// default sound player (fmod) (default) movie player
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
  strcpy(configbackend_tvgraber,"tv_grab_eu_dotmedia");     // default tv guide tv_grab_uk_tvguide
  strcpy(configbackend_tvgraberland,"");                    // default tv guide tv_grab_uk_tvguide other command
  strcpy(configbackend_starred_playlistname,"");                          // default spotify starred playlist 
  configbackend_openspotify_player=false;                   // default openspotify player local to play on.
  strcpy(configbackend_starred_playlistname,"");
  configtvguidelastupdate=0;                                // default tvguide update
  configrssguidelastupdate=0;                               // last date /unix time_t type) rssguide update
  configsoundvolume=1.0f;
  configuvmeter=1;                                          // default uv meter type 1
  for(int t=0;t<12;t++) {
    strcpy(configkeyslayout[t].cmdname,"");
    configkeyslayout[t].scrnr=0;
  }
  // set default keys
  strcpy(configkeyslayout[0].cmdname,"spotify");
  configkeyslayout[0].scrnr=0;
  strcpy(configkeyslayout[1].cmdname,"kodi");
  configkeyslayout[1].scrnr=1;
  strcpy(configkeyslayout[2].cmdname,"tidal");
  configkeyslayout[2].scrnr=1;
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
      fputs("#\n# Config file for mythtv-controller.\n#\n",file);
      fputs("mysqluser=mythtv\n",file);                  // write config info to config file
      fputs("mysqlpass=password\n",file);
      fputs("mysqlhost=localhost\n",file);
      fputs("mythhost=localhost\n",file);
      fputs("soundsystem=0\n",file);
      fputs("soundoutport=int\n",file);
      fputs("screensaver=60\n",file);
      // show types in config file.
      fputs("#Types of screen saver\n# digital\n# analog\n# 3D\n# 3D2\n# PICTURE3D\n# MUSICMETER\n# MUSICMETER2\n# MUSICMETER3\n# UV-METER\n",file);
      strcpy(temptxt,"screensavername=");
      strcat(temptxt,configaktivescreensavername);
      strcat(temptxt,"\n");
      fputs(temptxt,file);      
      fputs("screensize=3\n",file);
      fputs("tema=2\n",file);
      fputs("font=Bitstream Charter\n",file);
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
      fputs("rssgraberupdate=0\n",file);
      fputs("tvguidefontsize=18\n",file);
      fputs("radiofontsize=18\n",file);
      fputs("musicfontsize=18\n",file);
      fputs("streamfontsize=18\n",file);
      fputs("moviefontsize=18\n",file);
      fputs("tidalfontsize=18\n",file);
      fputs("spotifyfontsize=18\n",file);
      fputs("spotifydefaultdevice=\n",file);
      fputs("tidaldefaultdevice=\n",file);
      fputs("starred_playlistname=starred\n",file);       // default name for starred play list in spotify
      fputs("startspotifyonboot=yes\n",file);         // default
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
      write_logfile(logfile,(char *) debuglogdata);
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
          write_logfile(logfile,(char *) debuglogdata);
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
          write_logfile(logfile,(char *) debuglogdata);

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
    /*
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
    */
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
  
  std::ifstream keyfile("mythtv-controller.keys", std::ios::binary);
  if (keyfile) {
    keyfile.read(reinterpret_cast<char *>(configkeyslayout),sizeof(configkeytype)*12);
    keyfile.close();
  } else {
    fprintf(stderr,"No key file found. Use setup (F1) to config keys.\n");
    // write_logfile(logfile,"No key file found. Use setup (F1) to config keys.");
  }
  
  #ifdef ENABLE_SPOTIFY
  // get/set default play device on spotify
  /*
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
  */
  #endif
  if (conn) mysql_close(conn);
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





// ******************************************************************************************
//
// for touch mouse motions template
// 
// ******************************************************************************************


template<typename T>

void handleDragGeneric(bool visible, T& o, int x, int y) {
    if (!visible || !o.dragging) return;
    int dx = x - o.lastX;
    int dy = y - o.lastY;
    int dist2 = (x - o.downX)*(x - o.downX) + (y - o.downY)*(y - o.downY);
    if (dist2>16) {
      o.moved=true;
      o.dragging=true;
    }
    o.scrollPos -= dy;
    o.viewOffsetX += dx * o.scrollSpeed;
    o.viewOffsetY -= dy * o.scrollSpeed;
    o.lastDX = dx;
    o.lastDY = dy;
    o.lastX = x;
    o.lastY = y;
}


// ******************************************************************************************
//
// for touch mouse motions template
// 
// ******************************************************************************************

template<typename T>

void handlefrictionGeneric(T& o) {
  if (!o.dragging) {
    o.velocityX *= o.friction;
    o.velocityY *= o.friction;

    o.viewOffsetX += o.velocityX;
    o.viewOffsetY += o.velocityY;

    // stop når det er meget langsomt
    if (fabs(o.velocityX) < 0.001f) o.velocityX = 0;
    if (fabs(o.velocityY) < 0.001f) o.velocityY = 0;  
  }
}


// ******************************************************************************************
//
// for mouse motions
// 
// ******************************************************************************************

void mouseMotion(int x, int y) {
  handleDragGeneric(vis_stream_oversigt,  streamoversigt,  x, y);
  handleDragGeneric(vis_spotify_oversigt, spotify_oversigt, x, y);
  handleDragGeneric(vis_tidal_oversigt,   tidal_oversigt,   x, y);
  handleDragGeneric(vis_music_oversigt,   musicoversigt,   x, y);
  handleDragGeneric(vis_radio_oversigt,   radiooversigt,   x, y);
  handleDragGeneric(vis_film_oversigt,    film_oversigt,   x, y);
  glutPostRedisplay();
}


// ******************************************************************************************
//
// for idle scroll
// 
// ******************************************************************************************

void idle() {
  handlefrictionGeneric(spotify_oversigt);
  handlefrictionGeneric(tidal_oversigt);
  handlefrictionGeneric(musicoversigt);
  handlefrictionGeneric(radiooversigt);
  handlefrictionGeneric(streamoversigt);
  handlefrictionGeneric(film_oversigt);
  glutPostRedisplay();
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
  if (ok) do_system_call(command);
}


// ****************************************************************************************
//
// load img file
//
// ****************************************************************************************

GLuint loadgfxfile(char *temapath,char *dir,char *file) {
    GLuint gl_img=0;
    std::string fileload="";
    fileload=fileload + temapath;
    fileload=fileload + dir;
    fileload=fileload + file;              // filename - type
    fileload=fileload + ".png";            // add png
    if (file_exists(fileload.c_str())) gl_img = loadTexture ((char *) fileload.c_str());
    else {
      // check if file exist af .jpg file      
      fileload=fileload + temapath;
      fileload=fileload + dir;
      fileload=fileload + file;              // filename - type
      fileload=fileload + ".jpg";            // add jpg
      if (file_exists(fileload.c_str())) gl_img = loadTexture ((char *) fileload.c_str());
      else gl_img = 0;
    }
    if (gl_img == 0) fprintf(stderr,"GFXFILE %s in dir %s NOT FOUND \n",fileload.c_str(),dir);
    return(gl_img);
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
// show background picture
//
// ****************************************************************************************

void show_background() {
  // make background
  if (vis_music_oversigt) renderer.AddTextureRect(0,_textureIdback_music, 0, 0, 1920, 1280,1,1,1,1);
  else if (do_show_setup) renderer.AddTextureRect(0,_textureIdback_setup, 0, 0, 1920, 1280,1,1,1,1);
  else if (vis_radio_oversigt) renderer.AddTextureRect(0,_textureIdback_music, 0, 0, 1920, 1280,1,1,1,1);
  else renderer.AddTextureRect(0,_textureIdback_main, 0, 0, 1920, 1280,1,1,1,1);
}


// ****************************************************************************************
//
// Load tema gfx
//
// ****************************************************************************************


int team_settings_load() {

  // load json file
  std::string temaname=fmt::format("tema{}.json", tema);
  std::ifstream temasettingsfile(temaname);
  std::string temapath=fmt::format("/opt/mythtv-controller/tema{}/", tema);
  reader.parse(temasettingsfile, iRoot);
  try {
    config_menu.config_tema_path=(iRoot["tema1"].get("iconpath","0").asString());

    config_menu.config_tvguidex=(iRoot["tema1"]["icons"]["tvguide"].get("x","0").asInt());
    config_menu.config_tvguidey=(iRoot["tema1"]["icons"]["tvguide"].get("y","0").asInt());
    config_menu.config_tvguide_icon=(iRoot["tema1"]["icons"]["tvguide"].get("icon_path","0").asString());

    config_menu.config_musicx=(iRoot["tema1"]["icons"]["music"].get("x","0").asInt());
    config_menu.config_musicy=(iRoot["tema1"]["icons"]["music"].get("y","0").asInt());
    config_menu.config_music_icon=(iRoot["tema1"]["icons"]["music"].get("icon_path","0").asString());

    config_menu.config_music_activex=(iRoot["tema1"]["icons"]["music_active"].get("x","0").asInt());
    config_menu.config_music_activey=(iRoot["tema1"]["icons"]["music_active"].get("y","0").asInt());
    config_menu.config_music_active_icon=(iRoot["tema1"]["icons"]["music_active"].get("icon_path","0").asString());

    config_menu.config_mediax=(iRoot["tema1"]["icons"]["media"].get("x","0").asInt());
    config_menu.config_mediay=(iRoot["tema1"]["icons"]["media"].get("y","0").asInt());
    config_menu.config_media_icon=(iRoot["tema1"]["icons"]["media"].get("icon_path","0").asString());

    config_menu.config_media1x=(iRoot["tema1"]["icons"]["media1"].get("x","0").asInt());
    config_menu.config_media1y=(iRoot["tema1"]["icons"]["media1"].get("y","0").asInt());
    config_menu.config_media1_icon=(iRoot["tema1"]["icons"]["media1"].get("icon_path","0").asString());

    config_menu.config_spotifyx=(iRoot["tema1"]["icons"]["spotify"].get("x","0").asInt());
    config_menu.config_spotifyy=(iRoot["tema1"]["icons"]["spotify"].get("y","0").asInt());
    config_menu.config_spotify_icon=(iRoot["tema1"]["icons"]["spotify"].get("icon_path","0").asString());

    config_menu.config_spotify1x=(iRoot["tema1"]["icons"]["spotify1"].get("x","0").asInt());
    config_menu.config_spotify1y=(iRoot["tema1"]["icons"]["spotify1"].get("y","0").asInt());
    config_menu.config_spotify1_icon=(iRoot["tema1"]["icons"]["spotify1"].get("icon_path","0").asString());

    config_menu.config_tidalx=(iRoot["tema1"]["icons"]["tidal"].get("x","0").asInt());
    config_menu.config_tidaly=(iRoot["tema1"]["icons"]["tidal"].get("y","0").asInt());
    config_menu.config_tidal_icon=(iRoot["tema1"]["icons"]["tidal"].get("icon_path","0").asString());

    config_menu.config_tidal1x=(iRoot["tema1"]["icons"]["tidal1"].get("x","0").asInt());
    config_menu.config_tidal1y=(iRoot["tema1"]["icons"]["tidal1"].get("y","0").asInt());
    config_menu.config_tidal1_icon=(iRoot["tema1"]["icons"]["tidal1"].get("icon_path","0").asString());

    config_menu.config_radiox=(iRoot["tema1"]["icons"]["radio"].get("x","0").asInt());
    config_menu.config_radioy=(iRoot["tema1"]["icons"]["radio"].get("y","0").asInt());
    config_menu.config_radio_icon=(iRoot["tema1"]["icons"]["radio"].get("icon_path","0").asString());

    config_menu.config_radio1x=(iRoot["tema1"]["icons"]["radio1"].get("x","0").asInt());
    config_menu.config_radio1y=(iRoot["tema1"]["icons"]["radio1"].get("y","0").asInt());
    config_menu.config_radio1_icon=(iRoot["tema1"]["icons"]["radio1"].get("icon_path","0").asString());

    config_menu.config_moviex=(iRoot["tema1"]["icons"]["movie"].get("x","0").asInt());
    config_menu.config_moviey=(iRoot["tema1"]["icons"]["movie"].get("y","0").asInt());
    config_menu.config_movie_icon=(iRoot["tema1"]["icons"]["movie"].get("icon_path","0").asString());

    config_menu.config_movie1x=(iRoot["tema1"]["icons"]["movie1"].get("x","0").asInt());
    config_menu.config_movie1y=(iRoot["tema1"]["icons"]["movie1"].get("y","0").asInt());
    config_menu.config_movie1_icon=(iRoot["tema1"]["icons"]["movie1"].get("icon_path","0").asString());

    config_menu.config_recordedx=(iRoot["tema1"]["icons"]["recorded"].get("x","0").asInt());
    config_menu.config_recordedy=(iRoot["tema1"]["icons"]["recorded"].get("y","0").asInt());
    config_menu.config_recorded_icon=(iRoot["tema1"]["icons"]["recorded"].get("icon_path","0").asString());

    config_menu.config_recorded1x=(iRoot["tema1"]["icons"]["recorded1"].get("x","0").asInt());
    config_menu.config_recorded1y=(iRoot["tema1"]["icons"]["recorded1"].get("y","0").asInt());
    config_menu.config_recorded1_icon=(iRoot["tema1"]["icons"]["recorded1"].get("icon_path","0").asString());

    config_menu.config_closex=(iRoot["tema1"]["icons"]["close"].get("x","0").asInt());
    config_menu.config_closey=(iRoot["tema1"]["icons"]["close"].get("y","0").asInt());
    config_menu.config_close_icon=(iRoot["tema1"]["icons"]["close"].get("icon_path","0").asString());

    config_menu.config_setupx=(iRoot["tema1"]["icons"]["setup"].get("x","0").asInt());
    config_menu.config_setupy=(iRoot["tema1"]["icons"]["setup"].get("y","0").asInt());
    config_menu.config_setup_icon=(iRoot["tema1"]["icons"]["setup"].get("icon_path","0").asString());

    config_menu.config_loadingx=(iRoot["tema1"]["icons"]["loading"].get("x","0").asInt());
    config_menu.config_loadingy=(iRoot["tema1"]["icons"]["loading"].get("y","0").asInt());
    config_menu.config_loading_icon=(iRoot["tema1"]["icons"]["loading"].get("icon_path","0").asString());

    config_menu.config_playinfox=(iRoot["tema1"]["icons"]["playinfo_icon"].get("x","0").asInt());
    config_menu.config_playinfoy=(iRoot["tema1"]["icons"]["playinfo_icon"].get("y","0").asInt());
    config_menu.config_playinfo_icon=(iRoot["tema1"]["icons"]["playinfo_icon"].get("icon_path","0").asString());

    config_menu.config_downx=(iRoot["tema1"]["icons"]["down"].get("x","0").asInt());
    config_menu.config_downy=(iRoot["tema1"]["icons"]["down"].get("y","0").asInt());
    config_menu.config_down_icon=(iRoot["tema1"]["icons"]["down"].get("icon_path","0").asString());

    config_menu.config_upx=(iRoot["tema1"]["icons"]["up"].get("x","0").asInt());
    config_menu.config_upy=(iRoot["tema1"]["icons"]["up"].get("y","0").asInt());
    config_menu.config_up_icon=(iRoot["tema1"]["icons"]["up"].get("icon_path","0").asString());

    config_menu.config_musicplayer_coverx=(iRoot["tema1"]["icons"]["musicplayer_info_cover"].get("x","0").asInt());
    config_menu.config_musicplayer_covery=(iRoot["tema1"]["icons"]["musicplayer_info_cover"].get("y","0").asInt());
    config_menu.config_musicplayer_cover_sizx=(iRoot["tema1"]["icons"]["musicplayer_info_cover"].get("sizx","0").asInt());
    config_menu.config_musicplayer_cover_sizy=(iRoot["tema1"]["icons"]["musicplayer_info_cover"].get("sizy","0").asInt());
    config_menu.config_musicplayer_infox=(iRoot["tema1"]["icons"]["musicplayer_info"].get("x","0").asInt());
    config_menu.config_musicplayer_infoy=(iRoot["tema1"]["icons"]["musicplayer_info"].get("y","0").asInt());
    config_menu.config_musicplayer_info_icon=(iRoot["tema1"]["icons"]["musicplayer_info"].get("icon_path","0").asString());
    config_menu.config_musicplayer_ff_button_posx=(iRoot["tema1"]["icons"]["musicplayer_ff_button"].get("x","0").asInt());
    config_menu.config_musicplayer_ff_button_posy=(iRoot["tema1"]["icons"]["musicplayer_ff_button"].get("y","0").asInt());
    config_menu.config_musicplayer_ff_button_sizx=(iRoot["tema1"]["icons"]["musicplayer_ff_button"].get("sizx","0").asInt());
    config_menu.config_musicplayer_ff_button_sizy=(iRoot["tema1"]["icons"]["musicplayer_ff_button"].get("sizy","0").asInt());
    config_menu.config_musicplayer_bw_button_posx=(iRoot["tema1"]["icons"]["musicplayer_bw_button"].get("x","0").asInt());
    config_menu.config_musicplayer_bw_button_posy=(iRoot["tema1"]["icons"]["musicplayer_bw_button"].get("y","0").asInt());
    config_menu.config_musicplayer_bw_button_sizx=(iRoot["tema1"]["icons"]["musicplayer_bw_button"].get("sizx","0").asInt());
    config_menu.config_musicplayer_bw_button_sizy=(iRoot["tema1"]["icons"]["musicplayer_bw_button"].get("sizy","0").asInt());
    config_menu.config_musicplayer_play_button_posx=(iRoot["tema1"]["icons"]["musicplayer_play_button"].get("x","0").asInt());
    config_menu.config_musicplayer_play_button_posy=(iRoot["tema1"]["icons"]["musicplayer_play_button"].get("y","0").asInt());
    config_menu.config_musicplayer_play_button_sizx=(iRoot["tema1"]["icons"]["musicplayer_play_button"].get("sizx","0").asInt());
    config_menu.config_musicplayer_play_button_sizy=(iRoot["tema1"]["icons"]["musicplayer_play_button"].get("sizy","0").asInt());
    config_menu.config_musicplayer_stop_button_posx=(iRoot["tema1"]["icons"]["musicplayer_stop_button"].get("x","0").asInt());
    config_menu.config_musicplayer_stop_button_posy=(iRoot["tema1"]["icons"]["musicplayer_stop_button"].get("y","0").asInt());
    config_menu.config_musicplayer_stop_button_sizx=(iRoot["tema1"]["icons"]["musicplayer_stop_button"].get("sizx","0").asInt());
    config_menu.config_musicplayer_stop_button_sizy=(iRoot["tema1"]["icons"]["musicplayer_stop_button"].get("sizy","0").asInt());

    config_menu.config_spotifyplayer_infox=(iRoot["tema1"]["icons"]["spotifyplayer_info"].get("x","0").asInt());
    config_menu.config_spotifyplayer_infoy=(iRoot["tema1"]["icons"]["spotifyplayer_info"].get("y","0").asInt());
    config_menu.config_spotifyplayer_coverx=(iRoot["tema1"]["icons"]["spotifyplayer_info_cover"].get("x","0").asInt());
    config_menu.config_spotifyplayer_covery=(iRoot["tema1"]["icons"]["spotifyplayer_info_cover"].get("y","0").asInt());
    config_menu.config_spotifyplayer_cover_sizx=(iRoot["tema1"]["icons"]["spotifyplayer_info_cover"].get("sizx","0").asInt());
    config_menu.config_spotifyplayer_cover_sizy=(iRoot["tema1"]["icons"]["spotifyplayer_info_cover"].get("sizy","0").asInt());
    config_menu.config_spotifyplayer_info_icon=(iRoot["tema1"]["icons"]["spotifyplayer_info"].get("icon_path","0").asString());
    config_menu.config_spotifyplayer_ff_button_posx=(iRoot["tema1"]["icons"]["spotifyplayer_ff_button"].get("x","0").asInt());
    config_menu.config_spotifyplayer_ff_button_posy=(iRoot["tema1"]["icons"]["spotifyplayer_ff_button"].get("y","0").asInt());
    config_menu.config_spotifyplayer_ff_button_sizx=(iRoot["tema1"]["icons"]["spotifyplayer_ff_button"].get("sizx","0").asInt());
    config_menu.config_spotifyplayer_ff_button_sizy=(iRoot["tema1"]["icons"]["spotifyplayer_ff_button"].get("sizy","0").asInt());
    config_menu.config_spotifyplayer_bw_button_posx=(iRoot["tema1"]["icons"]["spotifyplayer_bw_button"].get("x","0").asInt());
    config_menu.config_spotifyplayer_bw_button_posy=(iRoot["tema1"]["icons"]["spotifyplayer_bw_button"].get("y","0").asInt());
    config_menu.config_spotifyplayer_bw_button_sizx=(iRoot["tema1"]["icons"]["spotifyplayer_bw_button"].get("sizx","0").asInt());
    config_menu.config_spotifyplayer_bw_button_sizy=(iRoot["tema1"]["icons"]["spotifyplayer_bw_button"].get("sizy","0").asInt());
    config_menu.config_spotifyplayer_play_button_posx=(iRoot["tema1"]["icons"]["spotifyplayer_play_button"].get("x","0").asInt());
    config_menu.config_spotifyplayer_play_button_posy=(iRoot["tema1"]["icons"]["spotifyplayer_play_button"].get("y","0").asInt());
    config_menu.config_spotifyplayer_play_button_sizx=(iRoot["tema1"]["icons"]["spotifyplayer_play_button"].get("sizx","0").asInt());
    config_menu.config_spotifyplayer_play_button_sizy=(iRoot["tema1"]["icons"]["spotifyplayer_play_button"].get("sizy","0").asInt());
    config_menu.config_spotifyplayer_stop_button_posx=(iRoot["tema1"]["icons"]["spotifyplayer_stop_button"].get("x","0").asInt());
    config_menu.config_spotifyplayer_stop_button_posy=(iRoot["tema1"]["icons"]["spotifyplayer_stop_button"].get("y","0").asInt());
    config_menu.config_spotifyplayer_stop_button_sizx=(iRoot["tema1"]["icons"]["spotifyplayer_stop_button"].get("sizx","0").asInt());
    config_menu.config_spotifyplayer_stop_button_sizy=(iRoot["tema1"]["icons"]["spotifyplayer_stop_button"].get("sizy","0").asInt());

    config_menu.config_tidalplayer_infox=(iRoot["tema1"]["icons"]["tidalplayer_info"].get("x","0").asInt());
    config_menu.config_tidalplayer_infoy=(iRoot["tema1"]["icons"]["tidalplayer_info"].get("y","0").asInt());
    config_menu.config_tidalplayer_info_icon=(iRoot["tema1"]["icons"]["tidalplayer_info"].get("icon_path","0").asString());
    config_menu.config_tidalplayer_coverx=(iRoot["tema1"]["icons"]["tidalplayer_info_cover"].get("x","0").asInt());
    config_menu.config_tidalplayer_covery=(iRoot["tema1"]["icons"]["tidalplayer_info_cover"].get("y","0").asInt());
    config_menu.config_tidalplayer_cover_sizx=(iRoot["tema1"]["icons"]["tidalplayer_info_cover"].get("sizx","0").asInt());
    config_menu.config_tidalplayer_cover_sizy=(iRoot["tema1"]["icons"]["tidalplayer_info_cover"].get("sizy","0").asInt());
    config_menu.config_tidalplayer_ff_button_posx=(iRoot["tema1"]["icons"]["tidalplayer_ff_button"].get("x","0").asInt());
    config_menu.config_tidalplayer_ff_button_posy=(iRoot["tema1"]["icons"]["tidalplayer_ff_button"].get("y","0").asInt());
    config_menu.config_tidalplayer_ff_button_sizx=(iRoot["tema1"]["icons"]["tidalplayer_ff_button"].get("sizx","0").asInt());
    config_menu.config_tidalplayer_ff_button_sizy=(iRoot["tema1"]["icons"]["tidalplayer_ff_button"].get("sizy","0").asInt());
    config_menu.config_tidalplayer_bw_button_posx=(iRoot["tema1"]["icons"]["tidalplayer_bw_button"].get("x","0").asInt());
    config_menu.config_tidalplayer_bw_button_posy=(iRoot["tema1"]["icons"]["tidalplayer_bw_button"].get("y","0").asInt());
    config_menu.config_tidalplayer_bw_button_sizx=(iRoot["tema1"]["icons"]["tidalplayer_bw_button"].get("sizx","0").asInt());
    config_menu.config_tidalplayer_bw_button_sizy=(iRoot["tema1"]["icons"]["tidalplayer_bw_button"].get("sizy","0").asInt());
    config_menu.config_tidalplayer_play_button_posx=(iRoot["tema1"]["icons"]["tidalplayer_play_button"].get("x","0").asInt());
    config_menu.config_tidalplayer_play_button_posy=(iRoot["tema1"]["icons"]["tidalplayer_play_button"].get("y","0").asInt());
    config_menu.config_tidalplayer_play_button_sizx=(iRoot["tema1"]["icons"]["tidalplayer_play_button"].get("sizx","0").asInt());
    config_menu.config_tidalplayer_play_button_sizy=(iRoot["tema1"]["icons"]["tidalplayer_play_button"].get("sizy","0").asInt());
    config_menu.config_tidalplayer_stop_button_posx=(iRoot["tema1"]["icons"]["tidalplayer_stop_button"].get("x","0").asInt());
    config_menu.config_tidalplayer_stop_button_posy=(iRoot["tema1"]["icons"]["tidalplayer_stop_button"].get("y","0").asInt());
    config_menu.config_tidalplayer_stop_button_sizx=(iRoot["tema1"]["icons"]["tidalplayer_stop_button"].get("sizx","0").asInt());
    config_menu.config_tidalplayer_stop_button_sizy=(iRoot["tema1"]["icons"]["tidalplayer_stop_button"].get("sizy","0").asInt());

    config_menu.config_radioplayer_infox=(iRoot["tema1"]["icons"]["radioplayer_info"].get("x","0").asInt());
    config_menu.config_radioplayer_infoy=(iRoot["tema1"]["icons"]["radioplayer_info"].get("y","0").asInt());
    config_menu.config_radioplayer_info_icon=(iRoot["tema1"]["icons"]["radioplayer_info"].get("icon_path","0").asString());
    config_menu.config_radioplayer_sizx=(iRoot["tema1"]["icons"]["radioplayer_info"].get("sizx","0").asInt());
    config_menu.config_radioplayer_sizy=(iRoot["tema1"]["icons"]["radioplayer_info"].get("sizy","0").asInt());
    config_menu.config_radioplayer_coverx=(iRoot["tema1"]["icons"]["radioplayer_info_cover"].get("x","0").asInt());
    config_menu.config_radioplayer_covery=(iRoot["tema1"]["icons"]["radioplayer_info_cover"].get("y","0").asInt());
    config_menu.config_radioplayer_cover_sizx=(iRoot["tema1"]["icons"]["radioplayer_info_cover"].get("sizx","0").asInt());
    config_menu.config_radioplayer_cover_sizy=(iRoot["tema1"]["icons"]["radioplayer_info_cover"].get("sizy","0").asInt());
    config_menu.config_radioplayer_ff_button_posx=(iRoot["tema1"]["icons"]["radioplayer_ff_button"].get("x","0").asInt());
    config_menu.config_radioplayer_ff_button_posy=(iRoot["tema1"]["icons"]["radioplayer_ff_button"].get("y","0").asInt());
    config_menu.config_radioplayer_ff_button_sizx=(iRoot["tema1"]["icons"]["radioplayer_ff_button"].get("sizx","0").asInt());
    config_menu.config_radioplayer_ff_button_sizy=(iRoot["tema1"]["icons"]["radioplayer_ff_button"].get("sizy","0").asInt());
    config_menu.config_radioplayer_bw_button_posx=(iRoot["tema1"]["icons"]["radioplayer_bw_button"].get("x","0").asInt());
    config_menu.config_radioplayer_bw_button_posy=(iRoot["tema1"]["icons"]["radioplayer_bw_button"].get("y","0").asInt());
    config_menu.config_radioplayer_bw_button_sizx=(iRoot["tema1"]["icons"]["radioplayer_bw_button"].get("sizx","0").asInt());
    config_menu.config_radioplayer_bw_button_sizy=(iRoot["tema1"]["icons"]["radioplayer_bw_button"].get("sizy","0").asInt());
    config_menu.config_radioplayer_play_button_posx=(iRoot["tema1"]["icons"]["radioplayer_play_button"].get("x","0").asInt());
    config_menu.config_radioplayer_play_button_posy=(iRoot["tema1"]["icons"]["radioplayer_play_button"].get("y","0").asInt());
    config_menu.config_radioplayer_play_button_sizx=(iRoot["tema1"]["icons"]["radioplayer_play_button"].get("sizx","0").asInt());
    config_menu.config_radioplayer_play_button_sizy=(iRoot["tema1"]["icons"]["radioplayer_play_button"].get("sizy","0").asInt());
    config_menu.config_radioplayer_stop_button_posx=(iRoot["tema1"]["icons"]["radioplayer_stop_button"].get("x","0").asInt());
    config_menu.config_radioplayer_stop_button_posy=(iRoot["tema1"]["icons"]["radioplayer_stop_button"].get("y","0").asInt());
    config_menu.config_radioplayer_stop_button_sizx=(iRoot["tema1"]["icons"]["radioplayer_stop_button"].get("sizx","0").asInt());
    config_menu.config_radioplayer_stop_button_sizy=(iRoot["tema1"]["icons"]["radioplayer_stop_button"].get("sizy","0").asInt());

    config_menu.config_mediaplayer_info_icon=(iRoot["tema1"]["icons"]["mediaplayer_info"].get("icon_path","0").asString());
    config_menu.config_mediaplayer_coverx=(iRoot["tema1"]["icons"]["mediaplayer_info_cover"].get("x","0").asInt());
    config_menu.config_mediaplayer_covery=(iRoot["tema1"]["icons"]["mediaplayer_info_cover"].get("y","0").asInt());
    config_menu.config_mediaplayer_cover_sizx=(iRoot["tema1"]["icons"]["mediaplayer_info_cover"].get("sizx","0").asInt());
    config_menu.config_mediaplayer_cover_sizy=(iRoot["tema1"]["icons"]["mediaplayer_info_cover"].get("sizy","0").asInt());
    config_menu.config_mediaplayer_ff_button_posx=(iRoot["tema1"]["icons"]["mediaplayer_ff_button"].get("x","0").asInt());
    config_menu.config_mediaplayer_ff_button_posy=(iRoot["tema1"]["icons"]["mediaplayer_ff_button"].get("y","0").asInt());
    config_menu.config_mediaplayer_ff_button_sizx=(iRoot["tema1"]["icons"]["mediaplayer_ff_button"].get("sizx","0").asInt());
    config_menu.config_mediaplayer_ff_button_sizy=(iRoot["tema1"]["icons"]["mediaplayer_ff_button"].get("sizy","0").asInt());
    config_menu.config_mediaplayer_bw_button_posx=(iRoot["tema1"]["icons"]["mediaplayer_bw_button"].get("x","0").asInt());
    config_menu.config_mediaplayer_bw_button_posy=(iRoot["tema1"]["icons"]["mediaplayer_bw_button"].get("y","0").asInt());
    config_menu.config_mediaplayer_bw_button_sizx=(iRoot["tema1"]["icons"]["mediaplayer_bw_button"].get("sizx","0").asInt());
    config_menu.config_mediaplayer_bw_button_sizy=(iRoot["tema1"]["icons"]["mediaplayer_bw_button"].get("sizy","0").asInt());
    config_menu.config_mediaplayer_play_button_posx=(iRoot["tema1"]["icons"]["mediaplayer_play_button"].get("x","0").asInt());
    config_menu.config_mediaplayer_play_button_posy=(iRoot["tema1"]["icons"]["mediaplayer_play_button"].get("y","0").asInt());
    config_menu.config_mediaplayer_play_button_sizx=(iRoot["tema1"]["icons"]["mediaplayer_play_button"].get("sizx","0").asInt());
    config_menu.config_mediaplayer_play_button_sizy=(iRoot["tema1"]["icons"]["mediaplayer_play_button"].get("sizy","0").asInt());
    config_menu.config_mediaplayer_stop_button_posx=(iRoot["tema1"]["icons"]["mediaplayer_stop_button"].get("x","0").asInt());
    config_menu.config_mediaplayer_stop_button_posy=(iRoot["tema1"]["icons"]["mediaplayer_stop_button"].get("y","0").asInt());
    config_menu.config_mediaplayer_stop_button_sizx=(iRoot["tema1"]["icons"]["mediaplayer_stop_button"].get("sizx","0").asInt());
    config_menu.config_mediaplayer_stop_button_sizy=(iRoot["tema1"]["icons"]["mediaplayer_stop_button"].get("sizy","0").asInt());
    config_menu.config_exitx=(iRoot["tema1"]["icons"]["exit"].get("x","0").asInt());
    config_menu.config_exity=(iRoot["tema1"]["icons"]["exit"].get("y","0").asInt());
    config_menu.config_exit_icon=(iRoot["tema1"]["icons"]["exit"].get("icon_path","0").asString());

    // config_menu.config_radio_or_music_oversigtx=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asInt());
    // config_menu.config_radio_or_music_oversigty=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asInt());
    // config_menu.config_radio_or_music_oversigt_icon=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asString());    config_menu.config_music_main_windowx=(iRoot["tema1"]["icons"]["music_main_window"].get("window_x","0").asInt());
    config_menu.config_music_main_windowy=(iRoot["tema1"]["icons"]["music_main_window"].get("window_y","0").asInt());
    config_menu.config_music_main_window_sizex=(iRoot["tema1"]["icons"]["music_main_window"].get("window_sizex","0").asInt());
    config_menu.config_music_main_window_sizey=(iRoot["tema1"]["icons"]["music_main_window"].get("window_sizey","0").asInt());
    config_menu.config_music_main_window_icon_sizex=(iRoot["tema1"]["icons"]["music_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_music_main_window_icon_sizey=(iRoot["tema1"]["icons"]["music_main_window"].get("window_icon_sizey","0").asInt());
    
    config_menu.config_radio_main_windowx=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_x","0").asInt());
    config_menu.config_radio_main_windowy=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_y","0").asInt());
    config_menu.config_radio_main_window_sizex=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_sizex","0").asInt());
    config_menu.config_radio_main_window_sizey=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_sizey","0").asInt());
    config_menu.config_radio_main_window_icon_sizex=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_radio_main_window_icon_sizey=(iRoot["tema1"]["icons"]["radio_main_window"].get("window_icon_sizey","0").asInt());
    
    config_menu.config_stream_main_windowx=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_x","0").asInt());
    config_menu.config_stream_main_windowy=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_y","0").asInt());
    config_menu.config_stream_main_window_sizex=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_sizex","0").asInt());
    config_menu.config_stream_main_window_sizey=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_sizey","0").asInt());
    config_menu.config_stream_main_window_icon_sizex=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_stream_main_window_icon_sizey=(iRoot["tema1"]["icons"]["stream_main_window"].get("window_icon_sizey","0").asInt());

    config_menu.config_movie_main_windowx=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_x","0").asInt());
    config_menu.config_movie_main_windowy=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_y","0").asInt());
    config_menu.config_movie_main_window_sizex=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_sizex","0").asInt());
    config_menu.config_movie_main_window_sizey=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_sizey","0").asInt());
    config_menu.config_movie_main_window_icon_sizex=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_movie_main_window_icon_sizey=(iRoot["tema1"]["icons"]["movie_main_window"].get("window_icon_sizey","0").asInt());

    config_menu.config_spotify_main_windowx=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_x","0").asInt());
    config_menu.config_spotify_main_windowy=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_y","0").asInt());
    config_menu.config_spotify_main_window_sizex=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_sizex","0").asInt());
    config_menu.config_spotify_main_window_sizey=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_sizey","0").asInt());
    config_menu.config_spotify_main_window_icon_sizex=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_spotify_main_window_icon_sizey=(iRoot["tema1"]["icons"]["spotify_main_window"].get("window_icon_sizey","0").asInt());

    config_menu.config_tidal_main_windowx=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_x","0").asInt());
    config_menu.config_tidal_main_windowy=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_y","0").asInt());
    config_menu.config_tidal_main_window_sizex=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_sizex","0").asInt());
    config_menu.config_tidal_main_window_sizey=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_sizey","0").asInt());
    config_menu.config_tidal_main_window_icon_sizex=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_icon_sizex","0").asInt());
    config_menu.config_tidal_main_window_icon_sizey=(iRoot["tema1"]["icons"]["tidal_main_window"].get("window_icon_sizey","0").asInt());

    config_menu.config_tv_main_windowx=(iRoot["tema1"]["icons"]["tv_main_window"].get("window_x","0").asInt());
    config_menu.config_tv_main_windowy=(iRoot["tema1"]["icons"]["tv_main_window"].get("window_y","0").asInt());
    config_menu.config_tv_main_window_sizex=(iRoot["tema1"]["icons"]["tv_main_window"].get("window_sizex","0").asInt());
    config_menu.config_tv_main_window_sizey=(iRoot["tema1"]["icons"]["tv_main_window"].get("window_sizey","0").asInt());

    
  } catch (const std::exception &e) {
    config_menu.config_tema_path="tema2/";

    config_menu.config_tvguidex=1720;
    config_menu.config_tvguidey=10;                    // 888
    config_menu.config_tvguide_icon="tv";

    config_menu.config_musicx=1720;
    config_menu.config_musicy=220;    // 696;
    config_menu.config_music_icon="music_button1";

    config_menu.config_music_activex=1720;
    config_menu.config_music_activey=220;       // 696;
    config_menu.config_music_active_icon="music_button1";

    config_menu.config_mediax=1720;
    config_menu.config_mediay=430;             // 696;
    config_menu.config_media_icon="stream_button";

    config_menu.config_media1x=1720;
    config_menu.config_media1y=430;            // 696;
    config_menu.config_media1_icon="stream_button1";

    config_menu.config_spotifyx=1720;
    config_menu.config_spotifyy=10;                // 888;
    config_menu.config_spotify_icon="spotify_button";

    config_menu.config_spotify1x=1720;
    config_menu.config_spotify1y=220;                // 696;
    config_menu.config_spotify1_icon="spotify_button1";

    config_menu.config_tidalx=1720;
    config_menu.config_tidaly=10+120;                  // 696;
    config_menu.config_tidal_icon="tidal_button";

    config_menu.config_tidal1x=1720;
    config_menu.config_tidal1y=220;                // 696;
    config_menu.config_tidal1_icon="tidal_button1";

    config_menu.config_radiox=1720;
    config_menu.config_radioy=430;    // 504;
    config_menu.config_radio_icon="radio_button";

    config_menu.config_radio1x=1720;
    config_menu.config_radio1y=220;         // 696;
    config_menu.config_radio1_icon="radio_button1";

    config_menu.config_media_buttonx=1720;
    config_menu.config_media_buttony=504;
    config_menu.config_media_button_icon="media_button";


    config_menu.config_moviex=1720;
    config_menu.config_moviey=430;
    config_menu.config_movie_icon="movie_button";

    config_menu.config_movie1x=1720;
    config_menu.config_movie1y=430; // 696;
    config_menu.config_movie1_icon="movie_button1";

    config_menu.config_recordedx=1720;
    config_menu.config_recordedy=640;                     // 640;
    config_menu.config_recorded_icon="recorded";

    config_menu.config_recorded1x=1720;
    config_menu.config_recorded1y=640;
    config_menu.config_recorded1_icon="recorded_selected";

    config_menu.config_closex=1720;
    config_menu.config_closey=860; // 126;
    config_menu.config_close_icon="close";

    config_menu.config_setupx=1720;
    config_menu.config_setupy=860;
    config_menu.config_setup_icon="setupmenu";

    config_menu.config_loadingx=1920;
    config_menu.config_loadingy=200;
    config_menu.config_loading_icon="loading";

    config_menu.config_playinfox=1720;
    config_menu.config_playinfoy=10; // 888
    config_menu.config_playinfo_icon="playinfo";

    config_menu.config_downx=1720;
    config_menu.config_downy=640;      // 312;
    config_menu.config_down_icon="pdown";

    config_menu.config_upx=1720;
    config_menu.config_upy=430;    // 504;
    config_menu.config_up_icon="pup";

    config_menu.config_musicplayer_infox=480;
    config_menu.config_musicplayer_infoy=520;
    config_menu.config_musicplayer_sizx=512;
    config_menu.config_musicplayer_sizy=320;

    config_menu.config_musicplayer_info_icon="musicplayer-info";

    config_menu.config_spotifyplayer_infox=480;
    config_menu.config_spotifyplayer_infoy=520;
    config_menu.config_spotifyplayer_sizx=512;
    config_menu.config_spotifyplayer_sizy=320;

    config_menu.config_spotifyplayer_info_icon="musicplayer-info";

    config_menu.config_tidalplayer_infox=480;
    config_menu.config_tidalplayer_infoy=520;
    config_menu.config_tidalplayer_sizx=512;
    config_menu.config_tidalplayer_sizy=320;

    config_menu.config_tidalplayer_info_icon="musicplayer-info";

    config_menu.config_radioplayer_infox=480;
    config_menu.config_radioplayer_infoy=420;
    config_menu.config_radioplayer_sizx=512;
    config_menu.config_radioplayer_sizy=320;
    config_menu.config_radioplayer_info_icon="musicplayer-info";

    config_menu.config_radioplayer_play_button_posx=40;
    config_menu.config_radioplayer_play_button_posy=630;

    config_menu.config_radioplayer_stop_button_posx=90;
    config_menu.config_radioplayer_stop_button_posy=630;
    config_menu.config_radioplayer_ff_button_posx=140;
    config_menu.config_radioplayer_ff_button_posy=630;
    config_menu.config_radioplayer_bw_button_posx=190;
    config_menu.config_radioplayer_bw_button_posy=630;

    config_menu.config_mediaplayer_infox=480;
    config_menu.config_mediaplayer_infoy=520;
    config_menu.config_mediaplayer_sizx=512;
    config_menu.config_mediaplayer_sizy=320;
    config_menu.config_mediaplayer_info_icon="musicplayer-info";

    config_menu.config_exitx=0;
    config_menu.config_exity=0;
    config_menu.config_exit_icon="exit";

    // config_menu.config_radio_or_music_oversigtx=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asInt());
    // config_menu.config_radio_or_music_oversigty=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asInt());
    // config_menu.config_radio_or_music_oversigt_icon=(iRoot["tema1"]["icons"]["radio_or_music"].get("icon_path","0").asString());

    config_menu.config_music_main_windowx=0;
    config_menu.config_music_main_windowy=0;
    config_menu.config_music_main_window_sizex=1920;
    config_menu.config_music_main_window_sizey=1080;
    config_menu.config_music_main_window_icon_sizex=180;
    config_menu.config_music_main_window_icon_sizey=180;
    
    config_menu.config_radio_main_windowx=0;
    config_menu.config_radio_main_windowy=0;
    config_menu.config_radio_main_window_sizex=1920;
    config_menu.config_radio_main_window_sizey=1080;
    config_menu.config_radio_main_window_icon_sizex=200;
    config_menu.config_radio_main_window_icon_sizey=180;
    
    config_menu.config_stream_main_windowx=0;
    config_menu.config_stream_main_windowy=0;
    config_menu.config_stream_main_window_sizex=1920;
    config_menu.config_stream_main_window_sizey=1080;
    config_menu.config_stream_main_window_icon_sizex=200;
    config_menu.config_stream_main_window_icon_sizey=180;

    config_menu.config_movie_main_windowx=0;
    config_menu.config_movie_main_windowy=0;
    config_menu.config_movie_main_window_sizex=1920;
    config_menu.config_movie_main_window_sizey=1080;
    config_menu.config_movie_main_window_icon_sizex=220;
    config_menu.config_movie_main_window_icon_sizey=240;

    config_menu.config_spotify_main_windowx=0;
    config_menu.config_spotify_main_windowy=0;
    config_menu.config_spotify_main_window_sizex=1920;
    config_menu.config_spotify_main_window_sizey=1080;
    config_menu.config_spotify_main_window_icon_sizex=200;
    config_menu.config_spotify_main_window_icon_sizey=220;

    config_menu.config_tidal_main_windowx=0;
    config_menu.config_tidal_main_windowy=0;
    config_menu.config_tidal_main_window_sizex=1920;
    config_menu.config_tidal_main_window_sizey=1080;
    config_menu.config_tidal_main_window_icon_sizex=180;
    config_menu.config_tidal_main_window_icon_sizey=180;

    config_menu.config_tv_main_windowx=0;
    config_menu.config_tv_main_windowy=0;
    config_menu.config_tv_main_window_sizex=1920;
    config_menu.config_tv_main_window_sizey=1080;
    return 0;
  }
  return(1);
}




// ****************************************************************************************
//
// Load all gfx tema data
//
// ****************************************************************************************


void loadgfx() {
    unsigned int i;
    std::string temapath="";
    printf ("Loading init graphic.\n");    
    if (tema == 1) temapath="/opt/mythtv-controller/tema1/"; else
    if (tema == 2) temapath="/opt/mythtv-controller/tema2/"; else
    if (tema == 3) temapath="/opt/mythtv-controller/tema3/"; else
    if (tema == 4) temapath="/opt/mythtv-controller/tema4/"; else
    if (tema == 5) temapath="/opt/mythtv-controller/tema5/"; else
    if (tema == 6) temapath="/opt/mythtv-controller/tema6/"; else
    if (tema == 7) temapath="/opt/mythtv-controller/tema7/"; else
    if (tema == 8) temapath="/opt/mythtv-controller/tema8/"; else
    if (tema == 9) temapath="/opt/mythtv-controller/tema9/"; else
    if (tema == 10) temapath="/opt/mythtv-controller/tema10/"; else {
      // default tema
      // strcpy(temapath,"tema1/");
      temapath="tema1/";
      tema=1;
    }
    _textureIdpup           = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_up_icon.c_str());
    _textureIdpdown         = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_down_icon.c_str());
    radiobutton             = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_radio_icon.c_str());
    spotifybutton           = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_spotify_icon.c_str());
    spotifybutton1          = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_spotify1_icon.c_str());
    tidalbutton             = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_tidal_icon.c_str());
    moviebutton             = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_movie_icon.c_str());
    musicbutton             = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_music_icon.c_str());
    streambutton            = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_media_icon.c_str());
    _textureIdtv         	  = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_tvguide_icon.c_str());
    tidalbutton1            = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_tidal1_icon.c_str());
    _textureIdrecorded      = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_recorded_icon.c_str());
    _textureIdrecorded_aktiv=loadgfxfile((char *) temapath.c_str(),(char *) "buttons/",(char *) "recorded_selected");
    _textureIdplayinfo  	  = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_playinfo_icon.c_str());
    _textureIdfilm_aktiv    = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_movie1_icon.c_str());
    moviebutton_2           = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_media_button_icon.c_str() );
    _texturesetupmenu   	  = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_setup_icon.c_str());
    _texturesetupmenu_select= loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_setup_selected_icon.c_str());
    streambutton_2          = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) "stream_button1"); // "stream_button");
    radiobutton1            = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_radio1_icon.c_str());
    _textureIdmusic         = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_music_icon.c_str());
    _textureIdmusic_aktiv   = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_music_active_icon.c_str());
    _textureIdloading       = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "images/",(char *) config_menu.config_loading_icon.c_str()); //
    _textureId_dir          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "dir1");
    _textureIdback          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "back-icon");
    onlineradio320          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "onlineradio320");
    _textureclosemain      	= loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "images/",(char *) config_menu.config_closemain_icon.c_str()); // "closemain");
    _textureId_song         = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "song1");
    onlineradio_empty       = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "onlineradio_empty");
    playing_tidal_icon_texture = loadTexture((char *) config_playing_record_icon.c_str());
    _texturetidalplayer     = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "images/",(char *) config_menu.config_tidalplayer_info_icon.c_str());  //
    _textureradioplayer     = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "images/",(char *) config_menu.config_tidalplayer_info_icon.c_str());  //
    _textureId28          	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "dirplaylist");
    big_search_bar          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar");
    music_big_search_bar_artist = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_artist");
    music_big_search_bar_album = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_album");
    music_big_search_bar_track = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_track");
    
    big_search_bar_playlist = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar_playlist");
    big_search_bar_track    = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar_song");
    big_search_bar_albumm   = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar_album");
    big_search_bar_artist   = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar_artist");
    big_search_bar          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "big_search_bar");
    tidal_big_search_bar_artist= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_artist");
    tidal_big_search_bar_album = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_album");
    tidal_big_search_bar_track = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidal_big_search_bar_track");
    volume_window              = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "volume_win");
    _texturemplay          	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mplay");
    _texturemstop       	  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mplaystop");
    _texturemlast         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mplaylast");
    _texturemlast2        	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mplaylast");
    _texturemnext         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mplaynext");
    _texturempause          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mpause");


    _texturesoundsetup		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupsound");
    _texturesourcesetup		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupsource");
    _textureimagesetup		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupimg");
    _texturetemasetup 		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptema");
    _texturemythtvsql 		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupsql");
    _texturesetupfont 		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupfont");
    _texturekeyssetup 		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupkeys");
    _texturekeysrss		    = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuprss");
    _texturespotify       = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupspotify");
    _texturetidal         = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptidal");
    _texturevideoplayersetup	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupplayer");
    _texturetvgrabersetup = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupxmltv");
    _texturesetupclose		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupclose");
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
    _texturelock        	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "lock");
    setuptexture         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setup");
    texturedot            = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "dot");
    _textureIdback_music  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "background_music");
    _textureIdback_setup  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tuxsetup");
    _defaultdvdcover      = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "dvdcover");
    _texturemovieinfobox  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "movie-infobox");   		// small screen 4/3
    _textureupdatetidalview= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "updatetidalview");
    _defaultdvdcover_mask = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "dvdcover_mask");
    _textureloadfile      = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "load_file");
    torrent_background   	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "torrent_background");
    setuptorrent_background = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptorrent_background");
    _textureclose        	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "close");
    _textureselect      	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "select");
    _texturetidalwait     = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "wait");
    mobileplayer_icon     = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "mobileplayer");
    pcplayer_icon         = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "pcplayer");
    unknownplayer_icon    = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "unknownplayer");
    newstuf_icon          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "new_stuf");
    spotify_icon_border   = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "spotify_icon_border.png");
    _textureexit          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "exit");
    spotify_askplay       = loadgfxfile((char *) temapath.c_str(),(char *) "buttons/",(char *) "spotify_askplay");
    spotify_askopen       = loadgfxfile((char *) temapath.c_str(),(char *) "buttons/",(char *) "spotify_askopen");
    _texturetidalloading    = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tidalplayer-loading");
    spotify_search          = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_search_icon.c_str());     // "search");
    spotify_search_back     = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "buttons/",(char *) config_menu.config_search_back_icon.c_str());     // "search_back");

    setupsoundback      	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setup-soundback");
    setupsqlback        	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupsqlback");
    setuptorrent_background = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptorrent_background");
    setuptemaback       	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptemaback");
    setupnetworkback    	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupnetworkback");
    setupnetworkwlanback  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupnetworkwlanback");
    setupscreenback     	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupscreenback");
    setupfontback       	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupfontback");
    setupkeysback       	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupkeysback");
    setuprssback         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuprssback");
    torrent_background   	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "torrent_background");
    setuptidalback       	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptidalscreen");
    // recorded stuf
    _textureId22         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "recordedbox1");
    _textureId23         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "recordedbox2");
    _textureId24         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "recordedbox3");
    screensaverbox        = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "3d_brix");
    // main background
    _textureIdback_main   = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "background_main");
    // music oversigt background
    _textureIdback_music  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "background_music");
    // setup background
    _textureIdback_setup  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tuxsetup");
    // other back
    _textureIdback_other  = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "background_other");


    _textureId_cursor     = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "cursor");

    analog_clock_background = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "clock_background");
    _texturemusicplayer   = loadgfxfile((char *) config_menu.config_tema_path.c_str(),(char *) "images/",(char *) config_menu.config_musicplayer_info_icon.c_str());

    _textureId9_askbox   	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "askbox");
    _textureIdplayicon   	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "play");
    _textureopen         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "open");
    _textureclose        	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "close");
    _textureselect      	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "select");
    _texturetidalwait    	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "wait");
    _textureswap         	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "swap");

    // tv stuf
    _tvbar1_1             = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvbar1_1");
    _tvbar3               = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvbar3");
    _tvoverskrift         = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvbar_top");
    _textureutvbgmask     = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tv_carbon");
    _tvrecordbutton   		= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvrecord");

    setuptvgraberback    	= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setuptvgraberback");
    setupbackend          = loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "setupbackend");
    tvguide_last_hour_icon= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvguide_last_hour_icon");
    tvguide_next_hour_icon= loadgfxfile((char *) temapath.c_str(),(char *) "images/",(char *) "tvguide_next_hour_icon");
}


void freegfx() {
    glDeleteTextures(1, &_textureIdpup);
    glDeleteTextures(1, &_textureIdpdown);
    glDeleteTextures(1, &radiobutton);
    glDeleteTextures(1, &spotifybutton);
    glDeleteTextures(1, &tidalbutton);
    glDeleteTextures(1, &moviebutton);
    glDeleteTextures(1, &streambutton);
    glDeleteTextures(1, &spotifybutton1);
    glDeleteTextures(1, &tidalbutton1);
    glDeleteTextures(1, &musicbutton);
    glDeleteTextures(1, &_textureIdrecorded);
    glDeleteTextures(1, &_textureIdrecorded_aktiv);
    glDeleteTextures(1, &_textureIdplayinfo);
    glDeleteTextures(1, &_textureIdtv);
    glDeleteTextures(1, &_textureIdfilm_aktiv);
    glDeleteTextures(1, &moviebutton_2);
    glDeleteTextures(1, &_texturesetupmenu);
    glDeleteTextures(1, &streambutton_2);
    glDeleteTextures(1, &radiobutton1);
    glDeleteTextures(1, &_textureIdmusic);
    glDeleteTextures(1, &_textureIdmusic_aktiv);
    glDeleteTextures(1, &_textureIdloading);
    glDeleteTextures(1, &_textureId_dir);				      // cd/dir icon in music oversigt (hvis ingen cd cover findes)
    glDeleteTextures(1, &_textureIdback);
    glDeleteTextures(1, &onlineradio320);
    glDeleteTextures(1, &_textureclosemain);
    glDeleteTextures(1, &_textureId_song);
    glDeleteTextures(1, &onlineradio_empty);
    glDeleteTextures(1, &playing_tidal_icon_texture);
    glDeleteTextures(1, &_texturetidalplayer);
    glDeleteTextures(1, &_textureradioplayer);
    glDeleteTextures(1, &_textureId28);
    glDeleteTextures(1, &music_big_search_bar_artist);       // big search bar used by tidal search
    glDeleteTextures(1, &music_big_search_bar_track);         // big search bar used by tidal search
    glDeleteTextures(1, &music_big_search_bar_album);

    glDeleteTextures(1, &big_search_bar_playlist);
    glDeleteTextures(1, &big_search_bar_track);
    glDeleteTextures(1, &big_search_bar_albumm);
    glDeleteTextures(1, &big_search_bar_artist);
    glDeleteTextures(1, &big_search_bar);
    glDeleteTextures(1, &tidal_big_search_bar_artist);
    glDeleteTextures(1, &tidal_big_search_bar_album);
    glDeleteTextures(1, &tidal_big_search_bar_track);
    glDeleteTextures(1, &volume_window);
    glDeleteTextures(1, &_texturemplay);
    glDeleteTextures(1, &_texturemstop);
    glDeleteTextures(1, &_texturemlast);
    glDeleteTextures(1, &_texturemlast2);
    glDeleteTextures(1, &_texturemnext);
    glDeleteTextures(1, &_texturempause);
    
    // setup texture
    glDeleteTextures(1, &_texturesoundsetup);
    glDeleteTextures(1, &_texturesourcesetup);
    glDeleteTextures(1, &_textureimagesetup);
    glDeleteTextures(1, &_texturetemasetup);
    glDeleteTextures(1, &_texturemythtvsql);
    glDeleteTextures(1, &_texturesetupfont);
    glDeleteTextures(1, &_texturekeyssetup);
    glDeleteTextures(1, &_texturekeysrss);
    glDeleteTextures(1, &_texturespotify);
    glDeleteTextures(1, &_texturetidal);
    glDeleteTextures(1, &_texturevideoplayersetup);
    glDeleteTextures(1, &_texturetvgrabersetup);
    glDeleteTextures(1, &_texturesetupclose);
    // ************************* screen shot *******************************
    glDeleteTextures(1, &screenshot1);
    glDeleteTextures(1, &screenshot2);
    glDeleteTextures(1, &screenshot3);
    glDeleteTextures(1, &screenshot4);
    glDeleteTextures(1, &screenshot5);
    glDeleteTextures(1, &screenshot6);
    glDeleteTextures(1, &screenshot7);
    glDeleteTextures(1, &screenshot8);
    glDeleteTextures(1, &screenshot9);
    glDeleteTextures(1, &screenshot10);
    glDeleteTextures(1, &_texturelock);
    glDeleteTextures(1, &setuptexture);
    glDeleteTextures(1, &texturedot);
    glDeleteTextures(1, &_textureIdback_music);
    glDeleteTextures(1, &_defaultdvdcover);
    glDeleteTextures(1, &_texturemovieinfobox);
    glDeleteTextures(1, &_textureupdatetidalview);
    glDeleteTextures(1, &_defaultdvdcover_mask);
    glDeleteTextures(1, &_textureloadfile);
    glDeleteTextures(1, &torrent_background);
    glDeleteTextures(1, &setuptorrent_background);
    glDeleteTextures(1, &_textureclose);
    glDeleteTextures(1, &_textureselect);
    glDeleteTextures(1, &_texturetidalwait);
    glDeleteTextures(1, &pcplayer_icon);
    glDeleteTextures(1, &mobileplayer_icon);
    glDeleteTextures(1, &newstuf_icon);
    glDeleteTextures(1, &unknownplayer_icon);
    glDeleteTextures(1, &spotify_icon_border);
    glDeleteTextures(1, &_textureexit);
    glDeleteTextures(1, &spotify_askplay);
    glDeleteTextures(1, &spotify_askopen);
    glDeleteTextures(1, &_texturetidalloading);
    glDeleteTextures(1, &spotify_search);
    glDeleteTextures(1, &spotify_search_back);

    // setup stuf
    glDeleteTextures(1, &setupsoundback);
    glDeleteTextures(1, &setupsqlback);
    glDeleteTextures(1, &setuptorrent_background);
    glDeleteTextures(1, &setuptemaback);
    glDeleteTextures(1, &setupnetworkback);
    glDeleteTextures(1, &setupnetworkwlanback);
    glDeleteTextures(1, &setupscreenback);
    glDeleteTextures(1, &setupfontback);
    glDeleteTextures(1, &setupkeysback);
    glDeleteTextures(1, &setuprssback);
    glDeleteTextures(1, &torrent_background);
    glDeleteTextures(1, &setuptidalback);

    glDeleteTextures(1, &_textureId22);
    glDeleteTextures(1, &_textureId23);
    glDeleteTextures(1, &_textureId24);
    glDeleteTextures(1, &screensaverbox);
    glDeleteTextures(1, &_textureIdback_main);
    // music oversigt background
    glDeleteTextures(1, &_textureIdback_music);
    // setup background
    glDeleteTextures(1, &_textureIdback_setup);
    // other back
    glDeleteTextures(1, &_textureIdback_other);
    glDeleteTextures(1, &_textureId_cursor);
    glDeleteTextures(1, &analog_clock_background);

    glDeleteTextures(1, &_texturemusicplayer);
    glDeleteTextures(1, &_textureId9_askbox);

    glDeleteTextures(1, &_textureIdplayicon);
    glDeleteTextures(1, &_textureopen);
    glDeleteTextures(1, &_textureswap);
    glDeleteTextures(1, &_tvbar1_1);
    glDeleteTextures(1, &_tvbar3);
    glDeleteTextures(1, &_tvoverskrift);
    glDeleteTextures(1, &_textureutvbgmask);
    glDeleteTextures(1, &_tvrecordbutton);
    glDeleteTextures(1, &setuptvgraberback);
    glDeleteTextures(1, &setupbackend);
    glDeleteTextures(1, &tvguide_last_hour_icon);
    glDeleteTextures(1, &tvguide_next_hour_icon);
  }



// ********************************************************************************
//
// in use
// search loader
// move torrent files
//
//
// ********************************************************************************

void datainfoloader_webserver_v2() {
  size_t size;
  std::string sourcefile="";
  std::string destfile="";
  int status;
  struct tm* t;
  static time_t lasttime=0;
  static time_t nowdate;
  int recc;
  time(&lasttime);
  time(&nowdate);
  // run the webserver
  bool run_web_server=true;
  while((run_web_server) && (runwebserver)) {
    /*
    mg_mgr_poll(&spotify_oversigt.mgr, 50);
    // run time server to update spotify token
    if (difftime(nowdate, lasttime)>500) {                                     // 3500
      time(&lasttime);
      write_logfile(logfile,(char *) "Update spotify token");
      if ((spotify_oversigt.spotify_get_token(),"")!=0) {
        //spotify_oversigt.spotify_refresh_token();       // old ver
        //spotify_oversigt.spotify_refresh_token2();        // new ver
      }
    }
    */
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
      spotify_oversigt.reset_amin_in_viewer();                              // reset anim
      //spotify_oversigt.type=2;
    }

    #ifdef ENABLE_TIDAL
    // get search result after search text is done (by enter key).
    if (do_hent_tidal_search_online) {
      printf("do_hent_tidal_search_online=%d\n",do_hent_tidal_search_online);
      tidal_oversigt.search_tidal_online_done=false;
      fprintf(stderr,"Update tidal search result thread.\n");
      write_logfile(logfile,(char *) "Tidal start search result thread");
      do_hent_tidal_search_online=false;
      tidal_oversigt_loaded_begin=true;
      // clear old
      tidal_oversigt.search_tidal_online_done=false;
      tidal_oversigt.clean_tidal_search_oversigt();
      // update from search
      // tidal_oversigt.searchtype=1;
      tidal_oversigt.searchtype=1;
      switch(tidal_oversigt.searchtype) {
        case 0: tidal_oversigt.opdatere_tidal_oversigt_searchtxt_online(keybuffer,0);               // ALBUMS
                break;
        case 1: tidal_oversigt.opdatere_tidal_oversigt_searchtxt_online(keybuffer,1);               // ARTISTS
                break;
        case 2: tidal_oversigt.opdatere_tidal_oversigt_searchtxt_online(keybuffer,2);               // TRACKS
                break;
        default:tidal_oversigt.opdatere_tidal_oversigt_searchtxt_online(keybuffer,0);               // ALBUMS
      }
      // printf("Done Update tidal search result thread.\n");
      write_logfile(logfile,(char *) "Tidal done search result thread");
      tidal_oversigt.search_tidal_online_done=true;
      tidal_oversigt_loaded_begin=false;
      tidal_oversigt.set_search_loaded();                                 // load icons
      // tidal_oversigt.reset_amin_in_viewer();                              // reset anim
      tidal_oversigt.search_loaded=true;
    }
    #endif
    // torrent file move to movie dir from tmp dir
    if ((do_move_torrent_file) && (do_show_torrent_options_move)) {
      do_move_torrent_file = false;           // only to it 1 time
      do_show_torrent_options_move = false;
      do_show_torrent_options = false;
      do_move_torrent_file_now = true;
      do_move_torrent_file_now_done = 0.0f;
      write_logfile(logfile,(char *) "TORRENT: Copy file start : ");
      write_logfile(logfile,(char *) torrent_downloader.get_name(torrent_downloader.get_edit_line()));

      printf("valgt %d file name %s \n ", torrent_downloader.get_edit_line_move_info(), torrent_downloader.get_name(torrent_downloader.get_edit_line()));

      // move torrent file to other path
      if (torrent_downloader.get_edit_line_move_info()==0) {
        sourcefile = "/tmp/";
        sourcefile = sourcefile + torrent_downloader.get_name(torrent_downloader.get_edit_line());
        // destfile = "/data2/Movie/";
        do_move_torrent_file_now = false;
        torrent_downloader.select_file_name_and_copy_to_otherdir((char *) sourcefile.c_str());
      }
      // move torrent file to music path
      if (torrent_downloader.get_edit_line_move_info()==1) {
        sourcefile = "/tmp/";
        sourcefile = sourcefile + torrent_downloader.get_name(torrent_downloader.get_edit_line());
        // destfile = "/data2/Music/";
        // destfile = configmusicpath;
        destfile = configdefaultmusicpath;
        if (destfile.empty() || destfile.back() != '/') destfile += '/';
        destfile = destfile + torrent_downloader.get_name(torrent_downloader.get_edit_line());
        if (torrent_downloader.copy_disk_entry(sourcefile,destfile)) {
          do_move_torrent_file_now = false;
          torrent_downloader.pause_torrent(torrent_downloader.get_edit_line());
        } else {
          do_move_torrent_file_now = false;
        }
      }
      // move torrent file to movie path      
      if (torrent_downloader.get_edit_line_move_info()==2) {
        sourcefile = "/tmp/";
        sourcefile = sourcefile + torrent_downloader.get_name(torrent_downloader.get_edit_line());
        // destfile = "/data2/Movie/";
        destfile = configmoviepath;
        if (destfile.empty() || destfile.back() != '/') destfile += '/';
        destfile = destfile + torrent_downloader.get_name(torrent_downloader.get_edit_line());
        if (torrent_downloader.copy_disk_entry(sourcefile,destfile)) {
          do_move_torrent_file_now = false;
          torrent_downloader.pause_torrent(torrent_downloader.get_edit_line());
        } else {
          do_move_torrent_file_now = false;
        }
      }
    }
    // automove torrent file to movie path
    if ((torrent_downloader.automove_to_movie_path) && (torrent_downloader.get_torrent_download_status() > 0 )) {
      // move torrent file to movie path
      recc=torrent_downloader.get_torrent_download_status();     // get downloaded record (is_finished flag)
      if ((recc-1>=0) && (torrent_downloader.get_automove_done(recc-1)==false)) {
        write_logfile(logfile,(char *) "TORRENT: Move file started.");
        sourcefile = "/tmp/";
        sourcefile = sourcefile + torrent_downloader.get_name(recc-1);
        // destfile = "/data2/Movie/";
        destfile = configmoviepath;
        if (destfile.empty() || destfile.back() != '/') destfile += '/';
        destfile = destfile + torrent_downloader.get_name(recc-1);
        do_move_torrent_file_now = true;
        do_move_torrent_file_now_done = 0.0f;
        if (torrent_downloader.copy_disk_entry(sourcefile,destfile)==1) {
          torrent_downloader.set_automove_done(recc-1); // set automove done
          // auto delete torrent file after move
          if (torrent_downloader.trash_torrent) {
            torrent_downloader.delete_torrent(recc-1); // delete torrent file from view after auto move ok.
            write_logfile(logfile,(char *) "TORRENT: Delete torrent file after move.");
            std::remove(sourcefile.c_str()); // remove file from disk.
          }
          write_logfile(logfile,(char *) "TORRENT: Move file done.");
        } else write_logfile(logfile,(char *) "TORRENT: Error move file.");
        do_move_torrent_file_now = false;           // stop show move
      }
    }

    if (tidal_oversigt.do_update_tidal_start_entry) {
      tidal_oversigt.do_update_tidal_start_entry = false;
      // do_setup_tidal_start_entry=false;
      tidal_oversigt.get_artist_from_file((char *) "",true,true);
    }
  }
}





  // ****************************************************************************************
//
//  in use thread running the torrent update function
//
// ****************************************************************************************

void opdate_threadfunction() {
  while (true) {
    torrent_downloader.opdate_torrent(); // Update torrent function
    if (do_update_music_icons_now_in_thread) {      
      musicoversigt.opdatere_music_oversigt_icons();
      printf("Now Opdate music icons \n");
      do_update_music_icons_now_in_thread=false;  // disale again
    }
  }
}



int main(int argc,char** argv) {
    int dircreatestatus;
    const char *build_str = __DATE__;
    char logfilepath[4096];
    strcpy(playlistfilename,"playlist");
    strcpy(movie_search_name,"");                                               // used then search for movies in movie view
    strcpy(localuserhomedir,"");
    getuserhomedir(localuserhomedir);
    printf("Build date  : %s\n", __DATE__);
    strcpy(logfilepath,localuserhomedir);
    strcat(logfilepath,"/mythtv-controller.log");
    logfile=fopen(logfilepath,"a");
    if (logfile==NULL) {
      logfile=fopen(logfilepath,"w");
      fputs("                 __  .__     __                                       __                .__  .__                    \n",logfile);
      fputs("  _____ ___.__._/  |_|  |___/  |____  __           ____  ____   _____/  |________  ____ |  | |  |   ___________     \n",logfile);
      fputs(" /     <   |  |\\   __\\  |  \\   __\\  \\/ /  ______ _/ ___\\/  _ \\ /    \\   __\\_  __ \\/  _ \\|  | |  | _/ __ \\_  __\\    \n",logfile);
      fputs("|  Y Y  \\___  | |  | |   Y  \\  |  \\   /  /_____/ \\  \\__(  <_> )   |  \\  |  |  | \\(  <_> )  |_|  |_\\  ___/|  | \\/   \n",logfile);
      fputs("|__|_|  / ____| |__| |___|  /__|   \\_/            \\___  >____/|___|  /__|  |__|   \\____/|____/____/\\___  >__|           \n",logfile);
      fputs("      \\/\\/                \\/                          \\/           \\/                                  \\/          \n",logfile);
      fputs("Ver 0.50.x \n",logfile);
    }
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
        printf("-t For Tidal mode\n");
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
    write_logfile(logfile,(char *) debuglogdata);
    // Load config
    load_config((char *) "/etc/mythtv-controller.conf");				// load setup config

    sock=initlirc();

    // create dir for json files and icon files downloaded
    if (!(file_exists("~/spotify_json"))) {
      dircreatestatus = mkdir("~/spotify_json", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    // create dir
    if (!(file_exists("~/spotify_gfx"))) {
      dircreatestatus = mkdir("~/spotify_gfx", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    // temp need loaded from config
    strcpy(configmysqlhost,"localhost");
    strcpy(configmysqluser,"mythtv");
    strcpy(configmysqlpass,"bimmer");
    strcpy(configbackend,"mythtv");
    init_sound_system(0);
    tidalok=tidal_oversigt.get_access_token((char *) "TnE1V1FtVmh2Mkw3UVdRTzp2eE9tRnAzOXJ3ZUlWRDJyYjIwcW1wRVRzb0FFQ3doR1VkblBJUFNY.cTRnPQ==.");
    if (tidalok) {
      printf("Token is ok.\n");
      if (checkartistdbexist()==false) {
        // File tidal_start_artistlists.txt
        tidal_oversigt.get_artist_from_file((char *) "",true,true);
      }      
      tidal_oversigt.get_artist_from_file_and_update_for_editor((char *) "");
      tidal_oversigt.opdatere_tidal_oversigt(0);
    } else {
      printf("Token is missing i code.\n");
      write_logfile(logfile,(char *) "Tidal no data downloaded.");
      exit(0);
    }

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1920,1080);
    glutCreateWindow("Mythtv-Controller");
    //
    // Start GLEW efter OpenGL context
    //
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        printf("GLEW fejl: %s\n",glewGetErrorString(err));
        return -1;
    }

    do_update_rss_show=true;
    do_update_rss=true;
    printf("OpenGL: %s\n",glGetString(GL_VERSION));
    printf("Renderer: %s\n",glGetString(GL_RENDERER));
    printf("Build: %s\n",build_str);
    init_ttf_fonts();
    team_settings_load();
    loadgfx();
    // load fonts
    if (myfont.Load("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf",16)==false) {
      myfont.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",12);
    }
    if (myfont2.Load("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf",32)==false) {
      myfont2.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",32);
    }
    if (myfont_torrent_list.Load("/usr/share/fonts/truetype/Anonymous/Anonymous.ttf",18)==false) {
      myfont_torrent_list.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",18);
    }
    if (myfont_torrent_overskrift.Load("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf",32)==false) {
      myfont_torrent_overskrift.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",32);
    }
    if (myfont_mini.Load("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf",11)==false) {
      myfont_mini.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",11);
    }
    if (myfont_tv_guide_overskrift.Load("/usr/share/fonts/truetype/dejavu/DejaVuMathTeXGyre.ttf",46)==false) {
      myfont_tv_guide_overskrift.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",46);
    }
    // tidal/music/spotify/radio search font
    if (myfont_search_bar.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",46)==false) {
      myfont_search_bar.Load("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",46);
    }


    if (create_radio_oversigt()) {
      int json_load_status=radiooversigt.load_radio_stations_from_json_file();    
      radiooversigt_antal = radiooversigt.opdatere_radio_oversigt();					// get numbers of radio stations
    }

      // radiooversigt.load_radio_stations_gfx();
    recorded_oversigt.opdatere_recorded_oversigt();
    film_oversigt.opdatere_film_oversigt(false);
    datainfoloader_music_v2();      // load music info
    // get tvguide from dr proguide
    aktiv_tv_oversigt.get_dr_proguide(0); // 0 = idag 1 = imorgen osv.
    aktiv_tv_oversigt.get_dr_proguide(1);
    aktiv_tv_oversigt.get_dr_proguide(2);
    // get local time
    
    aktiv_tv_oversigt.opdatere_tv_oversigt((char*) "localhost",(char*) "mythtv",(char*) "bimmer");

    thread t1(opdate_threadfunction); // start thread to update torrent
    // thread for tidal update from the tidal api
    thread t2(datainfoloader_webserver_v2);

    if (full_screen) {
      glutFullScreen();                                // set full screen mode
      write_logfile(logfile,(char *) "Enter full screen mode.");
    }
    // init render
    renderer.Init(1920,1080);

    film_oversigt.vlc_initOpenGL();

    glutDisplayFunc(display);                           // main loop func
    glutIdleFunc(idle);
    glutMouseFunc(handleMouse);                         // setup mousehandler
    glutMotionFunc(mouseMotion);                        // mouse
    glutKeyboardFunc(handleKeypress);                 // setup normal key handler
    glutSpecialFunc(handlespeckeypress);              // setup spacial key handler
    glutTimerFunc(25, update2, 0);                      // set start loop

    // start main loop now
    glutMainLoop();
    // after close down.
    #if defined USE_FMOD_MIXER
    result=sound->release();                                    // release sound
    ERRCHECK(result,0);
    result=sndsystem->close();                                  // release 
    ERRCHECK(result,0);
    result=sndsystem->release();                                // release sound system
    ERRCHECK(result,0);
    #endif
    freegfx();                                                  // free gfx
    return 0;
}

