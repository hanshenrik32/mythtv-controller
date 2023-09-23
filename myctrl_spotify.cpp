//#ifdef ENABLE_TIDAL

//
// Spotify settings/loaders
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <mysql.h>
#include <GL/glc.h>
#include <pthread.h>                   // multi thread support
#include <libxml/parser.h>
#include <sys/stat.h>
#include <time.h>
#include <curl/curl.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string>
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
#include "myctrl_spotify.h"

// web port
static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

const int spotify_pathlength=80;
const int spotify_namelength=80;
const int spotify_desclength=2000;
const int feed_url=2000;


const char *spotify_json_path = "spotify_json/";
const char *spotify_gfx_path = "spotify_gfx/";


size_t curl_writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}


// flags
bool process_playinfo_tracks=false;
bool process_playinfo_playlist=false;
bool process_playinfo_songs=false;
bool process_playinfo_href=false;
bool process_playinfo_description=false;
bool process_playinfo_image=false;
bool process_playinfo_name=false;
bool process_playinfo_items=false;
bool process_playinfo_artist=false;
bool process_playinfo_track_nr=false;
bool process_playinfo_date=false;
bool process_playinfo_progress_ms=false;
bool process_playinfo_duration_ms=false;
bool playinfo_json_debug=false;


// ****************************************************************************************
//
// text render is glcRenderString for freetype font support
//
// ****************************************************************************************
extern bool global_use_spotify_local_player;
// extern int spotifyknapnr;
extern int spotify_select_iconnr;
extern spotify_class spotify_oversigt;
extern GLuint _texturemovieinfobox;
extern GLuint big_search_bar_playlist;                    // big search bar used by sporify search
extern GLuint big_search_bar_track;                    // big search bar used by sporify search
extern GLuint big_search_bar_albumm;                    // big search bar used by sporify search
extern GLuint big_search_bar_artist;                    // big search bar used by sporify search
extern char *keybuffer;
extern int keybufferindex;
extern bool do_select_device_to_play;
extern GLuint mobileplayer_icon;
extern GLuint pcplayer_icon;
extern GLuint unknownplayer_icon;
extern GLuint spotify_pil;
extern float configdefaultstreamfontsize;
extern int tema;
extern char *dbname;                                           // internal database name in mysql (music,movie,radio)
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern int configmythtvver;
extern int screen_size;
extern int screensizey;
extern int screeny;
extern GLuint spotify_askplay;                                  // ask open icon
extern GLuint spotify_askopen;                                  // ask play icon
extern GLuint setuprssback;
extern GLuint _textureclose;
extern GLuint setupkeysbar1;
extern int debugmode;
extern unsigned int musicoversigt_antal;                        //
extern int do_stream_icon_anim_icon_ofset;                      //
extern GLuint radiooptions,radiooptionsmask;			              //
extern GLuint _textureIdback;  					                        // back icon
extern GLuint newstuf_icon;                                     //
extern int fonttype;
extern fontctrl aktivfont;
extern int orgwinsizey,orgwinsizex;
extern int _sangley;
extern int do_show_setup_select_linie;
extern GLuint _textureIdloading,_textureIdloading1;

extern GLuint spotify_icon_border;
// stream mask
extern GLuint onlinestreammask;
extern GLuint onlinestreammaskicon;		// icon mask on web icon
extern spotify_class streamoversigt;
extern GLint cur_avail_mem_kb;
extern bool stream_loadergfx_started;
extern bool stream_loadergfx_started_done;
extern bool stream_loadergfx_started_break;
extern char localuserhomedir[4096];                                    // user homedir set in main

//extern bool spotify_oversigt_loaded_begin;


// ****************************************************************************************
//
// web server handler (internal function)
//
// ****************************************************************************************

static void spotify_server_ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  const char *p;
  const char *pspace;
  int n=0;
  char user_token[1024];
  char sql[2048];
  struct http_message *hm = (struct http_message *) ev_data;
  struct mg_serve_http_opts opts;
  int curl_error;
  char* file_contents=NULL;
  size_t len;
  int error;
  FILE *tokenfile;
  char *base64_code;
  char data[512];
  char token_string[512];
  char token_refresh[512];
  unsigned int codel;
  strcpy(data,spotify_oversigt.spotify_client_id);
  strcat(data,":");
  strcat(data,spotify_oversigt.spotify_secret_id);
  char sed[]="cat spotify_access_token.txt | grep -Po '\"\\K[^:,\"}]*' | grep -Ev 'access_token|token_type|Bearer|expires_in|refresh_token|scope' > spotify_access_token2.txt";
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      // from spotify servers
      // is callback call from by login page index.html or spotify_index.html (login/index.html have the callback url as callback from spotify )
      if (mg_strncmp( hm->uri,mg_mk_str_n("/callback",9),9) == 0) {
        if (debugmode) fprintf(stdout,"Got reply server : %s \n", (mg_str) hm->uri);
        // get code pointer ( uri.p = pointer to url )
        p = strstr( hm->uri.p , "code="); // mg_mk_str_n("code=",5));
        // get sptify code from server
        if (p) {
          pspace=strchr(p,' ');
          if (pspace) {
            codel=(pspace-p);
            strncpy(user_token,p+5,pspace-p);                                         // copy code from return url from server
            *(user_token+(pspace-p))='\0';
          }
          user_token[codel-4]='\0';
        }
        snprintf(sql,sizeof(sql),"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code,user_token,spotify_oversigt.spotify_client_id,spotify_oversigt.spotify_secret_id);
        //printf("sql curl : %s \n ",sql);
        curl_error=system(sql);
        if (curl_error==0) {
          curl_error=system(sed);
          if (curl_error==0) {
            write_logfile((char *) "******** Got spotify token ********");
          } else {
            write_logfile((char *) "******** No spotify token ********");
          }
          if (curl_error==0) {
            tokenfile=fopen("spotify_access_token2.txt","r");
            error=getline(&file_contents,&len,tokenfile);
            strcpy(token_string,file_contents);
            token_string[strlen(token_string)-1]='\0';
            error=getline(&file_contents,&len,tokenfile);
            strcpy(token_refresh,file_contents);
            token_refresh[strlen(token_refresh)-1]='\0';
  //          printf("token     %s\n",token_string);
  //          printf("ref token %s\n",token_refresh);
            spotify_oversigt.spotify_set_token(token_string,token_refresh);
            fclose(tokenfile);
            free(file_contents);
            if (strcmp(token_string,"")!=0) {
              spotify_oversigt.spotify_get_user_id();                                   // get user id
              spotify_oversigt.active_spotify_device=spotify_oversigt.spotify_get_available_devices();
              // set default spotify device if none
              if (spotify_oversigt.active_spotify_device==-1) spotify_oversigt.active_spotify_device=0;
            }
          }
        }
        //c->flags |= MG_F_SEND_AND_CLOSE;
        mg_serve_http(c, (struct http_message *) ev_data, s_http_server_opts);  /* Serve static content */
      } else {
        // else show normal indhold
        memset(&opts, 0, sizeof(opts));
        opts.document_root = ".";       // Serve files from the current directory
        mg_serve_http(c, (struct http_message *) ev_data, s_http_server_opts);
      }
      // We have received an HTTP request. Parsed request is contained in `hm`.
      // Send HTTP reply to the client which shows full original request.
      //mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
      //mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
      break;
    case MG_EV_HTTP_REPLY:
      write_logfile((char *) "Other CALL BACK server reply");
      c->flags |= MG_F_CLOSE_IMMEDIATELY;
      fwrite(hm->body.p, 1, hm->body.len, stdout);
      putchar('\n');
      break;
    case MG_EV_CLOSE:
      fprintf(stdout,"Server closed connection\n");
  }
}


// ****************************************************************************************
//
// client handler
// get JSON return from spotify
//
// ****************************************************************************************

static int s_exit_flag = 0;
bool debug_json=false;


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  int connect_status;
  switch (ev) {
      case MG_EV_CONNECT:
        connect_status = *(int *) ev_data;
        if (connect_status != 0) {
          fprintf(stderr,"Error connecting %s\n", strerror(connect_status));
          s_exit_flag = 1;
        }
        break;
      case MG_EV_HTTP_REPLY:
        fwrite(hm->message.p, 1, (int)hm->message.len, stdout);
        fprintf(stdout,"Got reply client :\n%.*s\n", (int) hm->body.len, hm->body.p);
        fprintf(stdout,"***************************************** CALL BACK **************************************************");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        s_exit_flag = 1;
        break;
      case MG_EV_CLOSE:
        if (s_exit_flag == 0) {
          write_logfile((char *) "Server closed connection.");
          fprintf(stdout,"Server closed connection\n");
          s_exit_flag = 1;
        };
        break;
      default:
        break;
    }
}


// ****************************************************************************************
//
// Constructor spotify devices
//
// ****************************************************************************************

spotify_device_def::spotify_device_def() {
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
// Constructor spotify active player
//
// ****************************************************************************************

spotify_active_play_info_type::spotify_active_play_info_type() {
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
// Constructor spotify oversigt type
//
// ****************************************************************************************

spotify_oversigt_type::spotify_oversigt_type() {
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
// constructor
//
// ****************************************************************************************

spotify_class::spotify_class() : antal(0) {
    for(int i=0;i<maxantal;i++) stack[i]=0;
    stream_optionselect=0;							                                        // selected line in stream options
    spotify_oversigt_loaded=false;
    spotify_oversigt_loaded_nr=0;
    antal=0;
    type=0;
    searchtype=0;
    search_loaded=false;                                                        // load icobn gfx afload search is loaded done by thread.
    spotify_aktiv_song_antal=0;                                                 //
    gfx_loaded=false;			                                                      // gfx loaded default false
    spotify_is_playing=false;                                                   // is we playing any media
    spotify_is_pause=false;                                                     // is player on pause
    show_search_result=false;                                                   // are we showing search result in view
    antalplaylists=0;                                                           // antal playlists
    loaded_antal=0;                                                             // antal loaded
    search_playlist_song=0;
    int port_cnt, n;
    int err = 0;
    strcpy(spotify_aktiv_song[0].release_date,"");
    // create web server
    mg_mgr_init(&mgr, NULL);                                                    // Initialize event manager object
    // start web server
    write_logfile((char *) "Starting web server on port 8000");                 //
    this->c = mg_bind(&mgr, s_http_port, spotify_server_ev_handler);            // Create listening connection and add it to the event manager
    mg_set_protocol_http_websocket(this->c);                                    // make http protocol
    //mg_connect_http(&mgr, ev_handler, "", NULL, NULL);
    active_spotify_device=-1;                                                   // active spotify device -1 = no dev is active
    active_default_play_device=active_spotify_device;
    aktiv_song_spotify_icon=0;                                                  //
    strcpy(spotify_client_id,"");                                               //
    strcpy(spotify_secret_id,"");                                               //
    strcpy(spotifytoken,"");                                                    //
    strcpy(spotifytoken_refresh,"");                                            //
    strcpy(spotify_client_id,"05b40c70078a429fa40ab0f9ccb485de");
    strcpy(spotify_secret_id,"e50c411d2d2f4faf85ddff16f587fea1");
    strcpy(active_default_play_device_name,"");
    strcpy(overview_show_band_name,"");                                         //
    strcpy(overview_show_cd_name,"");                                           //
    spotify_device_antal=0;
    spotify_update_loaded_begin=false;                                          // true then we are update the stack data
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

spotify_class::~spotify_class() {
    mg_mgr_free(&mgr);                        // delete web server again
    mg_mgr_free(&client_mgr);                 // delete web client
    clean_spotify_oversigt();                 // clean spotify class
}


// ****************************************************************************************
//
// set loader flag
//
// ****************************************************************************************


void spotify_class::set_spotify_update_flag(bool flag) {
  spotify_update_loaded_begin=flag;
}

// ****************************************************************************************
//
// get loader flag
//
// ****************************************************************************************


bool spotify_class::get_spotify_update_flag() {
  return(spotify_update_loaded_begin);
}



// ****************************************************************************************
// file writer
// ****************************************************************************************

static size_t file_write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}



// ********************************************************************************************
//
// Refresh token
// return http code (normal 200)
//
// ********************************************************************************************

int spotify_class::spotify_refresh_token() {
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
  if ((curl) && (strcmp(spotifytoken_refresh,"")!=0)) {
    // add userinfo + basic auth
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, spotify_client_id);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, spotify_secret_id);
    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.spotify.com/api/token");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
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
    snprintf(post_playlist_data,sizeof(post_playlist_data),"grant_type=refresh_token&refresh_token=%s",spotifytoken_refresh);
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
      write_logfile((char *) "Spotify new token.");
      //printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      //value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      //process_value_playinfo(value, 0,0);                                                     // fill play info
      if ((response_string.size()>12) && (response_string.compare(2,12,"access_token")==0)) {
        strncpy(newtoken,response_string.c_str()+17,180);
        newtoken[181]='\0';
        write_logfile((char *) "Spotify token valid.");
        strcpy(spotifytoken,newtoken);                                         // update spotify token
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


// *********************************************************************************************************************************
// Download image
// sample call
// download_image("https://i.scdn.co/image/ab67616d0000b2737bded29598acbe1e2f4b4437","/home/hans/file.jpg");
// ********************************************************************************************

int download_image(char *imgurl,char *filename) {
  FILE *file;
  std::string response_string;
  CURLcode res;
  CURL *curl;
  char *base64_code;
  char errbuf[CURL_ERROR_SIZE];
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, imgurl);
    // send data to curl_writeFunction_file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
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
      printf("Error open file.\n");
    }
    curl_easy_cleanup(curl);
  }
  return(1);
}




// ****************************************************************************************
//
// save the refresh token in used
//
// ****************************************************************************************

void spotify_class::spotify_set_token(char *token,char *refresh) {
  strcpy(spotifytoken,token);
  strcpy(spotifytoken_refresh,refresh);
}




// ****************************************************************************************
// DEBUG code
//  json parser used to parse the return files from spotify api
//
// ****************************************************************************************

void spotify_class::playlist_print_depth_shift(int depth) {
  bool debug_json=false;
  int j;
  for (j=0; j < depth; j++) {
    if (debug_json) printf(" ");
  }
}


// * Default view ***********************************************************************************************************************************************


// ****************************************************************************************
//
// static void spotify_class::process_value(json_value* value, int depth);
//
// ****************************************************************************************

bool playlist_process_playlist=false;
bool playlist_process_songs=false;
bool playlist_process_url=false;
bool playlist_process_id=false;
bool playlist_process_image=false;
bool playlist_process_name=false;
bool playlist_process_items=false;

char playlistname[256];
char playlistid[256];
char playlistgfx[2048];
char playlistgfx_top[2048];

// ****************************************************************************************
// INTERNAL
// process types in file playlist
// parse user playlist
//
// Set process flags for  playlist_process_value(json_value* value, int depth,int x,MYSQL *conn) function
//
// ****************************************************************************************

void spotify_class::playlist_process_object(json_value* value, int depth,MYSQL *conn) {
  bool debug_json=false;
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    playlist_print_depth_shift(depth);
    if (debug_json) printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name , "url" )==0) {
      playlist_process_url=true;
    }
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      playlist_process_id=true;
    }
    if (strcmp(value->u.object.values[x].name , "images" )==0) {
      playlist_process_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      playlist_process_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      playlist_process_items=true;
    }
    playlist_process_value(value->u.object.values[x].value, depth+1,x,conn);
  }
}



// ****************************************************************************************
//
// parse user playlist
//
// ****************************************************************************************

void spotify_class::playlist_process_array(json_value* value, int depth,MYSQL *conn) {
  bool debug_json=false;
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  if (debug_json) printf("array\n");
  for (x = 0; x < length; x++) {
    playlist_process_value(value->u.array.values[x], depth,x,conn);
  }
}


// ****************************************************************************************
// INTERNAL lolevel func
// json parser start call function playlist db update
// parse user playlist
//
// ****************************************************************************************

void spotify_class::playlist_process_value(json_value* value, int depth,int x,MYSQL *conn) {
    char filename[512];
    char downloadfilenamelong[2048];
    MYSQL_RES *res;
    MYSQL_ROW row;
    char tempstring[1024];
    char sql[8192];
    bool debug_json=false;
    bool playlistexist=false;
    int j;
    if (value == NULL) return;
    if (value->type != json_object) {
      if (debug_json) playlist_print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        if (debug_json) printf("none\n");
        break;
      case json_object:
        playlist_process_object(value, depth+1,conn);
        break;
      case json_array:
        playlist_process_array(value, depth+1,conn);
        break;
      case json_integer:
        if (debug_json) printf("int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        if (debug_json) printf("double: %f\n", value->u.dbl);
        break;
      case json_string:
        if (debug_json) printf("string: %s\n", value->u.string.ptr);
        if (( playlist_process_id ) && ( depth == 5 ) && ( x == 3 )) {
          if (debug_json) printf("id found = %s  \n", value->u.string.ptr);
          strcpy(playlistid,value->u.string.ptr);
          playlist_process_id=false;
        }
        if (playlist_process_items) {
          // set start of items in list
          playlist_process_items=false;
        }
        if ( playlist_process_image ) {
          //printf(" Process_image song *************************depth=%d x=%d url = %s  ********************************************** \n",depth,x,value->u.string.ptr);
          if (( depth == 8 ) && ( x == 1 )) {
          }
          playlist_process_image=false;
        }
        // works for songs not playlist
        if (playlist_process_url) {
          //printf(" Process_image song *************************depth=%d x=%d url = %s  ********************************************** \n",depth,x,value->u.string.ptr);
          //printf("Song gfx icon found = %s \n", value->u.string.ptr);
          strcpy(filename,"");
          get_webfilenamelong(filename,value->u.string.ptr);
          strcpy(downloadfilenamelong,value->u.string.ptr);
          if (strcmp(filename,"")!=0) {
            //getuserhomedir(downloadfilenamelong);
            strcpy(downloadfilenamelong,localuserhomedir);
            strcat(downloadfilenamelong,"/");
            strcat(downloadfilenamelong,spotify_gfx_path);
            strcat(downloadfilenamelong,filename);
            strcat(downloadfilenamelong,".jpg");
          }
          strcpy(playlistgfx,downloadfilenamelong);
          playlist_process_url=false;
        }
        // get playlist name
        // and create datdabase record playlist info record
        if ((playlist_process_name) && (depth==5) && (x==5)) {
          //if (debug_json) printf("Name found = %s id %s \n", value->u.string.ptr,playlistid);
          strcpy(playlistname,value->u.string.ptr);
          playlistexist=false;
          snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontentplaylist where playlistname like '%s' limit 1", playlistname );
          try {
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                playlistexist=true;
              }
            }
            // if not exist create playlist
            if (!(playlistexist)) {
              //printf("Song name insert : %s \n", playlistname );
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontentplaylist values ('%s','%s','%s',0)",playlistname,playlistgfx,playlistid);
              mysql_query(conn,sql);
              res = mysql_store_result(conn);
            }
            playlist_process_name=false;
          }
          catch (...) {
            write_logfile((char *) "Error update mysql playlist db.");
          }
        }
        break;
      case json_boolean:
        if (debug_json) printf("bool: %d\n", value->u.boolean);
        break;
    }
}


// ****************************************************************************************
//
// Get a List of Current User's Playlists (playlist NOT songs)
// IN use
// get users play lists
// write to spotify_users_playlist.json
//
//
// ****************************************************************************************


int spotify_class::download_user_playlist(char *spotifytoken,int startofset) {

  std::size_t foundpos;
  std::string response_string;
  std::string response_val;
  int httpCode;
  CURLcode res;
  struct curl_slist *chunk = NULL;
  char doget[2048];
  char data[4096];
  char call[4096];
  CURL *curl;
  FILE *tokenfil;
  FILE *outfile;
  char *base64_code;
  char post_playlist_data[1024];
  char errbuf[CURL_ERROR_SIZE];
  char auth_kode[2048];
  curl = curl_easy_init();
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,spotifytoken);
  if (curl) {
    // add userinfo + basic auth
    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER,spotifytoken);
    //curl_easy_setopt(curl, CURLOPT_USERNAME, spotify_client_id);
    //curl_easy_setopt(curl, CURLOPT_PASSWORD, spotify_secret_id);
    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, auth_kode);
    //
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/playlists");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    // set type post
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    /* Add a custom header */
    errbuf[0] = 0;
    // set type post
    //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",spotifytoken_refresh);
    snprintf(post_playlist_data,sizeof(post_playlist_data),"limit=50&offset=%d",startofset);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    outfile=fopen("spotify_users_playlist.json","wb");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
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
      write_logfile((char *) "Download ok code 200");
      //printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      //value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      //process_value_playinfo(value, 0,0);                                                     // fill play info
    } else {
      fprintf(stderr,"Error code httpCode %d \n. ",httpCode);
      fprintf(stderr,"Curl error: %s\n", curl_easy_strerror(res));
    }
    // always cleanup
    fclose(outfile);
    curl_easy_cleanup(curl);
  }
  return(httpCode);
}



// ****************************************************************************************
//
// Check bout spotify have data in db.
// in use in main
//
// ****************************************************************************************

bool spotify_class::spotify_check_spotifydb_empty() {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res1;
  MYSQL_ROW row1;
  char *database = (char *) "mythtvcontroller";
  bool dbexist=false;
  conn = mysql_init(NULL);
  try {
    if (conn) {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
         mysql_error(conn);
         //exit(1);
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about rss table exist
      mysql_query(conn,"SELECT playlistname from mythtvcontroller.spotifycontentplaylist limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist = true;
        }
      }
      mysql_close(conn);
    }
  }
  catch (...) {
    write_logfile((char *) "Error use mysql");
  }
  return(dbexist);
}



// ****************************************************************************************
//
// Get users playlist
// in use
// The default view
//
// ****************************************************************************************

int spotify_class::spotify_get_user_playlists(bool force,int startoffset) {
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
  loaded_antal=0;
  unsigned int spotify_playlistantal=0;
  unsigned int spotify_playlistantal_loaded=0;
  bool spotifyplaylistloader_done=false;
  try {
    conn = mysql_init(NULL);
    if (conn==NULL) {
      fprintf(stderr,"Error opwn mysql connection.\n");
      exit(1);
    }
    if (conn) {
      if (!(mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0))) {
         if (mysql_error(conn)) {
           printf("ERROR OPEN MYSQL.\n");
           fprintf(stderr, "Error: %s\n", mysql_error(conn));
           //exit(1);
         }
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about rss table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist = true;
        }
      }
      if (dbexist==false) {
        snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontent (name varchar(255),paththumb text,playid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify songs)
        snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify playlists)
        snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
      }
      // download all playlist (name + id) in one big file NOT songs
      // if we have a token
      if (strcmp(spotifytoken,"")!=0) {
        // 50 is the max
        //download_user_playlist(spotifytoken,startoffset);
        while (spotifyplaylistloader_done==false) {
          snprintf(doget,sizeof(doget),"curl -X GET 'https://api.spotify.com/v1/me/playlists?limit=50&offset=%d' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.json",startoffset,spotifytoken);
          curl_error=system(doget);
          if (curl_error!=0) {
            fprintf(stderr,"Curl error get user playlists\n");
            exit(0);
          }
          // hent users playlist from spotify api
          system("cat spotify_users_playlist.json | grep spotify:playlist: | awk {'print substr($0,31,22)'} > spotify_users_playlist.txt");
          // get antal playliste first time (we can only load 50 at time)
          if (spotify_playlistantal_loaded==0) {
            system("cat spotify_users_playlist.json | grep total | tail -1 | awk {'print $3'} > spotify_users_playlist_antal.txt");
            json_file = fopen("spotify_users_playlist_antal.txt", "r");
            fscanf(json_file, "%s", temptxt);
            if (strcmp(temptxt,"")!=0) spotify_oversigt.spotify_playlist_antal = atoi(temptxt);
            else spotify_oversigt.spotify_playlist_antal = 0;
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
                spotify_oversigt.spotify_get_playlist(file_contents,force,1);
                spotify_oversigt.clean_spotify_oversigt();
                loaded_antal++;
              }
              fclose(json_file);
            }
            if (file_contents) free(file_contents);                                                          // free memory again
          }
          spotify_playlistantal_loaded+=startoffset;
          // next loop
          // 50 is loaded on each loop until end
          if ((startoffset+50)<spotify_oversigt.spotify_playlist_antal) {
            startoffset+=50;
          } else {
            startoffset=spotify_oversigt.spotify_playlist_antal-startoffset;
          }
          if (spotify_playlistantal_loaded>=spotify_oversigt.spotify_playlist_antal) spotifyplaylistloader_done=true;
        }
        if (remove("spotify_users_playlist.txt")!=0) write_logfile((char *) "Error remove user playlist file spotify_users_playlist.txt");
        // save data to mysql db
      } else {
          write_logfile((char *) "Error downloading user playlist");
          exit(0);
      }
      snprintf(sql,sizeof(sql),"select playlistname,playlistid from mythtvcontroller.spotifycontentplaylist");
      write_logfile((char *) "process playlist ......");
      mysql_query(conn,sql);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          if (debugmode & 4) fprintf(stdout,"playlist %-60s Spotifyid %-20s \n",row[0],row[1]);
          if (spotify_oversigt.spotify_get_playlist(row[1],force,0)==1) {
            fprintf(stderr,"Error create playlist %s \n",row[1]);
          }
        }
      }
      write_logfile((char *) "process playlist done..");
      mysql_close(conn);
    }
  }
  catch (...) {
    write_logfile((char *) "Error process playlist");
  }
  return(1);
}

// songs ********************************************************************************************************************************


// ****************************************************************************************
//
// json parser used to parse the return files from spotify api (playlist files (songs))
//
// ****************************************************************************************

void spotify_class::print_depth_shift(int depth) {
  int j;
  /*
  for (j=0; j < depth; j++) {
    printf("\t");
  }
  */
}

// ****************************************************************************************
//
// static void spotify_class::process_value(json_value* value, int depth);
// used to parse search result
// ****************************************************************************************

bool process_tracks=false;
bool process_playlist=false;
bool process_songs=false;
bool process_href=false;
bool process_description=false;
bool process_image=false;
bool process_name=false;
bool process_items=false;
bool process_track_nr=false;
bool process_release_date=false;
bool process_uri=false;
bool process_id=false;

// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

void spotify_class::process_object_playlist(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    //print_depth_shift(depth);
    //printf("x=%d depth=%d object[%d].name = %s     \n ",x,depth, x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"tracks")==0) {
      process_tracks=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      process_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "release_date" )==0) {
      process_release_date=true;
    }
    if (strcmp(value->u.object.values[x].name , "description" )==0) {
      process_description=true;
    }
    if (strcmp(value->u.object.values[x].name , "images" )==0) {
      process_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {

      //printf("Process name \n");

      process_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "id" )==0) {
      process_id=true;
    }
    if (strcmp(value->u.object.values[x].name , "track_number" )==0) {
      process_track_nr=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      process_items=true;
    }
    if (strcmp(value->u.object.values[x].name , "uri" )==0) {
      process_uri=true;
    }

    process_value_playlist(value->u.object.values[x].value, depth+1,x);
  }
  //printf("Next record ****************************************************************************\n");
}



//
// json
// process playlist
//


void spotify_class::process_array_playlist(json_value* value, int depth) {
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
// json parser start call function for process playlist
// do the data progcessing from json (value)
//
// ****************************************************************************************

void spotify_class::process_value_playlist(json_value* value, int depth,int x) {
    std::string artist="";
    char tempname[1024];
    int j;
    if (value == NULL) return;
    if (value->type != json_object) {
      //print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        if (debug_json) fprintf(stdout,"none\n");
        break;
      case json_object:
        process_object_playlist(value, depth+1);
        break;
      case json_array:
        process_array_playlist(value, depth+1);
        break;
      case json_integer:
        if (debug_json) fprintf(stdout,"int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        if (debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
        break;
      case json_string:
        //printf("string: %s\n", value->u.string.ptr);
        if ((process_description) && (depth==11) && (x==7)) {
          if (!(stack[antal])) {
            antal++;
            stack[antal]=new (struct spotify_oversigt_type);
          }
          //strcpy(stack[antal]->feed_desc,value->u.string.ptr);                           // desc
          process_description=false;
        }
        if ( process_release_date ) {
          //printf("antal %d process_release_date %s \n",antal,value->u.string.ptr);
          strcpy( stack[antal]->feed_release_date , value->u.string.ptr );
          process_release_date=false;
        }
        if ( process_items ) {
          // set start of items in list
          process_items=false;
        }
        if ( process_tracks ) {
          // playlist id from spotify
          if (stack[antal]) {
          }
          process_tracks=false;
        }
        // gfx url
        if ( process_image ) {
          //printf(" spotify_playlistname %s Process_image ***** depth=%d x=%d url = %s  *********** \n",spotify_playlistname,depth,x,value->u.string.ptr);
          // get playlist cover
          if (( depth == 5 ) && ( x == 1 )) {
            // get covver file url
            strcpy(playlistgfx_top,value->u.string.ptr);
          }
          // get song cover
          if (( depth == 14 ) && ( x == 1 )) {
            if (stack[antal]) {
              //printf("antal %d process gfx url %s \n",antal,value->u.string.ptr);
              strcpy( stack[antal]->feed_gfx_url , value->u.string.ptr );                           //
              strcpy(playlistgfx,value->u.string.ptr);
            }
          }
          process_image = false;
        }
        // get play link
        // process song spotify id CHECKED OK
        if ( process_uri ) {
          if (( depth == 9 ) && ( x == 18 )) {
            //if (playinfo_json_debug) printf("%s\n", value->u.string.ptr);
            if (stack[antal]) {
              if (stack[antal]) {
                strcpy( stack[antal]->playlisturl , value->u.string.ptr );                           //
              }
            }
          }
          process_uri=false;
        }
        if ( process_id ) {
          // get playlist id
          if (( depth == 2 ) && ( x == 5 )) {
            //printf("Process id %s depth = %d x = %d\n",value->u.string.ptr,depth,x);
            //printf("playlist id %s \n",value->u.string.ptr);
            strcpy(spotify_playlistid , value->u.string.ptr);
          }
          process_id=false;
        }
        if ( process_name ) {
          // get playlist name OK
          if (( depth == 2 ) && ( x == 7 )) {
            //printf("playlist name %-30s \n",value->u.string.ptr);
            // write to log file
            strcpy(spotify_playlistname , value->u.string.ptr);
            sprintf(tempname,"Spotify playlistname : %s", value->u.string.ptr);
            write_logfile(tempname);
          }
          // get playlist id
          if ((depth==2) && (x==5)) {
            //printf("playlist id %s \n",value->u.string.ptr);
            strcpy(spotify_playlistid , value->u.string.ptr);
          }
          // get artis
          if ((depth==13) && (x==3)) {
            if (stack[antal]) {
  //            printf("antal %d process_artist name %s \n",antal,value->u.string.ptr);
              //strcpy( stack[antal]->feed_artist , value->u.string.ptr );
            }
          }
          // get Song name OK
          if ((depth==9) && (x==12)) {
            if (antal==0) {
              stack[antal]=new (spotify_oversigt_type);
            } else {
              // new record
              antal++;
              antalplaylists++;
            }
            if ( antalplaylists<maxantal ) {
              if (!(stack[antal]))  {
                stack[antal]=new (spotify_oversigt_type);
              }
              if (stack[antal]) {
  //              printf("antal %d process_name %s \n",antal,value->u.string.ptr);
                strcpy(stack[antal]->feed_name,value->u.string.ptr);
              }
            }
            process_name=false;
          }
        }
        // get tracknr
        if (process_track_nr) {
          process_track_nr=false;
          //printf(" text %s \n",value->u.string.ptr);
          // if (stack[antal]) strcpy( stack[antal]->feed_artist , value->u.string.ptr );
        }
        break;
      case json_boolean:
        if (debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
        break;
    }
}



// ****************************************************************************************
//
// work
// get songs from playlist (any public user)
// write to spotify_playlist_{spotifyid}.json
// and update db from that file all the songs in playlist
//
// ****************************************************************************************

int spotify_class::spotify_get_playlist(const char *playlist,bool force,bool create_playlistdb) {
  int tt=0;
  bool dbexist=false;
  int refid;
  int curl_error;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char homedir[1024];
  char *database = (char *) "mythtvcontroller";
  char playlistfilename[2048];
  char auth_kode[1024];
  //getuserhomedir(homedir);
  strcpy(homedir,localuserhomedir);

  strcpy(playlistfilename,homedir);
  strcat(playlistfilename,"/");
  strcat(playlistfilename,spotify_json_path);
  strcat(playlistfilename,"spotify_playlist_");                                 // create playlist file name
  strcat(playlistfilename,playlist);                                            // add the spotify playlist id
  strcat(playlistfilename,".json");
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,spotifytoken);
  char sql[8594];
  char doget[4096];
  char filename[4096];
  char downloadfilenamelong[4096];
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  bool playlistexist;
  int file_size;
  char* file_contents;
  json_char* json;
  json_value* value;
  // curl stuf
  CURL *curl;
  std::string response_string;
  int httpCode;
  CURLcode curl_res;
  struct curl_slist *chunk = NULL;
  FILE *out_file;
  bool do_curl=true;
  if ((!(file_exists(playlistfilename))) || (force))  {
    if ((strcmp(spotifytoken,"")!=0) && (strcmp(playlist,"")!=0)) {
      // always here
      if (do_curl) {
        curl = curl_easy_init();
        snprintf(doget,sizeof(doget),"https://api.spotify.com/v1/playlists/%s",playlist);
        if (curl) {
          curl_easy_setopt(curl, CURLOPT_URL, doget);
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
          curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
          /* Add a custom header */
          chunk = curl_slist_append(chunk, "Accept: application/json");
          chunk = curl_slist_append(chunk, "Content-Type: application/json");
          chunk = curl_slist_append(chunk, auth_kode);
          out_file = fopen(playlistfilename, "wb");
          if (out_file) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            //curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_file);
            // set type post/put
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); /* !!! */
            curl_res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            fputs(response_string.c_str(),out_file);
            if (curl_res != CURLE_OK) {
              fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curl_res));
            }
            // always cleanup
            curl_easy_cleanup(curl);
            fclose(out_file);
            if (httpCode != 200) {
              fprintf(stderr,"Playlist loader error\n");
              fprintf(stderr,"Spotify error %d \n",httpCode);
              write_logfile((char *) "Spotify playlist loader error from web.");
              //exit(1);
            }
          }
        }
      } else {
        snprintf(doget,sizeof(doget),"curl -X 'GET' 'https://api.spotify.com/v1/playlists/%s' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > %s",playlist,spotifytoken,playlistfilename);
        curl_error=system(doget);
        if (curl_error!=0) {
          write_logfile((char *) "Spotify playlist error loading from web.");
          fprintf(stderr,"Curl error get playlist %s \n",playlist);
          fprintf(stderr,"Curl error %s \n",doget);
          //return 1;
        }
      }
    }
    stat(playlistfilename, &filestatus);                                          // get file info
    file_size = filestatus.st_size;                                               // get filesize
    file_contents = (char*) malloc(filestatus.st_size);
    json_file = fopen(playlistfilename, "rt");
    if (json_file == NULL) {
      fprintf(stderr, "Unable to open %s\n", playlistfilename);
      free(file_contents);                                                        //
      return 1;
    }
    if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
      fprintf(stderr, "Unable to read spotify playlist content of %s\n", playlistfilename);
      fclose(json_file);
      free(file_contents);                                                        //
      return 1;
    }
    fclose(json_file);
    json = (json_char*) file_contents;
    value = json_parse(json,file_size);                                           // parser
    // parse from root
    process_value_playlist(value, 0,0);                                           // fill stack array
    json_value_free(value);                                                       // json clean up
    free(file_contents);                                                          //
    conn = mysql_init(NULL);
    // Connect to database
    if (conn) {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)==0) {
        dbexist = false;
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist = true;
        }
      }
      // create db if not exist
      if (!(dbexist)) {
        if (dbexist==false) {
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontent (name varchar(255),paththumb text,playid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          res = mysql_store_result(conn);
          // create db (spotify songs)
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          res = mysql_store_result(conn);
          // create db (spotify playlists)
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          // create db if not exist
          res = mysql_store_result(conn);
        }
      }
      // create db spotify playlist process data
      // insert all record in db
      tt = 0;
      //
      // loop over songs in playlist and update db
      //
      while(tt<antalplaylists) {
        if (stack[tt]) {
          //if (debugmode & 4) fprintf(stdout,"Track nr #%2d Name %40s url %s  gfx url %s \n",tt,stack[tt]->feed_name,stack[tt]->playlisturl,stack[tt]->feed_gfx_url);
          // download gfx file to tmp dir
          get_webfilename(filename,stack[tt]->feed_gfx_url);
          if (strcmp(filename,"")) {
            //getuserhomedir(downloadfilenamelong);
            strcpy(downloadfilenamelong,localuserhomedir);
            strcat(downloadfilenamelong,"/");
            strcat(downloadfilenamelong,spotify_gfx_path);
            strcat(downloadfilenamelong,filename);
            strcat(downloadfilenamelong,".jpg");
            if (!(file_exists(downloadfilenamelong))) {
              // download icon image
              download_image(stack[tt]->feed_gfx_url,downloadfilenamelong);
              //get_webfile2(stack[tt]->feed_gfx_url,downloadfilenamelong);
            }
          }
          // check if playlist exist
          playlistexist=false;
          refid = 0;
          snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname);
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
          if (!(playlistexist)) {
            snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontent (name,paththumb,playid,id) values ('%s','%s','%s',%d)", spotify_playlistname , stack[tt+1]->feed_gfx_url,playlist, 0 );
            mysql_query(conn,sql);
            res=mysql_store_result(conn);
            if (refid==0) {
              snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname);
              mysql_query(conn,sql);
              res=mysql_store_result(conn);
              if (res) {
                while ((row = mysql_fetch_row(res)) != NULL) {
                  refid=atoi(row[0]);
                }
              }
            }
          }
          if (stack[tt+1]) {
            playlistexist=false;
            snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontentarticles where name like '%s' limit 1", stack[tt+1]->feed_name );
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                playlistexist=true;
              }
            }
            //
            // insert record created if not exist ( song name )
            //
            if (playlistexist==false) {              
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontentarticles (name,paththumb,gfxfilename,player,playlistid,artist,id) values ('%s','%s','%s','%s','%s','%s',%d)", stack[tt+1]->feed_name , stack[tt+1]->feed_gfx_url,downloadfilenamelong, stack[tt+1]->playlisturl, playlist , stack[tt+1]->feed_artist , 0 );
              mysql_query(conn,sql);
              mysql_store_result(conn);
            }
          }
        }
        tt++;
      }
      // create playlist if needed
      if ((create_playlistdb) && (strcmp(spotify_playlistname,"")!=0)) {
        snprintf(sql,sizeof(sql),"select playlistid from mythtvcontroller.spotifycontentplaylist where playlistid like '%s' limit 1",spotify_playlistid);
        mysql_query(conn,sql);
        res = mysql_store_result(conn);
        playlistexist=false;
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            playlistexist=true;
          }
        }
        // crete playlist if not exist
        if (!(playlistexist)) {
          //printf("save playlist : %s cover file %s \n", spotify_playlistname, playlistgfx_top );
          snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontentplaylist values ('%s','%s','%s',0)",spotify_playlistname,playlistgfx_top,spotify_playlistid);
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
        }
      }
    }
    mysql_close(conn);
  }
  return tt;
}

// ****************************************************************************************

// ****************************************************************************************
//
// work
// get songs from playlist (any public user)
// write to spotify_playlist_{spotifyid}.json
// and update db from that file all the songs in playlist
//
// ****************************************************************************************

int spotify_class::spotify_get_likedsongs(const char *playlist,bool force,bool create_playlistdb) {
  int tt=0;
  bool dbexist=false;
  int refid;
  int curl_error;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char homedir[1024];
  char *database = (char *) "mythtvcontroller";
  char playlistfilename[2048];
  char auth_kode[1024];
  //getuserhomedir(homedir);
  strcpy(homedir,localuserhomedir);
  strcpy(playlistfilename,homedir);
  strcat(playlistfilename,"/");
  strcat(playlistfilename,spotify_json_path);
  strcat(playlistfilename,"spotify_playlist_");                                 // create playlist file name
  strcat(playlistfilename,playlist);                                            // add the spotify playlist id
  strcat(playlistfilename,".json");
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,spotifytoken);
  char sql[8594];
  char doget[4096];
  char filename[4096];
  char downloadfilenamelong[4096];
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  bool playlistexist;
  int file_size;
  char* file_contents;
  json_char* json;
  json_value* value;
  // curl stuf
  CURL *curl;
  std::string response_string;
  int httpCode;
  CURLcode curl_res;
  struct curl_slist *chunk = NULL;
  FILE *out_file;
  bool do_curl=true;
  if ((!(file_exists(playlistfilename))) || (force))  {
    if ((strcmp(spotifytoken,"")!=0) && (strcmp(playlist,"")!=0)) {
      // always here
      if (do_curl) {
        curl = curl_easy_init();
        snprintf(doget,sizeof(doget),"https://api.spotify.com/v1/playlists/%s",playlist);
        if (curl) {
          curl_easy_setopt(curl, CURLOPT_URL, doget);
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
          curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
          /* Add a custom header */
          chunk = curl_slist_append(chunk, "Accept: application/json");
          chunk = curl_slist_append(chunk, "Content-Type: application/json");
          chunk = curl_slist_append(chunk, auth_kode);
          out_file = fopen(playlistfilename, "wb");
          if (out_file) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            //curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_file);
            // set type post/put
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); /* !!! */
            curl_res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            fputs(response_string.c_str(),out_file);
            if (curl_res != CURLE_OK) {
              fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(curl_res));
            }
            // always cleanup
            curl_easy_cleanup(curl);
            fclose(out_file);
            if (httpCode != 200) {
              fprintf(stderr,"Playlist loader error\n");
              fprintf(stderr,"Spotify error %d \n",httpCode);
              write_logfile((char *) "Spotify playlist loader error from web.");
              //exit(1);
            }
          }
        }
      } else {
        snprintf(doget,sizeof(doget),"curl -X 'GET' 'https://api.spotify.com/v1/playlists/%s' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > %s",playlist,spotifytoken,playlistfilename);
        curl_error=system(doget);
        if (curl_error!=0) {
          write_logfile((char *) "Spotify playlist error loading from web.");
          fprintf(stderr,"Curl error get playlist %s \n",playlist);
          fprintf(stderr,"Curl error %s \n",doget);
          //return 1;
        }
      }
    }
    stat(playlistfilename, &filestatus);                                          // get file info
    file_size = filestatus.st_size;                                               // get filesize
    file_contents = (char*) malloc(filestatus.st_size);
    json_file = fopen(playlistfilename, "rt");
    if (json_file == NULL) {
      fprintf(stderr, "Unable to open %s\n", playlistfilename);
      free(file_contents);                                                        //
      return 1;
    }
    if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
      fprintf(stderr, "Unable to read spotify playlist content of %s\n", playlistfilename);
      fclose(json_file);
      free(file_contents);                                                        //
      return 1;
    }
    fclose(json_file);
    json = (json_char*) file_contents;
    value = json_parse(json,file_size);                                           // parser
    // parse from root
    process_value_playlist(value, 0,0);                                           // fill stack array
    json_value_free(value);                                                       // json clean up
    free(file_contents);                                                          //
    conn = mysql_init(NULL);
    // Connect to database
    if (conn) {
      if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)==0) {
        dbexist = false;
      }
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      // test about table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist = true;
        }
      }
      // create db if not exist
      if (!(dbexist)) {
        if (dbexist==false) {
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontent (name varchar(255),paththumb text,playid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          res = mysql_store_result(conn);
          // create db (spotify songs)
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          res = mysql_store_result(conn);
          // create db (spotify playlists)
          snprintf(sql,sizeof(sql),"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
          if (mysql_query(conn,sql)!=0) {
            write_logfile((char *) "mysql create table error.");
            fprintf(stdout,"SQL : %s\n",sql);
          }
          // create db if not exist
          res = mysql_store_result(conn);
        }
      }
      // create db spotify playlist process data
      // insert all record in db
      tt = 0;
      while(tt<antalplaylists) {
        if (stack[tt]) {
          //if (debugmode & 4) fprintf(stdout,"Track nr #%2d Name %40s url %s  gfx url %s \n",tt,stack[tt]->feed_name,stack[tt]->playlisturl,stack[tt]->feed_gfx_url);
          // download gfx file to tmp dir
          get_webfilename(filename,stack[tt]->feed_gfx_url);
          if (strcmp(filename,"")) {
            //getuserhomedir(downloadfilenamelong);
            strcpy(downloadfilenamelong,localuserhomedir);
            strcat(downloadfilenamelong,"/");
            strcat(downloadfilenamelong,spotify_gfx_path);
            strcat(downloadfilenamelong,filename);
            strcat(downloadfilenamelong,".jpg");
            if (!(file_exists(downloadfilenamelong))) {
              // download icon image
              download_image(stack[tt]->feed_gfx_url,downloadfilenamelong);
              //get_webfile2(stack[tt]->feed_gfx_url,downloadfilenamelong);
            }
          }
          // check if playlist exist
          playlistexist=false;
          refid = 0;
          snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname);
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              refid=atoi(row[0]);
              playlistexist=true;
            }
          }
          // crete playlist
          if (!(playlistexist)) {
            snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontent (name,paththumb,playid,id) values ('%s','%s','%s',%d)", spotify_playlistname , stack[tt+1]->feed_gfx_url,playlist, 0 );
            mysql_query(conn,sql);
            res=mysql_store_result(conn);
            /*
            sprintf(sql,"insert into mythtvcontroller.spotifycontentplaylist (playlistname,paththumb,playlistid,id) values ('%s','%s','%s',%d)", spotify_playlistname , stack[tt+1]->feed_gfx_url,playlist, 0 );
            mysql_query(conn,sql);
            res=mysql_store_result(conn);
            */

            if (refid==0) {
              snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname);
              mysql_query(conn,sql);
              res=mysql_store_result(conn);
              if (res) {
                while ((row = mysql_fetch_row(res)) != NULL) {
                  refid=atoi(row[0]);
                }
              }
            }
          }
          if (stack[tt+1]) {                                                    // if (stack[tt+1]) {
            playlistexist=false;
            snprintf(sql,sizeof(sql),"select id from mythtvcontroller.spotifycontentarticles where name like '%s' limit 1", stack[tt+1]->feed_name );
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                playlistexist=true;
              }
            }
            //
            // insert record created if not exist ( song name )
            //
            if (playlistexist==false) {
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontentarticles (name,paththumb,gfxfilename,player,playlistid,artist,id) values ('%s','%s','%s','%s','%s','%s',%d)", stack[tt+1]->feed_name , stack[tt+1]->feed_gfx_url,downloadfilenamelong, stack[tt+1]->playlisturl, playlist , stack[tt+1]->feed_artist , 0 );
              mysql_query(conn,sql);
              mysql_store_result(conn);
            }
          }
        }
        tt++;
      }
      // create playlist if needed
      if ((create_playlistdb) && (strcmp(spotify_playlistname,"")!=0)) {
        snprintf(sql,sizeof(sql),"select playlistid from mythtvcontroller.spotifycontentplaylist where playlistid like '%s' limit 1",spotify_playlistid);
        mysql_query(conn,sql);
        res = mysql_store_result(conn);
        playlistexist=false;
        if (res) {
          while ((row = mysql_fetch_row(res)) != NULL) {
            playlistexist=true;
          }
        }
        // crete playlist
        if (!(playlistexist)) {
          //printf("save playlist : %s cover file %s \n", spotify_playlistname, playlistgfx_top );
          snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotifycontentplaylist values ('%s','%s','%s',0)",spotify_playlistname,playlistgfx_top,spotify_playlistid);
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
        }
      }

    }
    mysql_close(conn);
  }
  return tt;
}



// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

void spotify_class::process_object_playinfo(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    //printf("x=%d depth=%d object[%d].name = %s  \n",x,depth, x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"tracks")==0) {
      process_playinfo_tracks=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      process_playinfo_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "artist" )==0) {
      process_playinfo_artist=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      process_playinfo_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "description" )==0) {
      process_playinfo_description=true;
    }
    if (strcmp(value->u.object.values[x].name , "images" )==0) {
      process_playinfo_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      process_playinfo_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "track_number" )==0) {
      process_playinfo_track_nr=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      process_playinfo_items=true;
    }
    if (strcmp(value->u.object.values[x].name , "release_date" )==0) {
      process_playinfo_date=true;
    }
    if (strcmp(value->u.object.values[x].name , "progress_ms" )==0) {
      process_playinfo_progress_ms=true;
    }
    if (strcmp(value->u.object.values[x].name , "duration_ms" )==0) {
      process_playinfo_duration_ms=true;
    }
    process_value_playinfo(value->u.object.values[x].value, depth+1,x);
  }
}

// *****************************************************************************************
//
// ****************************************************************************************

void spotify_class::process_array_playinfo(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  //printf("array found\n");
  for (x = 0; x < length; x++) {
    process_value_playinfo(value->u.array.values[x], depth,x);
  }
}


// ****************************************************************************************
//
// json parser start call function for process playinfo
// do the data progcessing from json
//
// ****************************************************************************************

void spotify_class::process_value_playinfo(json_value* value, int depth,int x) {
    int j;
    if (value == NULL) return;
    if (value->type != json_object) {
      print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        if (debug_json) fprintf(stdout,"none\n");
        break;
      case json_object:
        process_object_playinfo(value, depth+1);
        break;
      case json_array:
        process_array_playinfo(value, depth+1);
        break;
      case json_integer:
        if (process_playinfo_duration_ms) {
          spotify_aktiv_song[0].duration_ms=value->u.integer;
          process_playinfo_duration_ms=false;
        }
        if (process_playinfo_progress_ms) {
          spotify_aktiv_song[0].progress_ms=value->u.integer;
          process_playinfo_progress_ms=false;
        }
        //printf("int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        if (debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
        break;
      case json_string:
        //printf("Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
        if (process_playinfo_items) {
          process_playinfo_items=false;
        }
        if (process_playinfo_tracks) {
          process_playinfo_tracks=false;
        }
        if (process_playinfo_image) {
          //if (debug_json) fprintf(stdout,"Image Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
          strcpy(spotify_aktiv_song[0].cover_image_url,value->u.string.ptr);
          process_playinfo_image=false;
        }
        if (process_playinfo_progress_ms) {
          //printf("progress ms Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
          if ((depth==1) && (x==5)) spotify_aktiv_song[0].progress_ms=atol(value->u.string.ptr);
          process_playinfo_progress_ms=false;
        }
        if (process_playinfo_duration_ms) {
          fprintf(stdout,"Value found = %s \n ",value->u.string.ptr);
          //if ((depth==3) && (x==4)) spotify_aktiv_song[0].duration_ms=atol(value->u.string.ptr);
          process_playinfo_duration_ms=false;
        }
        // release date
        if (process_playinfo_date) {
          if (debug_json) fprintf(stdout,"date Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
          strcpy(spotify_aktiv_song[0].release_date,value->u.string.ptr);
          process_playinfo_date=false;
        }
        // get playlist name
        if (process_playinfo_name) {
          if (debug_json) fprintf(stdout,"name type Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
          // artist name
          if ((depth==9) && (x==3)) strcpy(spotify_aktiv_song[0].artist_name,value->u.string.ptr);
          // album name
          if ((depth==6) && (x==7)) strcpy(spotify_aktiv_song[0].album_name,value->u.string.ptr);
          // song name
          if ((depth==4) && (x==11)) strcpy(spotify_aktiv_song[0].song_name,value->u.string.ptr);
          //
          if ((depth==1) && (x==5)) {
            fprintf(stdout,"progress ms Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
            spotify_aktiv_song[0].progress_ms=atol(value->u.string.ptr);
          }
          process_playinfo_name=false;
        }
        if (process_playinfo_artist) {

          process_playinfo_artist=false;
        }
        // get tracknr
        if (process_playinfo_track_nr) {
          process_playinfo_track_nr=false;
        }
        break;
      case json_boolean:
        if (debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
        break;
    }
}


// *********************************************************************************************************************************
// do we play ?
// return http code
// Work in use
// ********************************************************************************************

int spotify_class::spotify_do_we_play() {
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
  strcat(auth_kode,spotifytoken);
  CURL *curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
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
      //printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      process_value_playinfo(value, 0,0);                                                     // fill play info
      json_value_free(value);                                                                 // json clean up
    }
  }
  return(httpCode);
}



// *******************************************************************************************************
//
// work
// pause play
//
// ****************************************************************************************

int spotify_class::spotify_pause_play() {
  int curl_error;
  char call[4096];
  snprintf(call,sizeof(call),"curl -f -X PUT 'https://api.spotify.com/v1/me/player/pause' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (curl_error!=0) {
    return 1;
  }
  return 0;
}


// ****************************************************************************************
//
// work ( need testing )
// pause play
//
// ****************************************************************************************

int spotify_class::spotify_pause_play2() {
  char auth_kode[1024];
  std::string response_string;
  int httpCode;
  CURLcode res;
  json_char *json;
  json_value *value;
  struct curl_slist *chunk = NULL;
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,spotifytoken);
  CURL *curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/pause");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
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
    }
  }
}


//
// resume play
//
/*
int spotify_class::spotify_resume_play2() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    return 1;
  }
  return 0;
}
*/

// ****************************************************************************************
//
// in use resume play
//
// ****************************************************************************************

int spotify_class::spotify_resume_play() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/play";
  url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}


// ****************************************************************************************
//
// last play do not work
//
// ****************************************************************************************

int spotify_class::spotify_last_play() {
  int curl_error;
  char call[4096];
  snprintf(call,sizeof(call),"curl -f -X POST 'https://api.spotify.com/v1/me/player/last' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    return 1;
  }
  return 0;
}


// ****************************************************************************************
//
// last play
//
// ****************************************************************************************


int spotify_class::spotify_last_play2() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/last";
  url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}



// ****************************************************************************************
//
// work
// next play
//
// ****************************************************************************************

int spotify_class::spotify_next_play() {
  int curl_error;
  char call[4096];
  snprintf(call,sizeof(call),"curl -f -X POST 'https://api.spotify.com/v1/me/player/next' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (curl_error==0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    return 1;
  }
  return 0;
}


// ****************************************************************************************
// DO not work for now
// next play next song need testing
//
// ****************************************************************************************

int spotify_class::spotify_next_play2() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/next";
  url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}



// ************** DEBUG CODE start ****************************************
// enable it on earch start play type (func) to get the libcurl debug code
// ************************************************************************


int debug_callback(CURL *handle,curl_infotype type,char *data,size_t size,void *userptr);

//CURLcode curl_easy_setopt(CURL *handle, CURLOPT_DEBUGFUNCTION,debug_callback);

static void dump(const char *text,FILE *stream, unsigned char *ptr, size_t size) {
  size_t i;
  size_t c;
  const unsigned int width=0x10;

  fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
          text, (long)size, (long)size);

  for(i=0; i<size; i+= width) {
    fprintf(stream, "%4.4lx: ", (long)i);

    /* show hex to the left */
    for(c = 0; c < width; c++) {
      if(i+c < size)
        fprintf(stream, "%02x ", ptr[i+c]);
      else
        fputs("   ", stream);
    }

    /* show data on the right */
    for(c = 0; (c < width) && (i+c < size); c++) {
      char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
      fputc(x, stream);
    }

    fputc('\n', stream); /* newline */
  }
}


//
// trace
//

static int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
  const char *text;
  (void)handle; /* prevent compiler warning */
  (void)userp;

  switch (type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
  default: /* in case a new one is introduced to shock us */
    return 0;
  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }
  dump(text, stderr, (unsigned char *)data, size);
  return 0;
}

// ****************************************************************************************
// Upper is debug code *******************************************************************
// ****************************************************************************************

// ****************************************************************************************
// Works
// Play playlist on spotify device by id
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
// ****************************************************************************************

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

// HAVE some errors

int spotify_class::spotify_play_now_playlist(char *playlist_song,bool now) {
  std::string auth_kode;
  char response_string[8192];
  std::string url;
  //std::string post_playlist_data;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/play?device_id=";
  if (devid) url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    ////curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    sprintf((char *) post_playlist_data,"{\"context_uri\":\"spotify:playlist:%s\",\"offset\":{\"position\":5},\"position_ms\":0}",playlist_song);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data );
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(1);
    }
  }
  return(0);
}




// ****************************************************************************************
//
// work
// play song
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************


// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int spotify_class::spotify_play_now_song(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/play?device_id=";
  url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    snprintf(post_playlist_data,sizeof(post_playlist_data),"{\"uris\":[\"spotify:track:%s\"]}",playlist_song);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data );
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}

// ****************************************************************************************
//
// work
// play artist
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************


// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.


int spotify_class::spotify_play_now_artist(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/play?device_id=";
  url=url + devid;                                                                // set device to play
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    snprintf(post_playlist_data,sizeof(post_playlist_data),"{\"context_uri\":\"spotify:artist:%s\"}",playlist_song);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data );
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}





// ****************************************************************************************
//
// work
// play album
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int spotify_class::spotify_play_now_album(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/me/player/play?device_id=";
  url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    snprintf(post_playlist_data,sizeof(post_playlist_data),"{\"context_uri\":\"spotify:album:%s\"}",playlist_song);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data );
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    // always cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (httpCode == 200) {
      return(httpCode);
    }
  }
  return(0);
}


// ****************************************************************************************
// Do work now
// get user id from spotify api
//
// return 0 if fault
// 200 on ok
//
// ****************************************************************************************

int spotify_class::spotify_get_user_id() {
  static const char *userfilename = "spotify_user_id.txt";
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode=0;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=spotify_oversigt.get_active_device_id();
  if (devid) {
    auth_kode="Authorization: Bearer ";
    auth_kode=auth_kode + spotifytoken;
    url="https://api.spotify.com/v1/me";
    url=url + devid;
    printf("Spotify get user info.\n");
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
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_data);
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
      if (httpCode == 200) {
        return(httpCode);
      }
    }
  }
  return(httpCode);
}

// ****************************************************************************************
//
// Works
// get device list and have it in spotify class
//
// ****************************************************************************************

int spotify_class::spotify_get_available_devices() {
  int device_antal=0;
  int devicenr=0;
  size_t jsonfile_len = 0;
  char *tmp_content_line=NULL;
  FILE *json_file;
  int curl_exitcode;
  char call[4096];
  char sql[2048];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  bool dbexist;
  char *database = (char *) "mythtvcontroller";
  char call_sed[]="cat spotify_device_list.json | sed 's/\\\\\\\\\\\//\\//g' | sed 's/[{\\\",}]//g' | sed 's/ //g' | sed 's/:/=/g' | tail -n +6 > spotify_device_list.txt";
  snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/me/player/devices' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_device_list.json 2>&1",spotifytoken);
  curl_exitcode=system(call);
  //get json file
  if (WEXITSTATUS(curl_exitcode)==0) {
    // convert file by sed (call_sed) easy hack
    curl_exitcode=system(call_sed);
    if (WEXITSTATUS(curl_exitcode)!=0) {
      fprintf(stderr,"Error get devices\n");
    }
    json_file = fopen("spotify_device_list.txt", "r");
    if (json_file == NULL) {
      fprintf(stderr, "Unable to open spotify_device_list.txt\n");
      return 1;
    } else {
      // read devices info
      while ((!(feof(json_file))) && (devicenr<10)) {
        // get id
        getline(&tmp_content_line,&jsonfile_len,json_file);
        if (strlen(tmp_content_line)>5) {
          strcpy(spotify_device[devicenr].id,tmp_content_line+3);
          spotify_device[devicenr].id[strlen(spotify_device[devicenr].id)-1]='\0';  // remove newline char
          // get active status
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strncmp(tmp_content_line,"is_active=true",14)==0) spotify_device[devicenr].is_active=true; else spotify_device[devicenr].is_active=false;
          // if active rember to return from func
          if (( spotify_device[devicenr].is_active ) && (active_spotify_device==-1)) active_spotify_device=devicenr;
          // get is_private_session
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strcmp(tmp_content_line,"is_private_session=true")==0) spotify_device[devicenr].is_private_session=true; else spotify_device[devicenr].is_private_session=false;
          // get private info
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strcmp(tmp_content_line,"is_restricted=true")==0) spotify_device[devicenr].is_restricted=true; else spotify_device[devicenr].is_restricted=false;
          // get dev name
          getline(&tmp_content_line,&jsonfile_len,json_file);
          strcpy(spotify_device[devicenr].name,tmp_content_line+5);
          spotify_device[devicenr].name[strlen(spotify_device[devicenr].name)-1]='\0';   // remove newline char
          // get dev type
          getline(&tmp_content_line,&jsonfile_len,json_file);
          strcpy(spotify_device[devicenr].devtype,tmp_content_line+5);
          spotify_device[devicenr].devtype[strlen(spotify_device[devicenr].devtype)-1]='\0';   // remove newline char
          // get dev volume info
          getline(&tmp_content_line,&jsonfile_len,json_file);
          spotify_device[devicenr].devvolume=atoi(tmp_content_line+15);
          getline(&tmp_content_line,&jsonfile_len,json_file);
          devicenr++;
        }
      }
      free(tmp_content_line);
    }
    fclose(json_file);
    if ( devicenr>0 ) {
      conn=mysql_init(NULL);
      if (conn) {
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,"create TABLE mythtvcontroller.spotify_device (device_name varchar(255),active bool,devtype varchar (255),dev_id varchar (255),intnr INT AUTO_INCREMENT,PRIMARY KEY (intnr))");
        res = mysql_store_result(conn);
        fprintf(stderr,"Found devices : %d\n",devicenr);
        for( int t = 0 ; t < devicenr ; t++ ) {
          if ( strcmp(spotify_device[t].name,"") !=0 ) {
            device_antal++;
            fprintf(stderr,"Device name      : %s \n",spotify_device[t].name);
            fprintf(stderr,"Device is active : %d \n",spotify_device[t].is_active);
            fprintf(stderr,"Device type      : %s \n",spotify_device[t].devtype);
            fprintf(stderr,"Device id        : %s \n\n",spotify_device[t].id);
            snprintf(sql,sizeof(sql),"select dev_id from mythtvcontroller.spotify_device where dev_id like '%s' limit 1",spotify_device[t].id);
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            dbexist=false;
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                dbexist=true;
              }
            }
            // create if not exist
            if (dbexist==false) {
              snprintf(sql,sizeof(sql),"insert into mythtvcontroller.spotify_device values ('%s',%d,'%s','%s',0)",spotify_device[t].name,spotify_device[t].is_active,spotify_device[t].devtype,spotify_device[t].id);
              mysql_query(conn,sql);
              res = mysql_store_result(conn);
            }
          }
        }
        this->spotify_device_antal=device_antal;
        if (conn) mysql_close(conn);
        fprintf(stderr,"\nPlay device list is updated ok\n");
        write_logfile("Play device list is updated ok.");
      }
    }
  }
  // no device to use is found or no token
  if ( curl_exitcode == 22 ) {
    active_spotify_device=-1;
    fprintf(stderr,"Error loading device players info from spodify by api.\n");
    write_logfile("Error loading device players info from spodify by api.");
  }
  // if global_use_spotify_local_player (do use local player (spotify this is running in the background))
  // Set default local play always
  if (global_use_spotify_local_player) {
    set_default_device_to_play(1);
  }
  return active_spotify_device;
}


// ****************************************************************************************
//
// get user access token
// write to spotify_access_token.txt file
//
// ****************************************************************************************

int spotify_class::spotify_get_access_token2() {
  // works
  struct mg_connection *nc;
  char homedirfile[4096];
  FILE *myfile;
  char data[4096];
  char call[4096];
  char *base64_code;
  mg_mgr_init(&spotify_oversigt.client_mgr, NULL);
  nc = mg_connect_http(&spotify_oversigt.client_mgr, ev_handler, "https://accounts.spotify.com/api/token", "Content-Type: application/x-www-form-urlencoded\r\n", "");
  mg_set_protocol_http_websocket(nc);
  while (s_exit_flag == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
  strcpy(data,spotify_client_id);
  strcat(data,":");
  strcat(data,spotify_secret_id);
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  // works
  //sprintf(call,"curl -X 'POST' -H 'Authorization: Basic %s' -d grant_type=client_credentials https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code);

  //strcpy(homedirfile,getenv("HOME"));
  strcpy(homedirfile,"");
  strcat(homedirfile,"spotify_access_token.txt");
  myfile = fopen(homedirfile,"r");
  if (myfile) {
    fgets(data,4095,myfile);                      // read file
    fclose(myfile);
    //remove("spotify_access_token.txt");           // remove file again
  }
  return(1);
}





// ****************************************************************************************
//
// return the intnr
//
// ****************************************************************************************

int spotify_class::get_spotify_intnr(int nr) {
  if (nr < antal) return (stack[nr]->intnr); else return (0);
}

// ****************************************************************************************
//
// return the playlist/song name
//
// ****************************************************************************************

char *spotify_class::get_spotify_name(int nr) {
  if (nr < antal) return (stack[nr]->feed_name); else return (NULL);
}

// ****************************************************************************************
//
// return the spotify playlist id
//
// ****************************************************************************************

char *spotify_class::get_spotify_playlistid(int nr) {
  if (nr < antal) return (stack[nr]->playlistid); else return (NULL);
}

// ****************************************************************************************
//
// return the description
//
// ****************************************************************************************

char *spotify_class::get_spotify_desc(int nr) {
  if (nr < antal) return (stack[nr]->feed_desc); else return (NULL);
}



// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************

void spotify_class::clean_spotify_oversigt() {
    startup_loaded=false;
    for(int i=1;i<antal;i++) {
      if (stack[i]) {
        // crash
        if (stack[i]->textureId) {
          if (&stack[i]->textureId) {
            if (&stack[i]->textureId) {
              //if (&stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete spotify texture
            }
          }
        }
        delete stack[i];
      }
      stack[i]=NULL;
    }
    antal=0;
    antalplaylists=0;
    spotify_oversigt_loaded=false;			// set load icon texture again
    spotify_oversigt_loaded_nr=0;
    spotify_oversigt_nowloading=0;
}


// ****************************************************************************************
//
// set spotify icon image
//
// ****************************************************************************************

void spotify_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}

// ****************************************************************************************
//
// get nr of spotify playlists
//
// ****************************************************************************************

int spotify_class::get_antal_rss_feeds_sources(MYSQL *conn) {
  int antalrss_feeds=0;
  MYSQL_RES *res;
  MYSQL_ROW row;
  if (conn) {
    mysql_query(conn,"SELECT count(name) from mythtvcontroller.spotifycontent");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        antalrss_feeds=atoi(row[0]);
      }
    }
  }
  return(antalrss_feeds);
}


// ****************************************************************************************
//
// opdate show liste in view (det vi ser)
// loaded from db
//
// ****************************************************************************************
//
// load felt 7 = mythtv gfx icon
// fpath=stream path
// atr = stream name

int spotify_class::opdatere_spotify_oversigt(char *refid) {
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
      // test about rss table exist
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) dbexist=true;
      }
    }
    
    // clear old view
    clean_spotify_oversigt();

    strcpy(lasttmpfilename,"");
    if (debugmode & 4) {
      fprintf(stderr,"loading spotify data.\n");
    }
    // find records after type (0 = root, else = refid)
    if (refid == NULL) {
      show_search_result=false;
      snprintf(sqlselect,sizeof(sqlselect),"select playlistname,paththumb,playlistid,id from spotifycontentplaylist order by id DESC");
      getart = 0;
    } else {
      show_search_result=true;
      snprintf(sqlselect,sizeof(sqlselect),"select name,gfxfilename,player,playlistid from spotifycontentarticles where playlistid='%s' order by id DESC",refid);
      getart = 1;
    }
    this->type = getart;					                                                 // husk sql type
    if (debugmode & 4) fprintf(stderr,"spotify loader started... \n");
    write_logfile("spotify db loader started.");    
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
            stack[antal]=new (struct spotify_oversigt_type);
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
              stack[antal]->intnr=atoi(row[3]);
              stack[antal]->nyt=false;
              stack[antal]->type=0;
              // top level (load playlist)
              if (getart == playlisttype ) {
                strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
                strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
                if (row[1]) {
                  if (strncmp(row[1],"http",4)==0) {
                    get_webfilename(downloadfilename,row[1]);
                    //getuserhomedir(downloadfilenamelong);
                    strcpy(downloadfilenamelong,localuserhomedir);
                    strcat(downloadfilenamelong,"/");
                    strcat(downloadfilenamelong,spotify_gfx_path);
                    strcat(downloadfilenamelong,downloadfilename);
                    strcat(downloadfilenamelong,".jpg");
                    // download file
                    if (!(file_exists(downloadfilenamelong))) {
                      download_image(row[1],downloadfilenamelong);
                      //get_webfile2(row[1],downloadfilenamelong);                // download file
                    } else strcpy(downloadfilenamelong,row[1]);                 // no file name
                  } else strcpy(downloadfilenamelong,row[1]);
                  strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
                }
                strncpy(stack[antal]->playlistid,row[2],spotify_namelength);    //

                /*
                strncpy(downloadfilenamelong,row[2],spotify_pathlength);
                //if (row[1]) strcpy(stack[antal]->feed_path,row[1]);
                if (row[0]) stack[antal]->feed_group_antal=0;                             // get antal
                else stack[antal]->feed_group_antal=0;
                strcpy(downloadfilenamelong,row[2]);
                if (file_exists(downloadfilenamelong)) {
                  //stack[antal]->textureId=loadTexture (downloadfilenamelong);
                }
                */
                /*
                if (row[1]) {
                  get_webfilename(downloadfilename,stack[antal]->feed_gfx_url);
                  getuserhomedir(downloadfilenamelong);
                  strcat(downloadfilenamelong,"/datadisk/mythtv-controller-0.38/spotify_gfx/");
                  strcat(downloadfilenamelong,downloadfilename);          // now file path + filename
                  strcat(downloadfilenamelong,".jpg");
                  if (!(file_exists(downloadfilenamelong))) {
                    // download file
                    get_webfile2(stack[antal]->feed_gfx_url,downloadfilenamelong);
                  }
                  //texture=loadTexture ((char *) downloadfilenamelong);
                  if (texture) stack[antal]->textureId=texture;
                }
                */
/*
                if (row[3]) strncpy(stack[antal]->feed_desc,row[3],spotify_desclength);
                if (row[7]) strncat(tmpfilename,row[7],20);                               //
                strcpy(stack[antal]->feed_gfx_mythtv,tmpfilename);            	       		// icon file
                if (row[9]) strcpy(tmpfilename,row[9]);
                get_webfilenamelong(downloadfilename,tmpfilename);          // get file name from url
                // check filename
                strcpy(downloadfilename1,downloadfilename);                 // back name before change
                int mmm=0;
                while(mmm<strlen(downloadfilename)) {
                  if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                  mmm++;
                }
*/
                antal++;
              }
              // load playlist songs
              if (getart == songlisttype ) {
                // First create the back button
                if (antal == 0) {
                  strcpy(stack[antal]->feed_showtxt,"Back");
                  strcpy(stack[antal]->playlisturl,"");
                  stack[antal]->intnr=0;
                  antal++;
                  if (stack[antal]==NULL) stack[antal]=new (struct spotify_oversigt_type);
                }
                strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
                strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
                strncpy(stack[antal]->feed_gfx_url,row[1],spotify_namelength);
                strcpy(stack[antal]->playlisturl,row[2]);   // get trackid
                stack[antal]->type=1;
                //songstrpointer=strstr(row[2],"https://api.spotify.com/v1/tracks/");
                songstrpointer=strstr(row[2],"spotify:track:");
                // get track id from string
                if (songstrpointer) {
                  strcpy(temptxt2,row[2]+14);
                  strcpy(stack[antal]->playlistid,temptxt2);
                } else strcpy(stack[antal]->playlistid,row[2]);
                antal++;
              }
            }
          }
        }
        mysql_close(conn);
      } else {
        if (debugmode & 4) fprintf(stderr,"No spotify data loaded \n");
        write_logfile("No spotify data loaded from db.");
        write_logfile("Please check that the db exist.");
      }
      antalplaylists=antal;
      return(antal);
    } else  {
      fprintf(stderr,"Failed to update Spotify db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
      write_logfile("Failed to update Spotify db, can not connect to database.");
    }
    if (debugmode & 4) fprintf(stderr,"Spotify loader done... \n");
    write_logfile("spotify db loader done.");    
    return(0);
}



// ****************************************************************************************
//
// search for playlist or song in db (from users playlist data in db)
//
// ****************************************************************************************


int spotify_class::opdatere_spotify_oversigt_searchtxt(char *keybuffer,int type) {
  char sqlselect[2048];
  char tmpfilename[1024];
  char lasttmpfilename[1024];
  char downloadfilename[1024];
  char downloadfilename1[1024];
  char downloadfilenamelong[1024];
  char homedir[1024];
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
  if ((conn) && (strlen(keybuffer)>1)) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)==0) {
      dbexist=false;
    }
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    // test about rss table exist
    mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dbexist=true;
      }
    }
    // clear old list (view)
    clean_spotify_oversigt();                                                   // clean old list

    // find records after type (0 = root, else = refid)
    if (type == playlisttype ) {
      snprintf(sqlselect,sizeof(sqlselect),"select playlistname,paththumb,playlistid,id from spotifycontentplaylist where playlistname like '");
      strcat(sqlselect,"%");
      strcat(sqlselect,keybuffer);
      strcat(sqlselect,"%'");
      getart = 0;
    } else {
      snprintf(sqlselect,sizeof(sqlselect),"select name,paththumb,player,playlistid,id from spotifycontentarticles where name like '");
      strcat(sqlselect,"%");
      strcat(sqlselect,keybuffer);
      strcat(sqlselect,"%'");
      getart = 1;
    }
    this->type = getart;
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
        if (antal<maxantal) {
          stack[antal]=new (struct spotify_oversigt_type);
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
            stack[antal]->nyt=false;
            if (getart == playlisttype ) {
              stack[antal]->type=0;
              if (antal == 0) {
                strcpy(stack[antal]->feed_showtxt,"Back");
                strcpy(stack[antal]->playlisturl,"");
                stack[antal]->intnr=0;
                antal++;
                if (stack[antal]==NULL) stack[antal]=new (struct spotify_oversigt_type);
              }
              strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
              strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
              strncpy(stack[antal]->feed_gfx_url,row[1],spotify_namelength);
              strncpy(stack[antal]->playlistid,row[2],spotify_namelength);
              stack[antal]->intnr=atoi(row[3]);
              strcpy(downloadfilename,"");
              strcpy(downloadfilenamelong,"");
              get_webfilename(downloadfilename,stack[antal]->feed_gfx_url);
              //getuserhomedir(downloadfilenamelong);
              strcpy(downloadfilenamelong,localuserhomedir);
              strcat(downloadfilenamelong,"/");
              strcpy(downloadfilenamelong,spotify_gfx_path);
              strcat(downloadfilenamelong,downloadfilename);          // now file path + filename
              strcat(downloadfilenamelong,".jpg");
              if (file_exists(downloadfilenamelong)) {
                if (check_zerro_bytes_file(downloadfilenamelong)==false) {
                  //stack[antal]->textureId=loadTexture ((char *) downloadfilenamelong);
                  fprintf(stderr,"Set teture  %s *************************** \n",downloadfilenamelong);
                }
              }
              antal++;
            }
            if (getart == songlisttype ) {
              stack[antal]->type=1;
              if (antal == 0) {
                strcpy(stack[antal]->feed_showtxt,"Back");
                strcpy(stack[antal]->playlisturl,"");
                stack[antal]->intnr=0;
                antal++;
                if (stack[antal]==NULL) stack[antal]=new (struct spotify_oversigt_type);
              }
              strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
              strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
              strncpy(stack[antal]->feed_gfx_url,row[1],spotify_namelength);
              strncpy(stack[antal]->playlisturl,row[2],spotify_namelength);
              stack[antal]->intnr=atoi(row[4]);
              antal++;
            }
          }
        }
      }
      antalplaylists=antal;
    }
    mysql_close(conn);
  }
  show_search_result=true;
  return(antal);
}

// json_parser search process result
// *******************************************************************************************
//
// json parser used to parse the return files from spotify api
//
// ****************************************************************************************


// ****************************************************************************************
//
// static void spotify_class::process_value(json_value* value, int depth);
//
// ****************************************************************************************

bool search_process_tracks=false;
bool search_process_uri=false;
bool search_process_songs=false;
bool search_process_href=false;
bool search_process_description=false;
bool search_process_image=false;
bool search_process_name=false;
bool search_process_items=false;
bool search_process_track_nr=false;


// ****************************************************************************************
//
// process types in file for process playlist files (songs)
//
// ****************************************************************************************

void spotify_class::search_process_object(json_value* value, int depth,int art) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    //if (strcmp(value->u.object.values[x].name,"name")==0)
    //printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"tracks")==0) {
      search_process_tracks=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      search_process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "uri" )==0) {
      search_process_uri=true;
    }
    if (strcmp(value->u.object.values[x].name , "description" )==0) {
      search_process_description=true;
    }
    if (strcmp(value->u.object.values[x].name , "images" )==0) {
      search_process_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      search_process_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "track_number" )==0) {
      search_process_track_nr=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      search_process_items=true;
    }
    search_process_value(value->u.object.values[x].value, depth+1,x,art);
  }
}



// ****************************************************************************************
//
// json parser
//
// ****************************************************************************************


void spotify_class::search_process_array(json_value* value, int depth,int art) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  if (debug_json) printf("array\n");
  for (x = 0; x < length; x++) {
    search_process_value(value->u.array.values[x], depth,x,art);
  }
}


// ****************************************************************************************
//
// json parser start call function for process playlist songs
// do the data progcessing from json
//
// ****************************************************************************************

void spotify_class::search_process_value(json_value* value, int depth,int x,int art) {
  char artisid[1024];
  char filename[2048];
  char downloadfilenamelong[2048];
  int j;
  if (value == NULL) return;
  if (value->type != json_object) {
    print_depth_shift(depth);
  }
  switch (value->type) {
    case json_none:
      printf("none\n");
      break;
    case json_object:
      search_process_object(value, depth+1,art);
      break;
    case json_array:
      search_process_array(value, depth+1,art);
      break;
    case json_integer:
      //printf("int: %10" PRId64 "\n", value->u.integer);
      break;
    case json_double:
      //printf("double: %f\n", value->u.dbl);
      break;
    case json_string:
      if (debugmode) {
        if ((depth!=8) && (depth!=10)) {
          //printf("x = %2d deep = %2d art = %2d ",x,depth,art);
          //printf("string: %s\n", value->u.string.ptr);
        }
      }
      if (search_process_items) {
        // set start of items in list
        search_process_items=false;
      }
      if (search_process_tracks) {
        search_process_tracks=false;
      }
      if ( search_process_image ) {
        if ((depth==1) && (x==1)) {
          //strcpy(stack[antal-1]->feed_gfx_url,value->u.string.ptr);
        }
        // load artist conver icon gfx
        if ((depth==10)  && (x==1)) {
          //printf("antal = %d ******************************************************************************* Image process %s \n",antal,value->u.string.ptr);
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) stack[antal]=new (spotify_oversigt_type);
            stack[antal]->textureId=0;
            strcpy(downloadfilenamelong,"");
            // https://i.scdn.co/image/c717baedc02b00bae7707b6de69aad8800e76aaa
            // get name from url
            if (debugmode & 4) printf("# %d artist icon url found  : %s \n",antal,value->u.string.ptr);
            if (strncmp("https://i.scdn.co/image/",value->u.string.ptr,24)==0) {
              strcpy(filename,value->u.string.ptr+24);
              if (strcmp(value->u.string.ptr,"")) {
                //getuserhomedir(downloadfilenamelong);
                strcpy(downloadfilenamelong,localuserhomedir);
                strcat(downloadfilenamelong,"/");
                strcat(downloadfilenamelong,spotify_gfx_path);
                strcat(downloadfilenamelong,filename);
                strcat(downloadfilenamelong,".jpg");
                // save name to db to next time
                strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
                if (!(file_exists(downloadfilenamelong))) {
                  // download icon image
                  download_image(value->u.string.ptr,downloadfilenamelong);
                  //get_webfile2(value->u.string.ptr,downloadfilenamelong);
                }
              }
            }
          }
        }
        // load cd conver icon gfx
        if ((depth==12) && (x==1)) {
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) stack[antal]=new (spotify_oversigt_type);
            stack[antal]->textureId=0;
            strcpy(downloadfilenamelong,"");
            if (debugmode & 4) printf("# %d cd cover icon url found  : %s \n",antal,value->u.string.ptr);
            if (strncmp("https://i.scdn.co/image/",value->u.string.ptr,24)==0) {
              strcpy(filename,value->u.string.ptr+24);
              if (strcmp(value->u.string.ptr,"")) {
                //getuserhomedir(downloadfilenamelong);
                strcpy(downloadfilenamelong,localuserhomedir);
                strcat(downloadfilenamelong,"/");
                strcat(downloadfilenamelong,spotify_gfx_path);
                strcat(downloadfilenamelong,filename);
                strcat(downloadfilenamelong,".jpg");
                // save name to db to next time
                strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
                if (!(file_exists(downloadfilenamelong))) {
                  // download icon image
                  download_image(value->u.string.ptr,downloadfilenamelong);
                }
              }
            }
          }
        }
        search_process_image=false;
      }
      if (search_process_href) {
        search_process_href=false;
      }
      // trackid
      if (art==2) {
        if ((depth==7) && (x==9)) {
          // get artist playid
          //printf("playlistid is set to = %s \n",value->u.string.ptr);
          //strcpy(stack[antal-1]->playlistid,value->u.string.ptr);
        }
      }
      if ((art==0) || (art==1)) {
        if ((depth==7) && (x==9)) {
          // get artist playid
          if (strlen(value->u.string.ptr)>16) {
            strcpy(artisid,value->u.string.ptr+15);
            strcpy(stack[antal-1]->playlistid,artisid);
          }
        }
      }
      // art 0 = artist / art 1 = songs
      if ((art==0) || (art==1)) {
        // get name
        if ((search_process_name) && (depth==7) && (x==6)) {
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) {
              stack[antal]=new (spotify_oversigt_type);
            }
            if (stack[antal]) {
              strncpy(stack[antal]->feed_name,value->u.string.ptr,80);
              strncpy(stack[antal]->feed_showtxt,value->u.string.ptr,80);
            }

            //if (debugmode) fprintf(stderr,"# %d Artist name found : %s gfx url found %s \n",antal,stack[antal]->feed_name,stack[antal]->feed_gfx_url);

            stack[antal]->type=2;                                            // set type artist
            antal++;
            antalplaylists++;
          }
        }
      }
      // albumid
      if (art==2) {
        if ((depth==9) && (x==5)) {
          if (antal==0) {                                                       // first record
            // first record back
            stack[antal]=new (spotify_oversigt_type);
            strcpy(stack[antal]->feed_name,"Back");
            strcpy(stack[antal]->feed_showtxt,"Back");
            stack[antal]->textureId=0;
            stack[antal]->intnr=0;                                            // back button
            strcpy(playlistid,"");                                            // no play id
            antal++;
          }
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) {
              stack[antal]=new (spotify_oversigt_type);
            }
            // get album playid
            if (value->u.string.ptr) {
              //printf("# %d - album playlistid %s icon url \n",antal,value->u.string.ptr,stack[antal]->feed_gfx_url);
              if (!(stack[antal])) {
                printf("******************************************* NO STACK error.\n");
              }
              strcpy(stack[antal]->playlistid,value->u.string.ptr);
            }
          }
        }
        // track list (artist name)
        if ((depth==9) && (x==7)) {
          //strcpy(stack[antal-1]->feed_artist,value->u.string.ptr);
        }
      }

      // spotify online type for en kunsner udgivelser (i potify db) (album)
      if (art==2) {
        if ((search_process_name) && (depth==9) && (x==7)) {
          if (antalplaylists<maxantal) {
            if (stack[antal]) {
              strncpy(stack[antal]->feed_name,value->u.string.ptr,80);
              strncpy(stack[antal]->feed_showtxt,value->u.string.ptr,80);
            }
            stack[antal]->type=3;                                            // set type artist
            antal++;
            antalplaylists++;
          }
        }
        // artist name
        if ((search_process_name) && (depth==10) && (x==3)) {
          strcpy(stack[antal-1]->feed_artist,value->u.string.ptr);
        }
      }
      search_process_name=false;
      if ((search_process_uri) && (depth==7) && (x==9)) {
        //printf("URI=%s\n",value->u.string.ptr);
        //if (stack[antal]) strcpy(stack[antal]->playlisturl,value->u.string.ptr);
        search_process_uri=false;
      }
      // get track nr
      if (search_process_track_nr) {
        search_process_track_nr=false;
      }
      break;
    case json_boolean:
      if (debug_json) printf("bool: %d\n", value->u.boolean);
      break;
  }
}



// ****************************************************************************************
// NOt working not in use
// download file from search result
//
// ****************************************************************************************

int spotify_class::get_search_result_online(char *searchstring,int type) {
  static const char *userfilename = "spotify_search_result.json";
  FILE *userfile;
  std::string auth_kode;
  std::string url;
  std::string response_string;
  std::string searchstring_do;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;                                             // q=%s&type=artist&limit=50
  std::string post;
  post="q=";
  post=post + searchstring;
  switch(type) {
    case 0: post=post + "&type=artist&limit=50";
            break;
    case 1: post=post + "&type=album&limit=50";
            break;
    case 2: post=post + "&type=playlist&limit=50";
            break;
    case 3: post=post + "&type=track&limit=50";
            break;
    default: post=post + "&type=artist&limit=50";
            break;
  };
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + spotifytoken;
  url="https://api.spotify.com/v1/search";
  printf("Get search result info.\n");
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    /* Add a custom header */
    header = curl_slist_append(header, "Accept: application/json");
    header = curl_slist_append(header, "Content-Type: application/json");
    header = curl_slist_append(header, "charsets: utf-8");
    header = curl_slist_append(header, auth_kode.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
    // set type post/put/get
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); /* !!! */
    userfile=fopen(userfilename,"w");
    if (userfile) {
      //curl_easy_setopt(curl, CURLOPT_WRITEDATA, userfile);
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
      return(httpCode);
    }
  }
  return(0);
}


//******************************************************************************
//
// remove dub in the stack
//
// Used by sort_stack_byname() internal
//
// *****************************************************************************

bool spotify_class:: do_cleanup_stack() {
  int i,j,antalfundet;
  int t,jj;
  bool done=false;
  bool swaped=false;
  done=false;
  jj=0;
  j=0;
  do {
    // printf("Undersøger %s j=%d  \n",stack[j]->feed_name,j);
    if (strcmp(stack[j]->feed_name,stack[j+1]->feed_name)==0) {
      printf(" Fundet dublet : %s j=%d \n",stack[j]->feed_name,j+1);
      antalfundet=1;
      while((strcmp(stack[j+antalfundet]->feed_name,stack[j+antalfundet+1]->feed_name)==0) && (j+antalfundet<antal-1)) {
        antalfundet++;
      }
      done=true;
    }
    j++;
    if (j>=antal-1) done=true;
  } while(done==false);
  // printf("j=%d Antalfundet=%d \n",j,antalfundet);
  if ((antalfundet) && ((jj+j+antalfundet)<antal)) {
    t=j+antalfundet;
    jj=j;
    swaped=true;
    while(t<antal-1) {
      strcpy(stack[jj]->feed_showtxt,stack[jj+antalfundet]->feed_showtxt);
      strcpy(stack[jj]->feed_name,stack[jj+antalfundet]->feed_name);
      strcpy(stack[jj]->feed_artist,stack[jj+antalfundet]->feed_artist);
      strcpy(stack[jj]->feed_desc,stack[jj+antalfundet]->feed_desc);
      strcpy(stack[jj]->feed_gfx_url,stack[jj+antalfundet]->feed_gfx_url);
      strcpy(stack[jj]->feed_release_date,stack[jj+antalfundet]->feed_release_date);
      strcpy(stack[jj]->playlistid,stack[jj+antalfundet]->playlistid);
      strcpy(stack[jj]->playlisturl,stack[jj+antalfundet]->playlisturl);
      stack[jj]->feed_group_antal=stack[jj+antalfundet]->feed_group_antal;
      stack[jj]->feed_path_antal=stack[jj+antalfundet]->feed_path_antal;
      stack[jj]->nyt=stack[jj+antalfundet]->nyt;
      stack[jj]->textureId=stack[jj+antalfundet]->textureId;
      stack[jj]->intnr=stack[jj+antalfundet]->intnr;
      stack[jj]->type=stack[jj+antalfundet]->type;
      jj++;
      t++;
    }
    antal--;
  }
  // printf("******************* j=%d \n",j);
  return swaped;
}


//******************************************************************************
//
// sort stack by name
//
// *****************************************************************************


void spotify_class::sort_stack_byname() {
  int i,j,antalfundet;
  int t,jj;
  bool done=true;
  bool totaldone=false;
  struct spotify_oversigt_type tmp_stack;
  do {
    done=true;
    for(j=0;j<antal-1;j++) {
      if (strcmp(stack[j]->feed_name,stack[j+1]->feed_name)>0) {
        done=false;
        // save data
        strcpy(tmp_stack.feed_showtxt,stack[j]->feed_showtxt);
        strcpy(tmp_stack.feed_name,stack[j]->feed_name);
        strcpy(tmp_stack.feed_artist,stack[j]->feed_artist);
        strcpy(tmp_stack.feed_desc,stack[j]->feed_desc);
        strcpy(tmp_stack.feed_gfx_url,stack[j]->feed_gfx_url);
        strcpy(tmp_stack.feed_release_date,stack[j]->feed_release_date);
        strcpy(tmp_stack.playlistid,stack[j]->playlistid);
        strcpy(tmp_stack.playlisturl,stack[j]->playlisturl);
        tmp_stack.feed_group_antal=stack[j]->feed_group_antal;
        tmp_stack.feed_path_antal=stack[j]->feed_path_antal;
        tmp_stack.nyt=stack[j]->nyt;
        tmp_stack.textureId=stack[j]->textureId;
        tmp_stack.intnr=stack[j]->intnr;
        tmp_stack.type=stack[j]->type;
        // swap data
        strcpy(stack[j]->feed_showtxt,stack[j+1]->feed_showtxt);
        strcpy(stack[j]->feed_name,stack[j+1]->feed_name);
        strcpy(stack[j]->feed_artist,stack[j+1]->feed_artist);
        strcpy(stack[j]->feed_desc,stack[j+1]->feed_desc);
        strcpy(stack[j]->feed_gfx_url,stack[j+1]->feed_gfx_url);
        strcpy(stack[j]->feed_release_date,stack[j+1]->feed_release_date);
        strcpy(stack[j]->playlistid,stack[j+1]->playlistid);
        strcpy(stack[j]->playlisturl,stack[j+1]->playlisturl);
        stack[j]->feed_group_antal=stack[j+1]->feed_group_antal;
        stack[j]->feed_path_antal=stack[j+1]->feed_path_antal;
        stack[j]->nyt=stack[j+1]->nyt;
        stack[j]->textureId=stack[j+1]->textureId;
        stack[j]->intnr=stack[j+1]->intnr;
        stack[j]->type=stack[j+1]->type;
        // restore saved
        strcpy(stack[j+1]->feed_showtxt,tmp_stack.feed_showtxt);
        strcpy(stack[j+1]->feed_name,tmp_stack.feed_name);
        strcpy(stack[j+1]->feed_artist,tmp_stack.feed_artist);
        strcpy(stack[j+1]->feed_desc,tmp_stack.feed_desc);
        strcpy(stack[j+1]->feed_gfx_url,tmp_stack.feed_gfx_url);
        strcpy(stack[j+1]->feed_release_date,tmp_stack.feed_release_date);
        strcpy(stack[j+1]->playlistid,tmp_stack.playlistid);
        strcpy(stack[j+1]->playlisturl,tmp_stack.playlisturl);
        stack[j+1]->feed_group_antal=tmp_stack.feed_group_antal;
        stack[j+1]->feed_path_antal=tmp_stack.feed_path_antal;
        stack[j+1]->nyt=tmp_stack.nyt;
        stack[j+1]->textureId=tmp_stack.textureId;
        stack[j+1]->intnr=tmp_stack.intnr;
        stack[j+1]->type=tmp_stack.type;
      }
    }
  } while (done==false);

  printf("Sorted named...\n");
  for (j=0;j<antal;j++) printf("# %d Names %s \n",j,stack[j]->feed_name);
  printf("\n");
  totaldone=false;
  do {
    totaldone=do_cleanup_stack();
    if (totaldone) printf("Swaped \n");
  } while (totaldone);

  printf("New list...\n");
  for (j=0;j<antal;j++) printf("# %d Names %s \n",j,stack[j]->feed_name);

}


// ****************************************************************************************
//
// called from thread in main (IN USE)
//
// search for playlist artist or song or album by type
// 0 = artist (default)
// 1 = album
// 2 = playlist
// 3 = song
//
// ****************************************************************************************


int spotify_class::opdatere_spotify_oversigt_searchtxt_online(char *keybuffer,int type) {
  char sqlselect[2048];
  char homedir[1024];
  char call[4096];
  char searchstring[4096];
  bool rss_update=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  const char *database = (char *) "mythtvcontroller";
  bool online;
  int getart=0;
  int curl_error;
  bool loadstatus=true;
  bool dbexist=false;
  GLuint texture;
  json_char* json;
  json_value* value;
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  int file_size;
  char *file_contents=NULL;
  antal=0;
  int i=0;
  int ii=0;
  strcpy(searchstring,"");
  // swap space to %20
  while(i<strlen(keybuffer)) {
    if (keybuffer[i]!=32) {
      *(searchstring+ii)=*(keybuffer+i);
      ii++;
    } else {
      strcat(searchstring,"\%20");
      ii+=3;
    }
    searchstring[ii]='\0';
    i++;
  }
  // search string is now coded to web call
  curl_error=0;
  switch(type) {
            // search artist name
    case 0: snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=artist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search album name
    case 1: snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=album&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search playlist name
    case 2: snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=playlist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search track/song name
    case 3: snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=track&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // default search artist name
    default:snprintf(call,sizeof(call),"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=artist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
  }
  curl_error=system(call);
  if (curl_error!=0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    printf("call= %s \n",call);
    return 1;
  }
  // get file info
  stat("spotify_search_result.json", &filestatus);                              // get file info
  file_size = filestatus.st_size;                                               // get filesize
  file_contents = (char*) malloc(filestatus.st_size);
  json_file = fopen("spotify_search_result.json", "rt");
  // if filesize is 0 drop
  if (json_file == NULL) {
    fprintf(stderr, "Unable to open spotify_search_result.json\n");
    free(file_contents);                                                        //
    return 1;
  }
  // if unable to read file
  if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
    fprintf(stderr, "Unable to read spotify spotify_search_result content of spotify_search_result.json\n");
    write_logfile("Unable to read spotify spotify_search_result content of spotify_search_result.json");
    fclose(json_file);
    free(file_contents);                                                        //
    return 1;
  }
  fclose(json_file);
  json = (json_char*) file_contents;
  // parse from root
  value = json_parse(json,file_size);                                           // parser
  switch(searchtype) {
    case 0: printf("Search for playlist\n");
            break;
    case 1: printf("Search for songs\n");
            break;
    case 2: printf("Search for artist\n");
            break;
    case 3: printf("Search for songs\n");
            break;
  }
  // fill stack array
  if (type==0) {
    search_process_value(value, 0,0,type);                                     // fill stack array playlist
  }
  if (type==1) {
    search_process_value(value, 0,0,type);                                     // fill stack array songs
  }
  if (type==2) {
    search_process_value(value, 0,0,type);                                     // fill stack array artist
  }
  if (type==3) {
    search_process_value(value, 0,0,2);                                     // fill stack array songs
  }
  json_value_free(value);                                                       // json clean up
  free(file_contents);                                                          //
  return(antal);
}



// ****************************************************************************************
//
// thread web loader (loading all icons)
//
// ****************************************************************************************

void *load_spotify_web(void *data) {
  if (debugmode & 4) fprintf(stderr,"Start spotify loader thread\n");
  //streamoversigt.loadweb_stream_iconoversigt();
  if (debugmode & 4) fprintf(stderr,"Stop spotify loader thread\n");
}



// ****************************************************************************************
//
// load image
//
// ****************************************************************************************

int LoadImage(char *filename) {
    ILuint    image;
    ILboolean success;
    ilGenImages(1, &image);    /* Generation of one image name */
    ilBindImage(image);        /* Binding of image name */
    /* Loading of the image filename by DevIL */
    if ( success = ilLoadImage(filename) ) {
      success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
      if (!success) return -1;
    } else return -1;
    return image;
}


// ****************************************************************************************
//
// loading spotify songs gfx in array.
//
// ****************************************************************************************

void spotify_class::settextureidfile(int nr,char *filename) {
    if (stack[nr]->textureId==0) stack[nr]->textureId=loadTexture ((char *) filename);
}


// ****************************************************************************************
//
// loading spotify songs gfx in array.
// if filename is url then change it to file name by the last name in the source http filename path
//
// ****************************************************************************************

int spotify_class::load_spotify_iconoversigt() {
  int nr=0;
  int loadstatus;
  char *imagenamepointer;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  this->gfx_loaded=false;                                                           // set loaded flag to false
  if (debugmode & 4) printf("spotify icon loader.\n");
  while(nr<=streamantal()) {
    if (debugmode & 4) printf("Loading texture nr %-4d Title %40s  icon path %s\n",nr,stack[nr]->feed_name,stack[nr]->feed_gfx_url);
    if ((stack[nr]) && (strcmp(stack[nr]->feed_gfx_url,"")!=0)) {
      if (stack[nr]->textureId==0) {
        // if url
        if (strncmp("http",stack[nr]->feed_gfx_url,4)==0) {
          imagenamepointer=strrchr(stack[nr]->feed_gfx_url,'\/');
          if ((imagenamepointer) && (strlen(imagenamepointer)<1990)) {
            //getuserhomedir(tmpfilename);
            strcpy(tmpfilename,localuserhomedir);
            strcat(tmpfilename,"/spotify_gfx/");
            strcat(tmpfilename,imagenamepointer+1);
            strcat(tmpfilename,".jpg");
            stack[nr]->textureId=loadTexture (tmpfilename);
          }
        } else stack[nr]->textureId=loadTexture (stack[nr]->feed_gfx_url);          // load texture
      }
    }
    nr++;
  }
  // set loaded flag in class
  if (nr>0) this->gfx_loaded=true; else this->gfx_loaded=false;
  if (debugmode & 4) {
    if (gfx_loaded) fprintf(stderr,"spotify download done. \n");
    else fprintf(stderr,"spotify download error. \n");
  }
  gfx_loaded=true;
  return(1);
}


// ****************************************************************************************
//
// get active play device name
//
// ****************************************************************************************

char *spotify_class::get_active_spotify_device_name() {
  return(spotify_device[active_spotify_device].name);
}


// ****************************************************************************************
//
// Select device to play on
//
// ****************************************************************************************


void spotify_class::select_device_to_play() {
  static float select_device_to_playfader=1.0;
  static int playfader_timer=500;
  float yof=orgwinsizey/2+50;                                        // start ypos
  float xof=orgwinsizex/2+200;
  // background size (window)
  float winsizex=400;
  float winsizey=300;
  int i=0;
  int xof3=xof+330;
  int yof3=(yof+184)-(i*30);
  int winsizey3=400;
  int winsizex3=20;
  int xof2=xof+330;
  int yof2=(yof+184)-(i*30);
  int winsizey2=53;
  int winsizex2=43;
  char temptxt[1024];
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturemovieinfobox);
  glColor4f( 1.0f, 1.0f, 1.0f, select_device_to_playfader);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
  glTexCoord2f(0, 1); glVertex3f( xof+10, yof+winsizey-20, 0.0);
  glTexCoord2f(1, 1); glVertex3f( xof+winsizex-10, yof+winsizey-20 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( xof+winsizex-10, yof+10 , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor4f(1.0f, 1.0f, 1.0f,select_device_to_playfader);
  glTranslatef(xof+60,yof+240,0);
  glRasterPos2f(0.0f, 0.0f);
  glScalef(configdefaultstreamfontsize+8, configdefaultstreamfontsize+8, 1.0);
  glcRenderString("Select play device");
  glPopMatrix();
  //active_spotify_device
  while(strcmp(spotify_device[i].id,"")!=0) {
    xof3=xof+14;
    yof3=(yof+184)-(i*30);
    winsizey3=60;
    winsizex3=365;
    // make bar backgound
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,_texturemovieinfobox);
    glColor4f( 1.0f, 1.0f, 1.0f, select_device_to_playfader);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40+i);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( xof3+10, yof3+10, 0.0);
    glTexCoord2f(0, 1); glVertex3f( xof3+10, yof3+winsizey3-20, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xof3+winsizex3-10, yof3+winsizey3-20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( xof3+winsizex3-10, yof3+10 , 0.0);
    glEnd();
    glPopMatrix();
    // draw text
    glPushMatrix();
    glTranslatef(xof+30,(yof+200)-(i*30),0);
    glRasterPos2f(0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glScalef(configdefaultstreamfontsize+2, configdefaultstreamfontsize+2, 1.0);
    if (active_spotify_device>=0) strcpy( temptxt , spotify_device[i].name );
    else strcpy( temptxt , "None" );
    if ( i == active_spotify_device ) glColor4f( 1.0f, 1.0f, 0.0f, select_device_to_playfader); else glColor4f( 1.0f, 1.0f, 1.0f, select_device_to_playfader);
    glcRenderString(temptxt);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(xof+180,(yof+200)-(i*30),0);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(configdefaultstreamfontsize+2, configdefaultstreamfontsize+2, 1.0);
    glcRenderString(" - ");
    glcRenderString(spotify_device[i].devtype);
    glPopMatrix();
    //draw icon
    xof2=xof+330;
    yof2=(yof+184)-(i*30);
    winsizey2=53;
    winsizex2=43;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    if (strncmp(spotify_device[i].devtype,"Smartphone",10)==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
    else if (strncmp(spotify_device[i].devtype,"Computer",8)==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
    else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
    glColor4f( 1.0f, 1.0f, 1.0f, select_device_to_playfader);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40+i);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( xof2+10, yof2+10, 0.0);
    glTexCoord2f(0, 1); glVertex3f( xof2+10, yof2+winsizey2-20, 0.0);
    glTexCoord2f(1, 1); glVertex3f( xof2+winsizex2-10, yof2+winsizey2-20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( xof2+winsizex2-10, yof2+10 , 0.0);
    glEnd();
    glPopMatrix();
    i++;
  }
  // fader
  if (playfader_timer>0) playfader_timer--;
  if (playfader_timer==0) {
    select_device_to_playfader=select_device_to_playfader-0.05f;
    if (select_device_to_playfader<0.0f) {
      select_device_to_playfader=1.0;
      do_select_device_to_play=false;
      playfader_timer=500;
    }
  }
}



// ****************************************************************************************
//
// show spotify overview
//
// ****************************************************************************************

void spotify_class::show_spotify_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected) {
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
    char word[200];
    static char downloadfilename_last[1024];
    int antal_loaded=0;
    static int stream_oversigt_loaded_done=0;
    GLuint texture;
    static GLuint last_texture;
    char *base,*right_margin;
    int length,width;
    int pline=0;
    // last loaded filename
    if (spotify_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
    // load icons
    if (this->search_loaded) {
      this->search_loaded=false;
      printf("Searech loaded done. Loading icons\n");
      spotify_oversigt.load_spotify_iconoversigt();                       // load icons
    }
    // draw icons
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while((i<lstreamoversigt_antal) && (i+sofset<antalplaylists) && (stack[i+sofset]!=NULL)) {
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
      if (stack[i+sofset]->textureId) {
        // stream icon
        //  glEnable(GL_TEXTURE_2D);
        //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glBindTexture(GL_TEXTURE_2D,spotify_icon_border);                               // normal icon then the spotify have icon
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
        glPushMatrix();
        // indsite draw icon
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        if (tema==5) {
          glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        } else {
          glTexCoord2f(0, 0); glVertex3f( xof+12, yof+12, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+12,yof+buttonsizey-22, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-12, yof+buttonsizey-22 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-12, yof+12 , 0.0);
        }
        glEnd();
        glPopMatrix();
      } else {
        // no draw default icon
        glPushMatrix();
        // indsite draw radio station icon
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,_textureIdback);
          } else glBindTexture(GL_TEXTURE_2D,normal_icon);
        } else {
          if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,normal_icon);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
        // show nyt icon note
        if (stack[i+sofset]->nyt) {
          glBindTexture(GL_TEXTURE_2D,newstuf_icon);
          glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof+10+130, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10+130,yof+66-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+66-10+130, yof+66-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+66-10+130, yof+10 , 0.0);
          glEnd();
        }
        glPopMatrix();
      }
      // draw numbers in group
      if (stack[i+sofset]->feed_group_antal>1) {
        // show numbers in group
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(xof+22,yof+14,0);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        sprintf(temptxt,"Feeds %-4d",stack[i+sofset]->feed_group_antal);
        glcRenderString(temptxt);
        glPopMatrix();
      }
      // show text of element
      glPushMatrix();
      pline=0;
      glTranslatef(xof+20,yof-10,0);
      glDisable(GL_TEXTURE_2D);
      glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      base=temptxt;
      length=strlen(temptxt);                               // get length
      width = 19;                                           // max length to show
      bool stop=false;                                      // done
      while(*base) {
        // if text can be on line
        if(length <= width) {
          glTranslatef((width/5)-(strlen(base)/4),0.0f,0.0f);
          glcRenderString(base);
          pline++;
          break;
        }
        right_margin = base+width;
        while((!isspace(*right_margin)) && (stop==false)) {
          right_margin--;
          if (right_margin == base) {
            right_margin += width;
            while(!isspace(*right_margin)) {
              if (*right_margin == '\0') break;
              else stop=true;
              right_margin++;
            }
          }
        }
        if (stop) *(base+width)='\0';
        *right_margin = '\0';
        glcRenderString(base);
        pline++;
        glTranslatef(-(1.0f+(strlen(base)/2)),-pline*1.2f,0.0f);
        //glTranslatef(1.0f-(strlen(base)/1.6f)+1,-pline*1.2f,0.0f);
        length -= right_margin-base+1;                         // +1 for the space
        base = right_margin+1;
        if (pline>=2) break;
      }
      glPopMatrix();
      // next button
      i++;
      xof+=(buttonsize+10);
    }
    /*
    // no records loaded error
    if ((i==0) && (antal_spotify_streams()==0)) {
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+400, 200+150 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+400, 200 , 0.0);
      glEnd();

      glPushMatrix();
      xof=700;
      yof=260;
      glTranslatef(xof, yof ,0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(22.0, 22.0, 1.0);
      if (spotify_oversigt.search_spotify_online_done) glcRenderString("   Loading ..."); else glcRenderString("   Please run update ...");
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
    */
}




// ****************************************************************************************
//
// show search/create playlist spotify overview
//
// ****************************************************************************************

void spotify_class::show_spotify_search_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring) {
    int j,ii,k,pos;
    int buttonsize=200;                                                         // button size
    float buttonsizey=180.0f;                                                   // button size
    float yof=orgwinsizey-(buttonsizey*2.0)+10;                                    // start ypos 2.5
    float xof=0.0f;
    int lstreamoversigt_antal=8*4;
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
    char *base,*right_margin;
    int length,width;
    int pline=0;
    static time_t rawtime;
    static time_t last_rawtime=0;
    static bool cursor=true;
    rawtime=time(NULL);                                                         // hent now time
    if (last_rawtime==0) {
      last_rawtime=rawtime;
    }
    if (rawtime>(last_rawtime+1)) {
      cursor=!cursor;
      last_rawtime=rawtime;
    }
    // last loaded filename
    if (spotify_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
    // top search text box + cursor
    float yof_top=orgwinsizey-(buttonsizey*1)+20;                                    // start ypos
    float xof_top=((orgwinsizex-buttonsize)/2)-(1200/2);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // type of search
    switch (searchtype) {
      case 0: glBindTexture(GL_TEXTURE_2D,big_search_bar_artist);
              break;
      case 1: glBindTexture(GL_TEXTURE_2D,big_search_bar_albumm);
              break;
      case 2: glBindTexture(GL_TEXTURE_2D,big_search_bar_playlist);
              break;
      case 3: glBindTexture(GL_TEXTURE_2D,big_search_bar_track);
              break;
      default:glBindTexture(GL_TEXTURE_2D,big_search_bar_artist);
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
    glTranslatef(xof_top+40,yof_top+50,0);
    glDisable(GL_TEXTURE_2D);
    glScalef(120, 120, 1.0);
    glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
    glRasterPos2f(0.0f, 0.0f);
    if (strcmp(searchstring,"")!=0) glcRenderString(searchstring);
    // cursor
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (cursor) glcRenderString("_"); else glcRenderString(" ");
    glPopMatrix();
    //glPopMatrix();

    if (this->search_loaded) {
      this->search_loaded=false;
      printf("Searech loaded done. Loading icons\n");
      spotify_oversigt.load_spotify_iconoversigt();                       // load icons
    }
    // draw icons
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    while((i<lstreamoversigt_antal) && (i+sofset<antalplaylists) && (stack[i+sofset]!=NULL)) {
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

      //printf("nr %d feed gfx url : %s \n",i+sofset,stack[i+sofset]->feed_gfx_url);
      // search loader done

      if (stack[i+sofset]->textureId) {
        // border icon
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,spotify_icon_border);                               // normal icon then the spotify have icon
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
        glPushMatrix();
        // indsite draw icon rss gfx
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,stack[i+sofset]->textureId);
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        if (tema==5) {
          glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        } else {
          glTexCoord2f(0, 0); glVertex3f( xof+12, yof+12, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+12,yof+buttonsizey-22, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-12, yof+buttonsizey-22 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-12, yof+12 , 0.0);
        }
        glEnd();
        // show nyt icon note
        if (stack[i+sofset]->nyt) {
          glBindTexture(GL_TEXTURE_2D,newstuf_icon);
          //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof+10+130, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10+130,yof+66-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+66-10+130, yof+66-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+66-10+130, yof+10 , 0.0);
          glEnd();
        }
        glPopMatrix();
      } else {
        // no draw default icon
        glPushMatrix();
        // indsite draw radio station icon
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) {
            glBindTexture(GL_TEXTURE_2D,backicon);
          } else {
            if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,empty_icon);
          }
        } else {
          if (stack[i+sofset]->type==1) glBindTexture(GL_TEXTURE_2D,song_icon); else glBindTexture(GL_TEXTURE_2D,empty_icon);
        }
        glLoadName(100+i+sofset);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
        // show nyt icon note
        /*
        if (stack[i+sofset]->nyt) {
          glBindTexture(GL_TEXTURE_2D,newstuf_icon);
          glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( xof+10+130, yof+10, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+10+130,yof+66-20, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+66-10+130, yof+66-20 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+66-10+130, yof+10 , 0.0);
          glEnd();
        }
        */
        glPopMatrix();
      }
      // draw numbers in group
      if (stack[i+sofset]->feed_group_antal>1) {
        // show numbers in group
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(xof+22,yof+14,0);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
        glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        sprintf(temptxt,"Feeds %-4d",stack[i+sofset]->feed_group_antal);
        glcRenderString(temptxt);
        glPopMatrix();
      }
      // show text of element
      glPushMatrix();
      pline=0;
      glTranslatef(xof+20,yof-10,0);
      glDisable(GL_TEXTURE_2D);
      glScalef(configdefaultstreamfontsize, configdefaultstreamfontsize, 1.0);
      glColor4f(1.0f, 1.0f, 1.0f,1.0f);
      glRasterPos2f(0.0f, 0.0f);
      strcpy(temptxt,stack[i+sofset]->feed_showtxt);        // text to show
      base=temptxt;
      length=strlen(temptxt);
      width = 19;
      bool stop=false;
      while(*base) {
        // if text can be on line
        if(length <= width) {
          glTranslatef((width/5)-(strlen(base)/4),0.0f,0.0f);
          glcRenderString(base);
          pline++;
          break;
        }
        right_margin = base+width;
        while((!isspace(*right_margin)) && (stop==false)) {
          right_margin--;
          if (right_margin == base) {
            right_margin += width;
            while(!isspace(*right_margin)) {
              if (*right_margin == '\0') break;
              else stop=true;
              right_margin++;
            }
          }
        }
        if (stop) *(base+width)='\0';
        *right_margin = '\0';
        glcRenderString(base);
        pline++;
        glTranslatef(1.0f-(strlen(base)/1.6f)+1,-pline*1.2f,0.0f);
        length -= right_margin-base+1;                         // +1 for the space
        base = right_margin+1;
        if (pline>=2) break;
      }
      glPopMatrix();
      // next button
      i++;
      xof+=(buttonsize+10);
    }
    // no records loaded error
    if ((i==0) && (antal_spotify_streams()==0)) {
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glBindTexture(GL_TEXTURE_2D,_textureIdloading);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f((orgwinsizex/3), 200 , 0.0);
      glTexCoord2f(0, 1); glVertex3f((orgwinsizex/3), 200+150, 0.0);
      glTexCoord2f(1, 1); glVertex3f((orgwinsizex/3)+400, 200+150 , 0.0);
      glTexCoord2f(1, 0); glVertex3f((orgwinsizex/3)+400, 200 , 0.0);
      glEnd();
      glPushMatrix();
      xof=700;
      yof=260;
      glTranslatef(xof, yof ,0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(22.0, 22.0, 1.0);
      glcRenderString("   Loading ...");
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
    bool vis_band_name=true;
    if (vis_band_name) {
      if (strcmp(overview_show_band_name,"")!=0) {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,spotify_pil);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-60), 14 , 0.0);
        glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-60), 14+46, 0.0);
        glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-60)+60, 14+46 , 0.0);
        glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-60)+60, 14 , 0.0);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef((orgwinsizex/2)-(130+(strlen(overview_show_band_name)*10)), 30 ,0.0f);
        glScalef(22.0, 22.0, 1.0);
        glcRenderString(overview_show_band_name);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(((orgwinsizex/2)+30), 30 ,0.0f);
        glDisable(GL_TEXTURE_2D);
        glScalef(22.0, 22.0, 1.0);
        glcRenderString("Collection");
        glPopMatrix();
      }
    }
}


// ****************************************************************************************
//
// ********************* show setup spotify stuf like dev and clientid/secrect ************
//
// ****************************************************************************************

void spotify_class::show_setup_spotify() {
    int i;
    int winsizx=100;
    struct tm *xmlupdatelasttime;
    int winsizy=300;
    int xpos=0;
    int ypos=0;
    char text[200];
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int dev_nr=0;
    char temptxt[200];
    // ICON TEXT pos
    const int icon_text_posx1=510;
    const int icon_text_posy1=320;
    const int icon_text_posx2=510+120;
    const int icon_text_posy2=320;
    const int icon_text_posx3=510+120+120;
    const int icon_text_posy3=320;
    const int icon_text_posx4=510+120+120+120;
    const int icon_text_posy4=320;
    const int icon_text_posx5=510;
    const int icon_text_posy5=180;
    const int icon_text_posx6=510+120;
    const int icon_text_posy6=180;
    const int icon_text_posx7=510+120+120;
    const int icon_text_posy7=180;
    const int icon_text_posx8=510+120+120+120;
    const int icon_text_posy8=180;
    std::string devname;
    //
    static int spotify_device_antal=0;
    static bool first_time_update=true;
    char *database = (char *) "mythtvcontroller";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (first_time_update) {
      first_time_update=false;
      conn = mysql_init(NULL);
      if (conn) {
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,"SELECT device_name,active,devtype,intnr from mythtvcontroller.spotify_device limit 10");
        res = mysql_store_result(conn);
        if (res) {
          while (((row = mysql_fetch_row(res)) != NULL) && (dev_nr<10)) {
            strcpy(spotify_device[dev_nr].name,row[0]);
            if (strcmp(row[1],"1")==0) {
              spotify_device[dev_nr].is_active=true;
            } else {
              spotify_device[dev_nr].is_active=false;
            }
            strcpy(spotify_device[dev_nr].devtype,row[2]);
            // is one the default device set in config
            // and exist in db
            if ((strcmp(active_default_play_device_name,"")!=0) && (strcmp(active_default_play_device_name,row[0])==0)) {
              active_default_play_device=dev_nr;
              //spotify_device[0].is_active=true;
              //strcpy(spotify_device[0].name,active_default_play_device_name);
            } else {
              /// set active device
              if ((active_default_play_device!=-1) && (spotify_device[dev_nr].is_active)) {
                // is default defined in config set it active
                if (strcmp(active_default_play_device_name,"")!=0) {
                  if (strcmp(active_default_play_device_name,row[1])==0) active_default_play_device=dev_nr;
                } else {
                  active_default_play_device=dev_nr;
                }
              }
            }
            dev_nr++;
            spotify_device_antal++;
          }
        }
      }

    }
    // spotify setup
    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBindTexture(GL_TEXTURE_2D,setuprssback);
    glBegin(GL_QUADS);
    if (spotify_device_antal<4) {
      glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4)-50,100 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4)-50,800 , 0.0);
      glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+550,800 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+550,100 , 0.0);
    } else {
      glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4)-50,10 , 0.0);
      glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4)-50,800 , 0.0);
      glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+550,800 , 0.0);
      glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+550,10 , 0.0);
    }
    glEnd();
    glPopMatrix();
    // top text
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(550, 750, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(0.8f, 0.8f, 0.8f);
    glScalef(27.0, 27.0, 1.0);
    glcRenderString("Spotify Account setup");
    glPopMatrix();
    // close buttons
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    if (spotify_device_antal<4) {
      winsizx=100;
      winsizy=100;
      xpos=310;
      ypos=-10;
    } else {
      winsizx=100;
      winsizy=100;
      xpos=310;
      ypos=-70;
    }
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
    glPopMatrix();
    // start af input felter
    glPushMatrix();
    winsizx=310;
    winsizy=30;
    xpos=300;
    ypos=500;
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    // start af input felter
    winsizx=310;
    winsizy=30;
    xpos=300;
    ypos=450;
    glEnable(GL_TEXTURE_2D);
    glColor3f(0.7f, 0.7f, 0.7f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(680 , 600 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 650, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (do_show_setup_select_linie==0) glColor4f(1.0f, 1.0f, 0.0f, 1.0f); else glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Client ID        ");
    glcRenderString(spotify_client_id);
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 600, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (do_show_setup_select_linie==1) glColor4f(1.0f, 1.0f, 0.0f, 1.0f); else glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Client Secrect   ");
    glcRenderString(spotify_secret_id);
    if ((keybuffer) && (do_show_setup_select_linie>=0)) showcoursornow(301,500-(do_show_setup_select_linie*50),strlen(keybuffer));
    glPopMatrix();
    // show(select play use spotify local (start spotify understartup))
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 550, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (do_show_setup_select_linie==2) glColor4f(1.0f, 1.0f, 0.0f, 1.0f); else glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Use Spotify client ");
    if (global_use_spotify_local_player) glcRenderString("Yes "); else glcRenderString("No  ");
    if ((keybuffer) && (do_show_setup_select_linie>=0)) showcoursornow(301,500-(do_show_setup_select_linie*50),strlen(keybuffer));
    glPopMatrix();
    //
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 500, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Active play device ");
    sprintf(temptxt," devid = %d ",active_default_play_device);
    glcRenderString(temptxt);
    if (active_default_play_device==-1) glcRenderString("None");
    else glcRenderString(spotify_device[active_default_play_device].name);
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 460, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Device avable ");
    glPopMatrix();
    if (strcmp(spotify_device[0].name,"")!=0) {
      // playerid 1
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=150;
      ypos=200;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[0].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[0].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[0].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(10);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx1, icon_text_posy1, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[0].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(spotify_device[1].name,"")!=0) {
      // playerid 2
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=270;
      ypos=200;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[1].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[1].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[1].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(11);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx2, icon_text_posy2, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[1].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(spotify_device[2].name,"")!=0) {
      // playerid 3
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=390;
      ypos=200;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[2].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[2].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[2].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(12);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx3, icon_text_posy3, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[2].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(spotify_device[3].name,"")!=0) {
      // playerid 4
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=510;
      ypos=200;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[3].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[3].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[3].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(13);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx4, icon_text_posy4, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[3].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    // row 2
    if (strcmp(spotify_device[4].name,"")!=0) {
      // playerid 5
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=150;
      ypos=70;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[4].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[4].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[4].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(14);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx5, icon_text_posy5, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[4].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(spotify_device[5].name,"")!=0) {
      // playerid 6
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=270;
      ypos=70;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[4].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[4].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[4].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(15);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx6, icon_text_posy6, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[5].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }

    if (strcmp(spotify_device[6].name,"")!=0) {
      // playerid 7
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=270+120;
      ypos=70;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[5].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[5].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[5].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(16);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx7, icon_text_posy7, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[6].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(spotify_device[7].name,"")!=0) {
      // playerid 8
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=270+120+120;
      ypos=70;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[6].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[6].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[6].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glLoadName(17);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(icon_text_posx8, icon_text_posy8, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      devname=spotify_device[7].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
}

// ****************************************************************************************
//
// set default play device
//
// ****************************************************************************************

void spotify_class::set_default_device_to_play(int nr) {
   active_spotify_device=nr;
   active_default_play_device=nr;
}

//#endif
