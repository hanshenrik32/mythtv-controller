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

//#include <http_parser.h>

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

extern spotify_class spotify_oversigt;

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
                                                                // debug mode
                                                                // 1  = wifi net
                                                                // 2  = music
                                                                // 4  = stream
                                                                // 8  = keyboard/mouse move
                                                                // 16 = movie
                                                                // 32 = searcg
extern int debugmode;                                           // 64 = radio station land icon loader
                                                                // 128= stream search
                                                                // 256 = tv program stuf
                                                                // 512 = media importer
                                                                // 1024 = flag loader
extern unsigned int musicoversigt_antal;                        //
extern int do_stream_icon_anim_icon_ofset;                      //
extern GLuint radiooptions,radiooptionsmask;			              //
extern GLuint _textureIdback;  					                        // back icon
extern GLuint newstuf_icon;                                     //
extern int fonttype;
extern fontctrl aktivfont;
extern int orgwinsizey,orgwinsizex;
extern int _sangley;
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
  char user_token[255];
  struct http_message *hm = (struct http_message *) ev_data;
  struct mg_serve_http_opts opts;
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      // from spotify servers
      // is callback call
      if (mg_strncmp( hm->uri,mg_mk_str_n("/callback",9),9) == 0) {
        printf("Got reply: %s \n", hm->uri);
        p = strstr( hm->uri.p , "code="); // mg_mk_str_n("code=",5));
        // get soptify code from server
        if (p) {
          strncpy(user_token,p+5,251);
          *(user_token+251)='\0';
//          printf("\n");
//          printf("key is %s \n", callback_code);
            strcpy(spotify_oversigt.spotify_authorize_token,user_token);
            //printf("spotify authorize token : %s \n", spotify_oversigt.spotify_authorize_token);
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
        printf("Got reply:\n%.*s\n", (int) hm->body.len, hm->body.p);
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
// constructor
//

spotify_class::spotify_class() : antal(0) {
    int i;
    for(i=0;i<maxantal;i++) stack[i]=0;
    stream_optionselect=0;							// selected line in stream options
    stream_oversigt_loaded=false;
    stream_oversigt_loaded_nr=0;
    antal=0;
    gfx_loaded=false;			      // gfx loaded
    stream_is_playing=false;    // is we playing any media
    stream_is_pause=false;      // is player on pause
    int port_cnt, n;
    int err = 0;
    strcpy(spotify_authorize_token,"");
    // create web server
    mg_mgr_init(&mgr, NULL);                                    // Initialize event manager object
    printf("Starting web server on port %s\n", s_http_port);
//    this->c = mg_bind(&mgr, s_http_port, server_ev_handler);           // Create listening connection and add it to the event manager
//    mg_set_protocol_http_websocket(this->c);                    // make http protocol
    //mg_connect_http(&mgr, ev_handler, "", NULL, NULL);

    strcpy(spotify_client_id,"05b40c70078a429fa40ab0f9ccb485de");
    strcpy(spotify_secret_id,"e50c411d2d2f4faf85ddff16f587fea1");
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
// get users play lists
// write to spotify_users_playlist.txt

int spotify_class::spotify_get_users_playlist() {
/*
  struct mg_connection *nc;
  mg_mgr_init(&spotify_oversigt.client_mgr, NULL);
  nc = mg_connect_http(&spotify_oversigt.client_mgr, ev_handler, "", NULL, NULL);
  mg_set_protocol_http_websocket(nc);
  while (s_exit_flag == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
*/
  char doget[4096];
  if (strcmp(spotify_authorize_token,"")!=0) {
    sprintf(doget,"curl -X 'GET' 'https://api.spotify.com/v1/me/playlists' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_users_playlist.txt",spotify_authorize_token);
    system(doget);
  }

}

// json parser used to parse the return files from spotify api
/* ***************************************************************************************** */
//
void spotify_class::print_depth_shift(int depth)
{
  int j;
  for (j=0; j < depth; j++) {
    printf(" ");
  }
}

//static void spotify_class::process_value(json_value* value, int depth);

bool process_tracks=false;
bool process_playlist=false;
bool process_songs=false;
bool process_href=false;
bool process_description=false;
bool process_image=false;
bool process_name=false;
bool process_items=false;



void spotify_class::process_object(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    //if (strcmp(value->u.object.values[x].name,"name")==0)
    printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
    if (strcmp(value->u.object.values[x].name,"tracks")==0) {
      process_tracks=true;
    }
    if (strcmp(value->u.object.values[x].name , "href" )==0) {
      process_tracks=true;
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
    if (strcmp(value->u.object.values[x].name , "items" )==0) {
      process_items=true;
    }
    process_value(value->u.object.values[x].value, depth+1);
  }
}


void spotify_class::process_array(json_value* value, int depth) {
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  printf("array\n");
  for (x = 0; x < length; x++) {
    process_value(value->u.array.values[x], depth);
  }
}


//
// json parser start call function
//

void spotify_class::process_value(json_value* value, int depth) {
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
        process_object(value, depth+1);
        break;
      case json_array:
        process_array(value, depth+1);
        break;
      case json_integer:
        printf("int: %10" PRId64 "\n", value->u.integer);
        break;
      case json_double:
        printf("double: %f\n", value->u.dbl);
        break;
      case json_string:
        printf("string: %s\n", value->u.string.ptr);
        if (process_description) {
          if (antal<maxantal) {
            stack[antal]=new (struct spotify_oversigt_type);
            if (stack[antal]) {
              strcpy(stack[antal]->feed_desc,value->u.string.ptr);                           // desc
              strcpy(stack[antal]->feed_name,"");		                        // mythtv db feedtitle
              strcpy(stack[antal]->feed_showtxt,"");          	            // show name
              strcpy(stack[antal]->feed_name,"");		                        // mythtv db feedtitle
              strcpy(stack[antal]->feed_desc,"");                           // desc
              strcpy(stack[antal]->feed_gfx_url,"");
              strcpy(stack[antal]->feed_gfx_mythtv,"");
              stack[antal]->feed_group_antal=0;
              stack[antal]->feed_path_antal=0;
              stack[antal]->textureId=0;
              stack[antal]->intnr=0;
              stack[antal]->nyt=false;
            }
          }
          process_description=false;
        }
        if (process_items) {
          antal++;
          printf("Antal %d \n ",antal);
          // set start of items in list
          strcpy(stack[antal]->feed_name,value->u.string.ptr);                           // name
          process_items=false;
        }
        if (process_tracks) {
          // playlist id from spotify
          strcpy(stack[antal]->playlistid,value->u.string.ptr);                           // playlistid
          process_tracks=false;
        }
        if (process_image) {
          strcpy(stack[antal]->feed_gfx_url,value->u.string.ptr);                           //
          process_image=false;
        }
        break;
      case json_boolean:
        printf("bool: %d\n", value->u.boolean);
        break;
    }
}


// get user play list (any public user)
// write to spotify_playlist.txt

int spotify_class::spotify_get_playlist(char *playlist) {
  char doget[4096];
  FILE *json_file;
  char *jons_string;
  struct stat filestatus;
  int file_size;
  char* file_contents;
  json_char* json;
  json_value* value;
  if (strcmp(spotify_authorize_token,"")!=0) {
    sprintf(doget,"curl -X 'GET' 'https://api.spotify.com/v1/playlists/%s' -H 'Content-Type: application/json' -H 'Authorization: Bearer %s' > spotify_playlist.txt",playlist,spotify_authorize_token);
    system(doget);
  }
  stat("spotify_playlist.txt", &filestatus);
  file_size=filestatus.st_size;                                                // get filesize
  file_contents = (char*)malloc(filestatus.st_size);
  json_file = fopen("spotify_playlist.txt", "rt");
  if ( fread(file_contents, file_size, 1, json_file ) != 1 ) {
    fprintf(stderr, "Unable to read spotify playlist content of %s\n", "spotify_playlist.txt");
    fclose(json_file);
    free(file_contents);
    return 1;
  }
  fclose(json_file);
  json = (json_char*)file_contents;
  value = json_parse(json,file_size);
   //spotify_playlist.txt

  // parse from root
  process_value(value, 0);

  json_value_free(value);
  free(file_contents);
  return 0;
}


// get user access token
// write to spotify_access_token
//

int spotify_class::spotify_get_access_token() {
/*  struct mg_connection *nc;
  mg_mgr_init(&spotify_oversigt.client_mgr, NULL);
  nc = mg_connect_http(&spotify_oversigt.client_mgr, ev_handler, "https://accounts.spotify.com/api/token", "Content-Type: application/x-www-form-urlencoded\r\n", "");
  mg_set_protocol_http_websocket(nc);
  while (s_exit_flag == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
*/
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
  sprintf(call,"curl -X 'POST' -H 'Authorization: Basic %s' -d grant_type=client_credentials https://accounts.spotify.com/api/token > spotify_access_token.txt",base64_code);
  system(call);
  myfile=fopen("spotify_access_token.txt","r");
  if (myfile) {
    fgets(data,4095,myfile);                      // read file
    strcpy(spotify_authorize_token,data+17);      // and get token
    *(spotify_authorize_token+83)='\0';
    printf("Found token : %s\n",spotify_authorize_token);
    //printf("base64_code : %s\n",base64_code);
    fclose(myfile);
    remove("spotify_access_token.txt");           // remove file again
  }
}


// return the name
char *spotify_class::get_stream_name(int nr) {
  if (nr<antal) return (stack[nr]->feed_name); else return (NULL);
}


// return the description
char *spotify_class::get_stream_desc(int nr) {
  if (nr<antal) return (stack[nr]->feed_desc); else return (NULL);
}

// clean up number of created

void spotify_class::clean_spotify_oversigt() {
    startup_loaded=false;				// set radio station loaded in
    for(int i=1;i<antal;i++) {
        if (stack[i]->textureId) glDeleteTextures(1, &stack[i]->textureId);	// delete stream texture
        //if (stack[i]) delete stack[i];							// delete radio station
    }
    antal=0;
    stream_oversigt_loaded=false;			// set load icon texture again
    stream_oversigt_loaded_nr=0;
    stream_oversigt_nowloading=0;
}


// set en stream icon image
void spotify_class::set_texture(int nr,GLuint idtexture) {
    stack[nr]->textureId=idtexture;
}

//
// vlc player interface
//

// default player
// stop playing stream sound or video

void spotify_class::stopstream() {
  if ((vlc_in_playing()) && (stream_is_playing)) vlc_controller::stopmedia();
  stream_is_playing=false;
}

// vlc stop player

void spotify_class::softstopstream() {
  if ((vlc_in_playing()) && (stream_is_playing)) vlc_controller::stopmedia();
  stream_is_playing=false;
}


// get length on stream

unsigned long spotify_class::get_length_in_ms() {
  vlc_controller::get_length_in_ms();
}


// jump in player

float spotify_class::jump_position(float ofset) {
    ofset=vlc_controller::jump_position(ofset);
    return(ofset);
}

// to play streams from web
//vlc_m = libvlc_media_new_location(vlc_inst, "http://www.ukaff.ac.uk/movies/cluster.avi");


// pause stream

int spotify_class::pausestream(int pause) {
    //stream_is_playing=true;
    vlc_controller::pause(1);
    if (!(stream_is_pause)) stream_is_pause=true; else stream_is_pause=false;
    return(1);
}



// start playing movie by vlclib

int spotify_class::playstream(int nr) {
    char path[PATH_MAX];                                  // max path length from os
    strcpy(path,"");
//    strcat(path,get_stream_url(nr));
    stream_is_playing=true;
    vlc_controller::playmedia(path);
    return(1);
}


int spotify_class::playstream_url(char *path) {
    stream_is_playing=true;
    vlc_controller::playwebmedia(path);
    return(1);
}


float spotify_class::getstream_pos() {
    return(vlc_controller::get_position());
}


// update nr of view

void spotify_class::update_rss_nr_of_view(char *url) {
  // mysql vars
  char sqlinsert[32768];
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // get homedir
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    sprintf(sqlinsert,"update mythtvcontroller.internetcontentarticles set time=time+1 where mediaURL like '%s'",url);
    mysql_query(conn,sqlinsert);
    res = mysql_store_result(conn);
    mysql_free_result(res);
    mysql_close(conn);
  }
}


//
// used to download rss file from web to db info (url is flag for master rss file (mediaURL IS NULL))
// in db if mediaURL have url this is the rss feed loaded from rss file
// updaterssfile bool is do it now (u key in overview)

int spotify_class::loadrssfile(bool updaterssfile) {
  bool haveupdated=false;
  char sqlselect[2048];
  char sqlinsert[32768];
  char totalurl[2048];
  char parsefilename[2048];
  char homedir[2048];
  char baseicon[2048];
  unsigned int recantal;
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row;
  time_t timenow;
  char *database = (char *) "mythtvcontroller";
  struct stat attr;
  const int updateinterval=86400;
  time(&timenow);
  conn=mysql_init(NULL);
  // get homedir
  getuserhomedir(homedir);
  strcat(homedir,"/rss");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  strcat(homedir,"/images");
  if (!(file_exists(homedir))) mkdir(homedir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // Connect to database
  //strcpy(sqlselect,"select internetcontent.name,internetcontentarticles.path,internetcontentarticles.title,internetcontentarticles.description,internetcontentarticles.url,internetcontent.thumbnail,count(internetcontentarticles.feedtitle),internetcontent.thumbnail from internetcontentarticles left join internetcontent on internetcontentarticles.feedtitle=internetcontent.name group by internetcontentarticles.feedtitle");
  //strcpy(sqlselect,"select * from internetcontentarticles");
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    strcpy(sqlselect,"select count(mediaURL) from internetcontentarticles where mediaURL IS NULL");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        recantal=atoi(row[0]);
      }
      mysql_free_result(res);
    }
    stream_rssparse_nowloading=0;
    strcpy(sqlselect,"select * from internetcontentarticles where mediaURL is NULL");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    //
    //
    //
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        stream_rssparse_nowloading++;
        if (debugmode & 4) printf("Get rss file on stream title %10s \n",row[0]);
        if ((row[3]) && (strcmp(row[3],"")!=0)) {
          getuserhomedir(homedir);                                          // get user homedir
          strcpy(totalurl,"wget -U Netscape '");
          if (row[7]) strcat(totalurl,row[7]); else if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,"' -o '");
          strcat(totalurl,homedir);                                         // add user homedir
          strcat(totalurl,"/rss/wget.log'");                                // log file
          strcat(totalurl," -O '");
          strcat(totalurl,homedir);                                         // add user homedir
          strcat(totalurl,"/rss/");                                         // add output filename
          if (row[3]) strcat(totalurl,row[3]);
          strcat(totalurl,".rss'");
          strcpy(parsefilename,homedir);                                    // copy user homedir
          strcat(parsefilename,"/rss/");
          strcat(parsefilename,row[3]);
          strcat(parsefilename,".rss");
          strcpy(baseicon,"");
          stat(parsefilename, &attr);
          if ((file_exists(parsefilename)) && (attr.st_mtime+updateinterval<timenow)) {
            // download rss file
            system(totalurl);
            // parse file
            strcpy(parsefilename,homedir);
            strcat(parsefilename,"/rss/");
            strcat(parsefilename,row[3]);
            strcat(parsefilename,".rss");
            // if podcast is rss
            // if title ok and not podcast bud real rss feed
            if ((strcmp(row[3],"")!=0) && (!(row[23]))) {
              // parse downloaded xmlfile now (create db records)
              parsexmlrssfile(parsefilename,baseicon);
            }
          } else if ((!(file_exists(parsefilename))) || (updaterssfile)) {
            // download rss file
            system(totalurl);
            // parse file
            strcpy(parsefilename,homedir);
            strcat(parsefilename,"/rss/");
            strcat(parsefilename,row[3]);
            strcat(parsefilename,".rss");
            // if podcast is rss
            // if title ok and not podcast bud real rss feed
            if ((strcmp(row[3],"")!=0) && (!(row[23]))) {
              // parse downloaded xmlfile now (create db records)
              // and get base image from funccall (baseicon (url to image))
              parsexmlrssfile(parsefilename,baseicon);
            } else {
              printf("XML FILE is missing/not working on %s file.\n",row[3]);
            }
          }
          // update master icon if none
          if ((strcmp(row[0],"")!=0) && (strcmp(baseicon,"")!=0)) {
            sprintf(sqlinsert,"UPDATE internetcontentarticles set paththumb='%s' where feedtitle like '%s' and paththumb IS NULL",baseicon,row[0]);
            mysql_query(conn,sqlinsert);
            res1 = mysql_store_result(conn);
            haveupdated=true;
          }
          // if podcast is not rss and title ok
          if ((strcmp(row[3],"")!=0) && (row[23])) {
            if (atoi(row[23])==1) {
              sprintf(sqlinsert,"UPDATE internetcontentarticles set mediaURL=url where podcast=1 and feedtitle like '%s'",row[0]);
              mysql_query(conn,sqlinsert);
              res1 = mysql_store_result(conn);
              haveupdated=true;
            }
          }
        }
      }
      mysql_free_result(res);
      stream_rssparse_nowloading=0;
    }
    mysql_close(conn);
  } else return(-1);
  if (haveupdated) return(1); else return(0);
}



char spotify_search_and_replace2(char *text) {
  int n=0;
  int nn=0;
  char newtext[2048];
  strcpy(newtext,"");
  while(n<strlen(text)) {
    if (text[n]=='"') {
      strcat(newtext,"'");
      n++;
//      nn+=1;
    } else {
      newtext[nn]=text[n];
      newtext[nn+1]='\0';                               // null terminate string
      nn++;
      n++;
    }
  }
  newtext[n]=0;
  strcpy(text,newtext);
}

//
// xml parser
//

int spotify_class::parsexmlrssfile(char *filename,char *baseiconfile) {
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode,*subnode2,*subnode3;
  xmlChar *xmlrssid;
  xmlChar *content;
  char rssprgtitle[2048];
  char rssprgfeedtitle[2048];
  char rssprgdesc[2048];
  char rssprgimage[2048];
  char rssprgimage1[2048];
  char rssvideolink[2048];
  char rssprgpubdate[256];
  char rsstime[2048];
  char rssauthor[2048];
  char rssduration[2048];
  int rssepisode;
  int rssseason;
  char result[2048+1];
  char sqlinsert[32768];
  char sqlselect[32768];
  char *database = (char *) "mythtvcontroller";
  bool recordexist=false;
  time_t raw_tid;
  struct tm *opret_dato;
  char rssopretdato[200];
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  strcpy(rssprgtitle,"");
  strcpy(rssprgfeedtitle,"");
  strcpy(rssprgdesc,"");
  strcpy(rssvideolink,"");
  strcpy(rsstime,"");
  strcpy(rssauthor,"");
  strcpy(rssprgimage,"");
  strcpy(rssprgimage1,"");
  conn=mysql_init(NULL);
  document = xmlReadFile(filename, NULL, 0);            // open xml file
  // if exist do all the parse and update db
  // it use REPLACE in mysql to create/update records if changed in xmlfile
  raw_tid=time(NULL);
  opret_dato=localtime(&raw_tid);
  strftime(rssopretdato,28,"%F %T",opret_dato);
  if ((document) && (conn)) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    if (conn) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
    }
    root = xmlDocGetRootElement(document);
    first_child = root->children;
    for (node = first_child; node; node = node->next) {
      if (node->type==XML_ELEMENT_NODE) {
        if ((strcmp((char *) node->name,"channel")==0) || (strcmp((char *) node->name,"feed")==0)) {
          content = xmlNodeGetContent(node);
          if (content) {
            //strcpy(result,(char *) content);
          }
          subnode=node->xmlChildrenNode;
          while(subnode) {
            xmlrssid=xmlGetProp(subnode,( xmlChar *) "title");
            content = xmlNodeGetContent(subnode);
            if ((content) && (strcmp((char *) subnode->name,"title")==0)) {
              content = xmlNodeGetContent(subnode);
              strcpy(rssprgtitle,(char *) content);
            }
            // images
            // rssprgimage
            // paththumb
            //
            if ((content) && (strcmp((char *) subnode->name,"image")==0)) {
              content = xmlNodeGetContent(subnode);
              tmpdat=xmlGetProp(subnode,( xmlChar *) "href");
              if (tmpdat) {
                strcpy(rssprgimage,(char *) tmpdat);
                strcpy(baseiconfile,(char *) tmpdat);
                xmlFree(tmpdat);
              }
            }
            if ((content) && (strcmp((char *) subnode->name,"thumbnail")==0)) {
              content = xmlNodeGetContent(subnode);
              tmpdat=xmlGetProp(subnode,( xmlChar *) "href");
              if (tmpdat) {
                strcpy(rssprgimage,(char *) tmpdat);
                xmlFree(tmpdat);
              }
            }
            // item type element
            if ((content) && (strcmp((char *) subnode->name,"item")==0)) {
              subnode2=subnode->xmlChildrenNode;
              while(subnode2) {
                if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  strcpy(rssprgfeedtitle,(char *) content);
                }
                // get play url
                if ((content) && (strcmp((char *) subnode2->name,"enclosure")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  tmpdat=xmlGetProp(subnode2,( xmlChar *) "url");
                  if (tmpdat) {
                    strcpy(rssvideolink,(char *) tmpdat);
                    xmlFree(tmpdat);
                  }
                }
                // rssprgpubdate
                if ((content) && (strcmp((char *) subnode2->name,"pubDate")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  strcpy(rssprgpubdate,(char *) content);
                }
                // get length
                if ((content) && (strcmp((char *) subnode2->name,"duration")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) strcpy(rsstime,(char *) content);
                }
                // get episode
                if ((content) && (strcmp((char *) subnode2->name,"episode")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) rssepisode=atoi((char *) content);
                }
                // get season
                if ((content) && (strcmp((char *) subnode2->name,"season")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) rssseason=atoi((char *) content);
                }
                // get author
                if ((content) && (strcmp((char *) subnode2->name,"author")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) strcpy(rssauthor,(char *) content);
                }
                // get description
                if ((content) && (strcmp((char *) subnode2->name,"description")==0)) {
                  content = xmlNodeGetContent(subnode2);
                  if (content) {
                    xmlrssid=xmlGetProp(subnode2,( xmlChar *) "description");
                    if (xmlrssid) strcpy(rssprgdesc,(char *) content);
                    xmlFree(xmlrssid);
                  }
                }
                subnode2=subnode2->next;
              }
              recordexist=false;
              // check if record exist
              if (strcmp(rssvideolink,"")!=0) {
                sprintf(sqlinsert,"select feedtitle from internetcontentarticles where (feedtitle like '%s' and mediaURL like '%s')",rssprgtitle,rssvideolink);
                mysql_query(conn,sqlinsert);
                res = mysql_store_result(conn);
                if (res) {
                  while ((row = mysql_fetch_row(res)) != NULL) {
                    recordexist=true;
                  }
                }
                // creoate record if not exist
                if (!(recordexist)) {
                  spotify_search_and_replace2(rssprgtitle);
                  spotify_search_and_replace2(rssprgfeedtitle);
                  spotify_search_and_replace2(rssprgdesc);
                  if (debugmode & 4) printf("\t Update title %-20s Date %s\n",rssprgtitle,rssprgpubdate);
                  sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"%s\",'%s',\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage,rssopretdato,0);
                  if (mysql_query(conn,sqlinsert)!=0) {
                    printf("mysql REPLACE table error. %s\n",sqlinsert);
                  }
                  res = mysql_store_result(conn);
                }
              }
            }
            subnode=subnode->next;
          }
          // end while loop
        }
        // youtube type
        // get title
        if (strcmp((char *) node->name,"title")==0) {
          strcpy(rssprgtitle,"");
          content = xmlNodeGetContent(node);
          if (content) strcpy(rssprgtitle,(char *) content);
        }
        if ((strcmp((char *) node->name,"link")==0) || (strcmp((char *) node->name,"entry")==0)) {
          // reset for earch element loading
          strcpy(rssvideolink,"");
          strcpy(rssprgfeedtitle,"");
          rssepisode=0;
          rssseason=0;
          strcpy(rssauthor,"");
          strcpy(rssprgdesc,"");
          strcpy(rssprgimage1,"");
          strcpy(rssprgimage,"");
          if (strcmp((char *) node->name,"entry")==0) {
            subnode2=node->xmlChildrenNode;
            while(subnode2) {
              strcpy(rssprgimage,"");
              if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                content = xmlNodeGetContent(subnode2);
                strcpy(rssprgfeedtitle,(char *) content);
              }
              // get play url
              if ((content) && (strcmp((char *) subnode2->name,"link")==0)) {
                content = xmlNodeGetContent(subnode2);
                tmpdat=xmlGetProp(subnode2,( xmlChar *) "href");
                if (tmpdat) {
                  strcpy(rssvideolink,(char *) tmpdat);
                  xmlFree(tmpdat);
                }
              }
              if ((content) && (strcmp((char *) subnode2->name,"group")==0)) {
                subnode3=subnode2->xmlChildrenNode;
                while(subnode3) {
                  if ((content) && (strcmp((char *) subnode2->name,"title")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    strcpy(rssprgtitle,(char *) content);
                  }
                  if ((content) && (strcmp((char *) subnode2->name,"description")==0)) {
                    content = xmlNodeGetContent(subnode2);
                    strcpy(rssprgdesc,(char *) content);
                  }
                  // get icon gfx
                  if ((content) && (strcmp((char *) subnode3->name,"thumbnail")==0)) {
                    content = xmlNodeGetContent(subnode3);
                    tmpdat=xmlGetProp(subnode3,( xmlChar *) "url");
                    if (tmpdat) {
                      //if (debugmode & 4) printf("Get image url %s \n",tmpdat);
                      strcpy(rssprgimage1,(char *) tmpdat);
                      xmlFree(tmpdat);
                    }
                  }
                  subnode3=subnode3->next;
                }
              }
              subnode2=subnode2->next;
            }
            recordexist=false;
            sprintf(sqlinsert,"SELECT feedtitle from internetcontentarticles where (feedtitle like '%s' mediaURL like '%s' and title like '%s')",rssprgtitle,rssvideolink,rssprgfeedtitle);
            mysql_query(conn,sqlinsert);
            res = mysql_store_result(conn);
            if (res) {
              while ((row = mysql_fetch_row(res)) != NULL) {
                recordexist=true;
              }
            }
            if (!(recordexist)) {
              spotify_search_and_replace2(rssprgtitle);
              spotify_search_and_replace2(rssprgfeedtitle);
              spotify_search_and_replace2(rssprgdesc);
              if (debugmode & 4) printf("\t Update title %-20s Date %s\n",rssprgtitle,rssprgpubdate);
              sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb,date,time) values(\"%s\",'%s',\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage1,rssopretdato,0);
              //sprintf(sqlinsert,"REPLACE into internetcontentarticles(feedtitle,mediaURL,title,episode,season,author,path,description,paththumb) values('%s','%s','%s',%d,%d,'%s','%s','%s','%s')",rssprgtitle,rssvideolink,rssprgfeedtitle,rssepisode,rssseason,rssauthor,"",rssprgdesc,rssprgimage1);
              if (mysql_query(conn,sqlinsert)!=0) {
                printf("mysql REPLACE table error. %s\n",sqlinsert);
              }
              res = mysql_store_result(conn);
            }
          }
        }
      }
    }
    xmlFreeDoc(document);
    mysql_close(conn);
  } else {
    if (debugmode & 4) printf("Read error on %s xmlfile downloaded to rss dir \n",filename);
  }
  return(1);
}





//
// get nr of rss feed
//

int spotify_class::get_antal_rss_feeds_sources(MYSQL *conn) {
  char sqlselect[4096];
  MYSQL_RES *res;
  MYSQL_ROW row;

  printf("*************************************************************************************\n");

  if (conn) {
    sprintf(sqlselect,"SELECT count(name) from mythtvcontroller.internetcontent");
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
      while ((row = mysql_fetch_row(res)) != NULL) {
        antalrss_feeds=atoi(row[0]);
      }
    }
  }
  return(1);
}





//
// opdate show liste in view (det vi ser)
//

// load felt 7 = mythtv gfx icon
// fpath=stream path
// atr = stream name

int spotify_class::opdatere_stotify_oversigt(char *art,char *fpath) {
    char sqlselect[2048];
    char tmpfilename[1024];
    char lasttmpfilename[1024];
    char downloadfilename[1024];
    char downloadfilename1[1024];
    char downloadfilenamelong[1024];
    char homedir[1024];
    // mysql vars
    bool rss_update=false;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythtvcontroller";
    bool online;
    int getart=0;
    bool loadstatus=true;
    bool dbexist=false;
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
      //
      // create db if not exist
      // and dump some default rss feeed in
      //
      printf("Creating/Update spotify.\n");
      // thumbnail   = name of an local image file
      // commandline = Program to fetch content with
      // updated     = Time of last update
      if ((!(dbexist)) && (mysql_query(conn,"CREATE database if not exist mythtvcontroller")!=0)) {
        printf("mysql db create error.\n");
      }
      res = mysql_store_result(conn);
      // create db
      sprintf(sqlselect,"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontentarticles(name varchar(255),paththumb text,player varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sqlselect);
      }
      res = mysql_store_result(conn);
      // create db
      sprintf(sqlselect,"CREATE TABLE IF NOT EXISTS mythtvcontroller.spotifycontent(name varchar(255),thumbnail varchar(255),id int NOT NULL AUTO_INCREMENT PRIMARY KEY,INDEX Idx (name (15),thumbnail (15),type, author (15),  description (15),commandline (15),version,updated,search ,tree,podcast,download,host (15))) ENGINE=MyISAM AUTO_INCREMENT=60 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql create table error.\n");
        printf("SQL : %s\n",sqlselect);
      }
      if (!(dbexist)) {
        // create index
        sprintf(sqlselect,"CREATE INDEX `spotifycontentarticles_title`  ON `mythtvcontroller`.`spotifycontentarticles` (name) COMMENT '' ALGORITHM DEFAULT LOCK DEFAULT");
        if (mysql_query(conn,sqlselect)!=0) {
          printf("mysql create index error.\n");
        }
      }
      res = mysql_store_result(conn);
      // ok Aftenshowet
      // dr have removed the rss feed

      // close mysql
      if (conn) mysql_close(conn);
      // download new rrs files we just insert in db
      // and update db from rss file
      if (!(dbexist)) loadrssfile(1);
    }
    //if (debugmode & 4) printf("* art = %s fpath=%s *\n",art,fpath);
    clean_spotify_oversigt();                // clean old list
    strcpy(lasttmpfilename,"");    					// reset
    if (debugmode & 4) {
      printf("loading rss/stream data.\n");
      printf("art = %s tpath = %s \n",art,fpath);
    }
    // find records after type to find
    if ((strcmp(art,"")==0) && (strcmp(fpath,"")==0)) {
      sprintf(sqlselect,"select (spotifycontentarticles.name) as name from spotifycontentarticles");
      getart=0;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")==0)) {
      sprintf(sqlselect,"select (feedtitle) as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),count(path),(paththumb),(mediaURL),(time) as nroftimes,(id) as id from spotifycontentarticles where mediaURL is NOT NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' GROUP BY title ORDER BY id");
      getart=1;
    } else if ((strcmp(art,"")!=0) && (strcmp(fpath,"")!=0)) {
      sprintf(sqlselect,"select ANY_VALUE(feedtitle) as feedtitle,(path) as path,(title) as title,(description),(url),(thumbnail),(paththumb),(time) as nroftimes,(id) as id from spotifycontentarticles where mediaURL is NULL and feedtitle like '");
      strcat(sqlselect,art);
      strcat(sqlselect,"' AND path like '");
      strcat(sqlselect,fpath);
      strcat(sqlselect,"' ORDER BY abs(title) desc"); // ASC
      getart=2;
    }
    this->type=getart;					// husk sql type
    if (debugmode & 4) printf("spotify loader started... \n");
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser,configmysqlpass, database, 0, NULL, 0)) {
      get_antal_rss_feeds_sources(conn);
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      if (mysql_query(conn,sqlselect)!=0) {
        printf("mysql insert error.\n");
        printf("SQL %s \n",sqlselect);
      }
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
          //if (debugmode & 4) printf("Get info from stream nr %d %-20s\n",antal,row[2]);
          if (antal<maxantal) {
            stack[antal]=new (struct spotify_oversigt_type);
            if (stack[antal]) {
              strcpy(stack[antal]->feed_showtxt,"");          	            // show name
              strcpy(stack[antal]->feed_name,"");		                        // mythtv db feedtitle
              strcpy(stack[antal]->feed_desc,"");                           // desc
              //strcpy(stack[antal]->feed_path,"");                           // mythtv db path
              strcpy(stack[antal]->feed_gfx_url,"");
              strcpy(stack[antal]->feed_gfx_mythtv,"");
              stack[antal]->feed_group_antal=0;
              stack[antal]->feed_path_antal=0;
              stack[antal]->textureId=0;
              stack[antal]->intnr=0;
              stack[antal]->nyt=false;
              // top level
              if (getart==0) {
                strncpy(stack[antal]->feed_showtxt,row[0],spotify_pathlength);
                strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
                //if (row[1]) strcpy(stack[antal]->feed_path,row[1]);
                if (row[0]) stack[antal]->feed_group_antal=0;        // get antal
                else stack[antal]->feed_group_antal=0;
                if (row[3]) strncpy(stack[antal]->feed_desc,row[3],spotify_desclength);
                if (row[7]) strncat(tmpfilename,row[7],20);                                     //
                strcpy(stack[antal]->feed_gfx_mythtv,tmpfilename);            	       		      // icon file
                if (row[9]) strcpy(tmpfilename,row[9]);
                get_webfilenamelong(downloadfilename,tmpfilename);          // get file name from url
                // check filename
                strcpy(downloadfilename1,downloadfilename);                 // back name before change
                int mmm=0;
                while(mmm<strlen(downloadfilename)) {
                  if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                  mmm++;
                }
                getuserhomedir(homedir);                                                  // get homedir
                strcpy(downloadfilenamelong,homedir);
                strcat(downloadfilenamelong,"/spotify/images/");
                strcat(downloadfilenamelong,downloadfilename);
                if (!(file_exists(downloadfilenamelong))) {
                  if (debugmode & 4) printf("Loading image %s realname %s \n",tmpfilename,downloadfilenamelong);
                  // download gfx file and use as icon
                  if (get_webfile2(tmpfilename,downloadfilenamelong)==-1) {
                    printf("Download error \n");
                  } else strcpy(tmpfilename,"");
                }
                strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	                // mythtv icon file
                antal++;
              } else {
                // if first creat back button
                if (antal==0) {
                  stack[antal]->textureId=_textureIdback;			                    // back icon
                  strcpy(stack[antal]->feed_showtxt,"BACK");
                  if (row[0]) strncpy(stack[antal]->feed_name,row[0],spotify_namelength);
                  //if (row[1]) strncpy(stack[antal]->feed_path,row[1],spotify_pathlength);
                  //strcpy(stack[antal]->feed_streamurl,row[4]);
                  if (row[3]) strncpy(stack[antal]->feed_desc,row[3],spotify_desclength);
                  if (row[5]) strncpy(stack[antal]->feed_gfx_url,row[5],feed_url);            // feed (db link) url
                  stack[antal]->feed_group_antal=0;
                  stack[antal]->intnr=0;	                               					// intnr=0 = back button type
                  antal++;
                }
                // alloc new element in array
                if (stack[antal]==NULL) stack[antal]=new (struct spotify_oversigt_type);
                stack[antal]->intnr=1;
                if (row[5]) strncpy(stack[antal]->feed_gfx_url,row[5],feed_url);
                if (getart==1) {
                  // old ver used path from mysql
                  //if (row[1]) strncpy(stack[antal]->feed_showtxt,row[1],spotify_pathlength);
                  if (row[2]) strncpy(stack[antal]->feed_showtxt,row[2],spotify_pathlength);
                } else {
                  if (row[2]) strncpy(stack[antal]->feed_showtxt,row[2],spotify_pathlength);
                }
                //if (row[1]) strncpy(stack[antal]->feed_path,row[1],spotify_pathlength);		// path
                if (row[2]) strncpy(stack[antal]->feed_name,row[0],spotify_namelength);		// feedtitle
                // old ver
                //if (row[4]) strncpy(stack[antal]->feed_streamurl,row[4],feed_url);		  // save play url
                //if (row[8]) strncpy(stack[antal]->feed_streamurl,row[8],feed_url);		  // save play url
                switch(getart) {
                  case 0: if (row[9]) strcpy(tmpfilename,row[9]);
                          break;
                  case 1: if (row[7]) strcpy(tmpfilename,row[7]);
                          break;
                  case 2: if (row[5]) strcpy(tmpfilename,row[5]);
                          break;
                }
                // test antal afspillinger this 0 set som new rss
                if (row[9]) {
                  if (atoi(row[9])==0) stack[antal]->nyt=true; else stack[antal]->nyt=false;
                }
                if (strlen(tmpfilename)>7) {
                  if (strncmp(tmpfilename,"%SHAREDIR%",10)==0) {
                    strcpy(tmpfilename,"/usr/share/mythtv");                            // mythtv path
                    if (strlen(row[7])>10) {
                      if (getart==1) {
                        if (row[7]) strncat(tmpfilename,row[7]+10,100);
                      }
                      if (getart==2) {
                        if (row[6]) strncat(tmpfilename,row[6]+10,100);
                      }
                    }
                  } else {
                    // rss download
                    // downloadfilename = name on file, from tmpfilename = full web url
                    get_webfilenamelong(downloadfilename,tmpfilename);          // get file name from url
                    // check filename fro ? or = and replace to _
                    strcpy(downloadfilename1,downloadfilename);                 // back name before change
                    int mmm=0;
                    while(mmm<strlen(downloadfilename)) {
                      if ((downloadfilename[mmm]=='?') || (downloadfilename[mmm]=='=')) downloadfilename[mmm]='_';
                      mmm++;
                    }
                    strcpy(lasttmpfilename,tmpfilename);			              // husk file name
                    // save file in  user homedir rss/
                    getuserhomedir(homedir);                                  // get homedir
                    strcpy(downloadfilenamelong,homedir);
                    strcat(downloadfilenamelong,"/spotify/images/");
                    strcat(downloadfilenamelong,downloadfilename);
                    if (!(file_exists(downloadfilenamelong))) {
                      if (debugmode & 4) printf("Loading image %s realname %s \n",tmpfilename,downloadfilenamelong);
                      // download gfx file and use as icon
                      if (get_webfile2(tmpfilename,downloadfilenamelong)==-1) {
                        printf("Download error \n");
                      } else strcpy(tmpfilename,"");
                    }
                    strcpy(tmpfilename,downloadfilenamelong);
                  }
                } else strcpy(tmpfilename,"");
                strncpy(stack[antal]->feed_gfx_mythtv,tmpfilename,200);	                // save icon file path in stack struct
//                          strcpy(stack[antal]->feed_streamurl,row[4]);	// stream url
                if (row[3]) strncpy(stack[antal]->feed_desc,row[3],spotify_desclength);
                // no texture now
                stack[antal]->textureId=0;
                // opdatere group antal
                if (getart==1) stack[antal]->feed_group_antal=atoi(row[6]); else stack[antal]->feed_group_antal=0;
                antal++;
              }
            }
          }
        }
        mysql_close(conn);
      } else {
        if (debugmode & 4) printf("No stream data loaded \n");
      }
      //load_stream_gfx();
      //
      // load all the data in phread datainfoloader
      // web gfx file loader in phread
      pthread_t loaderthread;           // the load
      int rc=pthread_create(&loaderthread,NULL,load_spotify_web,NULL);
      if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
      return(antal-1);
    } else printf("Failed to update Spotify db, can not connect to database: %s Error: %s\n",dbname,mysql_error(conn));
    if (debugmode & 4) printf("Spotify loader done... \n");
    return(0);
}


//
// thread web loader (loading all icons)
//

void *load_spotify_web(void *data) {
  if (debugmode & 4) printf("Start spotify loader thread\n");
  streamoversigt.loadweb_stream_iconoversigt();
  if (debugmode & 4) printf("Stop spotify loader thread\n");
}


// in use
// downloading all rss podcast icon gfx

int spotify_class::loadweb_stream_iconoversigt() {
  int antal;
  int nr=0;
  int loadstatus;
  char tmpfilename[2000];
  char downloadfilename[2900];
  char downloadfilenamelong[5000];
  char homedir[200];
  antal=this->streamantal();
  this->gfx_loaded=false;
  if (debugmode & 4) printf("rss stream gfx download start \n");
  while(nr<antal) {
    if (strcmp(stack[nr]->feed_gfx_mythtv,"")!=0) {
      loadstatus=0;
      // return downloadfilename from stack[nr]->feed_gfx_mythtv
      strcpy(tmpfilename,stack[nr]->feed_gfx_mythtv);
      if (strncmp(tmpfilename,"http://",7)==0) {
        // download file from web
        // return dowloadfilebame = file downloaded name no path
        get_webfilename(downloadfilename,tmpfilename);
        // add download path
        // add download filename to class opbject
        getuserhomedir(downloadfilenamelong);
        strcat(downloadfilenamelong,"/spotify/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0))  {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
            //printf("File exist %s then set filename \n",downloadfilenamelong);
        }
      } else if (strncmp(tmpfilename,"https://",8)==0) {
        //strcpy(lastfile,downloadfilename);
        get_webfilename(downloadfilename,tmpfilename);
        getuserhomedir(downloadfilenamelong);
        // build path
        strcat(downloadfilenamelong,"/spotify/images/");
        strcat(downloadfilenamelong,downloadfilename);
        if ((!(file_exists(downloadfilenamelong))) && (check_zerro_bytes_file(downloadfilenamelong)==0)) {
          if (debugmode & 4) printf("nr %3d Downloading : %s \n",nr,tmpfilename);
          loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        } else {
          if (!(file_exists(downloadfilenamelong))) loadstatus=get_webfile2(tmpfilename,downloadfilenamelong);
          strcpy(stack[nr]->feed_gfx_mythtv,downloadfilenamelong);
        }
      }
      // set recordnr loaded info to update users view
      this->stream_oversigt_nowloading=nr;
    }
    nr++;
  }
  if (nr>0) this->gfx_loaded=true; else this->gfx_loaded=false;
  if (debugmode & 4) {
    if (gfx_loaded) printf("spotify download end ok. \n");
    else printf("spotify download error. \n");
  }
  return(1);
}


//
// play stream by
//

void spotify_class::playstream(char *url) {
    //vlc_controller::playmedia(url);
}


void spotify_class::show_spotify_oversigt(GLuint normal_icon,GLuint empty_icon,GLuint empty_icon1,int _mangley,int stream_key_selected)

{
    int j,ii,k,pos;
    int buttonsize=200;                                                         // button size
    float buttonsizey=160.0f;                                                   // button size
    float yof=orgwinsizey-(buttonsizey);                                        // start ypos
    float xof=0.0f;
    int lstreamoversigt_antal=9*6;
    int i=0;                                                                    // data ofset in stack array
    unsigned int sofset=0;
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

    if (stream_oversigt_loaded_nr==0) strcpy(downloadfilename_last,"");
    if ((this->streamantal()) && (stream_oversigt_loaded==false) && (this->stream_oversigt_loaded_nr<this->streamantal())) {
      if (stack[stream_oversigt_loaded_nr]) strcpy(gfxfilename,stack[stream_oversigt_loaded_nr]->feed_gfx_mythtv);
      else strcpy(gfxfilename,"");
      strcpy(gfxshortname,"");
      gfxshortnamepointer=strrchr(gfxfilename,'.');     // get last char = type of file
      if (gfxshortnamepointer) {
        strcpy(gfxshortname,gfxshortnamepointer);
      }
      // load texture if none loaded
      // get_texture return 0 if not loaded
      if (get_texture(stream_oversigt_loaded_nr)==0) {
        if (strcmp(gfxfilename,"")!=0) {
          // check om der findes en downloaded icon
          strcpy(downloadfilenamelong,"");
          strcat(downloadfilenamelong,gfxfilename);
          if (strcmp(downloadfilename_last,gfxfilename)!=0) {
            // check om filen findes i cache dir eller i mythtv netvision dir
            if (file_exists(gfxfilename)) {
              texture=loadTexture ((char *) gfxfilename);
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else if (file_exists(downloadfilenamelong)) {
              // er det ikke samme texture som sidst loaded så load it
              // else set last used
              texture=loadTexture ((char *) downloadfilenamelong);
              if (texture) set_texture(stream_oversigt_loaded_nr,texture);
              last_texture=texture;
              antal_loaded+=1;
            } else texture=0;
          } else {
            if (last_texture) set_texture(stream_oversigt_loaded_nr,last_texture);
            antal_loaded+=1;
          }
          // husk last file name
          strcpy(downloadfilename_last,downloadfilenamelong);
        }
      }
      // down loading ?
      if (stream_oversigt_loaded_nr==this->streamantal()) {
        stream_oversigt_loaded=true;
        stream_oversigt_loaded_done=true;
      } else stream_oversigt_loaded_nr++;
    }

    if (!(gfx_loaded)) {
      stream_oversigt_loaded_nr=0;
      stream_oversigt_loaded=false;
    }
    // calc start pos (ofset)
    sofset=(_sangley/40)*8;
    // draw icons
    while((i<lstreamoversigt_antal) && (i+sofset<antal) && (stack[i+sofset]!=NULL)) {
      if (((i % bonline)==0) && (i>0)) {
        yof=yof-(buttonsizey+20);
        xof=0;
      }
      // error is in this IF block
      // stream har et icon in db

      if (i+1==(int) stream_key_selected) buttonsizey=170.0f;
      else buttonsizey=160.0f;

      if (stack[i+sofset]->textureId) {
        // stream icon
//        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( xof+10, yof+10, 0.0);
        glTexCoord2f(0, 1); glVertex3f( xof+10,yof+buttonsizey-20, 0.0);
        glTexCoord2f(1, 1); glVertex3f( xof+buttonsize-10, yof+buttonsizey-20 , 0.0);
        glTexCoord2f(1, 0); glVertex3f( xof+buttonsize-10, yof+10 , 0.0);
        glEnd();
//        glPopMatrix();

        glPushMatrix();
        // indsite draw icon rss gfx
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
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
        glBlendFunc(GL_ONE, GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        //glBindTexture(GL_TEXTURE_2D,normal_icon);
        glBindTexture(GL_TEXTURE_2D,empty_icon1);
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
      width = 20;
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
        glTranslatef(1.0f-(strlen(base)/1.6f)+2,-pline*1.2f,0.0f);
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
    if ((i==0) && (antal_rss_streams()==0)) {
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
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
      glcRenderString("Please wait Loading ...");
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
