//
// All tidal functios
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
#include <string.h>
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


const char *tidal_gfx_path = "tidal_gfx/";

const int tidal_pathlength=80;
const int tidal_namelength=80;
const int tidal_desclength=2000;
const int feed_url=2000;


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
extern GLuint mobileplayer_icon;                   // mobile pplayer icon
extern GLuint pcplayer_icon;                       // pc player icon
extern GLuint unknownplayer_icon;
extern int tema;       					                            // aktiv tema (default is 3)
extern GLuint normal_icon;
extern GLuint _textureIdback;
extern GLuint newstuf_icon;
extern GLuint empty_icon;



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

bool process_token_token1=false;
bool process_token_token2=false;
bool tidal_debug_json=false;


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
        if (tidal_debug_json) fprintf(stdout,"none\n");
        break;
      case json_object:
        process_object_token(value, depth+1);
        break;
      case json_array:
        process_array_token(value, depth+1);
        break;
      case json_integer:
        //printf("int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        if (tidal_debug_json) fprintf(stdout,"double: %f\n", value->u.dbl);
        break;
      case json_string:
        //printf("Value found = %s x = %d deepth = %d \n ",value->u.string.ptr,x,depth);
        if (process_token_token1) {
          process_token_token1=false;
        }
        if (process_token_token2) {
          process_token_token2=false;
        }
        break;
      case json_boolean:
        if (tidal_debug_json) fprintf(stdout,"bool: %d\n", value->u.boolean);
        break;
    }
}





// ****************************************************************************************
// IN USE
//
// X-Tidal-Token have to token
//
// MEW web server handler (internal function)
// Login web https://account.tidal.com/login
//
// ****************************************************************************************

static void new_ev_handler(struct mg_connection *c, int ev, void *ev_data) {
  char user_token[1024];
  const char *returncode_pointer;
  struct mg_serve_http_opts opts;
  struct http_message *hm = (struct http_message *) ev_data;
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      printf("REQUEST Call \n");
      if (mg_strncmp( hm->uri,mg_mk_str_n("/call",5),5) == 0) {
        // Get server return code
        returncode_pointer = strstr( hm->uri.p , "code="); // mg_mk_str_n("code=",5));
        if (returncode_pointer) {
          // get user user_token
        }
        fprintf(stdout,"Got reply from server : %s \n", (mg_str) hm->uri);
        mg_serve_http(c, (struct http_message *) ev_data, s_http_server_opts);          /* Serve static content */
      } else {
        // else show normal indhold
        memset(&opts, 0, sizeof(opts));
        opts.document_root = ".";                                                       // Serve files from the current directory
        mg_serve_http(c, (struct http_message *) ev_data, s_http_server_opts);          /* Serve static content */
      }
      // We have received an HTTP request. Parsed request is contained in `hm`.
      // Send HTTP reply to the client which shows full original request.
      //mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
      //mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
      break;
    case MG_EV_HTTP_REPLY:
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
// web server handler (internal function)
//
// ****************************************************************************************



static void tidal_server_ev_handler(struct mg_connection *c, int ev, void *ev_data) {
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
  strcpy(data,"");
  //strcpy(data,spotify_oversigt.spotify_client_id);
  //strcat(data,":");
  //strcat(data,spotify_oversigt.spotify_secret_id);

  printf("Calling web server \n");

  char sed[]="cat spotify_access_token.txt | grep -Po '\"\\K[^:,\"}]*' | grep -Ev 'access_token|token_type|Bearer|expires_in|refresh_token|scope' > spotify_access_token2.txt";
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      // from spotify servers
      // is callback call
      if (mg_strncmp( hm->uri,mg_mk_str_n("/callback",9),9) == 0) {
        if (debugmode) fprintf(stdout,"Got reply server : %s \n", (mg_str) hm->uri);
        p = strstr( hm->uri.p , "code="); // mg_mk_str_n("code=",5));
        // get sptify code from server
        if (p) {
          pspace=strchr(p,' ');
          if (pspace) {
            codel=(pspace-p);
            strncpy(user_token,p+5,pspace-p);
            *(user_token+(pspace-p))='\0';
          }
          user_token[codel-4]='\0';
        }
        //sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code,user_token,spotify_oversigt.spotify_client_id,spotify_oversigt.spotify_secret_id);
        //printf("sql curl : %s \n ",sql);

        sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=https://account.tidal.com/login/tidal/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.tidal.com/api/token > tidal_access_token.txt",base64_code,user_token,tidal_oversigt->tidal_client_id,tidal_oversigt->tidal_secret_id);

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
            //spotify_oversigt.spotify_set_token(token_string,token_refresh);
            fclose(tokenfile);
            free(file_contents);
            if (strcmp(token_string,"")!=0) {
              //spotify_oversigt.spotify_get_user_id();                                   // get user id
              //spotify_oversigt.active_spotify_device=spotify_oversigt.spotify_get_available_devices();
              // set default spotify device if none
              //if (spotify_oversigt.active_spotify_device==-1) spotify_oversigt.active_spotify_device=0;
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
// get JSON return from tidal
//
// ****************************************************************************************

static int s_exit_flag = 0;



static void tidal_ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
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
// constructor
//
// ****************************************************************************************

tidal_class::tidal_class() : antal(0) {
    for(int i=0;i<maxantal;i++) stack[i]=0;
    stream_optionselect=0;							                                        // selected line in stream options
    tidal_oversigt_loaded=false;
    tidal_oversigt_loaded_nr=0;
    antal=0;
    type=0;
    searchtype=0;
    search_loaded=false;                                                        // load icobn gfx afload search is loaded done by thread.
    tidal_aktiv_song_antal=0;                                                 //
    gfx_loaded=false;			                                                      // gfx loaded default false
    tidal_is_playing=false;                                                   // is we playing any media
    tidal_is_pause=false;                                                     // is player on pause
    show_search_result=false;                                                   // are we showing search result in view
    antalplaylists=0;                                                           // antal playlists
    loaded_antal=0;                                                             // antal loaded
    search_playlist_song=0;
    int port_cnt, n;
    int err = 0;
    strcpy(tidal_aktiv_song[0].release_date,"");
    write_logfile((char *) "Starting web server on port 8000");                 //
    printf("Starting tidal web server on port %s \n",s_http_port);
    // start web server
    // create web server
    mg_mgr_init(&mgr, NULL);                                                    // Initialize event manager object
    c = mg_bind(&mgr, s_http_port, new_ev_handler); // old tidal_server_ev_handler);
    mg_set_protocol_http_websocket(c);
    active_tidal_device=-1;                                                   // active spotify device -1 = no dev is active
    active_default_play_device=active_tidal_device;
    aktiv_song_tidal_icon=0;                                                  //
    strcpy(tidal_client_id,"");                                               //
    strcpy(tidal_secret_id,"");                                               //
    strcpy(tidaltoken,"");                                                    //
    strcpy(tidaltoken_refresh,"");                                            //
    strcpy(tidal_client_id,"client_id");
    strcpy(tidal_secret_id,"secret_id");
    strcpy(active_default_play_device_name,"");
    strcpy(overview_show_band_name,"");                                         //
    strcpy(overview_show_cd_name,"");                                           //
    tidal_device_antal=0;
    tidal_update_loaded_begin=false;                                          // true then we are update the stack data
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

tidal_class::~tidal_class() {
    mg_mgr_free(&mgr);                      // delete web server again
    mg_mgr_free(&client_mgr);               // delete web client
    clean_tidal_oversigt();                 // clean tidal class
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
// Do work now
// get user id from spotify api
//
// return 0 if fault
// 200  on ok
//
// ****************************************************************************************

int tidal_class::tidal_get_user_id() {
  static const char *userfilename = "tidal_user_id.txt";
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
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
      if (httpCode == 200) {
        return(200);
      }
    }
  }
  return(httpCode);
}

// ****************************************************************************************
//
//
// get device list and have it in spotify class
//
// ****************************************************************************************

int tidal_class::tidal_get_available_devices() {
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
  char call_sed[]="cat tidal_device_list.json | sed 's/\\\\\\\\\\\//\\//g' | sed 's/[{\\\",}]//g' | sed 's/ //g' | sed 's/:/=/g' | tail -n +6 > tidal_device_list.txt";
  sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/me/player/devices' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > tidal_device_list.json 2>&1",tidaltoken);
  curl_exitcode=system(call);
  //get json file
  if (WEXITSTATUS(curl_exitcode)==0) {
    // convert file by sed (call_sed) easy hack
    curl_exitcode=system(call_sed);
    if (WEXITSTATUS(curl_exitcode)!=0) {
      fprintf(stderr,"Error get devices\n");
    }
    json_file = fopen("tidal_device_list.txt", "r");
    if (json_file == NULL) {
      fprintf(stderr, "Unable to open tidal_device_list.txt\n");
      return 1;
    } else {
      // read devices info
      while ((!(feof(json_file))) && (devicenr<10)) {
        // get id
        getline(&tmp_content_line,&jsonfile_len,json_file);
        if (strlen(tmp_content_line)>5) {
          strcpy(tidal_device[devicenr].id,tmp_content_line+3);
          tidal_device[devicenr].id[strlen(tidal_device[devicenr].id)-1]='\0';  // remove newline char
          // get active status
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strncmp(tmp_content_line,"is_active=true",14)==0) tidal_device[devicenr].is_active=true; else tidal_device[devicenr].is_active=false;
          // if active rember to return from func
          if (( tidal_device[devicenr].is_active ) && (active_tidal_device==-1)) active_tidal_device=devicenr;
          // get is_private_session
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strcmp(tmp_content_line,"is_private_session=true")==0) tidal_device[devicenr].is_private_session=true; else tidal_device[devicenr].is_private_session=false;
          // get private info
          getline(&tmp_content_line,&jsonfile_len,json_file);
          if (strcmp(tmp_content_line,"is_restricted=true")==0) tidal_device[devicenr].is_restricted=true; else tidal_device[devicenr].is_restricted=false;
          // get dev name
          getline(&tmp_content_line,&jsonfile_len,json_file);
          strcpy(tidal_device[devicenr].name,tmp_content_line+5);
          tidal_device[devicenr].name[strlen(tidal_device[devicenr].name)-1]='\0';   // remove newline char
          // get dev type
          getline(&tmp_content_line,&jsonfile_len,json_file);
          strcpy(tidal_device[devicenr].devtype,tmp_content_line+5);
          tidal_device[devicenr].devtype[strlen(tidal_device[devicenr].devtype)-1]='\0';   // remove newline char
          // get dev volume info
          getline(&tmp_content_line,&jsonfile_len,json_file);
          tidal_device[devicenr].devvolume=atoi(tmp_content_line+15);
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
        mysql_query(conn,"create TABLE mythtvcontroller.tidal_device (device_name varchar(255),active bool,devtype varchar (255),dev_id varchar (255),intnr INT AUTO_INCREMENT,PRIMARY KEY (intnr))");
        res = mysql_store_result(conn);
        fprintf(stderr,"Found devices : %d\n",devicenr);
        for( int t = 0 ; t < devicenr ; t++ ) {
          if ( strcmp(tidal_device[t].name,"") !=0 ) {
            device_antal++;
            fprintf(stderr,"Device name      : %s \n",tidal_device[t].name);
            fprintf(stderr,"Device is active : %d \n",tidal_device[t].is_active);
            fprintf(stderr,"Device type      : %s \n",tidal_device[t].devtype);
            fprintf(stderr,"Device id        : %s \n\n",tidal_device[t].id);
            sprintf(sql,"select dev_id from mythtvcontroller.tidal_device where dev_id like '%s' limit 1",tidal_device[t].id);
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
              sprintf(sql,"insert into mythtvcontroller.tidal_device values ('%s',%d,'%s','%s',0)",tidal_device[t].name,tidal_device[t].is_active,tidal_device[t].devtype,tidal_device[t].id);
              mysql_query(conn,sql);
              res = mysql_store_result(conn);
            }
          }
        }
        this->tidal_device_antal=device_antal;
        if (conn) mysql_close(conn);
        fprintf(stderr,"\n*****************\n");
      }
    }
  }
  // no device to use is found or no token
  if ( curl_exitcode == 22 ) {
    active_tidal_device=-1;
    fprintf(stderr,"Error loading device list from spodify by api.\n");
  }
  return active_tidal_device;
}



// ****************************************************************************************
//
// clean up number of created
//
// ****************************************************************************************

void tidal_class::clean_tidal_oversigt() {
    startup_loaded=false;
    for(int i=1;i<antal;i++) {
      if (stack[i]) {
        // crash
        if (stack[i]->textureId) {
              //if (&stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete spotify texture
        }
        delete stack[i];
      }
      stack[i]=NULL;
    }
    antal=0;
    antalplaylists=0;
    tidal_oversigt_loaded=false;			// set load icon texture again
    tidal_oversigt_loaded_nr=0;
    tidal_oversigt_nowloading=0;
}



// *********************************************************************************************************************************
// Download image
// sample call
// download_image("https://i.scdn.co/image/ab67616d0000b2737bded29598acbe1e2f4b4437","/home/hans/file.jpg");
// ********************************************************************************************

int tidal_download_image(char *imgurl,char *filename) {
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
      printf("Erro open file.\n");
    }
    curl_easy_cleanup(curl);
  }
  return(1);
}



// ****************************************************************************************
//
//
// ****************************************************************************************

char tidal_class::*tidal_get_token() {
  //return(ret);
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
// Get users playlist
// in use
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
      if (dbexist==false) {
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontent (name varchar(255),paththumb text,playid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify songs)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (spotify playlists)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          write_logfile((char *) "mysql create table error.");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
      }
      // download all playlist (name + id) in one big file NOT songs
      // if we have a token
      if (strcmp(tidaltoken,"")!=0) {
        // 50 is the max
        //download_user_playlist(spotifytoken,startoffset);
        while (tidalplaylistloader_done==false) {
          sprintf(doget,"curl -X GET 'https://api.tidal.com/v1/me/playlists?limit=50&offset=%d' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.json",startoffset,tidaltoken);
          curl_error=system(doget);
          if (curl_error!=0) {
            fprintf(stderr,"Curl error get user playlists\n");
            exit(0);
          }
          // hent users playlist from spotify api
          system("cat spotify_users_playlist.json | grep spotify:playlist: | awk {'print substr($0,31,22)'} > spotify_users_playlist.txt");
          // get antal playliste first time (we can only load 50 at time)
          if (tidal_playlistantal_loaded==0) {
            system("cat spotify_users_playlist.json | grep total | tail -1 | awk {'print $3'} > spotify_users_playlist_antal.txt");
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
        if (remove("tidal_users_playlist.txt")!=0) write_logfile((char *) "Error remove user playlist file tidal_users_playlist.txt");
        // save data to mysql db
      } else {
          write_logfile((char *) "Error downloading user playlist");
          exit(0);
      }
      sprintf(sql,"select playlistname,playlistid from mythtvcontroller.tidalcontentplaylist");
      write_logfile((char *) "process playlist ......");
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
      write_logfile((char *) "process tidal playlist done..");
      mysql_close(conn);
    }
  }
  catch (...) {
    write_logfile((char *) "Error process playlist");
  }
  return(1);
}




// ****************************************************************************************
//
// opdate from db
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
    mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) dbexist=true;
    }
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
    sprintf(sqlselect,"select playlistname,paththumb,playlistid,id from tidalcontentplaylist order by playlistname");
    getart = 0;
  } else {
    show_search_result=true;
    sprintf(sqlselect,"select name,gfxfilename,player,playlistid from tidalcontentarticles where playlistid='%s' order by id",refid);
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
            stack[antal]->intnr=atoi(row[3]);
            stack[antal]->nyt=false;
            stack[antal]->type=0;
            // top level (load playlist)
            if (getart == tidal_playlisttype ) {
              strncpy(stack[antal]->feed_showtxt,row[0],tidal_pathlength);
              strncpy(stack[antal]->feed_name,row[0],tidal_namelength);
              if (row[1]) {
                if (strncmp(row[1],"http",4)==0) {
                  get_webfilename(downloadfilename,row[1]);
                  getuserhomedir(downloadfilenamelong);
                  strcat(downloadfilenamelong,"/");
                  strcat(downloadfilenamelong,tidal_gfx_path);
                  strcat(downloadfilenamelong,downloadfilename);
                  strcat(downloadfilenamelong,".jpg");
                  // download file
                  if (!(file_exists(downloadfilenamelong))) {
                    tidal_download_image(row[1],downloadfilenamelong);
                    //get_webfile2(row[1],downloadfilenamelong);                // download file
                  } else strcpy(downloadfilenamelong,row[1]);                 // no file name
                } else strcpy(downloadfilenamelong,row[1]);
                strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
              }
              strncpy(stack[antal]->playlistid,row[2],tidal_namelength);    //
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
      fprintf(stderr,"No tidal data loaded \n");
    }
    antalplaylists=antal;
    return(antal);
  } else fprintf(stderr,"Failed to update tidal db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
  fprintf(stderr,"Tidal loader done... \n");
  return(0);
}




// ****************************************************************************************
//
// work
// get songs from playlist (any public user)
// write to spotify_playlist_{spotifyid}.json
// and update db from that file all the songs in playlist
//
// ****************************************************************************************

int tidal_class::tidal_get_playlist(const char *playlist,bool force,bool create_playlistdb) {

}

// ****************************************************************************************
//
//
// ****************************************************************************************

//bool do_select_device_to_play;

void tidal_class::select_device_to_play() {

// temp global
   static bool do_select_device_to_play;

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
  //active_tidal_device
  while(strcmp(tidal_device[i].id,"")!=0) {
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
    if (active_tidal_device>=0) strcpy( temptxt , tidal_device[i].name );
    else strcpy( temptxt , "None" );
    if ( i == active_tidal_device ) glColor4f( 1.0f, 1.0f, 0.0f, select_device_to_playfader); else glColor4f( 1.0f, 1.0f, 1.0f, select_device_to_playfader);
    glcRenderString(temptxt);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(xof+180,(yof+200)-(i*30),0);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(configdefaultstreamfontsize+2, configdefaultstreamfontsize+2, 1.0);
    glcRenderString(" - ");
    glcRenderString(tidal_device[i].devtype);
    glPopMatrix();
    //draw icon
    xof2=xof+330;
    yof2=(yof+184)-(i*30);
    winsizey2=53;
    winsizex2=43;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    if (strncmp(tidal_device[i].devtype,"Smartphone",10)==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
    else if (strncmp(tidal_device[i].devtype,"Computer",8)==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
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
//
// ****************************************************************************************

int tidal_class::tidal_next_play() {
  return(1);
}

// ****************************************************************************************
//
//
// ****************************************************************************************

int tidal_class::tidal_last_play() {
  return(1);
}


// ****************************************************************************************
//
//
// ****************************************************************************************

int tidal_class::tidal_resume_play() {
  return(1);
}

// ****************************************************************************************
//
//
// ****************************************************************************************



char *tidal_class::get_tidal_name(int nr) {
  if (nr < antal) return (stack[nr]->feed_name); else return (NULL);
}


// ****************************************************************************************
//
//
// ****************************************************************************************


char *tidal_class::get_tidal_playlistid(int nr) {
  if (nr < antal) return (stack[nr]->playlistid); else return (NULL);
}


// ****************************************************************************************
//
//
// ****************************************************************************************

int tidal_class::tidal_play_now_song(char *playlist_song,bool now) {
  return(1);
}


// ****************************************************************************************
//
//
// ****************************************************************************************



int tidal_class::tidal_play_now_playlist(char *playlist_song,bool now) {
  std::string auth_kode;
  char response_string[8192];
  std::string url;
  //std::string post_playlist_data;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.spotify.com/v1/me/player/play?device_id=";
  if (devid) url=url + devid;
  // use libcurl
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (curl) {
    // Add a custom header
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
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
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
// ****************************************************************************************


int tidal_class::tidal_do_we_play() {

}

// ****************************************************************************************
//
//
// ****************************************************************************************


int tidal_class::tidal_pause_play() {

}

// ****************************************************************************************
//
//
// ****************************************************************************************

int tidal_class::load_tidal_iconoversigt() {

}



// ****************************************************************************************
//
// play artist
//
// ****************************************************************************************

int tidal_class::tidal_play_now_artist(char *playlist_song,bool now) {

}

// ****************************************************************************************
//
//
// ****************************************************************************************

int tidal_class::tidal_play_now_album(char *playlist_song,bool now) {

}


// ****************************************************************************************
// gettoken from Tidal
//
// ****************************************************************************************


int tidal_class::getToken() {
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
      //printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      process_value_token(value, 0,0);                                                     // fill play info
      json_value_free(value);                                                                 // json clean up
    }
  }
  return(httpCode);
}


// ****************************************************************************************
//
//
// ****************************************************************************************


int tidal_class::spotify_refresh_token() {
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
      write_logfile((char *) "Tidal new token.");
      printf("%s \n", response_string.c_str());
      //printf("resp length %d \n",response_string.length());
      //value = json_parse((char *) response_string.c_str(),response_string.length());          // parser
      //process_value_playinfo(value, 0,0);                                                     // fill play info
      if ((response_string.size()>12) && (response_string.compare(2,12,"access_token")==0)) {
        strncpy(newtoken,response_string.c_str()+17,180);
        newtoken[181]='\0';
        write_logfile((char *) "Spotify token valid.");
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
//
// ****************************************************************************************

void tidal_class::show_tidal_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected) {
  char *base,*right_margin;
  int pline=0;
  char temptxt[200];
  int length,width;
  int i=0;
  float buttonsizey=180.0f;                                                   // button size
  float buttonsize=180.0f;
  float yof=orgwinsizey-(buttonsizey);                                        // start ypos
  int lstreamoversigt_antal=9*5;
  unsigned int bonline=9;                                                              // antal pr linie
  float xof=0.0f;
  float boffset;
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
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
}
