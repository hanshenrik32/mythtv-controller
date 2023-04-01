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
#include "myctrl_tidal.h"

// web port
static const char *ss_http_port = "8002";
static struct mg_serve_http_opts tidal_s_http_server_opts;

extern tidal_class tidal_oversigt;

const int tidal_pathlength=80;
const int tidal_namelength=80;
const int tidal_desclength=2000;
const int feed_url=2000;


const char *tidal_json_path = "tidal_json/";
const char *tidal_gfx_path = "tidal_gfx/";

size_t tidal_curl_writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}


// ****************************************************************************************
//
// text render is glcRenderString for freetype font support
//
// ****************************************************************************************

extern int tidalknapnr;
extern int tidal_select_iconnr;
//extern tidal_class tidal_oversigt;
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
extern GLuint tidal_pil;
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
extern GLuint tidal_askplay;                                  // ask open icon
extern GLuint tidal_askopen;                                  // ask play icon
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

extern GLuint tidal_icon_border;
// stream mask
extern GLuint onlinestreammask;
extern GLuint onlinestreammaskicon;		// icon mask on web icon
extern tidal_class streamoversigt;
extern GLint cur_avail_mem_kb;
extern bool stream_loadergfx_started;
extern bool stream_loadergfx_started_done;
extern bool stream_loadergfx_started_break;
//extern bool tidal_oversigt_loaded_begin;


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
  strcpy(data,"");                                                            // clientid
  strcat(data,":");
  strcat(data,"");                                                            // secretid
  char sed[]="cat tidal_access_token.txt | grep -Po '\"\\K[^:,\"}]*' | grep -Ev 'access_token|token_type|Bearer|expires_in|refresh_token|scope' > tidal_access_token2.txt";
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      // from tidal servers
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
        //sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.tidal.com/api/token > tidal_access_token.txt",base64_code,user_token,tidal_oversigt.tidal_client_id,tidal_oversigt.tidal_secret_id);
        //old snprintf(sql,2045,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d redirect_uri=http://localhost:8002/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.tidal.com/api/token > tidal_access_token.txt",base64_code);
        snprintf(sql,2045,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d redirect_uri=http://localhost:8002/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://account.tidal.com/api/token > tidal_access_token.txt",base64_code,user_token,tidal_oversigt.tidal_client_id);
        //printf("sql curl : %s \n ",sql);
        curl_error=system(sql);
        if (curl_error==0) {
          curl_error=system(sed);
          if (curl_error==0) {
          }
          write_logfile((char *) "******** Got tidal token ********");
          tokenfile=fopen("tidal_access_token2.txt","r");
          error=getline(&file_contents,&len,tokenfile);
          strcpy(token_string,file_contents);
          token_string[strlen(token_string)-1]='\0';
          error=getline(&file_contents,&len,tokenfile);
          strcpy(token_refresh,file_contents);
          token_refresh[strlen(token_refresh)-1]='\0';
//          printf("token     %s\n",token_string);
//          printf("ref token %s\n",token_refresh);
          //  tidal_oversigt.tidal_set_token(token_string,token_refresh);   // save it for later use
          fclose(tokenfile);
          free(file_contents);
          if (strcmp(token_string,"")!=0) {
            //tidal_oversigt.tidal_get_user_id();                                   // get user id
            //tidal_oversigt.active_tidal_device=tidal_oversigt.tidal_get_available_devices();
            // set default tidal device if none
            //if (tidal_oversigt.active_tidal_device==-1) tidal_oversigt.active_tidal_device=0;
          }
        }
        //c->flags |= MG_F_SEND_AND_CLOSE;
        mg_serve_http(c, (struct http_message *) ev_data, tidal_s_http_server_opts);  /* Serve static content */
      } else {
        // else show normal indhold
        memset(&opts, 0, sizeof(opts));
        opts.document_root = "./tidal_web";       // Serve files from the current directory
        mg_serve_http(c, (struct http_message *) ev_data, tidal_s_http_server_opts);
      }
      // We have received an HTTP request. Parsed request is contained in `hm`.
      // Send HTTP reply to the client which shows full original request.
      //mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
      //mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
      break;
    case MG_EV_HTTP_REPLY:
      fprintf(stdout,"***************************************** CALL BACK server reply ***************************************");
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
//bool debug_json=false;


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
    tidal_aktiv_song_antal=0;                                                   //
    gfx_loaded=false;			                                                      // gfx loaded default false
    tidal_is_playing=false;                                                     // is we playing any media
    tidal_is_pause=false;                                                       // is player on pause
    show_search_result=false;                                                   // are we showing search result in view
    antalplaylists=0;                                                           // antal playlists
    loaded_antal=0;                                                             // antal loaded
    search_playlist_song=0;
    int port_cnt, n;
    int err = 0;
    strcpy(tidal_aktiv_song[0].release_date,"");
    active_tidal_device=-1;                                                     // active tidal device -1 = no dev is active
    active_default_play_device=active_tidal_device;
    aktiv_song_tidal_icon=0;                                                    //
    strcpy(tidaltoken,"");                                                      //
    strcpy(tidaltoken_refresh,"");                                              //
    strcpy(tidal_client_id,"emailadresse");                                     //
    strcpy(tidal_secret_id,"");                                                 //
    strcpy(active_default_play_device_name,"");                                 // default device to play on
    strcpy(overview_show_band_name,"");                                         //
    strcpy(overview_show_cd_name,"");                                           //
    tidal_device_antal=0;                                                       //
    tidal_update_loaded_begin=false;                                            // true then we are update the stack data
    // create web server
    mg_mgr_init(&mgr, NULL);                                                      // Initialize event manager object
    // start web server
    write_logfile((char *) "Starting Tidal web server on port 8002");             //
    this->connection = mg_bind(&mgr, ss_http_port, tidal_server_ev_handler);      // Create listening connection and add it to the event manager
    mg_set_protocol_http_websocket(this->connection);



    strcpy(do_link_url,"");                                                     //
    strcpy(device_code,"");                                                     //
    strcpy(client_id,"");                                                       //
    strcpy(client_secret,"");                                                   //
    strcpy(client_platform,"");                                                 //
    strcpy(device_url_code_link,"");                                            // redirect link on auth
    strcpy(client_platform,"Fire TV");
    strcpy(client_id,"7m7Ap0JC9j1cOM3n");                                       // tidal client id
    strcpy(client_secret,"vRAdA108tlvkJpTsGZS8rGZ7xTlbJ0qaZ2K9saEzsgY=");       // tidal client id

}


// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

tidal_class::~tidal_class() {
  mg_mgr_free(&mgr);                        // delete web server again
  mg_mgr_free(&client_mgr);                 // delete web client
  clean_tidal_oversigt();                 // clean tidal class
}


// ****************************************************************************************
//
// Tidal web server for login and token
//
// ****************************************************************************************


int tidal_class::start_webserver() {
  // create web server
  mg_mgr_init(&mgr, NULL);                                                      // Initialize event manager object
  // start web server
  fprintf(stdout,"Starting web server on port %s\n", ss_http_port);             //
  write_logfile((char *) "Starting Tidal web server on port 8002");             //
  this->connection = mg_bind(&mgr, ss_http_port, tidal_server_ev_handler);      // Create listening connection and add it to the event manager
  if (this->connection) {
    mg_set_protocol_http_websocket(this->connection);                           // make http protocol
  } else {
    write_logfile((char *) "Error starting Tidal web server on port 8002");     //
  }
  //mg_connect_http(&mgr, tidal_ev_handler, "", NULL, NULL);
}

// ****************************************************************************************
// file writer
// ****************************************************************************************

static size_t file_write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
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
// get loader flag
//
// ****************************************************************************************


bool tidal_class::get_tidal_update_flag() {
  return(tidal_update_loaded_begin);
}



// NEW In use

/*
This function takes four parameters: client_id, client_secret, username, and password.
These values should be obtained from your Tidal API account.
The function uses these parameters to make a POST request to the Tidal API's token endpoint and retrieve an access token.
The access token is returned as a string. Note that this function assumes that the curl library is available and has been properly installed on the system.

*/


// This is the Tidal API endpoint for obtaining an access token

#define TOKEN_URL "https://auth.tidal.com/v1/oauth2/token"

// This is the maximum size of the response buffer
#define BUFFER_SIZE 1024 * 1024

long write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

// This function is used to handle the response data from the Tidal API
long write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;
    char *response_data = (char *)userdata;

    // Append the response data to the existing buffer
    strncat(response_data, ptr, realsize);
    return realsize;
}

//
// This function obtains an access token from the Tidal API using OAuth 2.0
//

// step 3

char *tidal_class::get_access_token(char *username, char *password) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char *response_data = (char *) calloc(BUFFER_SIZE, sizeof(char));
    long response_code = 0;
    // Initialize CURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return NULL;
    }
    // Set the request headers
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    // Set the request data
    char *request_data_formatted;
    asprintf(&request_data_formatted, "scope=r_usr+w_usr+w_sub&client_id=%s&device_code=%s&grant_type=urn:ietf:params:oauth:grant-type:device_code", client_id,device_code);
    // Set the request options
    curl_easy_setopt(curl, CURLOPT_URL, TOKEN_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data_formatted);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Perform the request
    res = curl_easy_perform(curl);
    // Get the response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    // Check for errors
    if (res != CURLE_OK || response_code != 200) {
        fprintf(stderr, "Failed to obtain access token: %s\n", curl_easy_strerror(res));
        printf("RESULT : %s \n",response_data);
        response_data = NULL;
    }
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(request_data_formatted);
    return response_data;
}

// step 1

#define DEVAUTH_URL "https://auth.tidal.com/v1/oauth2/device_authorization"

char *tidal_class::get_dev_auth() {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char *response_data = (char *) calloc(BUFFER_SIZE, sizeof(char));
    long response_code = 0;
    // Initialize CURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return NULL;
    }
    // Set the request headers
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    // Set the request data
    char *request_data_formatted;
    asprintf(&request_data_formatted, "scope=r_usr+w_usr+w_sub&client_id=%s", client_id);
    // Set the request options
    curl_easy_setopt(curl, CURLOPT_URL, DEVAUTH_URL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data_formatted);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Perform the request
    res = curl_easy_perform(curl);
    // Get the response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    // Check for errors
    if ( response_code != 200 ) {
        printf("response_code %d Failed to obtain access token: %s\n",response_code, curl_easy_strerror(res));
        response_data = NULL;
    }
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(request_data_formatted);

    if (strlen(response_data)>178) {
      strncpy(device_code,response_data+15,36);                                 // gem device code
      device_code[36]=0;
      strncpy(do_link_url,response_data+134,21);                                // gem redirect link
      do_link_url[20]=0;
    }
    return response_data;
}



// step 2 call url efter call af get_dev_auth()

// internal func

size_t write_header_func(char *ptr, size_t size, size_t nmemb, void *userdata)
{
        //write(STDOUT_FILENO, ptr, size*nmemb);
    size_t realsize = size * nmemb;
    char *response_header = (char *) userdata;
    //strncat(response_header, ptr, realsize);
    return realsize;
}

int tidal_class::do_link_tidal()  {

    /*
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    char *response_data = (char *) calloc(BUFFER_SIZE, sizeof(char));
    char *response_header = (char *) calloc(BUFFER_SIZE, sizeof(char));
    long response_code = 0;
    // Initialize CURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return NULL;
    }
    // Set the request headers
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    // Set the request data
    char *request_data_formatted;
    asprintf(&request_data_formatted, "");
    // Set the request options
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 5L);                         // follow header
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_func);
    curl_easy_setopt(curl, CURLOPT_URL, device_url_code_link);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data_formatted);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_header_func);
    // Perform the request
    res = curl_easy_perform(curl);
    // Get the response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    // Check for errors
    if ( response_code != 200 ) {
        printf("response_code %d Failed verfiy link %s\n",response_code, curl_easy_strerror(res));
        response_data = NULL;
    }


    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(request_data_formatted);

    */

    char redirect_url[2000];
    char call[2000];
    int curl_error;
    //sprintf(call,"curl -f -X PUT '%s' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",device_url_code_link);
    sprintf(call,"curl -L  -D /home/hans/header.log -o /dev/null '%s'",do_link_url);
    curl_error=system(call);
    if (curl_error==0) {
      printf("Reading redirect request OK.\n");
      FILE *fil;
      char datatxt[10000];
      fil=fopen("/home/hans/header.log","r");
      if (fil) {
        while(!(feof(fil))) {
          fgets(datatxt,5000,fil);
          if (strncmp(datatxt,"Location",8)==0) {
            strcpy(redirect_url,"/usr/bin/google-chrome ");
            strcat(redirect_url,datatxt);
            strcpy(device_url_code_link,datatxt);                               // gem redirect url
            printf("FUNDET ************ %s",datatxt);
            system(redirect_url);
          }
        }
        fclose(fil);
      }
      return 1;
    } else return 0;
}


/*  read more here,
 https://github.com/yusufusta/php-tidal/blob/master/src/Tidal/TidalAPI.php */

// NOT In use

int tidal_class::tidal_login_token2() {
  char sql[1024];
  // https://api.tidalhifi.com/v1/login/username?token=kgsOOmYk3zShYrNP
//sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.tidal.com/api/token > tidal_access_token.txt",base64_code,user_token,tidal_oversigt.tidal_client_id,tidal_oversigt.tidal_secret_id);
  // org
  try {
    sprintf(sql,"curl -X POST 'https://api.tidalhifi.com/v1/login/username=hanshenrik32@gmail.com&password=o60LbQGXJi5y' > tidal_access_token.txt");
    int curl_error=system(sql);
    if (curl_error==0) {
      //curl_error=system(sed);
      if (curl_error==0) {
      }
      fprintf(stdout,"\n******** Got token ********\n");
    }
  }
  catch (...) {
    fprintf(stdout,"Error on system call.\n");
  }
}


// In use

int tidal_class::tidal_login() {
  static char tidaltoken_refresh[200];
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
  char post_playlist_data[2048];
  char errbuf[CURL_ERROR_SIZE];
  strcpy(newtoken,"");

  strcpy(tidaltoken_refresh,"abc");
  curl = curl_easy_init();
  if ((curl) && (strcmp(tidaltoken_refresh,"")!=0)) {

    printf("Tidal login ..\n");
    // tidal_client_id
    // tidal_secret_id

    // add userinfo + basic auth
    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //curl_easy_setopt(curl, CURLOPT_USERNAME, "OmDtrzFgyVVL6uW56OnFA2COiabqm");
    //curl_easy_setopt(curl, CURLOPT_PASSWORD, "zxen1r3pO0hgtOC7j6twMo9UAqngGrmRiWpV7QC1zJ8");

    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "X-Tidal-Token:kgsOOmYk3zShYrNP");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    //curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2");          // /login/username

    curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/login/username");          // /login/username  https://api.tidalhifi.com/v1/login/
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "TIDAL_ANDROID/686 okhttp/3.3.1");

    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "X-Tidal-Token kgsOOmYk3zShYrNP");
    //chunk = curl_slist_append(chunk, base64_code);
    errbuf[0] = 0;
    // set type post
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",tidaltoken_refresh);
    //strcpy(data,"X-Tidal-Token:kgsOOmYk3zShYrNP");
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);

    sprintf(post_playlist_data,"username=hanshenrik32@gmail.com&password=o60LbQGXJi5y");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {                                                       // check if ok
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
      fprintf(stdout,"tidal new token. \n");
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
    } else if (httpCode == 405) {
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
    } else {
      fprintf(stderr,"Error code httpCode %d \n. ",httpCode);
      fprintf(stderr,"Curl error: %s\n", curl_easy_strerror(res));
      printf("%s \n", response_string.c_str());
    }
    // always cleanup
    printf("End curl login call.*******************\n");
    curl_easy_cleanup(curl);
  }
  return(httpCode);
}












// ************************************************************************************************************************
// new code
//
// https://github.com/Frikilinux/tidal-dl-test/blob/2ed222f6e4f7f082345442a25fc1b399d8929879/TIDALDL-PY/tidal_dl/tidal.py
// ************************************************************************************************************************

/*

You have to store the token_type, session_id, access_token, and optionally the refresh_token from the session object after logging in, and then you can use them with the load_oauth_session() method later. I'm not fully sure how the expired tokens look yet, so you might want to use the expiry_time datetime object to check if the access token has expired, and use token_refresh() if it has.



FWIW I was just able to log in to Tidal via a POST to https://api.tidalhifi.com/v1/login/username. I used this token: "GvFhCVAYp3n43EN3", which I believe is from the iOS app.

*/



void checkAuthStatus() {
  //char *data[12]={"clientid","device_code","grant_type"};
  // authurl = https://auth.tidal.com/v1/oauth2
  // URL =  https://api.tidalhifi.com/v1/
  // # known API key for Fire Stick HD(MQA, Dolby Vision enabled)
  // __API_KEY__ = {'clientId': 'aR7gUaTK1ihpXOEP', 'clientSecret': 'eVWBEkuL2FCjxgjOkR3yK0RYZEbcrMXRc2l8fU3ZCdE='}

  //open(authurl+/token , data , clienttd, client secrect)
  // if status = 200 ok
  // if auth is ok
  // userid
  // countryCode
  // access_token
  // refresh_token
  // expires_in
}



void verifyAccessToken() {
  char *header[50]={"authorization: Bearer {}.format(accessToken"};
  //open("https://api.tidal.com/v1/sessions" , data , clienttd, client secrect)
  // if status = 200 ok
}



// ***************************************************************************************************
// first call this function
// ***************************************************************************************************


void tidal_class::gettoken() {
  //get tidaltoken
  // msg = requests.get( "https://cdn.jsdelivr.net/gh/yaronzz/CDN@latest/app/tidal/tokens.json", timeout=(20.05, 27.05))
  // will return json
  strcpy(tidaltoken,"wc8j_yBJd20zOmx0");
  strcpy(tidaltoken2,"_DSTon1kC8pABnTw");
}


// *********************************************************************************************************************************
// NOT In use
// Refresh token
// return http code
//
// 1. https://listen.tidal.com/login
// 2. https://listen.tidal.com/login/username
// 3. https://listen.tidal.com/login
// 4. https://api.tidal.com/v1       - aktiv
//



int tidal_class::tidal_login_token() {
  static char tidaltoken_refresh[200];
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
  char post_playlist_data[2048];
  char errbuf[CURL_ERROR_SIZE];
  strcpy(newtoken,"");

  strcpy(tidaltoken_refresh,"abc");
  curl = curl_easy_init();
  if ((curl) && (strcmp(tidaltoken_refresh,"")!=0)) {

    printf("login tidal ..**************************\n");
    // tidal_client_id
    // tidal_secret_id

    // add userinfo + basic auth
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, "OmDtrzFgyVVL6uW56OnFA2COiabqm");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "zxen1r3pO0hgtOC7j6twMo9UAqngGrmRiWpV7QC1zJ8");

    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "X-Tidal-Token:kgsOOmYk3zShYrNP");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    //curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2");          // /login/username

    curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2");          // /login/username  https://api.tidalhifi.com/v1/login/
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "TIDAL_ANDROID/686 okhttp/3.3.1");

    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //chunk = curl_slist_append(chunk, base64_code);
    errbuf[0] = 0;
    // set type post
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",tidaltoken_refresh);
    //strcpy(data,"X-Tidal-Token:kgsOOmYk3zShYrNP");
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);

    sprintf(post_playlist_data,"username=hanshenrik32@gmail.com&password=o60LbQGXJi5y");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {                                                       // check if ok
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
      fprintf(stdout,"tidal new token. \n");
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());

      /*
      if ((response_string.size()>12) && (response_string.compare(2,12,"access_token")==0)) {
        strncpy(newtoken,response_string.c_str()+17,180);
        newtoken[181]='\0';
        fprintf(stdout,"Token valid.\n");
        strcpy(tidaltoken,newtoken);                                         // update tidal token
      }
      */
    } else if (httpCode == 405) {
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
    } else {
      fprintf(stderr,"Error code httpCode %d \n. ",httpCode);
      //if (strstr(response_string,"token")) fprintf(stderr,"Error Missing token.\n");
      fprintf(stderr,"Curl error: %s\n", curl_easy_strerror(res));
      printf("%s \n", response_string.c_str());
    }
    // always cleanup
    printf("End curl login call.*******************\n");
    curl_easy_cleanup(curl);
  }
  return(httpCode);
}





// ***********************************************************************
// check auth Status
//


int tidal_class::tidal_check_auth_status() {
  static char tidaltoken_refresh[200];
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
  char post_playlist_data[2048];
  char errbuf[CURL_ERROR_SIZE];
  strcpy(newtoken,"");

  strcpy(tidaltoken_refresh,"abc");
  curl = curl_easy_init();
  if ((curl) && (strcmp(tidaltoken_refresh,"")!=0)) {

    printf("login tidal ..**************************\n");
    // tidal_client_id
    // tidal_secret_id

    // add userinfo + basic auth
    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //curl_easy_setopt(curl, CURLOPT_USERNAME, "OmDtrzFgyVVL6uW56OnFA2COiabqm");
    //curl_easy_setopt(curl, CURLOPT_PASSWORD, "zxen1r3pO0hgtOC7j6twMo9UAqngGrmRiWpV7QC1zJ8");

    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "X-Tidal-Token:kgsOOmYk3zShYrNP");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    //curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2");          // /login/username

    curl_easy_setopt(curl, CURLOPT_URL, "https://auth.tidal.com/v1/oauth2/token");          // /login/username  https://api.tidalhifi.com/v1/login/
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "TIDAL_ANDROID/686 okhttp/3.3.1");

    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //chunk = curl_slist_append(chunk, base64_code);
    errbuf[0] = 0;
    // set type post
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",tidaltoken_refresh);
    //strcpy(data,"X-Tidal-Token:kgsOOmYk3zShYrNP");
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);

    sprintf(post_playlist_data,"username=hanshenrik32@gmail.com&password=o60LbQGXJi5y");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {                                                       // check if ok
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
      fprintf(stdout,"tidal new token. \n");
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());

      /*
      if ((response_string.size()>12) && (response_string.compare(2,12,"access_token")==0)) {
        strncpy(newtoken,response_string.c_str()+17,180);
        newtoken[181]='\0';
        fprintf(stdout,"Token valid.\n");
        strcpy(tidaltoken,newtoken);                                         // update tidal token
      }
      */
    } else if (httpCode == 405) {
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
    } else {
      fprintf(stderr,"Error code httpCode %d \n. ",httpCode);
      //if (strstr(response_string,"token")) fprintf(stderr,"Error Missing token.\n");
      fprintf(stderr,"Curl error: %s\n", curl_easy_strerror(res));
      printf("%s \n", response_string.c_str());
    }
    // always cleanup
    printf("End curl login call.*******************\n");
    curl_easy_cleanup(curl);
  }
  return(httpCode);
}







// *****************************************************************************
//
// 1. https://tidal.com/playlist/
// 2. https://embed.tidal.com/playlists
//
// *****************************************************************************

int tidal_class::tidal_play_playlist(char *playlist) {
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
  if (curl) {
    // add userinfo + basic auth
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, tidal_client_id);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, tidal_secret_id);
    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //
    strcpy(call,"https://embed.tidal.com/playlists");
    strcat(call,playlist);
    curl_easy_setopt(curl, CURLOPT_URL, call);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);                        // folow url 301 redirect
    /* Add a custom header */
    //chunk = curl_slist_append(chunk, "Accept: application/json");
    //chunk = curl_slist_append(chunk, "Content-Type: application/json");
    //chunk = curl_slist_append(chunk, base64_code);
    errbuf[0] = 0;
    // set type post
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",tidaltoken_refresh);
    sprintf(post_playlist_data,"x-tidal-token wdgaB1CilGA-S_s2");
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
      fprintf(stdout,"tidal new token. \n");
      printf("%s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
    }
    if (httpCode == 301) {
      fprintf(stdout,"redirect.\n");
      printf("Resp: %s \n", response_string.c_str());
      printf("resp length %d \n",(int) response_string.length());
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
// save the refresh token in used
//
// ****************************************************************************************

void tidal_class::tidal_set_token(char *token,char *refresh) {
  strcpy(tidaltoken,token);
  strcpy(tidaltoken_refresh,refresh);
}



// ****************************************************************************************
//
// Get a List of Current User's Playlists (playlist NOT songs)
// IN use
// get users play lists
// write to tidal_users_playlist.json
//
//
// ****************************************************************************************


int tidal_class::download_user_playlist(char *tidaltoken,int startofset) {

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
  strcat(auth_kode,tidaltoken);
  if (curl) {
    // add userinfo + basic auth
    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    //curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER,tidaltoken);
    //curl_easy_setopt(curl, CURLOPT_USERNAME, tidal_client_id);
    //curl_easy_setopt(curl, CURLOPT_PASSWORD, tidal_secret_id);
    /* Add a custom header */
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, auth_kode);
    //
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.tidal.com/v1/me/playlists");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
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
    //sprintf(post_playlist_data,"{\"grant_type\":\"refresh_token\",\"refresh_token\":%s}",tidaltoken_refresh);
    sprintf(post_playlist_data,"limit=50&offset=%d",startofset);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_playlist_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_playlist_data));
    outfile=fopen("tidal_users_playlist.json","wb");
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
      fprintf(stdout,"Download ok code 200. \n");
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
// Check bout tidal have data in db.
// in use in main
//
// ****************************************************************************************

bool tidal_class::tidal_check_tidaldb_empty() {
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
      mysql_query(conn,"SELECT playlistname from mythtvcontroller.tidalcontentplaylist limit 1");
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
    fprintf(stdout,"Error use mysql\n");
  }
  return(dbexist);
}



// ****************************************************************************************
//
// Get users playlist
// in use
//
// ****************************************************************************************


//#if defined(ENABLE_TIDAL)

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
          fprintf(stdout,"mysql create table error.\n");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (tidal songs)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          fprintf(stdout,"mysql create table error.\n");
          fprintf(stdout,"SQL : %s\n",sql);
        }
        res = mysql_store_result(conn);
        // create db (tidal playlists)
        sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.tidalcontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
        if (mysql_query(conn,sql)!=0) {
          fprintf(stdout,"mysql create table error.\n");
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
          sprintf(doget,"curl -X GET 'https://api.tidal.com/v1/me/playlists?limit=50&offset=%d' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > tidal_users_playlist.json",startoffset,tidaltoken);
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
            json_file = fopen("tidal_users_playlist_antal.txt", "r");
            fscanf(json_file, "%s", temptxt);
            //if (strcmp(temptxt,"")!=0) tidal_oversigt.tidal_playlist_antal = atoi(temptxt);
            if (strcmp(temptxt,"")!=0) tidal_playlist_antal = atoi(temptxt);
            else tidal_playlist_antal = 0;
            fclose(json_file);
          }
          stat("tidal_users_playlist.txt", &filestatus);                              // get file info
          file_size = filestatus.st_size;                                               // get filesize
          if (file_size>0) {
            file_contents = (char*) malloc(filestatus.st_size);                           // get some work mem
            json_file = fopen("tidal_users_playlist.txt", "r");
            if (json_file) {
              while(!(feof(json_file))) {
                fscanf(json_file, "%s", file_contents);
                // process playlist id
                //tidal_oversigt.tidal_get_playlist(file_contents,force,1);
                clean_tidal_oversigt();
                loaded_antal++;
              }
              fclose(json_file);
            }
            if (file_contents) free(file_contents);                                                          // free memory again
          }
          tidal_playlistantal_loaded+=startoffset;
          // next loop
          // 50 is loaded on each loop until end
          if ((startoffset+50)<tidal_playlist_antal) {
            startoffset+=50;
          } else {
            startoffset=tidal_playlist_antal-startoffset;
          }
          if (tidal_playlistantal_loaded>=tidal_playlist_antal) tidalplaylistloader_done=true;
        }
        if (remove("tidal_users_playlist.txt")!=0) fprintf(stdout,"Error remove user playlist file tidal_users_playlist.txt\n");
        // save data to mysql db
      } else {
          fprintf(stdout,"Error downloading user playlist");
          exit(0);
      }
      sprintf(sql,"select playlistname,playlistid from mythtvcontroller.tidalcontentplaylist");
      fprintf(stdout,"process playlist ...... \n");
      mysql_query(conn,sql);
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          if (debugmode & 4) fprintf(stdout,"playlist %-60s tidalid %-20s \n",row[0],row[1]);
//          if (tidal_oversigt.tidal_get_playlist(row[1],force,0)==1) {
//            fprintf(stderr,"Error create playlist %s \n",row[1]);
//          }
        }
      }
      fprintf(stdout,"process playlist done.. \n");
      mysql_close(conn);
    }
  }
  catch (...) {
    fprintf(stdout,"Error process playlist\n");
  }
  return(1);
}

//#endif

// *********************************************************************************************************************************
// do we play ?
// return http code
// Work in use
// ********************************************************************************************

int tidal_class::tidal_do_we_play() {
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
  try {
    CURL *curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "https://api.tidal.com/v1/me/player");
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
        //process_value_playinfo(value, 0,0);                                                     // fill play info
        json_value_free(value);                                                                 // json clean up
      }
    }
  }
  catch (...) {
    fprintf(stdout,"Error on system curl call.\n");
  }
  return(httpCode);
}





// *******************************************************************************************************
//
// work
// pause play
//
// ****************************************************************************************

int tidal_class::tidal_pause_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X PUT 'https://api.tidal.com/v1/me/player/pause' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",tidaltoken);
  try {
    curl_error=system(call);
  }
  catch (...) {
    fprintf(stdout,"Error on system call.\n");
  }
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

int tidal_class::tidal_pause_play2() {
  char auth_kode[1024];
  std::string response_string;
  int httpCode;
  CURLcode res;
  json_char *json;
  json_value *value;
  struct curl_slist *chunk = NULL;
  strcpy(auth_kode,"Authorization: Bearer ");
  strcat(auth_kode,tidaltoken);
  try {
    CURL *curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "https://api.tidal.com/v1/me/player/pause");
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
      }
    }
  }
  catch (...) {
    fprintf(stdout,"Error on system curl call.\n");
  }
}



// ****************************************************************************************
//
// in use resume play
//
// ****************************************************************************************

int tidal_class::tidal_resume_play() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/play";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
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

int tidal_class::tidal_last_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X POST 'https://api.tidal.com/v1/me/player/last' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",tidaltoken);
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


int tidal_class::tidal_last_play2() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/last";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
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

int tidal_class::tidal_next_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X POST 'https://api.tidal.com/v1/me/player/next' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",tidaltoken);
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

int tidal_class::tidal_next_play2() {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/next";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
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
// Upper is debug code *******************************************************************
// ****************************************************************************************

// ****************************************************************************************
// Works
// Optional. tidal URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
// ****************************************************************************************

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

// HAVE some errors

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
  url="https://api.tidal.com/v1/me/player/play?device_id=";
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
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    ////curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    sprintf((char *) post_playlist_data,"{\"context_uri\":\"tidal:playlist:%s\",\"offset\":{\"position\":5},\"position_ms\":0}",playlist_song);
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
// Optional. tidal URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************


// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int tidal_class::tidal_play_now_song(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/play?device_id=";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    sprintf(post_playlist_data,"{\"uris\":[\"tidal:track:%s\"]}",playlist_song);
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
// Optional. tidal URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************


// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.


int tidal_class::tidal_play_now_artist(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/play?device_id=";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    sprintf(post_playlist_data,"{\"context_uri\":\"tidal:artist:%s\"}",playlist_song);
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
// Optional. tidal URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes
//
// ****************************************************************************************

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int tidal_class::tidal_play_now_album(char *playlist_song,bool now) {
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me/player/play?device_id=";
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tidal_curl_writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (char *) &response_string);
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);                                    // enable stdio echo
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // set type post/put
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
    sprintf(post_playlist_data,"{\"context_uri\":\"tidal:album:%s\"}",playlist_song);
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
// get user id from tidal api
//
// ****************************************************************************************

int tidal_class::tidal_get_user_id() {
  static const char *userfilename = "tidal_user_id.txt";
  FILE *userfile;
  std::string auth_kode;
  std::string response_string;
  std::string url;
  char post_playlist_data[4096];
  int httpCode;
  CURLcode res;
  struct curl_slist *header = NULL;
  char *devid=get_active_device_id();
  auth_kode="Authorization: Bearer ";
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/me";
  url=url + devid;
  printf("Get user info.\n");
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
      return(200);
    }
  }
  return(httpCode);
}

// ****************************************************************************************
//
// Works
// get device list and have it in tidal class
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
  sprintf(call,"curl -f -X GET 'https://api.tidal.com/v1/me/player/devices' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > tidal_device_list.json 2>&1",tidaltoken);
  curl_exitcode=system(call);
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
          //if (( tidal_device[devicenr].is_active ) && (active_tidal_device==-1)) tidal_device=devicenr;
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
    fprintf(stderr,"Error loading device list from tidal by api.\n");
  }
  return active_tidal_device;
}


// ****************************************************************************************
//
// get user access token
// write to tidal_access_token
//
// ****************************************************************************************

int tidal_class::tidal_get_access_token2() {
  struct mg_connection *nc;
  // works
  //mg_mgr_init(&tidal_oversigt.client_mgr, NULL);
  //nc = mg_connect_http(&tidal_oversigt.client_mgr, ev_handler, "https://accounts.tidal.com/api/token", "Content-Type: application/x-www-form-urlencoded\r\n", "");
  //mg_set_protocol_http_websocket(nc);
//  while (s_exit_flag == 0) {
//    mg_mgr_poll(&mgr, 1000);
//  }
  FILE *myfile;
  char data[4096];
  char call[4096];
  char *base64_code;
  strcpy(data,tidal_client_id);
  strcat(data,":");
  strcat(data,tidal_secret_id);
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  // works
  sprintf(call,"curl -X 'POST' -H 'Authorization: Basic %s' -d grant_type=client_credentials https://accounts.tidal.com/api/token > tidal_access_token.txt",base64_code);
  myfile = fopen("tidal_access_token.txt","r");
  if (myfile) {
    fgets(data,4095,myfile);                      // read file
    fclose(myfile);
    //remove("tidal_access_token.txt");           // remove file again
  }
  return(1);
}





// ****************************************************************************************
//
// return the intnr
//
// ****************************************************************************************

int tidal_class::get_tidal_intnr(int nr) {
  if (nr < antal) return (stack[nr]->intnr); else return (0);
}

// ****************************************************************************************
//
// return the playlist/song name
//
// ****************************************************************************************

char *tidal_class::get_tidal_name(int nr) {
  if (nr < antal) return (stack[nr]->feed_name); else return (NULL);
}

// ****************************************************************************************
//
// return the tidal playlist id
//
// ****************************************************************************************

char *tidal_class::get_tidal_playlistid(int nr) {
  if (nr < antal) return (stack[nr]->playlistid); else return (NULL);
}

// ****************************************************************************************
//
// return the description
//
// ****************************************************************************************

char *tidal_class::get_tidal_desc(int nr) {
  if (nr < antal) return (stack[nr]->feed_desc); else return (NULL);
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
          if (&stack[i]->textureId) {
            if (&stack[i]->textureId) {
              //if (&stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete tidal texture
            }
          }
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


// ****************************************************************************************
//
// set tidal icon image
//
// ****************************************************************************************

void tidal_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}

// ****************************************************************************************
//
// get nr of tidal playlists
//
// ****************************************************************************************

int tidal_class::get_antal_rss_feeds_sources(MYSQL *conn) {
  int antalrss_feeds=0;
  MYSQL_RES *res;
  MYSQL_ROW row;
  if (conn) {
    mysql_query(conn,"SELECT count(name) from mythtvcontroller.tidalcontent");
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
    char *database = (char *) "mythtvcontroller";
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
    write_logfile((char *) "loading tidal data.");
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
    // write debug log
    write_logfile((char *) "tidal loader started...");
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
                      download_image(row[1],downloadfilenamelong);
                      //get_webfile2(row[1],downloadfilenamelong);                // download file
                    } else strcpy(downloadfilenamelong,row[1]);                 // no file name
                  } else strcpy(downloadfilenamelong,row[1]);
                  strncpy(stack[antal]->feed_gfx_url,downloadfilenamelong,1024);
                }
                strncpy(stack[antal]->playlistid,row[2],tidal_namelength);    //

                /*
                strncpy(downloadfilenamelong,row[2],tidal_pathlength);
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
                  strcat(downloadfilenamelong,"/datadisk/mythtv-controller-0.38/tidal_gfx/");
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
                if (row[3]) strncpy(stack[antal]->feed_desc,row[3],tidal_desclength);
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
                //songstrpointer=strstr(row[2],"https://api.tidal.com/v1/tracks/");
                songstrpointer=strstr(row[2],"tidal:track:");
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
        //if (debugmode & 4) fprintf(stderr,"No tidal data loaded \n");
        write_logfile((char *) "No tidal data loaded");
      }
      antalplaylists=antal;
      return(antal);
    } else fprintf(stderr,"Failed to update tidal db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
    //if (debugmode & 4) fprintf(stderr,"Tidal loader done... \n");
    write_logfile((char *) "Tidal loader done...");
    return(0);
}



// ****************************************************************************************
//
// search for playlist or song in db (from users playlist data in db)
//
// ****************************************************************************************


int tidal_class::opdatere_tidal_oversigt_searchtxt(char *keybuffer,int type) {
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
  char *database = (char *) "mythtvcontroller";
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
    mysql_query(conn,"SELECT feedtitle from mythtvcontroller.tidalcontentarticles limit 1");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dbexist=true;
      }
    }

    clean_tidal_oversigt();                                                   // clean old list

    // find records after type (0 = root, else = refid)
    if (type == tidal_playlisttype ) {
      sprintf(sqlselect,"select playlistname,paththumb,playlistid,id from tidalcontentplaylist where playlistname like '");
      strcat(sqlselect,"%");
      strcat(sqlselect,keybuffer);
      strcat(sqlselect,"%'");
      getart = 0;
    } else {
      sprintf(sqlselect,"select name,paththumb,player,playlistid,id from tidalcontentarticles where name like '");
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
            stack[antal]->nyt=false;
            if (getart == tidal_playlisttype ) {
              stack[antal]->type=0;
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
              strncpy(stack[antal]->playlistid,row[2],tidal_namelength);
              stack[antal]->intnr=atoi(row[3]);
              strcpy(downloadfilename,"");
              strcpy(downloadfilenamelong,"");
              get_webfilename(downloadfilename,stack[antal]->feed_gfx_url);
              getuserhomedir(downloadfilenamelong);
              strcat(downloadfilenamelong,"/");
              strcpy(downloadfilenamelong,tidal_gfx_path);
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
            if (getart == tidal_songlisttype ) {
              stack[antal]->type=1;
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
              strncpy(stack[antal]->playlisturl,row[2],tidal_namelength);
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



// ****************************************************************************************
// NOt working not in use
// download file from search result
//
// ****************************************************************************************

int tidal_class::get_search_result_online(char *searchstring,int type) {
  static const char *userfilename = "tidal_search_result.json";
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
  auth_kode=auth_kode + tidaltoken;
  url="https://api.tidal.com/v1/search";
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
    //curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
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



// ****************************************************************************************
//
// thread web loader (loading all icons)
//
// ****************************************************************************************

void *load_tidal_web(void *data) {
  write_logfile((char *) "Start tidal loader thread");
  //streamoversigt.loadweb_stream_iconoversigt();
  write_logfile((char *) "Stop tidal loader thread");
}




// ****************************************************************************************
//
// loading tidal songs gfx in array.
//
// ****************************************************************************************

void tidal_class::settextureidfile(int nr,char *filename) {
    if (stack[nr]->textureId==0) stack[nr]->textureId=loadTexture ((char *) filename);
}


// ****************************************************************************************
//
// loading tidal songs gfx in array.
// if filename is url then change it to file name by the last name in the source http filename path
//
// ****************************************************************************************

int tidal_class::load_tidal_iconoversigt() {
  int nr=0;
  int loadstatus;
  char *imagenamepointer;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  this->gfx_loaded=false;                                                           // set loaded flag to false
  // debug log
  write_logfile((char *) "Running tidal icon loader.");
  while(nr<=streamantal()) {
    if (debugmode & 4) printf("Loading texture nr %-4d Title %40s  icon path %s\n",nr,stack[nr]->feed_name,stack[nr]->feed_gfx_url);
    if ((stack[nr]) && (strcmp(stack[nr]->feed_gfx_url,"")!=0)) {
      if (stack[nr]->textureId==0) {
        // if url
        if (strncmp("http",stack[nr]->feed_gfx_url,4)==0) {
          imagenamepointer=strrchr(stack[nr]->feed_gfx_url,'\/');
          if ((imagenamepointer) && (strlen(imagenamepointer)<1990)) {
            getuserhomedir(tmpfilename);
            strcat(tmpfilename,"/tidal_gfx/");
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
  // write debug log
  if (gfx_loaded) write_logfile((char *) "tidal download done.");
  else write_logfile((char *) "tidal download error.");
  gfx_loaded=true;
  return(1);
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


void tidal_class::select_device_to_play() {
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
// show tidal overview
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
    if (tidal_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
    // load icons
    if (this->search_loaded) {
      this->search_loaded=false;
      printf("Searech loaded done. Loading icons\n");
      load_tidal_iconoversigt();                       // load icons
    }
    // draw icons
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
//        glBindTexture(GL_TEXTURE_2D,tidal_icon_border);                               // normal icon then the tidal have icon
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


    // no records loaded error
    if ((i==0) && (antal_tidal_streams()==0)) {
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
      if (tidal_oversigt.search_tidal_online_done) glcRenderString("   Loading ..."); else glcRenderString("   Please run update ...");
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();

      glPushMatrix();
      xof=700;
      yof=310;
      glTranslatef(xof, yof ,0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glDisable(GL_TEXTURE_2D);
      glScalef(22.0, 22.0, 1.2);
      glcRenderString("           Tidal");
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();

    }
}




// ****************************************************************************************
//
// show search/create playlist tidal overview
//
// ****************************************************************************************

void tidal_class::show_tidal_search_oversigt(GLuint normal_icon,GLuint song_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring) {
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
    if (tidal_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
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
      printf("Search loaded done. Loading icons\n");
      load_tidal_iconoversigt();                       // load icons
    }
    // draw icons
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
        //glBindTexture(GL_TEXTURE_2D,tidal_icon_border);                               // normal icon then the tidal have icon
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    if ((i==0) && (antal_tidal_streams()==0)) {
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
        //glBindTexture(GL_TEXTURE_2D,tidal_pil);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
// ********************* show setup tidal stuf like dev and clientid/secrect ************
//
// ****************************************************************************************

void tidal_class::show_setup_tidal() {
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
    static int tidal_device_antal=0;
    static bool first_time_update=true;
    char *database = (char *) "mythtvcontroller";
    if (first_time_update) {
      first_time_update=false;
      conn = mysql_init(NULL);
      if (conn) {
        mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,"SELECT device_name,active,devtype,intnr from mythtvcontroller.tidal_device limit 10");
        res = mysql_store_result(conn);
        if (res) {
          while (((row = mysql_fetch_row(res)) != NULL) && (dev_nr<10)) {
            strcpy(tidal_device[dev_nr].name,row[0]);
            if (strcmp(row[1],"1")==0) {
              tidal_device[dev_nr].is_active=true;
            } else {
              tidal_device[dev_nr].is_active=false;
            }
            strcpy(tidal_device[dev_nr].devtype,row[2]);
            // is one the default device set in config
            // and exist in db
            if ((strcmp(active_default_play_device_name,"")!=0) && (strcmp(active_default_play_device_name,row[0])==0)) {
              active_default_play_device=dev_nr;
              //tidal_device[0].is_active=true;
              //strcpy(tidal_device[0].name,active_default_play_device_name);
            } else {
              /// set active device
              if ((active_default_play_device!=-1) && (tidal_device[dev_nr].is_active)) {
                // is default defined in config set it active
                if (strcmp(active_default_play_device_name,"")!=0) {
                  if (strcmp(active_default_play_device_name,row[1])==0) active_default_play_device=dev_nr;
                } else {
                  active_default_play_device=dev_nr;
                }
              }
            }
            dev_nr++;
            tidal_device_antal++;
          }
        }
      }

    }
    // tidal setup
    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBindTexture(GL_TEXTURE_2D,setuprssback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    if (tidal_device_antal<4) {
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
    glcRenderString("Tidal Account setup");
    glPopMatrix();
    // close buttons
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (tidal_device_antal<4) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glcRenderString(tidal_client_id);
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 600, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (do_show_setup_select_linie==1) glColor4f(1.0f, 1.0f, 0.0f, 1.0f); else glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Client Secrect   ");
    glcRenderString(tidal_secret_id);
    if ((keybuffer) && (do_show_setup_select_linie>=0)) showcoursornow(301,500-(do_show_setup_select_linie*50),strlen(keybuffer));
    glPopMatrix();
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
    else glcRenderString(tidal_device[active_default_play_device].name);
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glTranslatef(520, 460, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(15.0, 15.0, 1.0);
    glcRenderString("Device avable ");
    glPopMatrix();
    if (strcmp(tidal_device[0].name,"")!=0) {
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
      if (strcmp(tidal_device[0].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[0].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[0].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[0].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(tidal_device[1].name,"")!=0) {
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
      if (strcmp(tidal_device[1].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[1].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[1].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[1].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(tidal_device[2].name,"")!=0) {
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
      if (strcmp(tidal_device[2].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[2].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[2].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[2].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(tidal_device[3].name,"")!=0) {
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
      if (strcmp(tidal_device[3].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[3].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[3].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[3].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    // row 2
    if (strcmp(tidal_device[4].name,"")!=0) {
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
      if (strcmp(tidal_device[4].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[4].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[4].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[4].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(tidal_device[5].name,"")!=0) {
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
      if (strcmp(tidal_device[4].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[4].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[4].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[5].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }

    if (strcmp(tidal_device[6].name,"")!=0) {
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
      if (strcmp(tidal_device[5].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[5].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[5].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[6].name;
      devname.resize(10);
      glcRenderString(devname.c_str());
      glPopMatrix();
    }
    if (strcmp(tidal_device[7].name,"")!=0) {
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
      if (strcmp(tidal_device[6].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(tidal_device[6].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(tidal_device[6].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      devname=tidal_device[7].name;
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

void tidal_class::set_default_device_to_play(int nr) {
   active_tidal_device=nr;
   active_default_play_device=nr;
}
