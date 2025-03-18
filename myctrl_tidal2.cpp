//
// All tidal functions
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <pthread.h>                    // multi thread support
#include <libxml/parser.h>              // xml parser
#include <sys/stat.h>
#include <time.h>
#include <curl/curl.h>                  // lib curl
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string>
#include <algorithm>
#include <string.h>
#include <dirent.h>
#include <filesystem>                   // File system access.
#include <fstream>
#include <iostream>
#include <vector>
#include <fmt/format.h>


// sound system include fmod

#include "/opt/mythtv-controller/fmodstudioapi20218linux/api/core/inc/fmod.hpp"
#include "/opt/mythtv-controller/fmodstudioapi20218linux/api/core/inc/fmod_errors.h"




// json parser
#include "json-parser/json.h"
// global def
#include "myth_setup.h"
// web server stuf used to get token
#include "mongoose-master/mongoose.h"
#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
// jpg/png file reader
#include "readjpg.h"
#include "loadpng.h"
// web file loader
#include "myctrl_readwebfile.h"
#include "myctrl_tidal2.h"
#include "myctrl_glprint.h"


const char *tidal_gfx_path = "tidal_gfx/";

const int tidal_pathlength=80;
const int tidal_namelength=80;
const int tidal_desclength=2000;
const int feed_url=2000;


//bool tidal_debug_json=false;

extern FILE *logfile;
extern char localuserhomedir[4096];                                                         // get in main
extern int debugmode;
extern tidal_class *tidal_oversigt;
extern char *dbname;                                           // internal database name in mysql (music,movie,radio)
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];

extern int orgwinsizey,orgwinsizex;
extern float configdefaultstreamfontsize;

extern GLuint spotify_icon_border;                 // spotify border icon for spotify icon
extern GLuint _texturemovieinfobox;
extern GLuint mobileplayer_icon;                   // mobile player icon
extern GLuint pcplayer_icon;                       // pc player icon
extern GLuint unknownplayer_icon;
extern int tema;
extern GLuint normal_icon;
extern GLuint _textureIdback;
extern GLuint newstuf_icon;
extern GLuint empty_icon;

extern GLuint big_search_bar_playlist;                    // big search bar used by sporify search
extern GLuint big_search_bar_track;                    // big search bar used by sporify search
extern GLuint big_search_bar_albumm;                    // big search bar used by sporify search
extern GLuint big_search_bar_artist;                    // big search bar used by sporify search
extern GLuint tidal_big_search_bar_artist;
extern GLuint tidal_big_search_bar_album;
extern GLuint tidal_big_search_bar_track;

extern float configsoundvolume;                           // default sound volume


extern FMOD::System    *sndsystem;
extern FMOD::Sound     *sound;
extern FMOD::Channel   *channel;
extern int fmodbuffersize;

// used by json passer

bool tidal_process_track=false;
bool tidal_process_playlist=false;
bool tidal_process_songs=false;
bool tidal_process_href=false;
bool tidal_process_description=false;
bool tidal_process_image=false;
bool tidal_process_name=false;
bool tidal_process_items=false;
bool tidal_process_track_nr=false;
bool tidal_process_release_date=false;
bool tidal_process_url=false;
bool tidal_process_id=false;
bool tidal_process_title=false;
bool tidal_process_duration=false;

// used by playlist json passer

bool tidal_process_resource=false;
bool tidal_process_attributes=false;

bool tidal_process_imagecover=false;
bool tidal_process_releasedate=false;
bool tidal_process_artist=false;
bool tidal_process_numberOfTracks=false;


/*
define('TIDAL_RESOURCES_URL','https://resources.tidal.com/images/');
define('TIDAL_ALBUM_URL','https://listen.tidal.com/album/');
define('TIDAL_ALBUM_URL_2','https://tidal.com/browse/album/');
define('TIDAL_ARTIST_URL','https://listen.tidal.com/artist/');
define('TIDAL_TRACK_URL','https://tidal.com/browse/track/');
define('TIDAL_TRACK_STREAM_URL','audio.tidal.com');
define('TIDAL_APP_ALBUM_URL','https://tidal.com/album/');
define('TIDAL_APP_TRACK_URL','https://tidal.com/track/');
define('TIDAL_MAX_CACHE_TIME', 21600); //6h in [s]
define('TIDAL_TOKEN_VERIFY_URL', 'api.tidal.com');
define('MPD_TIDAL_URL','tidal://track/');
*/


// #define TIDAL_ALBUM_URL 'https://openapi.tidal.com/v2/albums/';

// web port
static const char *s_http_port = "8100";
static struct mg_serve_http_opts s_http_server_opts;

size_t tidal_curl_writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}


// ****************************************************************************************
// file writer
// ****************************************************************************************

static size_t tidal_file_write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

bool process_token_token1;
bool process_token_token2;

void tidal_class::process_object_token(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    //print_depth_shift(depth);
    //printf("x=%d depth=%d object[%d].name = %s  \n",x,depth, x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"token1")==0) {
      process_token_token1=true;
    }
    if (strcmp(value->u.object.values[x].name , "token2" )==0) {
      process_token_token2=true;
    }
    process_value_token(value->u.object.values[x].value, depth+1,x);
  }
}

// *****************************************************************************************
//
//
//
// ****************************************************************************************

void tidal_class::process_array_token(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  //printf("array found\n");
  for (x = 0; x < length; x++) {
    process_value_token(value->u.array.values[x], depth,x);
  }
}


// ****************************************************************************************
// xml parse tokendata
//
// IN USE
//
// ****************************************************************************************

void tidal_class::process_value_token(json_value* value, int depth,int x) {
  int j;
  if (value == NULL) return;
  if (value->type != json_object) {
    //print_depth_shift(depth);
  }
  switch (value->type) {
    case json_none:
      //if (tidal_debug_json) fprintf(stdout,"none\n");
      break;
    case json_object:
      process_object_token(value, depth+1);
      break;
    case json_array:
      process_array_token(value, depth+1);
      break;
    case json_integer:
      printf("int: %10" PRId64 "\n", value->u.integer);
      break;
    case json_double:
      fprintf(stdout,"double: %f\n", value->u.dbl);
      break;
    case json_string:
      // printf("Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
      if (process_token_token1) {
        process_token_token1=false;
      }
      if (process_token_token2) {
        process_token_token2=false;
      }
      break;
    case json_boolean:
      //if (tidal_debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
      break;
  }
}



static int s_exit_flag = 0;


// ****************************************************************************************
//
// Constructor tidal devices
//
// ****************************************************************************************

tidal_device_def::tidal_device_def() {
  strcpy(id,"");
  is_active=false;
  is_private_session=false;
  is_restricted=false;
  strcpy(name,"");
  strcpy(devtype,"");
  devvolume=0;
}

// ****************************************************************************************
//
// Constructor tidal active player
//
// ****************************************************************************************

tidal_active_play_info_type::tidal_active_play_info_type() {
  progress_ms=0;
  duration_ms=0;
  strcpy(song_name,"");
  strcpy(artist_name,"");
  strcpy(cover_image_url,"");
  cover_image=0;
  strcpy(album_name,"");
  strcpy(release_date,"");
  popularity=0;
  is_playing=false;
};


// ****************************************************************************************
//
// Constructor tidal oversigt type
//
// ****************************************************************************************

tidal_oversigt_type::tidal_oversigt_type() {
  strcpy(feed_showtxt,"");
  strcpy(feed_name,"");
  strcpy(feed_desc,"");
  strcpy(feed_gfx_url,"");
  strcpy(feed_release_date,"");
  strcpy(playlistid,"");
  strcpy(playlisturl,"");
  feed_group_antal=0;
  feed_path_antal=0;
  nyt=false;
  textureId=0;
  type=0;                                 // 0 = playlist / 1 = songs / 2 = search artist / 3 = artist records/albums
  intnr=0;
};



// ****************************************************************************************
//
// used by icon anim
//
// ****************************************************************************************

bool tidal_class::reset_amin_in_viewer() {
  //printf("************** RESET ANIM ************************************* \n ");
  anim_viewer=true;
  anim_viewer_search=true;
  anim_angle=180.0f;
  return(true);
}



// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

tidal_class::tidal_class() : antal(0) {
  int port_cnt, n;
  int err = 0;
  anim_angle = 180.0f;
  anim_viewer = true;
  for(int i=0;i<maxantal;i++) stack[i]= 0 ;
  stream_optionselect = 0;							                                        // selected line in stream options
  tidal_oversigt_loaded = false;
  tidal_oversigt_loaded_nr = 0;
  antal = 0;
  type = 0;
  searchtype = 0;
  search_loaded = false;                                                        // load icobn gfx afload search is loaded done by thread.
  tidal_aktiv_song_antal = 0;                                                   //
  gfx_loaded = false;			                                                      // gfx loaded default false
  tidal_is_playing = false;                                                     // is we playing any media
  tidal_is_pause = false;                                                       // is player on pause
  show_search_result = false;                                                   // are we showing search result in view
  antalplaylists = 0;                                                           // antal playlists
  loaded_antal = 0;                                                             // antal loaded
  search_playlist_song = 0;
  texture_loaded = false;
  tidal_aktiv_song_nr = 0;
  //strcpy(tidal_aktiv_song[0].release_date,"");
  //  gfile(logfile,(char *) "Starting web server on port 8100");       //
  //printf("Starting tidal web server on port %s \n",s_http_port);
  // start web server
  // create web server
  /*
  mg_mgr_init(&mgr, NULL);                                                  // Initialize event manager object
  c = mg_bind(&mgr, s_http_port, ev_handler);                               // setup http req handler
  mg_set_protocol_http_websocket(c);                                        //
  */
  active_tidal_device = -1;                                                   // active tidal device -1 = no dev is active
  active_default_play_device = active_tidal_device;                           //
  aktiv_song_tidal_icon = 0;                                                  //
  strcpy(tidal_client_id,"");                                               //
  strcpy(tidal_secret_id,"");                                               //
  strcpy(tidaltoken,"");                                                    //
  strcpy(tidaltoken_refresh,"");                                            //
  strcpy(tidal_client_id,"client_id");
  strcpy(tidal_secret_id,"secret_id");
  strcpy(active_default_play_device_name,"");
  strcpy(overview_show_band_name,"");                                         //
  strcpy(overview_show_cd_name,"");                                           //
  tidal_device_antal = 0;
  tidal_update_loaded_begin = false;                                          // true then we are update the stack data
}



// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

tidal_class::~tidal_class() {
  mg_mgr_free(&mgr);                                             // delete web server again
  mg_mgr_free(&client_mgr);                                      // delete web client
  clean_tidal_oversigt();                                          // clean tidal class
}




// ****************************************************************************************
//
// set texture loaded flag
//
// ****************************************************************************************

void tidal_class::set_textureloaded(bool set) {
  texture_loaded=set; 
}


// ****************************************************************************************
//
// get album by artist NOT IN USE
//
// ****************************************************************************************


int Get_albums_by_artist() {
  std::string curlstring="curl -X 'GET' \
  'https://openapi.tidal.com/artists/1566/albums?countryCode=US&offset=0&limit=50' \
  -H 'accept: application/vnd.tidal.v1+json' \
  -H 'Authorization: Bearer eyJraWQiOiJ2OU1GbFhqWSIsImFsZyI6IkVTMjU2In0.eyJ0eXBlIjoibzJfYWNjZXNzIiwic2NvcGUiOiIiLCJnVmVyIjowLCJzVmVyIjowLCJjaWQiOjEwNjA2LCJleHAiOjE3MDI1MDMzMzksImlzcyI6Imh0dHBzOi8vYXV0aC50aWRhbC5jb20vdjEifQ._nwyfiDuTO-MdHIlyzflhHklh6o-as0wlgWnoeVEumU8opSrSakLLfrKH_X-VNBE0SN31jRmbk_XdViuu-dBOQ' \
  -H 'Content-Type: application/vnd.tidal.v1+json'";
  system(curlstring.c_str());
  return(1);
}



// *******************************************************************************************
// 
// save playlist in db
//
// *******************************************************************************************


int tidal_class::save_music_oversigt_playlists(char *playlistfilename,int tidalknapnr,char *cover_path,char *playlstid,char *artistname) {
  bool fault;
  std::string sql_insert;
  std::string sql_search;
  std::string playlistid;
  bool fundet;
  std::string temptxt;
  unsigned int i;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char database[256];
  strcpy(database,dbname);
  write_logfile(logfile,(char *) "Tidal start playlist save.");
  i=0;
  conn=mysql_init(NULL);
  // Connect to database
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  mysql_query(conn,"set NAMES 'utf8'");
  res = mysql_store_result(conn);
  if (conn) {
    // First inset into playlist db
    sql_insert = "insert into mythtvcontroller.tidalcontentplaylist (playlistname,paththumb,playlistid,release_date,artistid,id) values (\"";
    sql_insert = sql_insert + playlistfilename;                            // playlist name
    sql_insert = sql_insert + "\",'";
    sql_insert = sql_insert + cover_path;                                  // cover
    sql_insert = sql_insert + "','";
    sql_insert = sql_insert + playlstid;                                   // playlist id 0
    sql_insert = sql_insert + "',";
    sql_insert = sql_insert + "now()";                                     // dato
    sql_insert = sql_insert + ",'";
    sql_insert = sql_insert + artistname;                                  // artist name
    sql_insert = sql_insert + "',0)";
    // printf("PLAYLIST SQL : %s \n ",sql_insert.c_str());
    mysql_query(conn,sql_insert.c_str());
    res = mysql_store_result(conn);
    // get id
    sql_search = "select playlistid from mythtvcontroller.tidalcontentplaylist where playlistname like '";
    sql_search = sql_search + playlistfilename;
    sql_search = sql_search + "'";
    mysql_query(conn,sql_search.c_str());
    res = mysql_store_result(conn);
    fundet=false;
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        // playlistid = row[0];
        fundet = true;
      }
    }
    if (fundet) {
      // Now update play files in db.
      i=0;
      while(i<tidal_aktiv_song_antal) {
        temptxt=fmt::v8::format("insert into mythtvcontroller.tidalcontent (name,paththumb,playpath,playlistid,id) values ('{}','{}','{}','{}',0) ON DUPLICATE KEY UPDATE playpath='{}'", tidal_aktiv_song[i].song_name, tidal_aktiv_song[i].cover_image_url, tidal_aktiv_song[i].playurl,playlstid,tidal_aktiv_song[i].playurl);
        // printf("SQL : %s \n ",temptxt);
        mysql_query(conn,temptxt.c_str());
        res = mysql_store_result(conn);
        if (res) fault=false;
        i++;
      }
    }
    mysql_close(conn);
    write_logfile(logfile,(char *) "Tidal playlist save done.");
  } else {
    write_logfile(logfile,(char *) "Tidal playlist save error. No access to mariadb.");

  }
  return(!(fault));
}


// ***************************************************************************************************************************

//
// json
// process playlist
//



void tidal_class::process_array_playlist(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  //printf("array found\n");
  for (x = 0; x < length; x++) {
    process_value_playlist(value->u.array.values[x], depth,x);
  }
}


// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

void tidal_class::process_object_playlist(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    // print_depth_shift(depth);
    // printf("x=%d depth=%d object[%d].name = %s     \n ",x,depth, x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"track")==0) {
      tidal_process_track=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      tidal_process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      tidal_process_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "release_date" )==0) {
      tidal_process_release_date=true;
    }
    if (strcmp(value->u.object.values[x].name , "description" )==0) {
      tidal_process_description=true;
    }
    if (strcmp(value->u.object.values[x].name , "picture" )==0) {
      tidal_process_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      tidal_process_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "title" )==0) {
      tidal_process_title=true;
    }
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      tidal_process_id=true;
    }
    if (strcmp(value->u.object.values[x].name , "track_number" )==0) {
      tidal_process_track_nr=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      tidal_process_items=true;
    }
    if (strcmp(value->u.object.values[x].name , "url" )==0) {
      tidal_process_url=true;
    }
    if (strcmp(value->u.object.values[x].name , "duration" )==0) {
      tidal_process_duration=true;
    }
    process_value_playlist(value->u.object.values[x].value, depth+1,x);
  }
}



// ****************************************************************************************
//
// json parser start call function for process playlist
// do the data progcessing from get_users_albumjson (value)
//
// ****************************************************************************************

void tidal_class::process_value_playlist(json_value* value, int depth,int x) {
  std::string artist="";
  char tempname[1024];
  int j;
  int dircreatestatus;
  char downloadfilenamelong[8192];
  char downloadfilename[1024];
  char sql[1024];
  static int iconnr=0;
  MYSQL_RES *res;
  MYSQL_ROW row;
  std::string convertcommand;
  if (value == NULL) return;
  if (value->type != json_object) {
    // print_depth_shift(depth);
  }
  switch (value->type) {
    case json_none:
      //if (debug_json) fprintf(stdout,"none\n");
      break;
    case json_object:
      process_object_playlist(value, depth+1);
      break;
    case json_array:      
      process_array_playlist(value, depth+1);
      break;
    case json_integer:
      printf("depth = %d x = %d   integer: %s\n", depth, x, value->u.string.ptr);
      if (tidal_process_duration) {
        printf("duation %s \n", value->u.string.ptr );
        if ( antalplaylists<maxantal ) {
          // tidal_aktiv_song[0].duration_ms = atoi(value->u.string.ptr);
        }
        tidal_process_duration=false;
      }
      //
      // not in use
      //if (debug_json) fprintf(stdout,"int: %10" PRId64 "\n", value->u.integer);
      break;
    case json_double:
      //
      // not in use
      //if (debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
      break;
    case json_string:
      printf("depth = %d x = %d   string: %s\n", depth, x, value->u.string.ptr);
      // 1. sample title (ortobello Belle (Live) [Alternative Outtake] [2022 Remaster])
      if (( tidal_process_title ) && (depth==7) && (x==2)){
        if ( antalplaylists<maxantal ) {
          if (antal==0) {
            stack[antal]=new (struct tidal_oversigt_type);
            antalplaylists++;
          }
          if (stack[antal]) {
            strcpy( stack[antal]->feed_showtxt , value->u.string.ptr );                   // hent vist tekst
            //strcpy( stack[antal]->feed_name, value->u.string.ptr );
          }
          iconnr=0;                                                                       // reset gfx icon download counter
        }
        tidal_process_title=false;
      }
      // 2.
      // get name
      if ((tidal_process_track) && (depth==11) && (x==7)) {
        if (stack[antal]) {
          strcpy(stack[antal]->feed_name,value->u.string.ptr);
        }
        tidal_process_track=false;
      }

      if (tidal_process_duration) {
        printf("duation %s \n", value->u.string.ptr );
        if ( antalplaylists<maxantal ) {
          //printf("duation %s \n", value->u.string.ptr );
          // tidal_aktiv_song[0].duration_ms = atoi(value->u.string.ptr);
        }
        tidal_process_duration=false;
      }
      //
      // not work use
      if (( tidal_process_release_date )  && (depth==11) && (x==7)) {
        //printf("antal %d process_release_date %s \n",antal,value->u.string.ptr);
        strcpy( stack[antal]->feed_release_date , value->u.string.ptr );
        tidal_process_release_date=false;
      }
      if ( tidal_process_items ) {
        // set start of items in list
        tidal_process_items=false;
      }
      // gfx url
      // not in use
      if ( tidal_process_image ) {
        //printf(" tidal_playlistname %s Process_image ***** depth=%d x=%d url = %s  *********** \n",tidal_playlistname,depth,x,value->u.string.ptr);
        // get playlist cover
        if (( depth == 12 ) && ( x == 0 )) {
          // get covver file url
          strcpy(stack[antal]->feed_gfx_url,value->u.string.ptr);
        }
        // get song co                    get_webfilename(downloadfilename,value->u.string.ptr);ver
        if (( depth == 13 ) && ( x == 1 )) {
          if (stack[antal]) {
            // printf("process gfx url %s \n",value->u.string.ptr);
            strcpy(stack[antal]->feed_gfx_url , value->u.string.ptr );
          }
        }
        tidal_process_image = false;
      }
      // ok
      // get artist picture
      // 3.
      if ( tidal_process_url ) {
        if (( depth == 12 ) && ( x == 0 )) {
          // printf("img url %s\n", value->u.string.ptr);
          if (iconnr == 3 ) {
            if ( stack[antal] ) {
              // strcpy( stack[antal]->playlisturl , value->u.string.ptr );                           // playlist url
              if (strcmp(stack[antal]->feed_gfx_url,"")==0) {                    
                // check dir exist else create
                if (!(file_exists("~/tidal_gfx"))) {
                  dircreatestatus = mkdir("~/tidal_gfx", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }
                // get file name from url
                get_webfilename(downloadfilename,value->u.string.ptr);
                strcpy(downloadfilenamelong,localuserhomedir);
                strcat(downloadfilenamelong,"/tidal_gfx/");
                
                //strcat(downloadfilenamelong,stack[antal]->feed_showtxt);                              // add artist name to filename
                strcat(downloadfilenamelong,tidal_playlistid);

                strcat(downloadfilenamelong,"_");
                strcat(downloadfilenamelong,downloadfilename);
                for(int n=0;n<strlen(downloadfilenamelong);n++) {
                  //if (downloadfilenamelong[n]=='/') downloadfilenamelong[n]=='_';
                  if (downloadfilenamelong[n]==' ') downloadfilenamelong[n]=='_';
                }
                tidal_download_image(value->u.string.ptr,downloadfilenamelong);
                // strcpy( stack[antal]->feed_gfx_url, downloadfilenamelong);
                strcpy( stack[antal]->feed_gfx_url, downloadfilenamelong);

                convertcommand = "convert -size 320x320 ";
                convertcommand = convertcommand + downloadfilenamelong;
                convertcommand = convertcommand + " ";
                convertcommand = convertcommand + downloadfilenamelong;
                system(convertcommand.c_str());

                stack[antal]->type=1;
              }                
            }
          }
          iconnr++;
        }
        tidal_process_url=false;
      }
      // OK in use playlistid
      if ( tidal_process_id ) {
        // get playlist id
        if (( depth == 9 ) && ( x == 0 )) {
          //printf("Process id %s depth = %d x = %d\n",value->u.string.ptr,depth,x);
          //printf("playlist id %s \n",value->u.string.ptr);
          strcpy(tidal_playlistid , value->u.string.ptr);
          stpcpy(stack[antal]->playlistid, value->u.string.ptr);
        }
        tidal_process_id=false;
      }
      //
      if ( tidal_process_name ) {
        // get playlist name
        if (( depth == 9 ) && ( x == 1 )) {
          //printf("playlist name %-30s \n",value->u.string.ptr);
          // write to log file
          if (stack[antal]) {
            // strcpy(tidal_playlistname , value->u.string.ptr);
            // sprintf(tempname,"Tidal playlistname : %s", value->u.string.ptr);
            //write_logfile(logfile,(char *) tempname);
          }
        }
        //
        // get artis name
        //
        if ((depth==9) && (x==1)) {
          if (stack[antal]) {
            //printf("process_artist name %s \n", value->u.string.ptr);
            strcpy( stack[antal]->feed_artist , value->u.string.ptr );
          }
        }
        // 
        // get Song name
        //
        if ((depth==9) && (x==12)) {
          if (antal==0) {
            stack[antal]=new (tidal_oversigt_type);
          }
          if (stack[antal]) {
            // printf("antal %d process_name %s \n",antal,value->u.string.ptr);
            strcpy(stack[antal]->feed_name,value->u.string.ptr);
          }
          tidal_process_name=false;
        }
        //
        // do new record.
        //
        if ((depth==7) && (x==8)) {
          if ( antalplaylists<maxantal ) {
            antal++;
            antalplaylists++;
            if (!(stack[antal]))  {
              stack[antal]=new (tidal_oversigt_type);
            }
          }
        }
      }
      // get tracknr
      if (tidal_process_track_nr) {
        tidal_process_track_nr=false;
        //printf(" text %s \n",value->u.string.ptr);
        // if (stack[antal]) strcpy( stack[antal]->feed_artist , value->u.string.ptr );
      }
      break;
    case json_boolean:
      //if (debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
      break;
  }
  if (antal>0) antalplaylists=antal-1; else antalplaylists=0;
}


// Secret 
// vxOmFp39rweIVD2rb20qmpETsoAECwhGUdnPIPSXq4g=


// ****************************************************************************************
// IN USE IN MAIN startup
// ****************************************************************************************
//
// get users token (works and in use)
//
// ****************************************************************************************

int tidal_class::get_access_token(char *loginbase64) {
  // lib curl stuf
  FILE *tokenfil=NULL;
  int error=0;
  char curlstring[8192];
  sprintf(curlstring,"/bin/curl -X POST -H 'Authorization: Basic %s' -d 'grant_type=client_credentials' -d 'client_id=%s' https://auth.tidal.com/v1/oauth2/token > tidal_token.json",loginbase64,"Nq5WQmVhv2L7QWQO");
  error=system(curlstring);
  if (error) {
    printf("System call error.\n");    
    write_logfile(logfile,(char *) "Tidal token read fault.");
  } else {
    system("/bin/grep -o '\"access_token\":\"[^\"]*' tidal_token.json | /bin/sed 's/\"//g' | /bin/sed 's/access_token://g' > tidal_token.txt");
    tokenfil=fopen("tidal_token.txt","r");
    if (tokenfil) {
      fgets(tidaltoken,282,tokenfil);
      fclose(tokenfil);
      printf("Tidal token read OK.\n");
      write_logfile(logfile,(char *) "Tidal token read OK");
    }
  }
  if (error) return(0); else return(1);
}



// ****************************************************************************************
//
// Delete tidal playlist record from view and all record in playlist and songs in db
//
// ****************************************************************************************


bool tidal_class::delete_record_in_view(long tidalknapnr) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  long n;
  long db_recnr;
  std::string playlistid;
  std::string sql_delete;
  std::string tmptxt;
  conn = mysql_init(NULL);
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, "mythtvcontroller", 0, NULL, 0);
  n=tidalknapnr;
  db_recnr=stack[n]->intnr;
  playlistid = stack[n]->playlistid;
  // remove element from view
  while(n<antalplaylists-2) {
    stack[n]=stack[n+1];
    n++;
  }
  // delete stack[n];              // delete last element
  // delete from db
  if (tidalknapnr<=antalplaylists) {
    antalplaylists--;  
    // clean up in db to.
    if (atoi(stack[tidalknapnr]->playlistid)>0) {
      sql_delete = fmt::v8::format("delete from mythtvcontroller.tidalcontentplaylist where id='{}' limit 1", db_recnr);
      mysql_query(conn,sql_delete.c_str());
      res = mysql_store_result(conn);
      // delete files from playlist in disk
      sql_delete = fmt::v8::format("select playpath from mythtvcontroller.tidalcontent where playlistid='{}'",playlistid.c_str());
      mysql_query(conn,sql_delete.c_str());
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          printf("Song file to delete : %s \n",row[0]);
          tmptxt = fmt::v8::format("Song file to delete :{}",row[0]);
          write_logfile(logfile,(char *) tmptxt.c_str());
          remove(row[0]);
        }
      } else {
        printf("Tidal No song to delete on playlist %s \n",stack[tidalknapnr]->playlistid);
        tmptxt = fmt::v8::format("Tidal No song to delete on playlist: {}",stack[tidalknapnr]->playlistid);
        write_logfile(logfile,(char *) tmptxt.c_str());
      }
      sql_delete = fmt::v8::format("delete from mythtvcontroller.tidalcontent where playlistid='{}'",stack[tidalknapnr]->playlistid);
      mysql_query(conn,sql_delete.c_str());
      res = mysql_store_result(conn);
      sql_delete = fmt::v8::format("Tidal delete playlist {}",stack[tidalknapnr]->playlistid);
      
    }
  }
  if (conn) mysql_close(conn);
  return(1);
}


// ****************************************************************************************
//
// get uers playlist from file tidal_playlists.txt
//
// ****************************************************************************************


int tidal_class::get_playlist_from_file(char *filename) {
  FILE *fp;
  char *playlisttxt=NULL;
  ssize_t read;
  size_t len=0;
  bool readok=false;
  fp=fopen(filename,"r");
  if (fp) {
    readok=true;
    while ((read = getline(&playlisttxt , &len, fp)) != -1) {
      playlisttxt[strcspn(playlisttxt,"\n")]=0;                                   // remove \n from string
      if (playlisttxt) {
        if (*playlisttxt='#') {
          get_users_album((char *) playlisttxt);                                      // download album to db (works if users have a account (enable or not))
          // tidal_oversigt.tidal_get_artists_all_albums((char *) playlisttxt,true);       //
        }
      }
    }
    fclose(fp);
  }
  if (playlisttxt) free(playlisttxt);
  if (readok) return(1); else return(0);
}


// ****************************************************************************************
//
// use tidal_artistlists.txt to download.
// download all record json info and update db from artistid from the file
//
// ****************************************************************************************

// sub func

bool checkartistdbexist() {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  std::string antal="0";
  conn=mysql_init(NULL);
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"SELECT count(playlistname) FROM mythtvcontroller.tidalcontentplaylist");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        antal = row[0];
      }
    }
  }
  if (conn) mysql_close(conn);
  if (atoi(antal.c_str())>0) return(true); else return(false);
}


// ****************************************************************************************
//
// load from file.
//
// ****************************************************************************************

int tidal_class::get_artist_from_file(char *filename) {
  FILE *fp;
  char *artistidtxt=NULL;
  ssize_t read;
  size_t len=0;
  bool readok=false;
  if (strlen(filename)==0) {
    // open default name
    fp=fopen("tidal_artistlists.txt","r");
  } else {
    fp=fopen(filename,"r");
  }
  if (fp) {
    readok=true;
    while ((read = getline(&artistidtxt , &len, fp)) != -1) {
      artistidtxt[strcspn(artistidtxt,"\n")]=0;                                   // remove \n from string
      if (artistidtxt) {
        if (*artistidtxt!='#') {
          if (tidal_get_artists_all_albums((char *) artistidtxt,true)) sleep(3);
        }
      }
    }
    readok=true;
    fclose(fp);
  }
  if (artistidtxt) free(artistidtxt);
  if (readok) return(1); else return(0);
}



// ****************************************************************************************
//
// get uers playlist
//
// ****************************************************************************************


/*

/bin/curl -X GET 'https://openapi.tidal.com/users/{userId}/playlists

limit=limit
sessionid=sessionid
countryCode=countryCode


play playlist https://listen.tidal.com/playlist/{playlistid xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
play playalbum https://listen.tidal.com/album/{playlistid xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}

eks https://listen.tidal.com/playlist/f9075c2c-efe5-45ed-a68e-f3a1ef36ec8e



*/

char* replace_char(char* str, char find, char replace) {
    char *current_pos = strchr(str,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}


// ******************************************************************************************************
//
// works and in use (Retrieve album details by TIDAL album id.)
//
// get/download to db albumid (json file) (process data in json file to db)
//
// return true on ok 0 on error
//
// ******************************************************************************************************


int tidal_class::get_users_album(char *albumid) {
  int error;
  char curlstring[8192];
  struct stat filestatus;
  char *file_contents;
  int file_size;
  FILE *json_file;
  json_char* json;
  json_value* value;
  bool dbexist;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res1;
  MYSQL_ROW row1;
  char *database = (char *) "mythtvcontroller";
  char sql[8192];
  int curl_error;
  bool playlistexist;
  conn = mysql_init(NULL);
  loaded_antal=0;
  int refid;
  unsigned int tidal_playlistantal=0;
  unsigned int tidal_playlistantal_loaded=0;
  bool tidalplaylistloader_done=false;
  bool playlistsongexist=false;
  dbexist=false;
  std::string json_album_file_name;
  std::string logdata;
  int tt=0;
  // if albumid is not 0
  if (strlen(albumid)>1) {
    try {
      clean_tidal_oversigt();
      if (conn) {
        if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
          mysql_error(conn);
          //exit(1);
        }
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        // test about rss table exist
        mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
        res = mysql_store_result(conn);
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            dbexist = true;
          }
        }
      }
      if (dbexist==false) {
        // backup
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontent (name varchar(255),paththumb text,playpath varchar(255), playlistid varchar(255) ,id int NOT NULL AUTO_INCREMENT) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          // fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify songs)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          // fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify playlists)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),release_date DATE,artistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          // fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (tidal artist table)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentartist (artistname varchar(255),paththumb text,artistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          // fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create index for playlist
        sprintf(sql,"CREATE UNIQUE INDEX tidalcontent_playpath_IDX USING BTREE ON mythtvcontroller.tidalcontent (playpath)");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          // fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
      }
    }
    catch (...) {
      write_logfile(logfile,(char *) "Error process Tidal playlist.");
    }
  
    // ************************
    //
    // get album by libcurl used by get_users_songs
    //
    if (tidal_get_album_items(albumid)!=200) {                                 // download json file for albumid
      write_logfile(logfile,(char *) "Tidal - Error download json file.");
    }
    //
    //
    // ************************  
    json_album_file_name = localuserhomedir;
    json_album_file_name = json_album_file_name + "/";
    json_album_file_name = "tidal_users_album_";
    json_album_file_name = json_album_file_name + albumid;
    json_album_file_name = json_album_file_name + ".json";
    stat(json_album_file_name.c_str(), &filestatus);                              // get file info
    file_size = filestatus.st_size;                                               // get filesize
    if ((conn) && (file_size>0)) {
      file_contents = (char*) malloc(filestatus.st_size);                           // get some work mem
      json_file = fopen(json_album_file_name.c_str(), "r");
      if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
        write_logfile(logfile,(char *) "error load tidal album ......");
        fclose(json_file);
      }
      json = (json_char*) file_contents;
      try {
        value = json_parse(file_contents,file_size);                                  // parser
        process_value_playlist(value, 0,0);                                           // fill stack array
        json_value_free(value);                                                       // json clean up    
        if (file_contents) free(file_contents);                                       // free memory again
      }
      catch (...) {
        write_logfile(logfile,(char *) "error process json file.");
      }
      tt=0;
      while(tt<antalplaylists) {
        if (stack[tt]) {
          // check if playlist exist
          playlistexist=false;
          refid = 0;
          snprintf(sql,sizeof(sql),"select id from mythtvcontroller.tidalcontent where name like '%s' limit 1", albumid);
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              refid=atoi(row[0]);
              playlistexist=true;
            }
          }
          //
          // create playlist in db
          //
          replace_char(stack[tt]->feed_gfx_url, '\'', '_');
          replace_char(stack[tt]->feed_showtxt, '\'', '_');

          //replace_char(stack[tt]->feed_gfx_url, '/', '_');
          std::string sql1;
          if ((!(playlistexist)) && (stack)) {
            // snprintf(sql,sizeof(sql),"insert into mythtvcontroller.tidalcontent (name,paththumb, playpath, playlistid ,id) values ('%s','%s','%s',%d)", albumid , stack[0]->feed_gfx_url, albumid, "", 0 );
            sql1 = fmt::v8::format("insert into mythtvcontroller.tidalcontent (name,paththumb, playpath, playlistid ,id) values ('{}','{}','{}',{})", albumid , stack[0]->feed_gfx_url, albumid, "", 0 );
            //fprintf(stdout,"SQL : %s\n",sql);
            if (mysql_query(conn,sql1.c_str())!=0) {
              write_logfile(logfile,(char *) "mysql create table error.");
              fprintf(stdout,"ERROR SQL : %s\n",sql);
            }          
            res=mysql_store_result(conn);
          }

          if (stack[tt]) {
            playlistexist=false;
            snprintf(sql,sizeof(sql),"select id from mythtvcontroller.tidalcontentplaylist where playlistid like '%s' limit 1", albumid );
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                playlistexist=true;
              }
            }
            //
            // check if exist
            //  
            playlistsongexist=false;      
            snprintf(sql,sizeof(sql),"select id from mythtvcontroller.tidalcontentarticles where name like '%s' and paththumb like '%s' and gfxfilename like '%s' and player like '%s' and playlistid like '%s' and artist like '%s')", stack[tt]->feed_showtxt , stack[tt]->feed_gfx_url ,stack[tt]->feed_gfx_url, stack[tt]->playlisturl, albumid , stack[tt]->feed_artist);
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                playlistsongexist=true;
              }
            }
            //
            // insert record created if not exist ( song name )
            //  
            if (playlistsongexist==false) {
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.tidalcontentarticles (name,paththumb,gfxfilename,player,playlistid,artist,id) values ('%s','%s','%s','%s','%s','%s',%d)", stack[tt]->feed_showtxt , stack[tt]->feed_gfx_url ,stack[tt]->feed_gfx_url, stack[tt]->playlisturl, albumid , stack[tt]->feed_artist , 0 );
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"Error SQL : %s\n",sql);
              }
              mysql_store_result(conn);
            }
            //
            // insert record created playlist if not exist ( playlist name )
            //  
            if (playlistexist==false) {            
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.tidalcontentplaylist (playlistname,paththumb,playlistid,release_date,artistid,id) values (\"%s\",'%s','%s','%s','%s',%d)",  stack[tt]->feed_showtxt , stack[tt]->feed_gfx_url, albumid, stack[tt]->feed_release_date,stack[tt]->feed_artist, 0);
              //fprintf(stdout,"SQL : %s\n",sql);
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"Error SQL : %s\n",sql);
              }
              mysql_store_result(conn);
              logdata="TIDAL Update album id ";
              logdata=logdata + albumid;
              write_logfile(logfile,(char *) logdata.c_str());
            }
          }
        }
        tt++;
      }
      mysql_close(conn);
    }
    if (antalplaylists) return(1); else return(0);
  } else return(0);
}



// ****************************************************************************************
//
// NEW function works
//
// Retrieve a list of albums by TIDAL artist id.
//
// get allbum by artistid (download json file)
//
// return other that 200 if fault
// 200 on ok
//
// ****************************************************************************************

int tidal_class::tidal_get_album_by_artist(char *artistid) {
  std::string userfilename;
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode=0;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=NULL;
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  // old url="https://openapi.tidal.com/artists/"; // old ver
  url="https://openapi.tidal.com/v2/artists/";
  url= url + artistid;
  // old url= url + "/albums?countryCode=US&offset=0&limit=100";
  url= url + "?countryCode=US&offset=0&limit=100&include=albums";
  userfilename = localuserhomedir;
  userfilename = userfilename + "/";
  userfilename = userfilename + "tidal_artist_playlist_";
  userfilename = userfilename + artistid;
  userfilename = userfilename + ".json";
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if ((curl) && (strlen(auth_kode.c_str())>0)) {
    header = curl_slist_append(header, "accept: application/vnd.tidal.v1+json");
    header = curl_slist_append(header, auth_kode.c_str());
    header = curl_slist_append(header, "Content-Type: application/vnd.tidal.v1+json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // ask libcurl to use TLS version 1.3 or later
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_file_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    userfile=fopen(userfilename.c_str(),"w");
    if (userfile) {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
      res = curl_easy_perform(curl);
      fclose(userfile);
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);                   // get result in httpCode
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(200);
    }
  } else {
    write_logfile(logfile,(char *) "Tidal curl fault");
  }
  return(httpCode);
}


// ****************************************************************************************
//
// works ( used by get_users_album )
// 
// get/download album items (download json file)
//
// Retrieve a list of album items - tracks & videos - 
// by TIDAL album id. This endpoint require additional pagination-related properties: 
// 'offset' and 'limit'. The 'items' property in the response might contain both tracks and videos that can be distinguished 
// by the 'artifactType' property.
//
// return 0 if fault tidal_get_album_items
// ok http return code
//
// ****************************************************************************************

int tidal_class::tidal_get_album_items(char *albumid) {
  std::string userfilename;
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode=0;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=NULL;
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  // url="https://openapi.tidal.com/albums/";
  url="https://openapi.tidal.com/v2/albums/";
  // url=TIDAL_ALBUM_URL;
  url=url + albumid;
  url=url + "/items?countryCode=US&offset=0&limit=100";
  userfilename = localuserhomedir;
  userfilename = userfilename + "/";
  userfilename = "tidal_users_album_";
  userfilename = userfilename + albumid;
  userfilename = userfilename + ".json";
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if ((curl) && (strlen(auth_kode.c_str())>0)) {
    header = curl_slist_append(header, "accept: application/vnd.tidal.v1+json");
    header = curl_slist_append(header, auth_kode.c_str());    
    header = curl_slist_append(header, "Content-Type: application/vnd.tidal.v1+json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // ask libcurl to use TLS version 1.3 or later
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_file_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    userfile=fopen(userfilename.c_str(),"w");
    if (userfile) {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
      res = curl_easy_perform(curl);
      fclose(userfile);
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
  return(httpCode);
}


// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

void tidal_class::process_object_playlist_tidal_get_artists_all_albums(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    // print_depth_shift(depth);
    // printf("x=%d depth=%d object[%d].name = %s    \n ",x,depth, x, value->u.object.values[x].name);

    // new
    if (strcmp(value->u.object.values[x].name , "attributes")==0) {
      tidal_process_resource=true;
    }
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      tidal_process_id=true;
    }


    // all under old
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      tidal_process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      tidal_process_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "resource" )==0) {
      tidal_process_resource=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "imageCover" )==0) {
      tidal_process_imagecover=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      tidal_process_name=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "numberOfTracks" )==0) {
      tidal_process_numberOfTracks=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "title" )==0) {
      tidal_process_title=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      tidal_process_id=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "artists" )==0) {
      tidal_process_artist=true;
    }
    if (strcmp(value->u.object.values[x].name , "url" )==0) {
      tidal_process_url=true;
    }
    if (strcmp(value->u.object.values[x].name , "releaseDate" )==0) {
      tidal_process_releasedate=true;
    }
    if (strcmp(value->u.object.values[x].name , "duration" )==0) {
      tidal_process_duration=true;
    }
    process_tidal_get_artists_all_albums(value->u.object.values[x].value, depth+1,x);
  }  
}

void tidal_class::process_array_playlist_tidal_get_artists_all_albums(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  for (x = 0; x < length; x++) {
    process_tidal_get_artists_all_albums(value->u.array.values[x], depth,x);
  }
}


// ****************************************************************************************
//
// sub stuf for playlist
//
// ****************************************************************************************

void tidal_class::process_tidal_get_artists_all_albums(json_value* value, int depth,int x) {
    static std::string artist="";
    static std::string release_date="";
    static std::string playlistname="";
    static std::string playlisturl="";
    static std::string gfxurl="";
    std::string convertcommand;
    char tempname[1024];
    int j;
    int dircreatestatus;
    char downloadfilenamelong[8192];
    char downloadfilename[1024];
    char sql[1024];
    static int iconnr=0;
    MYSQL_RES *res;
    MYSQL_ROW row;
    static int setimg=0;
    if (value == NULL) return;
    if (value->type != json_object) {
      //print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        //if (debug_json) fprintf(stdout,"none\n");
        break;
      case json_object:
        process_object_playlist_tidal_get_artists_all_albums(value, depth+1);
        break;
      case json_array:      
        process_array_playlist_tidal_get_artists_all_albums(value, depth+1);
        break;
      case json_integer:
        // not in use
        //if (debug_json) fprintf(stdout,"int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        // not in use
        //if (debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
        break;
      case json_string:        
        if (tidal_process_title) {
          tidal_process_title = false;
        }
        if (tidal_process_resource) {
          if ((antal==-1) && (depth==6) && (x==0)) {
            artist=value->u.string.ptr;
            tidal_process_resource=false;           
          }
          if ((playlistname.length()==0) && ( depth == 7 ) && ( x == 0 )) {
            playlistname=value->u.string.ptr;
          }
          if (( depth == 10 ) && ( x == 0 )) {
            playlisturl=value->u.string.ptr;
          }
        }
        if ( tidal_process_id ) {
          if (( depth == 5 ) && ( x == 3 )) {
            // new record
            antalplaylists++;
            antal++;
            stack[antal]=new (struct tidal_oversigt_type);    
            // set playlistid + artist
            // update rec and go to next rec
            if (stack[antal]) {              
              strcpy(tidal_playlistid,value->u.string.ptr);
              strcpy(stack[antal]->playlistid,value->u.string.ptr);
              strcpy(stack[antal]->feed_showtxt,playlistname.c_str());
              strcpy(stack[antal]->feed_artist,artist.c_str());
              strcpy(stack[antal]->feed_release_date ,release_date.c_str());
              strcpy(stack[antal]->playlisturl ,playlisturl.c_str());
              strcpy(stack[antal]->feed_gfx_url,gfxurl.c_str());
              strcpy(stack[antal]->type_of_media , "ALBUM" );
              get_webfilename(downloadfilename,(char *) gfxurl.c_str());
              strcpy(downloadfilenamelong,localuserhomedir);
              strcat(downloadfilenamelong,"/tidal_gfx/");
              strcat(downloadfilenamelong,tidal_playlistid);
              strcat(downloadfilenamelong,".jpg");
              // download image
              tidal_download_image((char *) gfxurl.c_str(),downloadfilenamelong);
              // update gfx file name
              strcpy(stack[antal]->feed_gfx_url, downloadfilenamelong);
              stack[antal]->type=1;                                             // playlist type
              playlistname="";
              release_date="";
              playlisturl="";
              gfxurl="";
              setimg = 0;
            }
          }
          tidal_process_id=false;
        }
        // set image
        if ((antal>=0) && ( depth == 10 ) && ( x == 0 )) {
          // get cover file url
          if ((setimg==4) && (gfxurl.length()==0)) {
            gfxurl=value->u.string.ptr;            
          }
          setimg++;
        }      
        // release date
        if (( depth == 7 ) && ( x == 6 )) {
          release_date = value->u.string.ptr ;          
        }
        break;
      case json_boolean:
        //if (debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
        break;
    }
    if (antal>0) antalplaylists=antal-1; else antalplaylists=0;
}


// ****************************************************************************************
//
// fast file exist test func
//
// ****************************************************************************************

inline bool file_exists(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}


// ****************************************************************************************
// NEW function works (download wrong songs )
// get/download all albums items from artistid nad make playlist of it.
//
// return 0 if fault
// ok http return code
//
// ****************************************************************************************

std::string escapeSingleQuotesOss(const std::string& input) {
  std::ostringstream oss;
  for (char c : input) {
    if (c == '\'') {
      oss << "''"; // Escape apostrof
    } else {
      oss << c;
    }
  }
  return oss.str();
}

int tidal_class::tidal_get_artists_all_albums(char *artistid,bool force) {
  int httpcode;
  std::string logdata;
  struct stat filestatus;
  long file_size;
  char *file_contents;
  json_char* json;
  json_value* value;
  FILE *json_file;
  std::string tidal_artis_playlist_file;
  // mysql stuf
  char sql[2048];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  bool playlistexist;
  int recnr=0;
  bool dbexist=false;
  int create_new_record_antal=0;
  int created_playlist;
  std::string sqll;
  std::string last_artist="";
  // process json artist file after create file name
  
  // tidal_artist_playlist_$artistid.json have the data
  tidal_artis_playlist_file = localuserhomedir;
  tidal_artis_playlist_file = tidal_artis_playlist_file + "/";
  tidal_artis_playlist_file = tidal_artis_playlist_file + "tidal_artist_playlist_";
  tidal_artis_playlist_file = tidal_artis_playlist_file + artistid;
  tidal_artis_playlist_file = tidal_artis_playlist_file + ".json";
  if ((!(file_exists(tidal_artis_playlist_file))) || (force)) {
    // download artist album json file
    // filename is tidal_artist_playlist_{playlistid}.json
    httpcode=tidal_get_album_by_artist(artistid);                                       // (download all albums in in one json file by artistid)
    if ((httpcode==200) || (httpcode==207)) {
      stat(tidal_artis_playlist_file.c_str(), &filestatus);                             // get file info
      file_size = filestatus.st_size;                                                   // get filesize
      if (file_size>0) {
        file_contents = (char*) malloc(filestatus.st_size);                             // get some work mem
        json_file = fopen(tidal_artis_playlist_file.c_str(), "r");                      // load jons file to process
        if (json_file) {
          while(!(feof(json_file))) {
            fread(file_contents,file_size,1,json_file);
          }
          fclose(json_file);
        }
        json = (json_char*) file_contents;                                              // make pointer
        try {
          value = json_parse(file_contents,file_size);                                  // parser create value obj
          antal=-1;                                                                     // reset antal
          antalplaylists=0;                                                             // reset antal
          process_tidal_get_artists_all_albums(value, 0,0);                             // process to stack variable
          if (file_contents) free(file_contents);                                       // free memory again
          json_value_free(value);                                                       // json clean up
          // the array is ready
          conn=mysql_init(NULL);
          if (conn) {
            mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
            mysql_query(conn,"set NAMES 'utf8'");
            res = mysql_store_result(conn);        
            mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                dbexist = true;
              }
            }
            if (dbexist==false) {
              // create db (tidal songs table)           
              sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontent (name varchar(255),paththumb text,playpath varchar(255), playlistid varchar(255) ,id int NOT NULL AUTO_INCREMENT KEY) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              res = mysql_store_result(conn);
              // create db (tidal playlist table)
              sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              res = mysql_store_result(conn);
              // create db (tidal playlists table)
              sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),release_date DATE,artistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              res = mysql_store_result(conn);
              // create db (tidal artist table)
              sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentartist (artistname varchar(255),paththumb text,artistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              sprintf(sql,"CREATE UNIQUE INDEX IF NOT EXISTS tidalcontentartist_artistid_IDX USING BTREE ON mythtvcontroller.tidalcontentartist (artistid)");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create index error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              res = mysql_store_result(conn);
              // create index for playlist
              sprintf(sql,"CREATE UNIQUE INDEX IF NOT EXISTS tidalcontent_playpath_IDX USING BTREE ON mythtvcontroller.tidalcontent (playpath)");
              if (mysql_query(conn,sql)!=0) {
                write_logfile(logfile,(char *) "mysql create table error.");
                fprintf(stdout,"SQL : %s\n",sql);
              }
              res = mysql_store_result(conn);
            }
            //
            // create all playlists in db if not exist
            //  
            // Husk antal
            recnr=0;
            created_playlist=0;
            create_new_record_antal=antalplaylists;
            if (stack[recnr]) {
              while ((recnr<create_new_record_antal) && (stack[recnr])) {
                // check if exist
                playlistexist = false;
                strcpy(sql,"select playlistid from mythtvcontroller.tidalcontentplaylist where playlistid like '");
                strcat(sql,stack[recnr]->playlistid);
                strcat(sql,"'");
                mysql_query(conn,sql);
                res = mysql_store_result(conn);
                if (res) {
                  while ((row = mysql_fetch_row(res)) != NULL) {
                    playlistexist = true;
                  }
                }
                // if do not exist create in db playlist
                if ( playlistexist == false ) {
                  // only albums for now
                  if (strcmp(stack[recnr]->type_of_media,"ALBUM")==0) {
                    std::string showtext = stack[recnr]->feed_showtxt;                    
                    sqll = "insert into mythtvcontroller.tidalcontentplaylist (playlistname,paththumb,playlistid,release_date,artistid,id) values (";
                    sqll = sqll + "'";
                    sqll = sqll + escapeSingleQuotesOss(stack[recnr]->feed_showtxt);                 // playlist name
                    sqll = sqll + "','";
                    sqll = sqll + stack[recnr]->feed_gfx_url;                 // cover
                    sqll = sqll + "','";
                    sqll = sqll + stack[recnr]->playlistid;                   // playlist id 0
                    sqll = sqll + "','";
                    sqll = sqll + stack[recnr]->feed_release_date;            // dato
                    sqll = sqll + "','";
                    sqll = sqll + stack[recnr]->feed_artist;
                    sqll = sqll + "',0)";
                    if (mysql_query(conn,sqll.c_str())!=0) {
                      write_logfile(logfile,(char *) "mysql create insert error (insert into mythtvcontroller.tidalcontentplaylist).");
                      fprintf(stdout,"Error SQL : %s\n",sql);
                    }
                    mysql_store_result(conn);
                    // create artist if not exist
                    if (last_artist!=stack[recnr]->feed_artist) {
                      sqll = "insert into mythtvcontroller.tidalcontentartist (artistname,paththumb,artistid,id) values (";
                      sqll = sqll + "'";
                      sqll = sqll + stack[recnr]->feed_artist;
                      sqll = sqll + "','none','";
                      sqll = sqll + stack[recnr]->feed_artist;
                      sqll = sqll + "',0)";
                      if (mysql_query(conn,sqll.c_str())!=0) {
                        write_logfile(logfile,(char *) "mysql create insert error (insert into mythtvcontroller.tidalcontentartist).");
                      }
                      last_artist=stack[recnr]->feed_artist;
                      mysql_store_result(conn);
                    }
                    strcpy(stack[recnr]->feed_artist,"");                   // reset to get data again else it will ignore it.
                    printf("Tidal downloading album + json file %s  nr %d \n",stack[recnr]->playlistid,recnr);
                    // Download playlist json file + update db from the downloaded json file.
                    // get_users_album(stack[recnr]->playlistid);
                    logdata="TIDAL Update playlist id ";
                    logdata=logdata + stack[recnr]->playlistid;
                    write_logfile(logfile,(char *) logdata.c_str());
                  }
                  created_playlist++;
                }
                recnr++;
              }
            }
            // restore antal
            antalplaylists=create_new_record_antal;
          }
        }
        catch (...) {
          printf("Error write file.\n");
        }
      }
    } else {
      write_logfile(logfile,"Tidal return http code 404.");
    }
  }
  // sample
  //get_users_album("012345");
  if (create_new_record_antal==0) write_logfile(logfile,(char *) "Tidal can not download artist json file.");
  return(create_new_record_antal);
}



// ****************************************************************************************
//
// save the refresh token in used
//
// ****************************************************************************************

void tidal_class::tidal_set_token(char *token,char *refresh) {
  strcpy(tidaltoken,token);
  strcpy(tidaltoken_refresh,refresh);
}



// ****************************************************************************************
//
// in use in main in function (webupdate_loader_tidal)
// get user id from tidal api
//
// return 0 if fault
// 200 on ok
//
// ****************************************************************************************

int tidal_class::tidal_get_user_id() {
  static const char *userfilename = "tidal_user_id.txt";
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  std::string logdata;
  char post_playlist_data[4096];
  int httpCode=0;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=NULL;
  devid=get_active_device_id();
  if (devid) {
    auth_kode="Authorization: Bearer ";
    auth_kode=auth_kode + tidaltoken;
    url="https://api.tidal.com/v1/me";
    url=url + devid;
    printf("Get user info.\n");
    // use libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if ((curl) && (strlen(auth_kode.c_str())>0)) {
      /* Add a custom header */
      header = curl_slist_append(header, "Accept: application/json");
      header = curl_slist_append(header, "Content-Type: application/json");
      header = curl_slist_append(header, "charsets: utf-8");
      header = curl_slist_append(header, auth_kode.c_str());
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      // ask libcurl to use TLS version 1.3 or later
      curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_file_write_data);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
      curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
      curl_easy_setopt(curl, CURLOPT_POST, 0);
      // set type post/put
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); /* !!! */
      userfile=fopen(userfilename,"w");
      if (userfile) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
        res = curl_easy_perform(curl);
        fclose(userfile);
      }
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
      if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
      }
      // always cleanup
      curl_easy_cleanup(curl);
      curl_global_cleanup();
      if (httpCode == 200) logdata="TIDAL login ok"; else logdata="TIDAL login fault";
      write_logfile(logfile,(char *) logdata.c_str());
      if (httpCode == 200) return(200);
    }
  }
  return(httpCode);
}




// ****************************************************************************************
// test test test (NOT IN USE)
// 1 . auth_device
//
// return -1 if fault
// 200 on ok
//
// ****************************************************************************************

int tidal_class::auth_device_authorization() {
  static const char *userfilename = "tidal_user_id.txt";
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode=-1;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=NULL;
  sprintf((char *) post_playlist_data,"client_id: client_id&scope: r_usr+w_usr+w_sub");
  devid=get_active_device_id();
  if (devid) {
    auth_kode="Authorization: Bearer ";
    auth_kode=auth_kode + tidaltoken;
    url="https://auth.tidal.com/v1/oauth2/device_authorization";
    url=url + devid;
    printf("auth device\n ");
    // use libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if ((curl) && (strlen(auth_kode.c_str())>0)) {
      /* Add a custom header */
      header = curl_slist_append(header, "Accept: application/json");
      header = curl_slist_append(header, "Content-Type: application/json");
      header = curl_slist_append(header, "charsets: utf-8");
      //header = curl_slist_append(header, auth_kode.c_str());
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      // ask libcurl to use TLS version 1.3 or later
      curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_file_write_data);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
      curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
      //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
      curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
      curl_easy_setopt(curl, CURLOPT_POST, 1);
      // data to post
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data );
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
      // set type post/put
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"); /* !!! */
      userfile=fopen(userfilename,"w");
      if (userfile) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
        res = curl_easy_perform(curl);
        fclose(userfile);
      }
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
      if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
      }
      // always cleanup
      curl_easy_cleanup(curl);
      curl_global_cleanup();
      if (httpCode == 200) {
        return(200);
      }
    }
  }
  return(httpCode);
}




// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************

void tidal_class::clean_tidal_oversigt() {
    startup_loaded=false;
    for(int i=1;i<antal-1;i++) {
      if (stack[i]) {
        // crash
        if (stack[i]->textureId) {
          // if (stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete spotify texture
        }
        if (stack[i]) delete stack[i];
      }
      stack[i]=NULL;
    }
    antal=0;
    antalplaylists=0;
    tidal_oversigt_loaded=false;			// set load icon texture again
    tidal_oversigt_loaded_nr=0;
    tidal_oversigt_nowloading=0;
    // if (get_tidal_aktiv_cover_image()) glDeleteTextures(1, get_tidal_aktiv_cover_image());
}



// *********************************************************************************************************************************
// Download image
//
// sample call
// download_image("https://i.scdn.co/image/ab67616d0000b2737bded29598acbe1e2f4b4437","/home/user/image_filename.jpg");
//
// ********************************************************************************************

int tidal_class::tidal_download_image(char *imgurl,char *filename) {
  FILE *file;
  std::string response_string;
  CURLcode res;
  CURL *curl;
  char *base64_code;
  char errbuf[CURL_ERROR_SIZE];
  if (!(file_exists(filename))) {
    curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, imgurl);
      // send data to curl_writeFunction_file
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
      // ask libcurl to use TLS version 1.3 or later
      curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
      curl_easy_setopt(curl, CURLOPT_VERBOSE,0L);
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // <-- ssl don't forget this
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); // <-- ssl and this
      errbuf[0] = 0;
      try {
        file = fopen(filename, "wb");
        if (file) {
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
          // get file
          res = curl_easy_perform(curl);
          fclose(file);
        }
        if(res != CURLE_OK) {
          fprintf(stderr, "%s\n", curl_easy_strerror(res));
        }
      }
      catch (...) {
        printf("Error write file.\n");
      }
      curl_easy_cleanup(curl);
    }
  }
  return(1);
}




// ****************************************************************************************
//
// get loader flag
//
// ****************************************************************************************


bool tidal_class::get_tidal_update_flag() {
  return(tidal_update_loaded_begin);
}


// ****************************************************************************************
//
// set loader flag
//
// ****************************************************************************************


void tidal_class::set_tidal_update_flag(bool flag) {
  tidal_update_loaded_begin=flag;
}




// ****************************************************************************************
//
// IN USE (old test)
// Get users playlist NOT in use
// The default view
//
// ****************************************************************************************

int tidal_class::tidal_get_user_playlists(bool force,int startoffset) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res1;
  MYSQL_ROW row1;
  char *database = (char *) "mythtvcontroller";
  char sql[8192];
  char doget[4096];
  char filename[4096];
  char temptxt[80];
  char downloadfilenamelong[4096];
  int curl_error;
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  int file_size;
  char* file_contents;
  json_char* json;
  json_value* value;
  bool playlistexist;
  bool dbexist=false;
  conn = mysql_init(NULL);
  loaded_antal=0;
  unsigned int tidal_playlistantal=0;
  unsigned int tidal_playlistantal_loaded=0;
  bool tidalplaylistloader_done=false;
  try {
    if (conn) {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
         mysql_error(conn);
         //exit(1);
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about rss table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist = true;
        }
      }
      // create db
      if (dbexist==false) {
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontent (name varchar(255),paththumb text,playpath varchar(255), playlistid varchar(255) ,id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify songs)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify playlists)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (tidal artist table)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentartist (artistname varchar(255),paththumb text,artistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create index for playlist
        sprintf(sql,"CREATE UNIQUE INDEX tidalcontent_playpath_IDX USING BTREE ON mythtvcontroller.tidalcontent (playpath)");
        if (mysql_query(conn,sql)!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
      }
      // download all playlist (name + id) in one big file NOT songs
      // if we have a token
      if (strcmp(tidaltoken,"")!=0) {
        // 50 is the max
        //download_user_playlist(tidaltoken,startoffset);
        while (tidalplaylistloader_done==false) {
          sprintf(doget,"curl -X GET 'https://api.tidal.com/v1/me/playlists?limit=50&offset=%d' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.json",startoffset,tidaltoken);
          curl_error=system(doget);
          if (curl_error!=0) {
            fprintf(stderr,"Curl error get user playlists\n");
            exit(0);
          }
          // hent users playlist from tidal api
          system("cat tidal_users_playlist.json | grep tidal:playlist: | awk {'print substr($0,31,22)'} > tidal_users_playlist.txt");
          // get antal playliste first time (we can only load 50 at time)
          if (tidal_playlistantal_loaded==0) {
            system("cat tidal_users_playlist.json | grep total | tail -1 | awk {'print $3'} > tidal_users_playlist_antal.txt");
            json_file = fopen("spotify_users_playlist_antal.txt", "r");
            fscanf(json_file, "%s", temptxt);
            if (strcmp(temptxt,"")!=0) tidal_oversigt->tidal_playlist_antal = atoi(temptxt);
            else tidal_oversigt->tidal_playlist_antal = 0;
             fclose(json_file);
          }
          stat("spotify_users_playlist.txt", &filestatus);                              // get file info
          file_size = filestatus.st_size;                                               // get filesize
          if (file_size>0) {
            file_contents = (char*) malloc(filestatus.st_size);                           // get some work mem
            json_file = fopen("spotify_users_playlist.txt", "r");
            if (json_file) {
              while(!(feof(json_file))) {
                fscanf(json_file, "%s", file_contents);
                // process playlist id
                tidal_oversigt->tidal_get_playlist(file_contents,force,1);
                tidal_oversigt->clean_tidal_oversigt();
                loaded_antal++;
              }
              fclose(json_file);
            }
            if (file_contents) free(file_contents);                                                          // free memory again
          }
          tidal_playlistantal_loaded+=startoffset;
          // next loop
          // 50 is loaded on each loop until end
          if ((startoffset+50)<tidal_oversigt->tidal_playlist_antal) {
            startoffset+=50;
          } else {
            startoffset=tidal_oversigt->tidal_playlist_antal-startoffset;
          }
          if (tidal_playlistantal_loaded>=tidal_oversigt->tidal_playlist_antal) tidalplaylistloader_done=true;
        }
        if (remove("tidal_users_playlist.txt")!=0) write_logfile(logfile,(char *) "Error remove user playlist file tidal_users_playlist.txt");
        // save data to mysql db
      } else {
          write_logfile(logfile,(char *) "Error downloading user playlist");
          exit(0);
      }
      sprintf(sql,"select playlistname,playlistid from mythtvcontroller.tidalcontentplaylist");
      write_logfile(logfile,(char *) "process playlist ......");
      mysql_query(conn,sql);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          fprintf(stdout,"playlist %-60s Tidalid %-20s \n",row[0],row[1]);
          if (tidal_oversigt->tidal_get_playlist(row[1],force,0)==1) {
            fprintf(stderr,"Error create playlist %s \n",row[1]);
          }
        }
      }
      write_logfile(logfile,(char *) "process tidal playlist done..");
      mysql_close(conn);
    } else {
      write_logfile(logfile,(char *) "No mysql db ..");
    }
  }
  catch (...) {
    write_logfile(logfile,(char *) "Error process playlist");
  }
  return(1);
}



// ****************************************************************************************
//
// opdate from db first call
//
// ****************************************************************************************

int tidal_class::opdatere_tidal_oversigt(char *refid) {
  char temptxt1[2048];
  char temptxt2[2048];
  char *songstrpointer;
  char sqlselect[2048];
  char tmpfilename[1024];
  char lasttmpfilename[1024];
  char downloadfilename[1024];
  char downloadfilename1[1024];
  char downloadfilenamelong[1024];
  char homedir[1024];
  std::string convertcommand;
  bool rss_update=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  const char *database = (char *) "mythtvcontroller";
  bool online;
  int getart=0;
  bool loadstatus=true;
  bool dbexist=false;
  GLuint texture;
  antal=0;
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)==0) {
      dbexist=false;
    }
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test about table exist
    mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) dbexist=true;
    }
    mysql_close(conn);
  }
  // clear old view
  clean_tidal_oversigt();
  strcpy(lasttmpfilename,"");
  if (debugmode & 4) {
    fprintf(stderr,"loading tidal data.\n");
  }
  // find records after type (0 = root, else = refid)
  if (refid == NULL) {
    show_search_result=false;
    sprintf(sqlselect,"select playlistname,paththumb,playlistid,release_date,artistid,id  from mythtvcontroller.tidalcontentplaylist group by (playlistname) order by artistid,release_date desc,playlistname");
    getart = 0;
  } else {
    show_search_result=true;  
    sprintf(sqlselect,"select name,p.paththumb,playpath,p.playlistid from mythtvcontroller.tidalcontent t left join mythtvcontroller.tidalcontentplaylist p on t.playlistid = p.playlistid WHERE p.playlistid like '%s' order by t.name",refid);
    getart = 1;
  }
  this->type = getart;					                                                 // husk sql type
  fprintf(stderr,"tidal loader started... \n");
  conn=mysql_init(NULL);
  // Connect to database
  if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
    //get_antal_rss_feeds_sources(conn);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    if (mysql_query(conn,sqlselect)!=0) {
      fprintf(stderr,"mysql insert error.\n");
      fprintf(stderr,"SQL %s \n",sqlselect);
    }
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
        if (antal<maxantal) {
          stack[antal]=new (struct tidal_oversigt_type);
          if (stack[antal]) {
            strcpy(stack[antal]->feed_showtxt,"");          	            // show name
            strcpy(stack[antal]->feed_name,"");		                        // mythtv db feedtitle
            strcpy(stack[antal]->feed_desc,"");                           // desc
            strcpy(stack[antal]->feed_gfx_url,"");
            strcpy(stack[antal]->feed_release_date,"");
            strcpy(stack[antal]->playlisturl,"");
            stack[antal]->feed_group_antal=0;
            stack[antal]->feed_path_antal=0;
            stack[antal]->textureId=0;
            stack[antal]->intnr=atoi(row[5]);                               // før 3
            stack[antal]->nyt=false;
            stack[antal]->type=0;            
            // top level (load playlist)
            if (getart == tidal_playlisttype ) {
              strncpy(stack[antal]->feed_showtxt,row[0],tidal_pathlength);
              strncpy(stack[antal]->feed_name,row[0],tidal_namelength);
              if (row[1]) {
                if (strncmp(row[1],"http",4)==0) {
                  get_webfilename(downloadfilename,row[1]);
                  //getuserhomedir(downloadfilenamelong);
                  strcpy(downloadfilenamelong,localuserhomedir);
                  strcat(downloadfilenamelong,"/");
                  strcat(downloadfilenamelong,tidal_gfx_path);
                  strcat(downloadfilenamelong,downloadfilename);
                  strcat(downloadfilenamelong,".jpg");
                  // download file if not exist
                  if (!(file_exists(downloadfilenamelong))) {
                    tidal_download_image(row[1],downloadfilenamelong);                // download file
                  } else strcpy(downloadfilenamelong,row[1]);                         // no file name
                } else strcpy(downloadfilenamelong,row[1]);                           // NOT url
                strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
              }
              strncpy(stack[antal]->playlistid,row[2],tidal_namelength);    //
              // convert to -size 320x320 size of org
              convertcommand = "convert -size 320x320 ";
              convertcommand = convertcommand + downloadfilenamelong;
              convertcommand = convertcommand + " ";
              convertcommand = convertcommand + downloadfilenamelong;
              system(convertcommand.c_str());
              antal++;
            }
            // load playlist songs
            if (getart == tidal_songlisttype ) {
              // First create the back button
              if (antal == 0) {
                strcpy(stack[antal]->feed_showtxt,"Back");
                strcpy(stack[antal]->playlisturl,"");
                stack[antal]->intnr=0;
                antal++;
                if (stack[antal]==NULL) stack[antal]=new (struct tidal_oversigt_type);
              }
              strncpy(stack[antal]->feed_showtxt,row[0],tidal_pathlength);
              strncpy(stack[antal]->feed_name,row[0],tidal_namelength);
              strncpy(stack[antal]->feed_gfx_url,row[1],tidal_namelength);
              stack[antal]->type=1;              
              strcpy(stack[antal]->playlistid,row[2]);                              // id is path here
              antal++;
            }
          }
        }
      }
      mysql_close(conn);
    } else {
      fprintf(stderr,"No tidal data loaded \n");
    }
    antalplaylists=antal;
    return(antal);
  } else {
    write_logfile(logfile,(char *) "Failed to update tidal db, can not connect to database.");
    fprintf(stderr,"Failed to update tidal db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
  }
  fprintf(stderr,"Tidal loader done... Antal records %d \n",antalplaylists);
  return(0);
}



// ****************************************************************************************
//
// get songs from playlist (any public user)
// write to spotify_playlist_{spotifyid}.json
// and update db from that file all the songs in playlist
//
// ****************************************************************************************

int tidal_class::tidal_get_playlist(const char *playlist,bool force,bool create_playlistdb) {
  return(0);
}


// ****************************************************************************************
//
//
// ****************************************************************************************


int tidal_class::opdatere_tidal_oversigt_searchtxt(char *keybuffer,int type) {
  // not in use for now  
  return(1);
}


// ****************************************************************************************
//
// Used by process_tidal_search_result
//
// ****************************************************************************************


void tidal_class::process_object_tidal_search_result(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    // print_depth_shift(depth);
    printf("x=%d depth=%d object[%d].name = %s     \n ",x,depth, x, value->u.object.values[x].name);
    // new
    if (strcmp(value->u.object.values[x].name , "attributes")==0) {      
      tidal_process_attributes=true;
    }
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      tidal_process_id=true;
    }

    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      tidal_process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      tidal_process_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "resource" )==0) {
      tidal_process_resource=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "imageCover" )==0) {
      tidal_process_imagecover=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      tidal_process_name=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "title" )==0) {
      tidal_process_title=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      tidal_process_id=true;
    }
    // ok
    if (strcmp(value->u.object.values[x].name , "artists" )==0) {
      tidal_process_artist=true;
    }
    if (strcmp(value->u.object.values[x].name , "url" )==0) {
      tidal_process_url=true;
    }
    if (strcmp(value->u.object.values[x].name , "releaseDate" )==0) {
      tidal_process_releasedate=true;
    }
    process_tidal_search_result(value->u.object.values[x].value, depth+1,x);
  }  
}


// ****************************************************************************************
//
// Used by process_tidal_search_result
//
// ****************************************************************************************


void tidal_class::process_array_tidal_search_result(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  //printf("array found\n");
  for (x = 0; x < length; x++) {
    process_tidal_search_result(value->u.array.values[x], depth, x);
  }
}

// ****************************************************************************************
//
// Used by opdatere_tidal_oversigt_searchtxt_online
// Works fine
//
// ****************************************************************************************


void tidal_class::process_tidal_search_result(json_value* value, int depth,int x) {
    char tempname[1024];
    int n;
    int dircreatestatus;
    char downloadfilenamelong[8192];
    char downloadfilename[1024];
    char sql[1024];
    static int iconnr=0;
    MYSQL_RES *res;
    MYSQL_ROW row;
    static int setimg=0;
    static std::string artist="";
    static std::string release_date="";
    static std::string playlistname="";
    static std::string playlistid="";
    static std::string playlisturl="";
    static std::string title="";
    static std::string gfxurl="";

    if (value == NULL) return;
    if (value->type != json_object) {
      //print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        //if (debug_json) fprintf(stdout,"none\n");
        break;
      case json_object:
        process_object_tidal_search_result(value, depth+1);
        break;
      case json_array:      
        process_array_tidal_search_result(value, depth+1);
        break;
      case json_integer:
        // not in use
        //if (debug_json) fprintf(stdout,"int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        // not in use
        //if (debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
        break;
      case json_string:        
        printf("string: %s\n", value->u.string.ptr);
        // 1.
        if (tidal_process_attributes) {
          printf("%s depth = %d x = %d \n ",value->u.string.ptr ,depth,x);
          if ((depth==7) && (x==0)) {
            // title
            if (playlistname.length()==0) playlistname = value->u.string.ptr;
          }
          tidal_process_attributes=false; 
        }

        if ( tidal_process_id ) {
          if ((depth==7) && (x==6)) {
            // dato 
            release_date = value->u.string.ptr;
          }
          if ((depth==10) && (x==0)) {
            // gfx 160x160
            if ((setimg==5) && (gfxurl.length()==0)) {
              gfxurl = value->u.string.ptr;
            }
            // playurl
            if ((setimg==7) && (playlisturl.length()==0)) {
              playlisturl = value->u.string.ptr;
            }
            setimg++;
          }
          if ((depth==9) && (x==0)) {
            playlistid = value->u.string.ptr;
          }
          // update rec
          if (playlistname.length()>0) {
            if (( depth == 11 ) && ( x == 0 )) {
              antalplaylists++;
              antal++;
              stack[antal]=new (struct tidal_oversigt_type);    
              // set playlistid + artist
              // update rec and go to next rec
              if (stack[antal]) {
                size_t lastSlash = playlisturl.find_last_of('/');
                playlistid = (lastSlash != std::string::npos) ? playlisturl.substr(lastSlash + 1) : "";
                strcpy(stack[antal]->playlistid,playlistid.c_str());
                strcpy(stack[antal]->feed_showtxt,playlistname.c_str());
                strcpy(stack[antal]->feed_artist,artist.c_str());
                strcpy(stack[antal]->feed_release_date ,release_date.c_str());
                strcpy(stack[antal]->playlisturl ,playlisturl.c_str());
                strcpy(stack[antal]->feed_gfx_url,gfxurl.c_str());
                strcpy(stack[antal]->type_of_media , "ALBUM" );
                stack[antal]->type=1;                                             // playlist type
                get_webfilename(downloadfilename,(char *) gfxurl.c_str());
                strcpy(downloadfilenamelong,localuserhomedir);
                strcat(downloadfilenamelong,"/tidal_gfx/");
                strcat(downloadfilenamelong,playlistid.c_str());
                strcat(downloadfilenamelong,".jpg");
                // download image
                tidal_download_image((char *) gfxurl.c_str(),downloadfilenamelong);
                // update gfx file name
                strcpy(stack[antal]->feed_gfx_url, downloadfilenamelong);
                stack[antal]->textureId = 0;
                stack[antal]->type = 2;                                             // playlist type
                playlistname = "";
                release_date = "";
                playlisturl = "";
                gfxurl = "";
                setimg = 0;
              }
            }
          }
          
        }

        if (tidal_process_title) {
          tidal_process_title= false;
        }

        
        // 1.
        /*
        if (tidal_process_resource) {
          iconnr=0;
          antalplaylists++;
          antal++;
          stack[antal]=new (struct tidal_oversigt_type);
          // printf("New record *********************************************** \n");
          // printf("antal %d antalplaylists %d \n",antal,antalplaylists);
          tidal_process_resource=false;
        }

        if ( tidal_process_title ) {
          if ((depth==7) && (x==2)) {
            // printf("Set Title string: %s\n", value->u.string.ptr);         
            if (stack[antal]) {
              strcpy( stack[antal]->feed_showtxt , value->u.string.ptr );
            }
            iconnr=0;                                                                       // reset gfx icon download counter
          }
          tidal_process_title=false;
        }
        if ( tidal_process_image ) {
          // get playlist cover
          if (( depth == 12 ) && ( x == 0 )) {
            // get cover file url
            // printf("Set Image string: %s\n", value->u.string.ptr);
            if (stack[antal]) {
              // strcpy(stack[antal]->feed_gfx_url,value->u.string.ptr);
            }
          }
          if (( depth == 13 ) && ( x == 1 )) {
            if (stack[antal]) {
              // strcpy(stack[antal]->feed_gfx_url, value->u.string.ptr );
            }
          }
          tidal_process_image = false;
        }
        // ok
        // get cover
        // 3.
        if ( tidal_process_url ) {
          if (( depth == 10 ) && ( x == 0 )) {
            if (iconnr==3) {
              // printf("img cover url %s icon nr %d \n", value->u.string.ptr, iconnr);
              if (stack[antal]) {
                  if (!(file_exists("~/tidal_gfx"))) {
                    dircreatestatus = mkdir("~/tidal_gfx", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                  }
                  // get file name from url
                  get_webfilename(downloadfilename,value->u.string.ptr);
                  strcpy(downloadfilenamelong,localuserhomedir);
                  strcat(downloadfilenamelong,"/tidal_gfx/");
                  strcat(downloadfilenamelong,stack[antal]->feed_showtxt);                        // add artist name
                  strcat(downloadfilenamelong,"_");
                  strcat(downloadfilenamelong,downloadfilename);                                  // add filename
                  n=0;
                  while(n<strlen(downloadfilenamelong)) {
                    if (downloadfilenamelong[n]==' ') downloadfilenamelong[n]='_';
                    n++;
                  }
                  // Set tidal cover file name 
                  if (tidal_download_image( value->u.string.ptr, downloadfilenamelong )) {
                    strcpy( stack[antal]->feed_gfx_url, downloadfilenamelong );
                  } else strcpy( stack[antal]->feed_gfx_url,"" );
                  stack[antal]->type=2;                                                           // 2 = type is playlist
              }
            }
            iconnr++;
          }
          tidal_process_url=false;
        }
        //
        // ok image cover stuf
        //
        if ( tidal_process_imagecover ) {
          tidal_process_imagecover=false;
        }
        // OK in use id
        if ( tidal_process_id ) {
          // get artist id
          if (( depth == 7 ) && ( x == 0 )) {
            // printf("Set Playlist id %s \n",value->u.string.ptr);
            strcpy(stack[antal]->playlistid,value->u.string.ptr);
            strcpy(tidal_playlistid , value->u.string.ptr);
          }
          tidal_process_id=false;
        }
        // ok for name
        if ( tidal_process_name ) {
          // get album name
          if (( depth == 6 ) && ( x == 2 )) {
            // printf("Set Playlist name %-30s \n",value->u.string.ptr);
            if (stack[antal]) {
              // strcpy(tidal_playlistname , value->u.string.ptr);
            }
            tidal_process_name=false;
          }
          //
          // get artis name ok
          //
          if ((depth==9) && (x==1)) {
            if (stack[antal]) {
              // printf("Set artist name %s \n", value->u.string.ptr);
              // strcpy( stack[antal]->feed_artist , value->u.string.ptr );
            }
          }
          //
          // get artis name ok
          //
          if (( depth == 10 ) && ( x == 1 )) {
            if (stack[antal]) {
              // printf("Set artist name %s \n", value->u.string.ptr);
              strcpy( stack[antal]->feed_artist , value->u.string.ptr );
            }
            tidal_process_name=false;
          }
        }
        // set release date
        if (tidal_process_releasedate) {
          if (stack[antal]) {
            // printf("Set release day %s \n", value->u.string.ptr);
            strcpy( stack[antal]->feed_release_date , value->u.string.ptr );
          }
          tidal_process_releasedate=false;
        }       
        // Set artist id
        // we do not need artist id
        if (tidal_process_artist) {
          if (stack[antal]) {
            // printf("Set artist %s \n", value->u.string.ptr);
            // strcpy( stack[antal]->feed_artist , value->u.string.ptr );
          }
          tidal_process_artist=false;
        }
        // get tracknr
        if (tidal_process_track_nr) {
          tidal_process_track_nr=false;
        }
        */
        break;
      case json_boolean:
        //if (debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
        break;
    }
    if (antal>0) antalplaylists=antal-1; else antalplaylists=0;
}




// ****************************************************************************************
//
// Tidal search online and process file
//
// ****************************************************************************************

int tidal_class::opdatere_tidal_oversigt_searchtxt_online(char *keybuffer,int type) {
  char *database = (char *) "mythtvcontroller";
  std::string userfilename;
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  int error;
  char post_playlist_data[4096];
  int httpCode=0;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=NULL;
  std::string searchbuffer;
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  long file_size;  
  char* file_contents;
  json_char* json;
  json_value* value;
  MYSQL *conn;
  MYSQL_RES *mysql_res;
  MYSQL_ROW mysql_row;
  search_loaded=false;
  /*
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://openapi.tidal.com/v2/searchresults/";
  url=url + keybuffer;
  // 1 = artist, 2 = track
  if (type==1) url=url + "&type=ARTISTS&offset=0&limit=100&countryCode=US&popularity=WORLDWIDE";
  else if (type==2) url=url + "&type=TRACKS&offset=0&limit=100&countryCode=US&popularity=WORLDWIDE";  
  else url=url + "&type=ARTISTS&offset=0&limit=100&countryCode=US&popularity=WORLDWIDE";
  userfilename = "tidal_search_result.json";
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if ((curl) && (strlen(auth_kode.c_str())>0)) {
    header = curl_slist_append(header, "accept: application/vnd.tidal.v1+json");
    header = curl_slist_append(header, auth_kode.c_str());
    header = curl_slist_append(header, "Content-Type: application/vnd.tidal.v1+json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // ask libcurl to use TLS version 1.3 or later
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_file_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    // we tell libcurl to follow redirection
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    userfile=fopen(userfilename.c_str(),"w");
    if (userfile) {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
      res = curl_easy_perform(curl);
      fclose(userfile);
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }
  */
  searchbuffer="";
  int n=0;
  while(n<strlen(keybuffer)) {
    if (keybuffer[n]!=' ') searchbuffer=searchbuffer+keybuffer[n];
    else searchbuffer=searchbuffer+"%20";
    n++;
  }
    // old url="curl -X GET 'https://openapi.tidal.com/search?query=";
  url="curl -X GET 'https://openapi.tidal.com/v2/searchresults/";
  url=url + searchbuffer;
  switch (type) {
            // albums
    case 0: url=url + "/relationships/albums?countryCode=US&include=albums' -H 'accept: application/vnd.tidal.v1+json' -H 'Authorization: Bearer " + tidaltoken + "' -H 'Content-Type: application/vnd.tidal.v1+json' > tidal_search_result.json";
            break;
            // artist
    case 1: url=url + "/relationships/artists?countryCode=US&include=artists'  -H 'accept: application/vnd.tidal.v1+json' -H 'Authorization: Bearer " + tidaltoken + "' -H 'Content-Type: application/vnd.tidal.v1+json' > tidal_search_result.json";
            break;
            // tracks
    case 2: url=url + "/relationships/tracks?countryCode=US&include=tracks'  -H 'accept: application/vnd.tidal.v1+json' -H 'Authorization: Bearer " + tidaltoken + "' -H 'Content-Type: application/vnd.tidal.v1+json' > tidal_search_result.json";
            break;
            // default albums
    default:url=url + "/relationships/albums?countryCode=US&include=albums'  -H 'accept: application/vnd.tidal.v1+json' -H 'Authorization: Bearer " + tidaltoken + "' -H 'Content-Type: application/vnd.tidal.v1+json' > tidal_search_result.json";
  }
  error=system(url.c_str());
  // if no error we have json file have the search result
  if (error==0) {
    stat("tidal_search_result.json", &filestatus);                                  // get file info
    file_size = filestatus.st_size;                                                 // get filesize
    if (file_size>0) {
      file_contents = (char*) malloc(filestatus.st_size);                           // get some work mem
      json_file = fopen("tidal_search_result.json", "r");
      if (json_file) {
        while(!(feof(json_file))) {
          fread(file_contents,file_size,1,json_file);
        }
        fclose(json_file);
      }
      json = (json_char*) file_contents;
      try {
        value = json_parse(file_contents,file_size);                                  // parser create value obj
        antal=-1;                                                                     // reset antal  
        antalplaylists=0;
        process_tidal_search_result(value, 0,0);                                      // process json to stack variable
        if (file_contents) free(file_contents);                                       // free memory again
        json_value_free(value);                                                       // json clean up
        texture_loaded = false;                                                       // set load flag

        // stack is ready
        // the array is ready
        /*
        conn=mysql_init(NULL);
        if (conn) {
          mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
          mysql_query(conn,"set NAMES 'utf8'");
          mysql_res = mysql_store_result(conn);        
          // create records if needed

          mysql_close(conn);
        }
        */
      } catch (...) {
        write_logfile(logfile,(char *) "error process json file.");
      }
    }
  } else write_logfile(logfile,(char *) "Curl error : https://openapi.tidal.com/search");
  search_loaded=true;
  return(httpCode);
}


// ****************************************************************************************
//
// play next song
//
// ****************************************************************************************

int tidal_class::tidal_next_play() {
  int result;  
  std::string logstring;
  if (tidal_aktiv_song_nr<tidal_aktiv_song_antal) {
    tidal_aktiv_song_nr++;
    tidal_pause_play();
    result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);  
    result = sndsystem->createSound(tidal_aktiv_song[tidal_aktiv_song_nr].playurl, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
    if (result==FMOD_OK) {
      if (sound) {
        result = sndsystem->playSound(sound,NULL, false, &channel);
        set_tidal_playing_flag(true);
      }
      if (sndsystem) channel->setVolume(configsoundvolume);                                        // set play volume from configfile          
      logstring="Tidal play song : ";
      logstring = logstring + tidal_aktiv_song[tidal_aktiv_song_nr].playurl;
      write_logfile(logfile,(char *) logstring.c_str());
      return(1);
    }
  }
  return(0);
}

// ****************************************************************************************
//
// play last songs
//
// ****************************************************************************************

int tidal_class::tidal_last_play() {
  int result;
  std::string logstring;
  if (tidal_aktiv_song_nr>0) {
    tidal_aktiv_song_nr--;
    tidal_pause_play();
    result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);  
    result = sndsystem->createSound(tidal_aktiv_song[tidal_aktiv_song_nr].playurl, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
    if (result==FMOD_OK) {
      if (sound) {
        result = sndsystem->playSound(sound,NULL, false, &channel);
        set_tidal_playing_flag(true);
      }
      if (sndsystem) channel->setVolume(configsoundvolume);                                        // set play volume from configfile          
      logstring="Tidal play song : ";
      logstring = logstring + tidal_aktiv_song[tidal_aktiv_song_nr].playurl;
      write_logfile(logfile,(char *) logstring.c_str());
    }
  }
  return(1);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

int tidal_class::tidal_resume_play() {
  return(1);
}

// ****************************************************************************************
//
// get name
//
// ****************************************************************************************



char *tidal_class::get_tidal_name(int nr) {
  if (nr < antal) return (stack[nr]->feed_name); else return (NULL);
}


// ****************************************************************************************
//
// return playlist id
//
// ****************************************************************************************


char *tidal_class::get_tidal_playlistid(int nr) {
  if (nr < antal) return (stack[nr]->playlistid); else return (NULL);
}

// ****************************************************************************************
//
// return play status
//
// ****************************************************************************************

bool tidal_class::get_tidal_playing_flag() {
  return(tidal_is_playing);
}

// ****************************************************************************************
//
// set play status
//
// ****************************************************************************************

void tidal_class::set_tidal_playing_flag(bool flag) {
  tidal_is_playing=flag;
}

// *********************************************************************************************************
//
// convert m4a files to flac then fmod can play it
//
// *********************************************************************************************************


void *thread_convert_m4a_to_flac(void *path) {
  DIR *dir;
  struct dirent *ent;
  std::string temptxt;
  std::string dirtocheck;
  std::string filename;
  std::size_t found;
  std::string checkfilexist_name;
  std::filesystem::path dir_files;
  std::vector<std::string> files;
  std::string playlist_id;
  std::string sql1;
  std::string dbfilename;
  char sql[8192];
  char *database = (char *) "mythtvcontroller";
  MYSQL *conn;
  MYSQL_RES *mysql_res;
  MYSQL_ROW mysql_row;
  int i = 0;
  int error;
  struct dirent *ep;
  FILE *fp;
  bool dir_found=false;
  conn=mysql_init(NULL);
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  dirtocheck=tidal_download_home + (char *) path;
  // first check if it have dirs that have m4a files check 4 dirs
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD1/";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ent = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ent->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD1/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD2/";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ent = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ent->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD2/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD3/";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ent = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ent->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD3/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD4/";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD4/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);    
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD5";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD5/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD6";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD6/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD7";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD7/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD8";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD8/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  temptxt = dirtocheck;
  temptxt = temptxt + "/CD9";
  if ((dir = opendir (temptxt.c_str())) != NULL) {
    while (((ep = readdir (dir)) != NULL)) {
      dir_found=true;
      filename=ep->d_name;
      if (filename.find(".m4a") != std::string::npos) {
        dir_files=filename;
        dbfilename= "CD9/" + dir_files.stem().string();
        files.push_back(dbfilename);
      }
    }
    if (dir) closedir (dir);
  }
  // normal no subdirs
  if (dir_found == false) {
    dir = opendir (dirtocheck.c_str());
    if (dir != NULL) {
      while (ep = readdir (dir)) {
        filename=ep->d_name;
        dir_files=filename;
        if (filename.find(".m4a") != std::string::npos) {
          dbfilename= dir_files.stem().string();
          files.push_back(dbfilename);  
        }
      }
      if (dir) closedir (dir);
    }
  }
  // get id from tidal-dl AlbumInfo.txt file
  dirtocheck = dirtocheck + "/AlbumInfo.txt";
  std::ifstream myfile (dirtocheck);
  if (myfile.is_open()) {
    getline(myfile,playlist_id);
    playlist_id.erase(0,14);                                                  // get playlistid
    myfile.close();
  }
  while(i<files.size()) {
    checkfilexist_name = tidal_download_home;
    checkfilexist_name = checkfilexist_name + (char *) path;
    checkfilexist_name = checkfilexist_name + "/";
    checkfilexist_name = checkfilexist_name + files[i];
    checkfilexist_name = checkfilexist_name + ".wav";
    if (!(file_exists(checkfilexist_name.c_str()))) {
      temptxt = "ffmpeg -y -i \"";
      temptxt = temptxt + tidal_download_home;
      temptxt = temptxt + (char *) path;
      temptxt = temptxt + "/";
      temptxt = temptxt + files[i];
      temptxt = temptxt + ".m4a\" \"";
      temptxt = temptxt + tidal_download_home;
      temptxt = temptxt + (char *) path;
      temptxt = temptxt + "/";      
      temptxt = temptxt + files[i];
      temptxt = temptxt + ".wav\"";
      error=system(temptxt.c_str());
      if (error!=0) {
        write_logfile(logfile,(char *) "Tidal error: can not convert m4a files to wav.");
      }
      // store in db
      if ((conn) && (error==0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        mysql_res = mysql_store_result(conn);
        checkfilexist_name = tidal_download_home;
        checkfilexist_name = checkfilexist_name + (char *) path;
        checkfilexist_name = checkfilexist_name + "/";
        checkfilexist_name = checkfilexist_name + files[i];
        checkfilexist_name = checkfilexist_name + ".wav";        
        // create records if needed
        sql1="insert into mythtvcontroller.tidalcontent (name, paththumb, playpath, playlistid, id) values (\"" + files[i] + "\",\"\",\"" + checkfilexist_name + "\"," + playlist_id + "," + "0)";
        // printf("sql %s \n",sql1.c_str());
        if (conn) {
          if (mysql_query(conn,sql1.c_str())!=0) {
            write_logfile(logfile,(char *) "mysql create table error. (tidalcontent)");
            printf("error sql %s \n",sql1.c_str());
            // exit by error
            exit(1);
          }
          mysql_res=mysql_store_result(conn);
        }
      }
    }
    i++;
  }
  if (conn) mysql_close(conn);
}




// ****************************************************************************************
//
// Play functios track.
//
// ****************************************************************************************

/*
eks

/bin/curl -v 'https://listen.tidal.com/us/album/315509960/track/315509961' -H 'accept: application/vnd.tidal.v1+json' -H 'Authorization: Bearer eyJraWQi....'
// sample
https://tidal.com/browse/playlist/1b087082-ab54-4e7d-a0d3-b1cf1cf18ebc

*/

// ************************************************************************************************
//
// start play first song in playlist and start one thread to download the rest and update the db. (in use)
//
// ************************************************************************************************

int tidal_class::tidal_play_now_album(char *playlist_song,int tidalknapnr,bool now) {
  DIR *dir;
  struct dirent *ent;
  std::string temptxt;
  std::string temptxt1;
  std::string convcommand;
  std::string songpathtoplay;
  std::string destfilename_to_check;
  std::string playfile;
  std::string dir_file_array[1024];
  std::string filename;
  std::string sqlstring;
  std::string ffilename;
  int error=0;
  std::string sysstring;
  int result;
  int entry = 0;
  char sql[4096];
  int recnr;
  char *database = (char *) "mythtvcontroller";  
  std::size_t found;
  int entryindir=0;
  MYSQL *conn;
  MYSQL_RES *mysql_res;
  MYSQL_ROW mysql_row;
  bool skip_download_of_files = false;
  bool swap=true;
  // download stuf to be played if not downloaded before
  // check if exist
  recnr=0;
  while (recnr<199) {
    strcpy( tidal_aktiv_song[recnr].artist_name, "" );
    strcpy( tidal_aktiv_song[recnr].cover_image_url, "" );
    strcpy( tidal_aktiv_song[recnr].song_name, "" );
    strcpy( tidal_aktiv_song[recnr].playlistid, "" );                                // playlistid
    strcpy( tidal_aktiv_song[recnr].playurl, "" );                                                     // play path
    tidal_aktiv_song[recnr].cover_image=0;
    recnr++;
  }
  conn=mysql_init(NULL);
  mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
  // check playlist exist
  sqlstring = "select name,playpath,playlistid from tidalcontent where playlistid like ";
  sqlstring = sqlstring + stack[tidalknapnr]->playlistid;
  if (mysql_query(conn,sqlstring.c_str())!=0) {
    write_logfile(logfile,(char *) "mysql select table error.");
    fprintf(stdout,"ERROR SQL : %s\n",sql);
  }
  mysql_res = mysql_store_result(conn);
  if (mysql_res) {
    while (((mysql_row = mysql_fetch_row(mysql_res)) != NULL)) {
      // if (file_exists(mysql_row[1])) 
      skip_download_of_files=true;
    }
  }
  // if exist in db load the files to play and be happy
  if (skip_download_of_files) {
    sqlstring = "select playlistname,paththumb,playlistid,release_date,artistid from tidalcontentplaylist where playlistid like ";
    sqlstring = sqlstring + stack[tidalknapnr]->playlistid;
    if (mysql_query(conn,sqlstring.c_str())!=0) {
      write_logfile(logfile,(char *) "mysql select table error.");
      fprintf(stdout,"ERROR SQL : %s\n",sql);
    }
    // set artist name + cover iamge path on disk
    mysql_res = mysql_store_result(conn);
    if (mysql_res) {
      while (((mysql_row = mysql_fetch_row(mysql_res)) != NULL)) {
        strcpy(tidal_aktiv_song[0].artist_name , mysql_row[4]);
        strcpy(tidal_aktiv_song[0].cover_image_url, mysql_row[1]);
      }
    }
    recnr=0;
    // get playlist from db
    // sqlstring = "select name,playpath,playlistid,playlistgfx from mythtvcontroller.tidalcontent where playlistid like ";
    sqlstring = "select name,playpath,tidalcontent.playlistid,tidalcontentplaylist.paththumb,release_date from mythtvcontroller.tidalcontent , mythtvcontroller.tidalcontentplaylist where tidalcontentplaylist.playlistid=tidalcontent.playlistid and tidalcontentplaylist.playlistid like ";
    sqlstring = sqlstring + stack[tidalknapnr]->playlistid;
    sqlstring = sqlstring + " order by name";
    if (mysql_query(conn,sqlstring.c_str())!=0) {
      write_logfile(logfile,(char *) "mysql select table error.");
      fprintf(stdout,"ERROR SQL : %s\n",sql);
    }
    // get playlist from db
    tidal_aktiv_song_antal=0;
    mysql_res = mysql_store_result(conn);
    if (mysql_res) {      
      while (((mysql_row = mysql_fetch_row(mysql_res)) != NULL)) {
        if (recnr>=0) {
          strcpy( tidal_aktiv_song[recnr].artist_name, tidal_aktiv_song[0].artist_name );                              // playlist name
          strcpy( tidal_aktiv_song[recnr].cover_image_url, mysql_row[3] );                                             //
          strcpy( tidal_aktiv_song[recnr].song_name, mysql_row[0] );                                                   //
          strcpy( tidal_aktiv_song[recnr].playlistid, mysql_row[2] );                                                  // playlistid
          strcpy( tidal_aktiv_song[recnr].playurl, mysql_row[1] );                                                     // play path
          strcpy( tidal_aktiv_song[recnr].release_date, mysql_row[4] );                                                // release date
        }
        recnr++;
        tidal_aktiv_song_antal++;
      }
      if (tidal_aktiv_song_antal>0) tidal_aktiv_song_antal--;
      tidal_aktiv_song_nr=0;
      if (sound) sound->release();                                                                                      // stop last played
      if ( file_exists(tidal_aktiv_song[0].playurl) == true ) {
        result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);  
        // start play first song
        result = sndsystem->createSound(tidal_aktiv_song[0].playurl, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
        if (result==FMOD_OK) {
          if (sound) result = sndsystem->playSound(sound,NULL, false, &channel);
          if (sndsystem) channel->setVolume(configsoundvolume);                                                         // set play volume from configfile
        }
      }
    }
  }
  // No not happy
  // we have to download all the songs
  //
  if ( skip_download_of_files == false ) {
    error = 0;
    temptxt = tidal_download_home;
    temptxt = temptxt + stack[tidalknapnr]->feed_showtxt;
    temptxt = temptxt + "/";
    if ((dir = opendir (temptxt.c_str())) != NULL) {
      while (((ent = readdir (dir)) != NULL) && (found==false)) {
        found = filename.find("m4a");
        if ( found ) {
          printf ("m4a file found %s. Skip downloading.\n", ent->d_name);
          skip_download_of_files = true;
        }
      }
    }
    if (!(skip_download_of_files)) {
      if  (file_exists("/bin/gnome-terminal")) {
        sysstring="/bin/gnome-terminal --wait -t 'tidal' -- bash -c '/usr/local/bin/tidal-dl -l https://listen.tidal.com/album/";
        sysstring = sysstring + playlist_song;
        sysstring = sysstring + "'";  
      } else {
        sysstring="/usr/local/bin/tidal-dl -l https://listen.tidal.com/album/";
        sysstring = sysstring + playlist_song;
      }
      error = system(sysstring.c_str());                                                                      // do it (download songs by tidal-dl)
      // error bits desc.
      // Bits 15-8 = Exit code.
      // Bit     7 = 1 if a core dump was produced.
      // Bits  6-0 = Signal number that killed the process.
      error = error / 256;                                      // get exit code
    }
    // then downloaded play the stuf
    // first set homedir from global var have the users homedir
    if ((sndsystem) && (error==0)) {
      // convert m4a files to wav if needed
      temptxt = tidal_download_home;
      temptxt = temptxt + stack[tidalknapnr]->feed_showtxt;
      temptxt = temptxt + "/";
      entry=0;
      // read dir and find *.m4a files.
      if ((dir = opendir (temptxt.c_str())) != NULL) {
        while (((ent = readdir (dir)) != NULL) && (entry<1000)) {
          printf ("%s\n", ent->d_name);
          ffilename=ent->d_name;
          if (ffilename.find(".m4a")!= std::string::npos) {
            dir_file_array[entry]=ent->d_name;
            entry++;
          }
        }
        if (dir) closedir (dir);
        // It must be dirs
        // open dir and read it.
        if ( entry == 0 ) {
          temptxt = tidal_download_home;
          temptxt = temptxt + stack[tidalknapnr]->feed_showtxt;
          temptxt = temptxt + "/CD1/";
          if ((dir = opendir (temptxt.c_str())) != NULL) {
            while (((ent = readdir (dir)) != NULL) && (entry<1000)) {
              ffilename=ent->d_name;
              if (ffilename.find(".m4a") != std::string::npos) {
                dir_file_array[entry]="CD1/";
                dir_file_array[entry]=dir_file_array[entry] + ent->d_name;
                entry++;
              }
            }
            if (dir) closedir (dir);
          }
        }
      } else {
        // could not open directory
        perror ("could not open directory");
        return EXIT_FAILURE;
      }
      // sort playlist files
      swap = true;
      while(swap) {
        int n = 0;
        swap=false;      
        while(n<entry) {
          if ((strcmp(dir_file_array[n].c_str(),dir_file_array[n+1].c_str())>0) && ((n+1)<entry)) {
            swap=true;
            std::string tmparray;
            tmparray = dir_file_array[n];
            dir_file_array[n] = dir_file_array[n+1];
            dir_file_array[n+1] = tmparray;
          }
          n++;
        }
      }
      temptxt1 = tidal_download_home;
      temptxt1 = temptxt1 + stack[tidalknapnr]->feed_showtxt;
      temptxt1 = temptxt1 + "/";
      temptxt1 = temptxt1 + dir_file_array[0];                    // add song nr 1
      // create wav file if not exist by ffmpeg
      if ( file_exists(temptxt1.c_str()) == true ) {
        convcommand = "ffmpeg -y -i \"";
        convcommand = convcommand + temptxt1;
        convcommand = convcommand + "\" \"";
        convcommand = convcommand + temptxt1;
        convcommand = convcommand + ".wav\"";
        error=system(convcommand.c_str());
      }
      // build file path to play.
      playfile = tidal_download_home;
      playfile = playfile + stack[tidalknapnr]->feed_showtxt;
      playfile = playfile + "/";
      playfile = playfile + dir_file_array[0];
      playfile = playfile + ".wav";   
      if ( file_exists(playfile.c_str()) == true ) {
        result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);  
        // start play first song
        result = sndsystem->createSound(playfile.c_str(), FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
        if ( result == FMOD_OK ) {
          if (sound) result = sndsystem->playSound(sound,NULL, false, &channel);
          if (sndsystem) channel->setVolume(configsoundvolume);                                        // set play volume from configfile          
        }
        // convert the rest of the m4a files we have downloed to be able to play it in fmod
        pthread_t loaderthread; // thread_convert_m4a_to_flac() used to convert m4a files
        int rc2 = pthread_create( &loaderthread , NULL , thread_convert_m4a_to_flac , (void *) stack[tidalknapnr]->feed_showtxt);
        if (rc2) {
          fprintf(stderr,"ERROR webupdate_loader_tidal function\nreturn code from pthread_create() is %d\n", rc2);
          exit(-1);
        }
        // wait for thread is done downloading songs
        pthread_join(loaderthread,NULL);
        conn=mysql_init(NULL);
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        // hent song names from db
        std::string sql1;
        sql1 = fmt::v8::format("select name, playpath from mythtvcontroller.tidalcontent where playlistid like '{}' order by name",stack[tidalknapnr]->playlistid);
        if (mysql_query(conn,sql1.c_str()) != 0) {
          write_logfile(logfile,(char *) "mysql select table error.");
          fprintf(stdout,"ERROR SQL : %s\n",sql);
        }
        // get antal record and update song name + play url
        recnr=0;
        tidal_aktiv_song_antal = 0;
        mysql_res = mysql_store_result(conn);
        if (mysql_res) {
          while ((( mysql_row = mysql_fetch_row(mysql_res)) != NULL )) {
            strcpy( tidal_aktiv_song[recnr].song_name, mysql_row[0] );
            strcpy( tidal_aktiv_song[recnr].playurl, mysql_row[1] );
            recnr++;
          }
          if (recnr>0) tidal_aktiv_song_antal = recnr-1;                                                   // set antal songs in playlist
        }
        // hent artist name from db
        sql1 = fmt::v8::format("select playlistname,tidalcontentartist.artistname,release_date from tidalcontentplaylist left join tidalcontentartist on tidalcontentplaylist.artistid=tidalcontentartist.artistid where playlistid like '{}'",stack[tidalknapnr]->playlistid);
        if (mysql_query(conn,sql1.c_str())!=0) {
          write_logfile(logfile,(char *) "mysql create table error.");
          fprintf(stdout,"ERROR SQL : %s\n",sql);
        }
        mysql_res = mysql_store_result(conn);
        if (mysql_res) {
          recnr=0;
          while ((( mysql_row = mysql_fetch_row(mysql_res)) != NULL )) {
            if (strlen(mysql_row[0])>0) strcpy( tidal_aktiv_song[0].album_name,mysql_row[0] );
            if (strlen(mysql_row[1])>0) strcpy( tidal_aktiv_song[0].artist_name,mysql_row[1] );
            strcpy( tidal_aktiv_song[0].release_date,mysql_row[2] );
            strcpy( tidal_playlistname,mysql_row[0] );                                            // set playlist name to show
          }
          // set the rest of the songs artist/album info
          recnr=1;
          while(recnr<tidal_aktiv_song_antal) {
            strcpy( tidal_aktiv_song[recnr].album_name, tidal_aktiv_song[0].album_name );
            strcpy( tidal_aktiv_song[recnr].artist_name, tidal_aktiv_song[0].artist_name );
            strcpy( tidal_aktiv_song[recnr].release_date, tidal_aktiv_song[0].release_date );
            recnr++;
          }
          // set icon texture
          if ( stack[tidalknapnr]->textureId ) {
            tidal_aktiv_song[0].cover_image = stack[tidalknapnr]->textureId;
            aktiv_song_tidal_icon = stack[tidalknapnr]->textureId;
          }
          tidal_aktiv_song_nr=0;
        }  
      }
    }    
  }
  if (conn) mysql_close(conn);
  if ((tidal_aktiv_song_antal) && (error==0)) return(tidal_aktiv_song_antal); else return(0);
}


// ****************************************************************************************
//
// play song (in use).
//
// ****************************************************************************************

int tidal_class::tidal_play_now_song(char *playlist_song,int tidalknapnr,bool now) { 
  std::string songpath;
  int result;  
  songpath=tidal_aktiv_song[tidalknapnr].playurl;
  if (sndsystem) {
    result = sndsystem->setStreamBufferSize(fmodbuffersize, FMOD_TIMEUNIT_RAWBYTES);  
    result = sndsystem->createSound(playlist_song, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM  , 0, &sound);
    if (result==FMOD_OK) {
      if (sound) {
        result = sndsystem->playSound(sound,NULL, false, &channel);                       // start play
        set_tidal_playing_flag(true);
      }
      if (sndsystem) channel->setVolume(configsoundvolume);                                        // set play volume from configfile
    }
    strcpy(tidal_aktiv_song[0].song_name,stack[tidalknapnr]->feed_showtxt);
  }
  if (result!=FMOD_OK) return(1); else return(0);
}



// ****************************************************************************************
//
// get active play device name
//
// ****************************************************************************************

char *tidal_class::get_active_tidal_device_name() {
  return(tidal_device[active_tidal_device].name);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


int tidal_class::tidal_do_we_play() {
  if (sound) return(1); else return(0);
}

// ****************************************************************************************
//
// Stop play
//
// ****************************************************************************************


int tidal_class::tidal_pause_play() {
  sound->release();                                                                       // stop last playing song 
  set_tidal_playing_flag(false);
  return(1);
}

// ****************************************************************************************
//
// load icons texture
//
// ****************************************************************************************

int tidal_class::load_tidal_iconoversigt() {
  int nr=0;
  int loadstatus;
  char *imagenamepointer;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  this->gfx_loaded=false;                                                           // set loaded flag to false
  while(nr<=streamantal()) {
    if ((stack[nr]) && (strcmp(stack[nr]->feed_gfx_url,"")!=0)) {
      if (stack[nr]->textureId==0) {
        // if url
        if (strncmp("http",stack[nr]->feed_gfx_url,4)==0) {
          imagenamepointer = strrchr(stack[nr]->feed_gfx_url,'\/');
          if ( imagenamepointer ) {
            if (strlen(imagenamepointer)<1990) {
              strcpy(tmpfilename,localuserhomedir);
              strcat(tmpfilename,"/tidal_gfx/");
              strcat(tmpfilename,imagenamepointer+1);
              strcat(tmpfilename,".jpg");
              stack[nr]->textureId=loadTexture (tmpfilename);
            }
          }
        } else {
          // else load normal from disk
          if ((stack[nr]->textureId==NULL) && (stack[nr]->feed_gfx_url)) {
            if (strlen( stack[nr]->feed_gfx_url)>0) stack[nr]->textureId=loadTexture (stack[nr]->feed_gfx_url);          // load texture
          }
        }
      }
    }
    nr++;
  }
  // set loaded flag in class
  if (nr>0) this->gfx_loaded = true; else this->gfx_loaded = false;
  gfx_loaded = true;
  return(1);
}




// ****************************************************************************************
//
// WORKS OK
// gettoken from Tidal
//
// ****************************************************************************************


int tidal_class::gettoken() {

  std::size_t foundpos;
  char auth_kode[1024];
  std::string response_string;
  std::string response_val;
  int httpCode;
  CURLcode res;
  json_char *json;
  json_value *value;
  struct curl_slist *chunk = NULL;
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,tidaltoken);
  CURL *curl = curl_easy_init();
  printf("Get token now\n\n");
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://cdn.jsdelivr.net/gh/yaronzz/CDN@latest/app/tidal/tokens.json");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);
    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, auth_kode);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    if (httpCode == 200) {
      printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      process_value_token(value, 0,0);                                                        // fill tokenid1+2
      json_value_free(value);                                                                 // json clean up
    }
  }
  return(httpCode);
}



// ****************************************************************************************
//
// New get users play and favorite playlists
// do not work for now
//
// ****************************************************************************************

int tidal_class::get_users_playlist_plus_favorite(bool cleandb) {  
  std::size_t foundpos;
  char auth_kode[1024];
  std::string response_string;
  std::string response_val;
  std::string url;
  int httpCode;
  CURLcode res;
  json_char *json;
  json_value *value;
  struct curl_slist *chunk = NULL;
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,tidaltoken);
  CURL *curl = curl_easy_init();

  // userid="12";
  url = "ttps://openapi.tidal.com/v2/playlists/me?include=items";
  /*
  url = "https://api.tidal.com/v1/users/";
  url = url + userid;
  url = url + "/playlistsAndFavoritePlaylists?countryCode=US";
  */
  printf("Get users playlist and favorite playlist's \n\n");
  write_logfile(logfile,(char *) "tidal Get users playlist and favorite playlist");
  if (curl) {    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_COOKIE, "cookies.txt");
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, auth_kode);                                  // header
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    if (httpCode == 200) {
      printf("***** %s ***** \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      process_value_token(value, 0,0);                                                        // fill tokenid1+2
      json_value_free(value);                                                                 // json clean up
    } else {
      printf("***** ERROR ***** \n");
      printf("***** %s ***** \n", response_string.c_str());
      write_logfile(logfile,(char *) "Tidal error get users playlists and favorite");
    }
  }
  return(httpCode);
}




// ****************************************************************************************
//
// NOT in use.
//
// ****************************************************************************************


int tidal_class::tidal_refresh_token() {
  std::size_t foundpos;
  char auth_kode[1024];
  std::string response_string;
  std::string response_val;
  int httpCode=0;
  CURLcode res;
  struct curl_slist *chunk = NULL;
  char doget[2048];
  char data[4096];
  char call[4096];
  CURL *curl;
  FILE *tokenfil;
  char *base64_code;
  char newtoken[1024];
  char post_playlist_data[1024];
  char errbuf[CURL_ERROR_SIZE];
  strcpy(newtoken,"");
  curl = curl_easy_init();
  if ((curl) && (strcmp(tidaltoken_refresh,"")!=0)) {
    // add userinfo + basic auth
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, tidal_client_id);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, tidal_secret_id);
    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2/token");
    // ask libcurl to use TLS version 1.3 or later
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_3);    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //chunk = curl_slist_append(chunk, base64_code);
    errbuf[0] = 0;
    // set type post
    //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",spotifytoken_refresh);
    sprintf(post_playlist_data,"grant_type=refresh_token&refresh_token=%s&client_id=%s",tidaltoken_refresh,"");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      size_t len = strlen(errbuf);
      fprintf(stderr, "\nlibcurl: (%d) ", res);
      if(len)
        fprintf(stderr, "%s%s", errbuf,((errbuf[len - 1] != '\n') ? "\n" : ""));
      else
        fprintf(stderr, "%s\n", curl_easy_strerror(res));
    }
    // get respons code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    if (httpCode == 200) {
      write_logfile(logfile,(char *) "Tidal new token.");
      printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      //value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      //process_value_playinfo(value, 0,0);                                                     // fill play info
      if ((response_string.size()>12) && (response_string.compare(2,12,"access_token")==0)) {
        strncpy(newtoken,response_string.c_str()+17,180);
        newtoken[181]='\0';
        write_logfile(logfile,(char *) "Spotify token valid.");
        strcpy(tidaltoken,newtoken);                                         // update spotify token
      }
    } else {
      fprintf(stderr,"Error code httpCode %d \n. ",httpCode);
      fprintf(stderr,"Curl error: %s\n", curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
  }
  return(httpCode);
}



// ****************************************************************************************
//
// Show tidal view
//
// ****************************************************************************************

void tidal_class::show_tidal_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected) {
  int j,ii,k,pos;
  int buttonsize=200;                                                         // button size
  float buttonsizey=180.0f;                                                   // button size
  float yof=orgwinsizey-(buttonsizey);                                        // start ypos
  float xof=0.0f;
  int lstreamoversigt_antal=8*5;
  int i=0;                                                                    // data ofset in stack array
  int bonline=8;                                                              // antal pr linie
  float boffset;
  char gfxfilename[200];
  char downloadfilename[200];
  char downloadfilenamelong[1024];
  char *gfxshortnamepointer;
  char gfxshortname[200];
  char temptxt[200];
  std::string temptxt1;
  char word[200];
  static char downloadfilename_last[1024];
  int antal_loaded=0;
  static int stream_oversigt_loaded_done=0;
  GLuint texture;
  static GLuint last_texture;
  char *base,*right_margin;
  int length,width;
  int pline=0;
  static bool timefirsttime=false;
  struct timeval lasttime;
  struct timeval tim;
  static bool gfx_loaded=false;
  static int gfx_loadnr=0;
  if (timefirsttime==false) {
    timefirsttime=true;
    tim.tv_usec=0;
    lasttime.tv_usec=0;
  }
  // last loaded filename
  if (tidal_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
  // load icons
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTranslatef(0,0,0.0f);
  // do tidal works ?
  if (strcmp(tidaltoken,"")) {
    while((i<lstreamoversigt_antal) && (i+sofset<antalplaylists) && (stack[i+sofset]!=NULL)) {
      // load texture if not loaded
      if (this->texture_loaded==false) {
        if (stack[i+sofset]->textureId==0) {        
          if (file_exists(stack[i+sofset]->feed_gfx_url)) {
            if (check_zerro_bytes_file(stack[i+sofset]->feed_gfx_url)) {
              stack[i+sofset]->textureId=loadTexture ((char *) stack[i+sofset]->feed_gfx_url);
            }
          }
        }
        gfx_loadnr++;
        // stop loading more if all is loaded
        // if (gfx_loadnr>=antalplaylists) texture_loaded=true;
      }
      if (((i % bonline)==0) && (i>0)) {
        yof=yof-(buttonsizey+20);
        xof=0;
      }
      if (i+1==(int) stream_key_selected) {
        buttonsizey=200.0f;
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      } else {
        buttonsizey=180.0f;
        glColor4f(0.8f, 0.8f, 0.8f,1.0f);
      }
      // stream icon
      glPushMatrix();
      if (anim_angle>360) {
        anim_angle=180.0f;
        anim_viewer=false;
      } else {
        if (anim_viewer) {
          gettimeofday(&tim,NULL);
          if (lasttime.tv_usec==0) {
            lasttime.tv_usec=tim.tv_usec;
          } else {
            if (tim.tv_usec>(lasttime.tv_usec+100)) {
              anim_angle+=0.50;
            } else {
              anim_viewer=false;
              anim_angle=0.0f;
            }
          }
        } else anim_angle=0.0f;
      }
      glTranslatef(xof+20+(buttonsize/2),yof-10,0);
      glRotatef(anim_angle,0.0f,1.0f,0.0f);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,spotify_icon_border);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize/2), 10, 0.0);
      glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize/2),buttonsizey-20, 0.0);
      glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize/2), buttonsizey-20 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize/2), 10 , 0.0);
      glEnd();                
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if (stack[i+sofset]->textureId) {
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,_textureIdback);
          } else glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        } else {
          glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        }        
      } else {
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,_textureIdback);            
          } else glBindTexture(GL_TEXTURE_2D,normal_icon);
        } else {
          if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,normal_icon);
        }
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      if (tema==5) {
        glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize/2), 10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize/2),buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize/2), buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize/2), 10 , 0.0);
      } else {
        glTexCoord2f(0, 0); glVertex3f( 12-(buttonsize/2), 12, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 12-(buttonsize/2),buttonsizey-22, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsize-12-(buttonsize/2), buttonsizey-22 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsize-12-(buttonsize/2), 12 , 0.0);
      }
      glEnd();        
      glPopMatrix();
      drawLinesOfText(stack[i+sofset]->feed_showtxt,xof+30,yof-20,0.38f,20,2,1,true);

      // next button
      i++;
      xof+=(buttonsize+10);
    }
  } else {
    // No tidal account
    glTranslatef(xof+20+(buttonsize/2),yof-10,0);
    glRotatef(anim_angle,0.0f,1.0f,0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,spotify_icon_border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(100+i+sofset);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize*4), 10, 0.0);
    glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize*4),buttonsizey*4, 0.0);
    glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize*4), buttonsizey*4 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize*4), 10 , 0.0);
    glEnd();
    glcRenderString("No tidal account enabled.");
  }
}



// ****************************************************************************************
//
// Show search tidal view
//
// ****************************************************************************************

void tidal_class::show_tidal_search_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring) {
  int j,ii,k,pos;
  int buttonsize=200;                                                         // button size
  float buttonsizey=180.0f;                                                   // button size
  float yof=orgwinsizey-(buttonsizey*2);                                      // start ypos
  float xof=0.0f;
  float yof_top=orgwinsizey-(buttonsizey*1)+20;                               // start ypos
  float xof_top=((orgwinsizex-buttonsize)/2)-(1200/2);
  int lstreamoversigt_antal=8*5;
  int i=0;                                                                    // data ofset in stack array
  int bonline=8;                                                              // antal pr linie
  float boffset;
  char gfxfilename[200];
  char downloadfilename[200];
  char downloadfilenamelong[1024];
  char *gfxshortnamepointer;
  char gfxshortname[200];
  char temptxt[200];
  char word[200];
  static char downloadfilename_last[1024];
  int antal_loaded=0;
  static int stream_oversigt_loaded_done=0;
  GLuint texture;
  static GLuint last_texture;
  // static bool texture_loaded=false;
  char *base,*right_margin;
  int length,width;
  int pline=0;
  static bool timefirsttime=false;
  struct timeval lasttime;
  struct timeval tim;
  static bool cursor=true;
  static time_t rawtime;
  static time_t last_rawtime=0;
  static bool gfx_loaded=false;
  static int gfx_loadnr=0;
  static bool doneloadsearch=false;
  rawtime=time(NULL);                                                         // hent now time
  if (timefirsttime==false) {
    timefirsttime=true;
    tim.tv_usec=0;
    lasttime.tv_usec=0;
  }  
  if (last_rawtime==0) {
    last_rawtime=rawtime;
  }
  if (rawtime>(last_rawtime+1)) {
    cursor=!cursor;
    last_rawtime=rawtime;
  }
  // last loaded filename
  if (tidal_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
  // top
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // type of search
  switch (searchtype) {
    case 0: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_artist);
            break;
    case 1: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_album);
            break;
    case 2: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_artist);
            break;
    case 3: glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_track);
            break;
    default:glBindTexture(GL_TEXTURE_2D,tidal_big_search_bar_artist);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(0);
  glBegin(GL_QUADS); 
  glTexCoord2f(0, 0); glVertex3f( xof_top+10, yof_top+10, 0.0);
  glTexCoord2f(0, 1); glVertex3f( xof_top+10,yof_top+buttonsizey-20, 0.0);
  glTexCoord2f(1, 1); glVertex3f( xof_top+1200-10, yof_top+buttonsizey-20 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( xof_top+1200-10, yof_top+10 , 0.0);
  glEnd();

  // show tidal search string
  glPushMatrix();
  glTranslatef(xof+210+(buttonsize/2),yof+240,0);
  glDisable(GL_TEXTURE_2D);
  glScalef(120, 120, 1.0);
  if (strcmp(searchstring,"")!=0) {
    glcRenderString(searchstring);
  }
  if (cursor) glcRenderString("_"); else glcRenderString(" ");
  glPopMatrix();

  // draw icons
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTranslatef(0,0,0.0f);
  // do tidal works ?
  if (strcmp(tidaltoken,"")) {
    // printf(" doneloadsearch loaded flag = %d  search_loaded = %d \n",doneloadsearch,search_loaded);
    if (search_loaded==false) doneloadsearch=false;
    if ((search_loaded==true) && (doneloadsearch==false)) {
      doneloadsearch=true;
      load_tidal_iconoversigt();
    }
    while((i<lstreamoversigt_antal) && (i+sofset<antalplaylists) && (stack[i+sofset]!=NULL)) {
      // load texture
      // printf("nr %d antal %d \n",gfx_loadnr,antalplaylists);
      if (this->texture_loaded==false) {
        if (stack[i+sofset]->textureId==0) {        
          if (file_exists(stack[i+sofset]->feed_gfx_url)) {
            stack[i+sofset]->textureId=loadTexture ((char *) stack[i+sofset]->feed_gfx_url);
          }
        }
        gfx_loadnr++;
        // stop loading more if all is loaded
        if (gfx_loadnr>=antalplaylists) texture_loaded=true;
      }
      if (((i % bonline)==0) && (i>0)) {
        yof=yof-(buttonsizey+20);
        xof=0;
      }
      if (i+1==(int) stream_key_selected) {
        buttonsizey=200.0f;
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      } else {
        buttonsizey=180.0f;
        glColor4f(0.8f, 0.8f, 0.8f,1.0f);
      }
      // stream icon
      glPushMatrix();
      if (anim_angle>360) {
        anim_angle=180.0f;
        anim_viewer=false;
      } else {
        if (anim_viewer) {
          gettimeofday(&tim,NULL);
          if (lasttime.tv_usec==0) {
            lasttime.tv_usec=tim.tv_usec;
          } else {
            if (tim.tv_usec>(lasttime.tv_usec+100)) {
              anim_angle+=0.50;
            } else {
              anim_viewer=false;
              anim_angle=0.0f;
            }
          }
        } else anim_angle=0.0f;
      }
      // Draw icon
      glTranslatef(xof+20+(buttonsize/2),yof-10,0);
      glRotatef(anim_angle,0.0f,1.0f,0.0f);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,spotify_icon_border);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize/2), 10, 0.0);
      glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize/2),buttonsizey-20, 0.0);
      glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize/2), buttonsizey-20 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize/2), 10 , 0.0);
      glEnd();                
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if (stack[i+sofset]->textureId) {
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,_textureIdback);
          } else glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        } else {
          if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        }
      } else {
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,_textureIdback);            
          } else glBindTexture(GL_TEXTURE_2D,normal_icon);
        } else {
          if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,normal_icon);
        }
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(100+i+sofset);
      glBegin(GL_QUADS);
      if (tema==5) {
        glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize/2), 10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize/2),buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize/2), buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize/2), 10 , 0.0);
      } else {
        glTexCoord2f(0, 0); glVertex3f( 12-(buttonsize/2), 12, 0.0);
        glTexCoord2f(0, 1); glVertex3f( 12-(buttonsize/2),buttonsizey-22, 0.0);
        glTexCoord2f(1, 1); glVertex3f( buttonsize-12-(buttonsize/2), buttonsizey-22 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( buttonsize-12-(buttonsize/2), 12 , 0.0);
      }
      glEnd();        
      glPopMatrix();
      // show text of element
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      drawLinesOfText(temptxt, xof+20, yof-20, 0.4f , 16, 2 , 1,true);
      // next button
      i++;
      xof+=(buttonsize+10);
    }
  } else {
    // No tidal account
    glTranslatef(xof+20+(buttonsize/2),yof-10,0);
    glRotatef(anim_angle,0.0f,1.0f,0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,spotify_icon_border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(100+i+sofset);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( 10-(buttonsize*4), 10, 0.0);
    glTexCoord2f(0, 1); glVertex3f( 10-(buttonsize*4),buttonsizey*4, 0.0);
    glTexCoord2f(1, 1); glVertex3f( buttonsize-10-(buttonsize*4), buttonsizey*4 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( buttonsize-10-(buttonsize*4), 10 , 0.0);
    glEnd();
    glcRenderString("No tidal account enabled.");
    // drawText("No tidal account enabled.", xof+20+(buttonsize/2),yof-10, 0.4f,1);
  }
}
