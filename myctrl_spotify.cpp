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
#include "json-parser/json.h"
#include "readjpg.h"
#include "myctrl_readwebfile.h"
#include "myth_setup.h"

// web server stuf
#include "mongoose-master/mongoose.h"

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

const int spotify_pathlength=80;
const int spotify_namelength=80;
const int spotify_desclength=2000;

const int feed_url=2000;

#include "utility.h"
#include "myth_ttffont.h"
#include "utility.h"
#include "readjpg.h"
#include "loadpng.h"
// web file loader
#include "myctrl_readwebfile.h"
#include "myctrl_spotify.h"

//
// text render is glcRenderString for freetype font support
//



extern int spotifyknapnr;
extern int spotify_select_iconnr;

extern spotify_class spotify_oversigt;
extern GLuint _texturemovieinfobox;

extern GLuint big_search_bar;

extern char *keybuffer;
extern int keybufferindex;

extern bool do_select_device_to_play;

extern GLuint mobileplayer_icon;
extern GLuint pcplayer_icon;
extern GLuint unknownplayer_icon;

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
//extern GLuint _textureIdloading_mask;


// stream mask
extern GLuint onlinestreammask;
extern GLuint onlinestreammaskicon;		// icon mask on web icon
extern spotify_class streamoversigt;
extern GLint cur_avail_mem_kb;
extern bool stream_loadergfx_started;
extern bool stream_loadergfx_started_done;
extern bool stream_loadergfx_started_break;

// server handler

static void server_ev_handler(struct mg_connection *c, int ev, void *ev_data) {
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
      // is callback call
      if (mg_strncmp( hm->uri,mg_mk_str_n("/callback",9),9) == 0) {
        printf("Got reply server : %s \n", hm->uri);
        p = strstr( hm->uri.p , "code="); // mg_mk_str_n("code=",5));
        // get soptify code from server
        if (p) {
          pspace=strchr(p,' ');
          if (pspace) {
            codel=(pspace-p);
            strncpy(user_token,p+5,pspace-p);
            *(user_token+(pspace-p))='\0';
          }
          user_token[codel-4]='\0';
        }
        //sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.spotify.com/api/token > spotify_access_token2.txt",base64_code,user_token);
        sprintf(sql,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=authorization_code -d code=%s -d redirect_uri=http://localhost:8000/callback/ -d client_id=%s -d client_secret=%s -H 'Content-Type: application/x-www-form-urlencoded' https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code,user_token,spotify_oversigt.spotify_client_id,spotify_oversigt.spotify_secret_id);
        //printf("sql curl : %s \n ",sql);
        curl_error=system(sql);
        if (curl_error==0) {
          curl_error=system(sed);
          if (curl_error==0) {
          }
          printf("\n******** Got token ********\n");
          tokenfile=fopen("spotify_access_token2.txt","r");
          error=getline(&file_contents,&len,tokenfile);
          strcpy(token_string,file_contents);
          token_string[strlen(token_string)-1]='\0';
          error=getline(&file_contents,&len,tokenfile);
          strcpy(token_refresh,file_contents);
          token_refresh[strlen(token_refresh)-1]='\0';
          printf("token     %s\n",token_string);
          printf("ref token %s\n",token_refresh);
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
      printf("***************************************** CALL BACK server reply ***************************************");
      c->flags |= MG_F_CLOSE_IMMEDIATELY;
      fwrite(hm->body.p, 1, hm->body.len, stdout);
      putchar('\n');
      break;
    case MG_EV_CLOSE:
      printf("Server closed connection\n");
  }
}



// client handler
// get JSON return from spotify

static int s_exit_flag = 0;
bool debug_json=false;


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  int connect_status;
  switch (ev) {
      case MG_EV_CONNECT:
        connect_status = *(int *) ev_data;
        if (connect_status != 0) {
          printf("Error connecting %s\n", strerror(connect_status));
          s_exit_flag = 1;
        }
        break;
      case MG_EV_HTTP_REPLY:
        fwrite(hm->message.p, 1, (int)hm->message.len, stdout);
        printf("Got reply client :\n%.*s\n", (int) hm->body.len, hm->body.p);
        printf("***************************************** CALL BACK **************************************************");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        s_exit_flag = 1;
        break;
      case MG_EV_CLOSE:
        if (s_exit_flag == 0) {
          printf("Server closed connection\n");
          s_exit_flag = 1;
        };
        break;
      default:
        break;
    }
}



//
// constructor spotify devices
//

spotify_device_def::spotify_device_def() {
  strcpy(id,"");
  is_active=false;
  is_private_session=false;
  is_restricted=false;
  strcpy(name,"");
  strcpy(devtype,"");
  devvolume=0;
}

//
// constructor spotify active player
//


spotify_active_play_info_type::spotify_active_play_info_type() {                // sample data down here
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



//
// constructor spotify oversigt type
//

spotify_oversigt_type::spotify_oversigt_type() {
  strcpy(feed_showtxt,"");
  strcpy(feed_name,"");
  strcpy(feed_desc,"");
  strcpy(feed_gfx_url,"");
  strcpy(feed_gfx_mythtv,"");
  strcpy(playlistid,"");
  strcpy(playlisturl,"");
  feed_group_antal=0;
  feed_path_antal=0;
  nyt=false;
  textureId=0;
  intnr=0;
};




//
// constructor
//

spotify_class::spotify_class() : antal(0) {
    for(int i=0;i<maxantal;i++) stack[i]=0;
    stream_optionselect=0;							                                        // selected line in stream options
    spotify_oversigt_loaded=false;
    spotify_oversigt_loaded_nr=0;
    antal=0;
    spotify_aktiv_song_antal=0;                                                 //
    gfx_loaded=false;			                                                      // gfx loaded
    spotify_is_playing=false;                                                   // is we playing any media
    spotify_is_pause=false;                                                     // is player on pause
    show_search_result=false;                                                   // are we showing search result in view
    antalplaylists=0;                                                           // antal playlists
    search_playlist_song=0;
    int port_cnt, n;
    int err = 0;
    strcpy(spotify_aktiv_song[0].release_date,"");
    // create web server
    mg_mgr_init(&mgr, NULL);                                                    // Initialize event manager object
    // start web server
    fprintf(stdout,"Starting web server on port %s\n", s_http_port);
    this->c = mg_bind(&mgr, s_http_port, server_ev_handler);                    // Create listening connection and add it to the event manager
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
    spotify_device_antal=0;
}


//
// destructor
//
spotify_class::~spotify_class() {
    mg_mgr_free(&mgr);                        // delete web server again
    mg_mgr_free(&client_mgr);
    clean_spotify_oversigt();
}


//
// refresh token
//

int spotify_class::spotify_refresh_token() {
  int curl_error;
  char doget[2048];
  char data[4096];
  char call[4096];
  FILE *tokenfil;
  char *base64_code;
  char newtoken[1024];
  strcpy(data,spotify_client_id);
  strcat(data,":");
  strcat(data,spotify_secret_id);
  strcpy(newtoken,"");
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  if (strcmp(spotifytoken,"")!=0) {
    sprintf(doget,"curl -X POST -H 'Authorization: Basic %s' -d grant_type=refresh_token -d refresh_token=%s https://accounts.spotify.com/api/token > spotify_refresh_token.txt", base64_code ,spotifytoken_refresh);
    curl_error=system(doget);
    if (WEXITSTATUS(curl_error)==0) {
      printf("Ok Spotify new token. ");
      tokenfil=fopen("spotify_refresh_token.txt","rt");
      if (tokenfil) {
        fgets(data,4096,tokenfil);
        if (strcmp(data,"")!=0) {
          if (strncmp(data,"{\"access_token\":",16)==0) {
            strncpy(newtoken,data+17,180);
            newtoken[181]='\0';
          }
        }
        printf(" %s \n",newtoken);
        strcpy(spotifytoken,newtoken);                                         // update spotify token
        fclose(tokenfil);
      }
    } else {
      printf("Error Spotify renew token.\n");
    }
  }
  return 1;
}



//
// Spotify Get user id
//

int spotify_class::spotify_get_user_id() {
  int curl_error;
  char doget[2048];
  if (strcmp(spotifytoken,"")!=0) {
    sprintf(doget,"curl -X GET 'https://api.spotify.com/v1/me' -H 'Authorization: Bearer %s' > spotify_user_id.txt",spotifytoken);
    printf("curl = %s \n",doget);
    curl_error=system(doget);
    if (WEXITSTATUS(curl_error)==0) {
    }
  }
  return 1;
}


//
// save the token in used
//

void spotify_class::spotify_set_token(char *token,char *refresh) {
  strcpy(spotifytoken,token);
  strcpy(spotifytoken_refresh,refresh);
}

//
// Get a List of a User's Playlists
// get only the id of playlists (not songs)
//

int spotify_class::spotify_get_list_of_users_playlists(char *client_id) {
  int curl_error;
  char doget[2048];
  if (strcmp(spotifytoken,"")!=0) {
    sprintf(doget,"curl -X GET 'https://api.spotify.com/v1/users/%s/playlists' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.json",client_id,spotifytoken);
    //printf("doget = %s \n",doget);
    //system(doget);
    if (WEXITSTATUS(curl_error)==0) {
    }
  }
}



//
//  json parser used to parse the return files from spotify api
//


void spotify_class::playlist_print_depth_shift(int depth) {
  bool debug_json=false;
  int j;
  for (j=0; j < depth; j++) {
    if (debug_json) printf(" ");
  }
}

//
// static void spotify_class::process_value(json_value* value, int depth);
//
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

//
// process types in file playlist
//

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


//
// json parser start call function playlist db update
//

void spotify_class::playlist_process_value(json_value* value, int depth,int x,MYSQL *conn) {

    char filename[512];
    char downloadfilenamelong[2048];

    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythtvcontroller";
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
        if (( playlist_process_image ) && ( depth == 8 ) && ( x == 1 )) {
          playlist_process_image=false;
        }
        // works for songs not playlist
        if (playlist_process_url) {
          printf("gfx icon found = %s \n", value->u.string.ptr);
          strcpy(filename,"");
          get_webfilenamelong(filename,value->u.string.ptr);
          strcpy(downloadfilenamelong,value->u.string.ptr);
          if (strcmp(filename,"")!=0) {
            strcpy(downloadfilenamelong,"tmp/");
            strcat(downloadfilenamelong,filename);
            strcat(downloadfilenamelong,".jpg");
          }
          strcpy(playlistgfx,downloadfilenamelong);
          playlist_process_url=false;
        }
        if (playlist_process_name) {
          //printf("x = %2d deep=%2d ",x,depth);
          //printf(" string = %10s \n ",value->u.string.ptr);
        }
        // get playlist name
        // and create datdabase record playlist info
        if ((playlist_process_name) && (depth==5) && (x==5)) {
          //if (debug_json) printf("Name found = %s id %s \n", value->u.string.ptr,playlistid);
          strcpy(playlistname,value->u.string.ptr);
          playlistexist=false;
          sprintf(sql,"select id from mythtvcontroller.spotifycontentplaylist where playlistname like '%s' limit 1", playlistname );
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              playlistexist=true;
            }
          }
          if (!(playlistexist)) {
            sprintf(sql,"insert into mythtvcontroller.spotifycontentplaylist values ('%s','%s','%s',0)",playlistname,playlistgfx,playlistid);
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
          }
          playlist_process_name=false;
        }
        break;
      case json_boolean:
        if (debug_json) printf("bool: %d\n", value->u.boolean);
        break;
    }
}



// Get a List of Current User's Playlists
//
// get users play lists
// write to spotify_users_playlist.json
//

int spotify_class::spotify_get_user_playlists(bool force) {
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  MYSQL_RES *res1;
  MYSQL_ROW row1;
  char *database = (char *) "mythtvcontroller";
  char sql[8192];
  char doget[4096];
  char filename[4096];
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
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
       mysql_error(conn);
       //exit(1);
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
      sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontent (name varchar(255),paththumb text,playid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sql)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sql);
      }
      res = mysql_store_result(conn);
      // create db (spotify songs)
      sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentarticles (name varchar(255),paththumb text,gfxfilename varchar(255),player varchar(255),playlistid varchar(255),artist varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sql)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sql);
      }
      res = mysql_store_result(conn);
      // create db (spotify playlists)
      sprintf(sql,"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentplaylist (playlistname varchar(255),paththumb text,playlistid varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sql)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sql);
      }
      res = mysql_store_result(conn);
    }
    // download all playlist (name + id) in one big file NOT songs
    if (strcmp(spotifytoken,"")!=0) {
      sprintf(doget,"curl -X GET 'https://api.spotify.com/v1/me/playlists?limit=50&offset=0' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.json",spotifytoken);
      printf("doget = %s \n",doget);
      curl_error=system(doget);
      if (curl_error!=0) {
        fprintf(stderr,"Curl error get user playlists\n");
      }
      stat("spotify_users_playlist.json", &filestatus);                              // get file info
      file_size = filestatus.st_size;                                               // get filesize
      file_contents = (char*) malloc(filestatus.st_size);
      json_file = fopen( "spotify_users_playlist.json", "rt");
        if (json_file == NULL) {
        fprintf(stderr, "Unable to open spotify_users_playlist.json\n");
        free(file_contents);                                                        //
        return 1;
      }
      if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
        fprintf(stderr, "Unable to read spotify playlist content of spotify_users_playlist.json\n");
        fclose(json_file);
        free(file_contents);                                                        //
        return 1;
      }
      fclose(json_file);
      json = (json_char*) file_contents;
      value = json_parse(json,file_size);                                           // parse playlist file
      // parse from root and inster in db (playlist table)
      playlist_process_value(value, 0,0,conn);                                      // fill stack array
      json_value_free(value);                                                       // json clean up
      free(file_contents);                                                          // free memory again
      // save data to mysql db
    }
    sprintf(sql,"select playlistname,playlistid from mythtvcontroller.spotifycontentplaylist");
    printf("process playlist ...... \n");
    mysql_query(conn,sql);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        printf("playlist %-60s Spotifyid %-20s \n",row[0],row[1]);
        if (spotify_oversigt.spotify_get_playlist(row[1],force)==1) {
          printf("Error create playlist %s \n",row[1]);
        }
      }
    }
    printf("process playlist done.. \n");
    mysql_close(conn);
  }
}

// json parser used to parse the return files from spotify api
//
//

void spotify_class::print_depth_shift(int depth)
{
  int j;
  for (j=0; j < depth; j++) {
    if (debug_json) printf(" ");
  }
}

//
// static void spotify_class::process_value(json_value* value, int depth);
//

bool process_tracks=false;
bool process_playlist=false;
bool process_songs=false;
bool process_href=false;
bool process_description=false;
bool process_image=false;
bool process_name=false;
bool process_items=false;
bool process_track_nr=false;

//
// process types in file for process playlist files (songs)
//

void spotify_class::process_object(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    //if (strcmp(value->u.object.values[x].name,"name")==0)
    if (debug_json) printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"tracks")==0) {
      process_tracks=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      process_href=true;
    }
    if (strcmp(value->u.object.values[x].name , "process_playlist" )==0) {
      process_playlist=true;
    }
    if (strcmp(value->u.object.values[x].name , "description" )==0) {
      process_description=true;
    }
    if (strcmp(value->u.object.values[x].name , "images" )==0) {
      process_image=true;
    }
    if (strcmp(value->u.object.values[x].name , "name" )==0) {
      process_name=true;
    }
    if (strcmp(value->u.object.values[x].name , "track_number" )==0) {
      process_track_nr=true;
    }
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      process_items=true;
    }
    process_value(value->u.object.values[x].value, depth+1,x);
  }
}


void spotify_class::process_array(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  if (debug_json) printf("array\n");
  for (x = 0; x < length; x++) {
    process_value(value->u.array.values[x], depth,x);
  }
}


//
// json parser start call function for process playlist
// do the data progcessing from json

void spotify_class::process_value(json_value* value, int depth,int x) {
    int j;
    if (value == NULL) return;
    if (value->type != json_object) {
      if (debug_json) print_depth_shift(depth);
    }
    switch (value->type) {
      case json_none:
        if (debug_json) printf("none\n");
        break;
      case json_object:
        process_object(value, depth+1);
        break;
      case json_array:
        process_array(value, depth+1);
        break;
      case json_integer:
        if (debug_json) printf("int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        if (debug_json) printf("double: %f\n", value->u.dbl);
        break;
      case json_string:
        //if (debug_json) printf("string: %s\n", value->u.string.ptr);
        if ((process_description) && (depth==11) && (x==7)) {
          if (!(stack[antal])) {
            antal++;
            stack[antal]=new (struct spotify_oversigt_type);
          }
          //strcpy(stack[antal]->feed_desc,value->u.string.ptr);                           // desc
          process_description=false;
        }
        if (process_items) {
          // set start of items in list
          process_items=false;
        }
        if (process_tracks) {
          // playlist id from spotify
          if (stack[antal]) {
            //strcpy(stack[antal]->playlistid,value->u.string.ptr);                           // playlistid
          }
          process_tracks=false;
        }
        if (( process_image ) && ( depth == 14 ) && ( x == 1 )) {
          if (stack[antal]) {
            strcpy( stack[antal]->feed_gfx_url , value->u.string.ptr );                           //
          }
          process_image=false;
        }
        if ((process_href) && (depth==9) && (x==9)) {
          if (stack[antal]) {
            strcpy( stack[antal]->playlisturl , value->u.string.ptr );                           //
          }
          process_href=false;
        }
        // get playlist name
        if ((process_name) && (depth==2) && (x==7)) {
          strcpy(spotify_playlistname , value->u.string.ptr);
        }
        // get Song name
        if ((process_name) && (depth==9) && (x==12)) {
          if (antal==0) {
            stack[antal]=new (spotify_oversigt_type);
          } else {
            // new record
            antal++;
            antalplaylists++;
          }
          if (antalplaylists<maxantal) {
            if (!(stack[antal]))  {
              stack[antal]=new (spotify_oversigt_type);
            }
            if (stack[antal]) {
              strcpy(stack[antal]->feed_name,value->u.string.ptr);
            }
          }
          process_name=false;
        }
        // get artis
        if ((process_name) && (depth==12) && (x==3)) {
          if (stack[antal]) strcpy( stack[antal]->feed_artist , value->u.string.ptr );
        }

        // get tracknr
        if (process_track_nr) {
          process_track_nr=false;
          //printf(" text %s \n",value->u.string.ptr);
          // if (stack[antal]) strcpy( stack[antal]->feed_artist , value->u.string.ptr );
        }

        break;
      case json_boolean:
        if (debug_json) printf("bool: %d\n", value->u.boolean);
        break;
    }
}



// work
// get songs from playlist (any public user)
// write to spotify_playlist_{spotifyid}.json
// and update db from that file

int spotify_class::spotify_get_playlist(char *playlist,bool force) {
  int tt;
  bool dbexist=false;
  int refid;
  int curl_error;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  char playlistfilename[1024];
  strcpy(playlistfilename,"spotify_playlist_");                                 // create playlist file name
  strcat(playlistfilename,playlist);                                            // add the spotify playlist id
  strcat(playlistfilename,".json");
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
  if ((!(file_exists(playlistfilename))) || (force))  {
    if ((strcmp(spotifytoken,"")!=0) && (strcmp(playlist,"")!=0)) {
      sprintf(doget,"curl -X 'GET' 'https://api.spotify.com/v1/playlists/%s' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > %s",playlist,spotifytoken,playlistfilename);
      curl_error=system(doget);
      if (curl_error!=0) {
        fprintf(stderr,"Curl error get playlist \n");
        fprintf(stderr,"Curl error %s \n",doget);
        //return 1;
      }
    }
    stat(playlistfilename, &filestatus);                                          // get file info
    file_size = filestatus.st_size;                                               // get filesize
    file_contents = (char*)malloc(filestatus.st_size);
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
    process_value(value, 0,0);                                                    // fill stack array
    json_value_free(value);                                                       // json clean up
    free(file_contents);                                                          //
    // save data to mysql db
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
      if ((!(dbexist)) && (mysql_query(conn,"CREATE database if not exist mythtvcontroller")!=0)) {
        //printf("mysql db create error. mythtvcontroller\n");
      }
      //
      // create db if not exist
      res = mysql_store_result(conn);
      // create db spotify playlist process data
      // insert all record in db
      tt = 0;
      while(tt<antalplaylists) {
        if (stack[tt]) {
          if (debugmode & 4) printf("Track nr #%2d Name %40s url %s  gfx url %s \n",tt,stack[tt]->feed_name,stack[tt]->playlisturl,stack[tt]->feed_gfx_url);
          get_webfilename(filename,stack[tt]->feed_gfx_url);
          if (strcmp(filename,"")) {
            strcpy(downloadfilenamelong,"tmp/");
            strcat(downloadfilenamelong,filename);
            strcat(downloadfilenamelong,".jpg");
            if (!(file_exists(downloadfilenamelong))) {
              // download icon image
              get_webfile2(stack[tt]->feed_gfx_url,downloadfilenamelong);
            }
          }
          // check if playlist exist
          playlistexist=false;
          refid = 0;
          sprintf(sql,"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname , "", "" , 0 );
          mysql_query(conn,sql);
          res = mysql_store_result(conn);
          if (res) {
            while ((row = mysql_fetch_row(res)) != NULL) {
              refid=atoi(row[0]);
              playlistexist=true;
            }
          }
          if (!(playlistexist)) {
            sprintf(sql,"insert into mythtvcontroller.spotifycontent (name,paththumb,playid,id) values ('%s','%s','%s',%d)", spotify_playlistname , stack[tt+1]->feed_gfx_url,playlist, 0 );
            mysql_query(conn,sql);
            res=mysql_store_result(conn);
            if (refid==0) {
              sprintf(sql,"select id from mythtvcontroller.spotifycontent where name like '%s' limit 1", spotify_playlistname , "", "" , 0 );
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
            sprintf(sql,"select id from mythtvcontroller.spotifycontentarticles where name like '%s' limit 1", stack[tt+1]->feed_name );
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
              sprintf(sql,"insert into mythtvcontroller.spotifycontentarticles (name,paththumb,gfxfilename,player,playlistid,artist,id) values ('%s','%s','%s','%s','%s','%s',%d)", stack[tt+1]->feed_name , stack[tt+1]->feed_gfx_url,downloadfilenamelong, stack[tt+1]->playlisturl, playlist , stack[tt+1]->feed_artist , 0 );
              mysql_query(conn,sql);
              mysql_store_result(conn);
            }
          }
        }
        tt++;
        //printf("tt=%d playlist id %s \n",tt,playlist);
      }
    }
    mysql_close(conn);
  }
  return 0;
}


// *********************************************************************************************************************************


//
// work
// get active song playing
//

int spotify_class::spotify_do_we_play() {
  bool find_length=true;
  bool find_progress=true;
  char call[4096];
  FILE *json_file;
  int file_size;
  int curl_error;
  ssize_t nread;
  char *file_contents=NULL;
  size_t stringlength=0;
  char filename_song_playing[]="spotify_song_playing.json";
  char sed[]="cat spotify_song_playing.json | sed 's/\\\\\\\\\\\//\\//g' | sed 's/[{\\\",}]//g' | sed 's/ //g' | sed 's/:/=/g' > spotify_song_playing.txt";
  sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/me/player' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s ' > spotify_song_playing.json",spotifytoken);
  curl_error=system(call);
  if (curl_error>0) {
    fprintf(stderr,"curl_error = %d \n",curl_error);
    fprintf(stderr,"curl *%s* \n",call);
  }
  if (curl_error==0) {
    curl_error=system(sed);                                                     // make info file (.txt)
    strcpy(filename_song_playing,"spotify_song_playing.txt");
    json_file = fopen(filename_song_playing, "rt");
    if (json_file == NULL) {
      fprintf(stderr, "Unable to open %s\n", filename_song_playing);
      free(file_contents);
      return 0;
    }
    spotify_aktiv_song_antal=0;
    this->spotify_is_playing=true;                                                                  // set play flag
    while ((nread = getline(&file_contents, &stringlength, json_file)) != -1) {
      if (nread>4) {
        if ((find_length) && (strncmp(file_contents,"duration_ms=",12)==0)) {
          find_length=false;
          spotify_aktiv_song[spotify_aktiv_song_antal].duration_ms=atol(file_contents+12);
        }
        if ((find_progress) && (strncmp(file_contents,"progress_ms=",12)==0)) {
          find_progress=false;
          spotify_aktiv_song[spotify_aktiv_song_antal].progress_ms=atol(file_contents+12);
        }
        if (strncmp(file_contents,"name=",5)==0) {
          strcpy(spotify_aktiv_song[spotify_aktiv_song_antal].song_name,file_contents+5);
        }
        if (strncmp(file_contents,"release_date=",13)==0) {
          strcpy(spotify_aktiv_song[spotify_aktiv_song_antal].release_date,file_contents+13);
        }
        if (strncmp(file_contents,"url=https=//i.scdn.co/image/",28)==0) {
          if (aktiv_song_spotify_icon==0) {
            strcpy(spotify_aktiv_song[spotify_aktiv_song_antal].cover_image_url,"https:");
            strcat(spotify_aktiv_song[spotify_aktiv_song_antal].cover_image_url,file_contents+10);
            //get_webfile2(spotify_aktiv_song[spotify_aktiv_song_antal].cover_image_url,"/tmp/gfx_icon.jpg");
            //aktiv_song_spotify_icon=loadTexture("/tmp/gfx_icon.jpg");
          }
        }
      }
    }
    // get artist name play
    if (spotifyknapnr>0) strcpy(spotify_aktiv_song[spotify_aktiv_song_antal].artist_name,stack[spotifyknapnr-1]->feed_artist);
    else strcpy(spotify_aktiv_song[spotify_aktiv_song_antal].artist_name,"Unknown");
    free(file_contents);
    fclose(json_file);
  } else {
    fprintf(stderr,"Error processs playing info\n");
  }
  return 1;
}




// *******************************************************************************************************

//
// work
// pause play
//

int spotify_class::spotify_pause_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/pause' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (curl_error!=0) {
    return 1;
  }
  return 0;
}

//
// resume play
//

int spotify_class::spotify_resume_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    return 1;
  }
  return 0;
}


//
// last play
//

int spotify_class::spotify_last_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X POST 'https://api.spotify.com/v1/me/player/last' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    return 1;
  }
  return 0;
}

//
// work
// next play
//

int spotify_class::spotify_next_play() {
  int curl_error;
  char call[4096];
  sprintf(call,"curl -f -X POST 'https://api.spotify.com/v1/me/player/next' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s'",spotifytoken);
  curl_error=system(call);
  if (curl_error==0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    return 1;
  }
  return 0;
}




// work
// play song/playlist
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int spotify_class::spotify_play_now(char *playlist_song,bool now) {
  int curl_error;
  char call[4096];
  char *devid=spotify_oversigt.get_active_device_id();
  devid[40]='\0';
  printf("Devid *%s*\n",devid);
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"context_uri\\\":\\\"spotify:playlist:%s\\\",\\\"offset\\\":{\\\"position\\\":5},\\\"position_ms\\\":0}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,playlist_song,spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"Error start play %d \n",WEXITSTATUS(curl_error));
  }
  return(curl_error);
}




// work
// play song
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.

int spotify_class::spotify_play_now_song(char *playlist_song,bool now) {
  int curl_error;
  char call[4096];
  char temptxt[2048];
  char *devid=spotify_oversigt.get_active_device_id();
  devid[40]='\0';
  printf("Devid *%s*\n",devid);
  strcpy(temptxt,playlist_song);
  //songstrpointer=strstr(temptxt,"https://api.spotify.com/v1/tracks/");
  if (strlen(temptxt)>34) {
    strcpy(temptxt,playlist_song+34);
  }
  printf("Go play song id %s \n",temptxt);
//sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"context_uri\\\":\\\"spotify:playlist:%s\\\",\\\"offset\\\":{\\\"position\\\":5},\\\"position_ms\\\":0}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,playlist_song,spotifytoken);
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"curis\\\":[\\\"spotify:track:%s\\\"]}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,temptxt,spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"Error start play %d \n",WEXITSTATUS(curl_error));
    fprintf(stderr,"sql=%s\n",call);
  }
  return(curl_error);
}

// work
// play artist
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.


int spotify_class::spotify_play_now_artist(char *playlist_song,bool now) {
  int curl_error;
  char call[4096];
  char temptxt[2048];
  char *devid=spotify_oversigt.get_active_device_id();
  devid[40]='\0';
  printf("Devid *%s*\n",devid);
  strcpy(temptxt,playlist_song);
  //songstrpointer=strstr(temptxt,"https://api.spotify.com/v1/tracks/");
  if (strlen(temptxt)>34) {
    strcpy(temptxt,playlist_song+34);
  }
  fprintf(stderr,"Go play Artist id %s \n",temptxt);
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"context_uri\\\":\\\"spotify:artist:%s\\\"}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,temptxt,spotifytoken);
  //fprintf(stderr,"sql=%s\n",call);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"Error start play %d \n",WEXITSTATUS(curl_error));
    fprintf(stderr,"sql=%s\n",call);
  }
  return(curl_error);
}

// work
// play album
// Optional. Spotify URI of the context to play. Valid contexts are albums, artists, playlists.
// error codes

// 200	OK - The request has succeeded. The client can read the result of the request in the body and the headers of the response.
// 401	Unauthorized - The request requires user authentication or, if the request included authorization credentials, authorization has been refused for those credentials.
// 404	Not Found - The requested resource could not be found. This error can be due to a temporary or permanent condition
// 429	Too Many Requests - Rate limiting has been applied.


int spotify_class::spotify_play_now_album(char *playlist_song,bool now) {
  int curl_error;
  char call[4096];
  char temptxt[2048];
  char *devid=spotify_oversigt.get_active_device_id();
  devid[40]='\0';
  printf("Devid *%s*\n",devid);
  strcpy(temptxt,playlist_song);
  //songstrpointer=strstr(temptxt,"https://api.spotify.com/v1/tracks/");
  if (strlen(temptxt)>34) {
    strcpy(temptxt,playlist_song+34);
  }
  printf("Go play album id %s \n",temptxt);
//sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"context_uri\\\":\\\"spotify:playlist:%s\\\",\\\"offset\\\":{\\\"position\\\":5},\\\"position_ms\\\":0}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,playlist_song,spotifytoken);
  sprintf(call,"curl -f -X PUT 'https://api.spotify.com/v1/me/player/play?device_id=%s' --data \"{\\\"curis\\\":[\\\"spotify:album:%s\\\"]}\" -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s'",devid,temptxt,spotifytoken);
  curl_error=system(call);
  if (WEXITSTATUS(curl_error)!=0) {
    fprintf(stderr,"Error start play %d \n",WEXITSTATUS(curl_error));
    fprintf(stderr,"sql=%s\n",call);
  }
  return(curl_error);
}




//
// Works
// get device list and have it in spotify class
//

// char call_sed[]="cat spotify_device_list.json | sed 's/\\\\\//\//g' | sed 's/[{\",}]//g' | sed 's/ //g' | sed 's/:/=/g' | tail -n +3 | head -n 7 > spotify_device_list.txt";

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
  sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/me/player/devices' -H 'Accept: application/json' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_device_list.json 2>&1",spotifytoken);
  curl_exitcode=system(call);
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
              sprintf(sql,"select dev_id from mythtvcontroller.spotify_device where dev_id like '%s' limit 1",spotify_device[t].id);
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
                sprintf(sql,"insert into mythtvcontroller.spotify_device values ('%s',%d,'%s','%s',0)",spotify_device[t].name,spotify_device[t].is_active,spotify_device[t].devtype,spotify_device[t].id);
                mysql_query(conn,sql);
                res = mysql_store_result(conn);
              }
            }
          }
          this->spotify_device_antal=device_antal;
          if (conn) mysql_close(conn);
          fprintf(stderr,"\n*****************\n");
      }
    }
  }
  // no device to use is found or no token
  if ( curl_exitcode == 22 ) {
    active_spotify_device=-1;
    fprintf(stderr,"Error loading device list from spodify by api.\n");
  }
  return active_spotify_device;
}



// get user access token
// write to spotify_access_token
//

int spotify_class::spotify_get_access_token2() {
  struct mg_connection *nc;
  mg_mgr_init(&spotify_oversigt.client_mgr, NULL);
  nc = mg_connect_http(&spotify_oversigt.client_mgr, ev_handler, "https://accounts.spotify.com/api/token", "Content-Type: application/x-www-form-urlencoded\r\n", "");
  mg_set_protocol_http_websocket(nc);
  while (s_exit_flag == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
  FILE *myfile;
  char data[4096];
  char call[4096];
  char *base64_code;
  strcpy(data,spotify_client_id);
  strcat(data,":");
  strcat(data,spotify_secret_id);
  //calc base64
  base64_code=b64_encode((const unsigned char *) data, 65);
  *(base64_code+88)='\0';
  // works
  //sprintf(call,"curl -X 'POST' -H 'Authorization: Basic %s' -d grant_type=client_credentials https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code);
  myfile = fopen("spotify_access_token.txt","r");
  if (myfile) {
    fgets(data,4095,myfile);                      // read file
    fclose(myfile);
    //remove("spotify_access_token.txt");           // remove file again
  }
  return(1);
}






// return the intnr
int spotify_class::get_spotify_intnr(int nr) {
  if (nr < antal) return (stack[nr]->intnr); else return (0);
}


// return the playlist/song name
char *spotify_class::get_spotify_name(int nr) {
  if (nr < antal) return (stack[nr]->feed_name); else return (NULL);
}

// return the spotify playlist id
char *spotify_class::get_spotify_playlistid(int nr) {
  if (nr < antal) return (stack[nr]->playlistid); else return (NULL);
}


// return the description
char *spotify_class::get_spotify_desc(int nr) {
  if (nr < antal) return (stack[nr]->feed_desc); else return (NULL);
}

// clean up number of created
void spotify_class::clean_spotify_oversigt() {
    startup_loaded=false;
    for(int i=1;i<antal;i++) {
      if (stack[i]) {
        if (stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete spotify texture
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


//
// set spotify icon image
//
void spotify_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}


//
// get nr of spotify playlists
//

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



//
// opdate show liste in view (det vi ser)
//

// load felt 7 = mythtv gfx icon
// fpath=stream path
// atr = stream name

int spotify_class::opdatere_spotify_oversigt(char *refid) {
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
      mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
      res = mysql_store_result(conn);
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          dbexist=true;
        }
      }
    }
    clean_spotify_oversigt();                                                   // clean old list
    strcpy(lasttmpfilename,"");
    if (debugmode & 4) {
      fprintf(stderr,"loading spotify data.\n");
    }
    // find records after type (0 = root, else = refid)
    if (refid == NULL) {
      show_search_result=false;
      sprintf(sqlselect,"select playlistname,paththumb,playlistid,id from spotifycontentplaylist order by playlistname");
      getart = 0;
    } else {
      show_search_result=true;
      sprintf(sqlselect,"select name,gfxfilename,player,playlistid from spotifycontentarticles where playlistid='%s' order by id",refid);
      getart = 1;
    }
    this->type = getart;					                                                 // husk sql type
    if (debugmode & 4) fprintf(stderr,"spotify loader started... \n");
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
              strcpy(stack[antal]->feed_gfx_mythtv,"");
              strcpy(stack[antal]->playlisturl,"");
              stack[antal]->feed_group_antal=0;
              stack[antal]->feed_path_antal=0;
              stack[antal]->textureId=0;
              stack[antal]->intnr=atoi(row[3]);
              stack[antal]->nyt=false;
              // top level (load playlist)
              if (getart == 0) {
                strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
                strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
                strncpy(stack[antal]->feed_gfx_url,row[1],2048);
                strncpy(stack[antal]->playlistid,row[2],spotify_namelength);

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
                  strcat(downloadfilenamelong,"/datadisk/mythtv-controller-0.37/tmp/");
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
              if (getart == 1) {
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
                strncpy(stack[antal]->playlisturl,row[2],spotify_namelength);

                char temptxt1[2048];
                char temptxt2[2048];
                char *songstrpointer;
                songstrpointer=strstr(row[2],"https://api.spotify.com/v1/tracks/");
                // get track id
                if (songstrpointer) {
                  strcpy(temptxt2,songstrpointer);
                  strncpy(stack[antal]->playlistid,temptxt2,spotify_namelength);
                } else strcpy(stack[antal]->playlistid,"");
                antal++;
              }
            }
          }
        }
        mysql_close(conn);
      } else {
        if (debugmode & 4) fprintf(stderr,"No spotify data loaded \n");
      }
      antalplaylists=antal;
      return(antal);
    } else fprintf(stderr,"Failed to update Spotify db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
    if (debugmode & 4) fprintf(stderr,"Spotify loader done... \n");
    return(0);
}



//
// search for playlist or song
//


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
    mysql_query(conn,"SELECT feedtitle from mythtvcontroller.spotifycontentarticles limit 1");
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        dbexist=true;
      }
    }
    clean_spotify_oversigt();                                                   // clean old list
    // find records after type (0 = root, else = refid)
    if (type == 0) {
      sprintf(sqlselect,"select playlistname,paththumb,playlistid,id from spotifycontentplaylist where playlistname like '");
      strcat(sqlselect,"%");
      strcat(sqlselect,keybuffer);
      strcat(sqlselect,"%'");
      getart = 0;
    } else {
      sprintf(sqlselect,"select name,paththumb,player,playlistid,id from spotifycontentarticles where name like '");
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
            strcpy(stack[antal]->feed_gfx_mythtv,"");
            strcpy(stack[antal]->playlisturl,"");
            stack[antal]->feed_group_antal=0;
            stack[antal]->feed_path_antal=0;
            stack[antal]->textureId=0;
            stack[antal]->nyt=false;
            if (getart == 0) {
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
              strcpy(downloadfilenamelong,"tmp/");
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
            if (getart == 1) {
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
//

//
// static void spotify_class::process_value(json_value* value, int depth);
//

bool search_process_tracks=false;
bool search_process_uri=false;
bool search_process_songs=false;
bool search_process_href=false;
bool search_process_description=false;
bool search_process_image=false;
bool search_process_name=false;
bool search_process_items=false;
bool search_process_track_nr=false;

//
// process types in file for process playlist files (songs)
//

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


//
// json parser start call function for process playlist
// do the data progcessing from json

void spotify_class::search_process_value(json_value* value, int depth,int x,int art) {
  char artisid[1024];
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
      printf("x = %2d deep = %2d art = %2d ",x,depth,art);
      printf("string: %s\n", value->u.string.ptr);
      if (search_process_items) {
        // set start of items in list
        search_process_items=false;
      }
      if (search_process_tracks) {
        search_process_tracks=false;
      }
      if ( search_process_image ) {
        // get playid
        search_process_image=false;
      }
      if (search_process_href) {
        search_process_href=false;
      }

      if ((depth==7) && (x==9)) {
        // get artist id
        if (strlen(value->u.string.ptr)>16) {
          strcpy(artisid,value->u.string.ptr+15);
          strcpy(stack[antal-1]->playlistid,artisid);
        }
      }

      if ((art==0) || (art==1)) {
        // get name
        if ((search_process_name) && (depth==7) && (x==6)) {
          if (antal==0) {
            stack[antal]=new (spotify_oversigt_type);
          }
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) {
              stack[antal]=new (spotify_oversigt_type);
            }
            //printf("Antal %d \nTitle : %s \n",antal,value->u.string.ptr);
            if (stack[antal]) {
              strncpy(stack[antal]->feed_name,value->u.string.ptr,80);
              strncpy(stack[antal]->feed_showtxt,value->u.string.ptr,80);
            }
            antal++;
            antalplaylists++;
          }
        }
      }
      // spotify online type for en kunsner udgivelser (i potify db)
      if (art==2) {
        if ((search_process_name) && (depth==9) && (x==7)) {
          if (antal==0) {
            // first record back
            stack[antal]=new (spotify_oversigt_type);
            strcpy(stack[antal]->feed_name,"Back");
            strcpy(stack[antal]->feed_showtxt,"Back");
            stack[antal]->textureId=0;
            stack[antal]->intnr=0;                                            // back button
            antal++;
          }
          if (antalplaylists<maxantal) {
            if (!(stack[antal])) {
              stack[antal]=new (spotify_oversigt_type);
            }
            //printf("Antal %d \nTitle : %s \n",antal,value->u.string.ptr);
            if (stack[antal]) {
              strncpy(stack[antal]->feed_name,value->u.string.ptr,80);
              strncpy(stack[antal]->feed_showtxt,value->u.string.ptr,80);
            }
            antal++;
            antalplaylists++;
          }
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






//
// search for playlist or song
//


int spotify_class::opdatere_spotify_oversigt_searchtxt_online(char *keybuffer,int type) {
  char sqlselect[2048];
  char homedir[1024];
  char call[4096];
  char searchstring[4096];
  bool rss_update=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
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
  switch(type) {
            // search artist name
    case 0: sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=artist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search album name
    case 1: sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=album&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search playlist name
    case 2: sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=playlist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // search track name
    case 3: sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=track&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
            break;
            // default search artist name
    default: sprintf(call,"curl -f -X GET 'https://api.spotify.com/v1/search?q=%s&type=artist&limit=50' -H \"Content-Type: application/json\" -H 'Authorization: Bearer %s' > spotify_search_result.json",searchstring,spotifytoken);
  }
  curl_error=system(call);
  if (curl_error!=0) {
    fprintf(stderr,"curl_error %d \n",curl_error);
    printf("call= %s \n",call);
    return 1;
  }
  stat("spotify_search_result.json", &filestatus);                              // get file info
  file_size = filestatus.st_size;                                               // get filesize
  file_contents = (char*) malloc(filestatus.st_size);
  json_file = fopen("spotify_search_result.json", "rt");
  if (json_file == NULL) {
    fprintf(stderr, "Unable to open spotify_search_result.json\n");
    free(file_contents);                                                        //
    return 1;
  }
  if (fread(file_contents, file_size, 1, json_file ) != 1 ) {
    fprintf(stderr, "Unable to read spotify spotify_search_result content of spotify_search_result.json\n");
    fclose(json_file);
    free(file_contents);                                                        //
    return 1;
  }
  fclose(json_file);
  json = (json_char*) file_contents;
  value = json_parse(json,file_size);                                           // parser

  // parse from root
  if (type==0) search_process_value(value, 0,0,type);                                     // fill stack array
  if (type==1) search_process_value(value, 0,0,type);                                     // fill stack array
  if (type==2) search_process_value(value, 0,0,type);                                     // fill stack array
  if (type==3) search_process_value(value, 0,0,2);                                        // fill stack array

  json_value_free(value);                                                       // json clean up
  free(file_contents);                                                          //
  return(antal);
}



//
// thread web loader (loading all icons)
//

void *load_spotify_web(void *data) {
  if (debugmode & 4) fprintf(stderr,"Start spotify loader thread\n");
  //streamoversigt.loadweb_stream_iconoversigt();
  if (debugmode & 4) fprintf(stderr,"Stop spotify loader thread\n");
}



//
//
//

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



//
// loading spotify songs gfx in array.
//

int spotify_class::load_spotify_iconoversigt() {
  int texture;
  int nr=0;
  int loadstatus;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  this->gfx_loaded=false;
  if (debugmode & 4) printf("spotify icon loader.\n");
  while(nr<streamantal()) {
    //printf("Loading texture %d %s \n",nr,stack[nr]->feed_gfx_url);
    if ((stack[nr]) && (strcmp(stack[nr]->feed_gfx_url,"")!=0)) {
      //texture=LoadImage(stack[nr]->feed_gfx_url);
      //if (texture!=-1) stack[nr]->textureId=texture;
    }
    nr++;
  }
  if (nr>0) this->gfx_loaded=true; else this->gfx_loaded=false;
  if (debugmode & 4) {
    if (gfx_loaded) fprintf(stderr,"spotify download end ok. \n");
    else fprintf(stderr,"spotify download error. \n");
  }
  return(1);
}


//
// get active play device name
//

char *spotify_class::get_active_spotify_device_name() {
  return(spotify_device[active_spotify_device].name);
}


//
//
//

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



//
// show spotify overview
//

void spotify_class::show_spotify_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected) {
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
        glBindTexture(GL_TEXTURE_2D,empty_icon);
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
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
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
          glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-20, yof+buttonsizey-30 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-20, yof+20 , 0.0);
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
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if ((i+sofset)==0) {
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) glBindTexture(GL_TEXTURE_2D,backicon);
          else glBindTexture(GL_TEXTURE_2D,normal_icon);
        } else glBindTexture(GL_TEXTURE_2D,normal_icon);
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
}





//
// show search/create playlist spotify overview
//

void spotify_class::show_spotify_search_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint backicon,int sofset,int stream_key_selected,char *searchstring) {
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
    glBindTexture(GL_TEXTURE_2D,big_search_bar);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,empty_icon);
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
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
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
          glTexCoord2f(0, 0); glVertex3f( xof+20, yof+20, 0.0);
          glTexCoord2f(0, 1); glVertex3f( xof+20,yof+buttonsizey-30, 0.0);
          glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-20, yof+buttonsizey-30 , 0.0);
          glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-20, yof+20 , 0.0);
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
          if (strcmp(stack[i+sofset]->feed_showtxt,"Back")==0) glBindTexture(GL_TEXTURE_2D,backicon);
          else glBindTexture(GL_TEXTURE_2D,normal_icon);
        } else glBindTexture(GL_TEXTURE_2D,normal_icon);
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
}








const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t b64_encoded_size(size_t inlen) {
	size_t ret;
	ret = inlen;
	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);
	ret /= 3;
	ret *= 4;
	return ret;
}


char *b64_encode(const unsigned char *in, size_t len) {
	char   *out;
	size_t  elen;
	size_t  i;
	size_t  j;
	size_t  v;
	if (in == NULL || len == 0)
		return NULL;
	elen = b64_encoded_size(len);
	out  = (char *) malloc(elen+1);
	out[elen] = '\0';
	for (i=0, j=0; i<len; i+=3, j+=4) {
		v = in[i];
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8;
		out[j]   = b64chars[(v >> 18) & 0x3F];
		out[j+1] = b64chars[(v >> 12) & 0x3F];
		if (i+1 < len) {
			out[j+2] = b64chars[(v >> 6) & 0x3F];
		} else {
			out[j+2] = '=';
		}
		if (i+2 < len) {
			out[j+3] = b64chars[v & 0x3F];
		} else {
			out[j+3] = '=';
		}
	}
	return out;
}



//
// ********************* show setup spotify stuf like dev and clientid/secrect ***************************
//

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
    static int spotify_device_antal=0;
    static bool first_time_update=true;
    char *database = (char *) "mythtvcontroller";
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
            /// set active device
            if ((active_default_play_device!=-1) && (spotify_device[dev_nr].is_active)) {
              active_default_play_device=dev_nr;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      glTranslatef(510, 310, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[0].name);
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
      glTranslatef(510+120, 310, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[1].name);
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
      glTranslatef(510+120+120, 310, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[2].name);
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
      glTranslatef(510+120+120+120, 310, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[3].name);
      glPopMatrix();
    }

    // row 2
    if (strcmp(spotify_device[4].name,"")!=0) {
      // playerid 1
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
      glTranslatef(510, 180, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[4].name);
      glPopMatrix();
    }
    if (strcmp(spotify_device[4].name,"")!=0) {
      // playerid 1
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
      glTranslatef(510+120, 180, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[4].name);
      glPopMatrix();
    }
    if (strcmp(spotify_device[5].name,"")!=0) {
      // playerid 1
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
      glTranslatef(510+120+120, 180, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[5].name);
      glPopMatrix();
    }
    if (strcmp(spotify_device[6].name,"")!=0) {
      // playerid 1
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
      glTranslatef(510+120+120+120, 180, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[6].name);
      glPopMatrix();
    }
    if (strcmp(spotify_device[7].name,"")!=0) {
      // playerid 1
      glPushMatrix();
      winsizx=100;
      winsizy=100;
      xpos=270+120+120+120;
      ypos=70;
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(0.0f, 0.0f, 0.0f);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      if (strcmp(spotify_device[7].devtype,"Unknown")==0) glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      else if (strcmp(spotify_device[7].devtype,"Smartphone")==0) glBindTexture(GL_TEXTURE_2D,mobileplayer_icon);
      else if (strcmp(spotify_device[7].devtype,"Computer")==0) glBindTexture(GL_TEXTURE_2D,pcplayer_icon);
      else glBindTexture(GL_TEXTURE_2D,unknownplayer_icon);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glLoadName(18);                                                             // update button name
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glTranslatef(510+120+120+120+120, 180, 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glScalef(15.0, 15.0, 1.0);
      glcRenderString(spotify_device[7].name);
      glPopMatrix();
    }
}

 void spotify_class::set_default_device_to_play(int nr) {
   active_spotify_device=nr;
   active_default_play_device=nr;
 }
