//
// All setup functions and windows
//
#include <GL/glew.h>
// #include <GL/freeglut.h>
// #include <GL/glc.h>                     // glc true type font system
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdarg.h>
// opengl stuf
// #include <X11/Intrinsic.h>    /* Display, Window */
// #include <GL/glx.h>           /* GLXContext */
#include <libxml/parser.h>
#include <fstream>
#include <fmt/format.h>
#include <filesystem>
#include <iostream>

// mysql support
#include <mysql.h>

// sound system
#include "/opt/mythtv-controller/fmodstudioapi20311linux/api/core/inc/fmod.hpp"
#include "/opt/mythtv-controller/fmodstudioapi20311linux/api/core/inc/fmod_errors.h"

// program include
#include "renderer.h"
#include "text3d.h"
#include "readjpg.h"
#include "myth_setup.h"
#include "checknet.h"
#include "utility.h"
#include "myctrl_tvprg.h"
#include "myctrl_torrent.h"
#include "myth_ttffont.h"

extern Renderer renderer;
extern FT_Face face;


using namespace std;
namespace fs = std::filesystem;

extern FILE *logfile;
extern char debuglogdata[1024];                                                 // used by log system
extern float configdefaultradiofontsize;                                        // font size in overviews
extern float configdefaulttvguidefontsize;
extern float configdefaultmusicfontsize;
extern float configdefaultstreamfontsize;
extern float configdefaultmoviefontsize;

// uv stuf
extern float spectrum_left[];                                                      // used for spectium
extern float spectrum_right[];                                                     // used for spectium
extern float spectrum[];                                                     // used for spectium

extern unsigned int keybufferindex;                          // keyboard buffer index

extern int configuvmeter;
extern int debugmode;
extern int vis_nyefilm_oversigt;
extern char localuserhomedir[4096];

extern GLuint textureId_uv2;
extern GLuint _textureId28;
extern GLuint _textureId_cursor;



extern FMOD::System    *sndsystem;


extern Font myfont;
extern Font myfont2;



// controll rss
//
rss_stream_class rssstreamoversigt;
//


extern bool hent_tv_channels;
extern tv_graber_config  aktiv_tv_graber;
extern int PRGLIST_ANTAL;
extern bool global_use_internal_music_loader_system;
extern bool full_screen;
extern char configmythsoundsystem[256];
extern fontctrl aktivfont;
extern char configmysqluser[256];               // mysql username
extern char configmysqlpass[256];               // mysql pass
extern char configmysqlhost[256];               // mysql host name
extern char configxbmcuser[256];                // xbmc access info
extern char configxbmcpass[256];                //
extern char configxbmchost[256];                //
extern char confighostname[256];                // host name
extern char configbackend[20];					        // backend system xbmc/mythtv
extern char confighostip[256];				        	// mysql host ip adress
extern char confighostwlanname[256];				    // wlan name id
extern char configpicturepath[256];             // path til pictures gallery source
extern char configmusicpath[256];               // path til music source (found in sqlbase/mythtvbackend)
extern char configmoviepath[256];				        // path til movie source (found in sqlbase/mythtvbackend)
extern char configrecordpath[256];	       			// path til recorded source (found in sqlbase/mythtvbackend)
extern char configdeviceid[256];		         		// default music player device
extern char configscreensavertimeout[256];			// screen saver time out i sekunder
extern char keybuffer[512];                     // keyboard buffer
extern std::string keybuffer1;                  // keyboard buffer
extern char configclosemythtvfrontend[256];			// close mythtvfront end on startup
extern char configaktivescreensavername[256];		// screen saver name
extern char configsoundoutport[256];				    // sound output port (hdmi/spdif/analog)
extern char configfontname[200];				        // default ttf font name to load and use
extern long configtvguidelastupdate;            // last xmltv update
extern char configbackend_tvgraber[256];        // internal tv graber to use
extern char configbackend_tvgraberland[256];    // internal tv graber to use country
struct configkeytype configkeyslayout[12];			// functions keys startfunc
extern char configuse3deffect[20];
// extern char configvideoplayer[200];             // default video player
extern char configdefaultplayer[200];
extern int configdefaultplayer_screenmode;
extern int configland;
extern char *configlandsprog[];
extern int configxbmcver;

extern torrent_loader torrent_downloader;

extern GLuint setuptvgraberback;
extern GLuint _texturesetupclose;
extern GLuint setuptexture;
extern GLuint _texturesoundsetup;
extern GLuint _texturesourcesetup;
extern GLuint _textureimagesetup;
extern GLuint _texturetemasetup;
extern GLuint _texturesetupfont;
extern GLuint _texturekeyssetup;
extern GLuint _texturekeysrss;
extern GLuint _texturespotify;
extern GLuint _texturetidal;
extern GLuint _texturelock;			                  // en hænge lås
extern GLuint setupkeysbar1;
extern GLuint setupkeysbar2;
extern GLuint setupsoundback;
extern GLuint setupsqlback;
extern GLuint setuptorrent_background;
extern GLuint setupnetworkback;
extern GLuint setupnetworkwlanback;
extern GLuint setupscreenback;
extern GLuint setupbackend;
extern GLuint setuptemaback;                        //
extern GLuint setupfontback;                        //
extern GLuint setupkeysback;                        //
extern GLuint setuprssback;                         //
extern GLuint _texturevideoplayersetup;             // setup
extern GLuint _texturemythtvsql;
extern GLuint _textureclose;
extern GLuint _textureselect;
extern GLuint _texturetvgrabersetup;                //
extern GLuint setupupdatebutton;
extern GLuint screenshot1,screenshot2,screenshot3,screenshot4,screenshot5,screenshot6,screenshot7,screenshot8,screenshot9,screenshot10;
extern int do_show_setup_select_linie;
extern int tema;
extern int screen_size;
extern int configmythtvver;
extern wifinetdef wifinets;
extern unsigned int setupwlanselectofset;                                       // valgte wlan nr i oversigt
extern int orgwinsizey;                                                         // screen size
extern int orgwinsizex;                                                         //
static bool fontselected=false;
//static int fontnr=0;
const GLfloat selectcolor[3]={1.0f,1.0f,0.0f};		                              // text select color

extern channel_list_type channel_list;
extern char keybuffer[];                                    // keyboard buffer

extern Renderer renderer;

float leftLevel = 0.0f;
float rightLevel = 0.0f;
float vuLeftSmooth = 0.0f;
float vuRightSmooth = 0.0f;

float peakLeft = 0.0f;
float peakRight = 0.0f;
// float peakFallSpeed = 0.005f;
float peakHoldValue = 0.005f;     // 0..1
float peakFallSpeed = 0.0005f; // how fast the peak falls



void updatePeakHold(float vuValue) {
  if (vuValue > peakHoldValue)
    peakHoldValue = vuValue;          // new peak
  else
    peakHoldValue -= peakFallSpeed;  // fall slowly
  if (peakHoldValue < 0.0f) peakHoldValue = 0.0f;
}


// *****************************************************************************
//
// 
//
// *****************************************************************************


void drawPeakMarker(float cx, float cy, float value) {
    if (value < 0) value = 0;
    if (value > 1) value = 1;

    const float minAngle = -80.0f;
    const float maxAngle =  80.0f;

    float angle = minAngle + (maxAngle - minAngle) * value;
    float rad   = angle * 3.14159265f / 180.0f;

    float inner = 150.0f;
    float outer = 170.0f;

    float x1 = cx + sin(rad) * inner;
    float y1 = cy + cos(rad) * inner;
    float x2 = cx + sin(rad) * outer;
    float y2 = cy + cos(rad) * outer;

    glColor3f(1.0f, 1.0f, 0.0f); // bright yellow
    glLineWidth(4.0f);

    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
}



// *****************************************************************************
//
// ---------------- SMOOTHING ----------------
//
// *****************************************************************************

float smoothVU(float current, float target) {
    const float attack = 0.35f;
    const float release = 0.08f;
    if (target > current)
      current += (target - current) * attack;
    else
      current += (target - current) * release;
    return current;
}


// *****************************************************************************
//
// ---------------- GET LEVELS ----------------
// 
// *****************************************************************************

void getLevels() {
  float left[512];
  float right[512];
  float faktor=120.0f;                             /// faktor 512 default 
  float l = 0, r = 0;

  for (int i = 0; i < 512; ++i) {    
    l += fabs(spectrum_left[i]);
    r += fabs(spectrum_right[i]);
  }

  l /= faktor;
  r /= faktor;

  vuLeftSmooth  = smoothVU(vuLeftSmooth, l);
  vuRightSmooth = smoothVU(vuRightSmooth, r);

  // updatePeakHold(vuLeftSmooth);

  // Peak hold
  if (vuLeftSmooth > peakLeft)  peakLeft  = vuLeftSmooth;
  else peakLeft -= peakFallSpeed;

  if (vuRightSmooth > peakRight) peakRight = vuRightSmooth;
  else peakRight -= peakFallSpeed;

  if (peakLeft < 0)  peakLeft = 0;
  if (peakRight < 0) peakRight = 0;

}

// *********************************************************************************
//
// ---------------- DRAW NEEDLE ----------------
//
// *********************************************************************************

void drawNeedle(float cx, float cy, float value) {
  float minAngle = -60.0f;
  float maxAngle = 60.0f;
  // if (value < 0) value = 0;
  // if (value > 1) value = 1;
  float angle = minAngle + (maxAngle - minAngle) * value;
  float rad = angle * 3.14159265f / 180.0f;
  float len = 160;
  float x2 = cx + sin(rad) * len;
  float y2 = cy + cos(rad) * len;

  glColor3f(1, 0, 0);
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2f(cx, cy);
  glVertex2f(x2, y2);
  glEnd();
}



// ******************************************
//
// DRAW METER
//
// ******************************************

void drawVUMeter(float x, float y, float value,GLuint textureId) {
  float cx = x + 220;
  float cy = y + 50;
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glColor3f(0.2f, 0.2f, 0.2f);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(x, y);
  glTexCoord2f(1, 0); glVertex2f(x + 442, y);
  glTexCoord2f(1, 1); glVertex2f(x + 442, y + 326);
  glTexCoord2f(0, 1); glVertex2f(x, y + 326);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  drawNeedle(cx, cy, value);
  
  // drawPeakMarker(cx,cy,value);
}



// ****************************************************************************************
// 
// ---------------- MAIN LOOP include function ----------------
//
// ****************************************************************************************

void render_uv() {
  getLevels();
  drawVUMeter( 500, 400, vuLeftSmooth , textureId_uv2);
  drawVUMeter( 1000, 400, vuRightSmooth, textureId_uv2);
}

// ****************************************************************************************
//
// draw cursor on screen at pos
//
// ****************************************************************************************

void showcoursornow(int cxpos,int cypos,int txtlength) {
  cxpos+=4+(txtlength*9);
  cypos+=6;
  struct timespec timer;
  long lasttimer=0;
  static bool showcursor=false;
  clock_gettime(CLOCK_REALTIME, &timer);
  if (timer.tv_nsec>lasttimer+243600692) showcursor=true; else showcursor=false;
  if (showcursor) {
    lasttimer=timer.tv_nsec;
    renderer.AddTextureRect(0,_textureId_cursor, cxpos, cypos, 6, 14,1,1,1,1);
  }
}



// *********************************************************************************************************



// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

rss_stream_class::rss_stream_class() {
  antal=0;
  for(int n=0;n<maxantal;n++) {
    // rss_source_feed[n].stream_name=new char[namemaxlength];
    // rss_source_feed[n].stream_url=new char[urlmaxlength];
    // strcpy(rss_source_feed[n].stream_name,"");
    // strcpy(rss_source_feed[n].stream_url,"");
  }
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

rss_stream_class::~rss_stream_class() {
  for(int n=0;n<maxantal;n++) {
    // delete[] rss_source_feed[n].stream_name;
    // delete[] rss_source_feed[n].stream_url;
  }
}



std::string rss_stream_class::get_stream_name_std(int nr) { 
  if ((nr>=0) && (nr < rss_source_feed_vector.size())) {
    return (rss_source_feed_vector[nr].stream_name);
  }
  return "";
}


std::string rss_stream_class::get_stream_url_std(int nr) { 
  if ((nr>=0) && (nr < rss_source_feed_vector.size())) {
    return (rss_source_feed_vector[nr].stream_url);
  }
  return "";
}



// ****************************************************************************************
//
// loader from db
//
// ****************************************************************************************

int rss_stream_class::load_rss_data() {
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  antal=0;
  rss_stream_struct new_rss_source_feed;
  rss_source_feed_vector.clear();
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,"select feedtitle,title,url from internetcontentarticles where mediaURL is NULL order by feedtitle asc");
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (antal<maxantal)) {
        if (row[0]) {
          new_rss_source_feed.stream_name=row[0];
        }
        if (row[2]) {
          new_rss_source_feed.stream_url=row[2];          
        }
        rss_source_feed_vector.push_back(new_rss_source_feed);
      }
    }
    mysql_close(conn);
    antal=rss_source_feed_vector.size();
  } else write_logfile(logfile,(char *) "Error connect to mysql.");
  return(rss_source_feed_vector.size());
}

// ****************************************************************************************
//
// save/update rss db
//
// ****************************************************************************************

int rss_stream_class::save_rss_data() {
  bool doexist;
  char sqlstring[2048];
  char ftitle[2048];
  char ftitle2[2048];
  char furl[2048];
  bool db_state_status;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row,row1;
  int n=0;
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    for(n=0;n<rss_source_feed_vector.size();n++) {
      // find record
      doexist=false;
      strcpy(ftitle,"");
      strcpy(ftitle2,"");
      strcpy(furl,"");
      if ((rss_source_feed_vector[n].stream_name.length()>0) && (rss_source_feed_vector[n].stream_name.length()>0)) {
        sprintf(sqlstring,"select feedtitle,url from internetcontentarticles where feedtitle like '%s' limit 1",rss_source_feed_vector[n].stream_name.c_str());
        mysql_query(conn,sqlstring);
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res)) != NULL) {
          doexist=true;
          strcpy(ftitle,row[0]);
          strcpy(furl,row[0]);
        }
        if (doexist) {
          // find record in db and do the change and update record again
          if (ftitle) {
            if (rss_source_feed_vector[n].stream_name.length()==0) {
              sprintf(sqlstring,"update internetcontentarticles set url='%s' where title like '%s' limit 1",rss_source_feed_vector[n].stream_url.c_str(),rss_source_feed_vector[n].stream_name.c_str());
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              //
              // update name
              //
              // save old name
              sprintf(sqlstring,"select title from internetcontentarticles where url like '%s' limit 1",rss_source_feed_vector[n].stream_url.c_str());
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              while (((row1 = mysql_fetch_row(res1)) != NULL) && (antal<100)) {
                strcpy(ftitle2,row1[0]);
              }
              // update db record with new name
              sprintf(sqlstring,"update internetcontentarticles set name='%s' where url like '%s' limit 1",rss_source_feed_vector[n].stream_name.c_str(),rss_source_feed_vector[n].stream_url.c_str());
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              if (res1) {
                // if okay update
                // chnage/update name
                sprintf(sqlstring,"update internetcontent set name='%s' where name like '%s'",rss_source_feed_vector[n].stream_name.c_str(),ftitle2);
                mysql_query(conn,sqlstring);
                res1 = mysql_store_result(conn);
              }
            }
          }
        } else { 
          // no update of name of url create new
          sprintf(sqlstring,"insert into internetcontentarticles (feedtitle,title,url) values('%s','%s','%s')",rss_source_feed_vector[n].stream_name.c_str(),rss_source_feed_vector[n].stream_name.c_str(),rss_source_feed_vector[n].stream_url.c_str());
          db_state_status=mysql_query(conn,sqlstring);
          res1 = mysql_store_result(conn);
          if (db_state_status==false) {
            write_logfile(logfile,(char *) "Error insert new rss feed to mysql db.");
          }
          res1 = mysql_store_result(conn);
          sprintf(sqlstring,"insert into internetcontent (name,type) values('%s',0)",rss_source_feed_vector[n].stream_name.c_str());
          db_state_status=mysql_query(conn,sqlstring);
          res1 = mysql_store_result(conn);
          if (db_state_status==false) {
            write_logfile(logfile,(char *) "Error insert new rss feed to mysql db.");
          }
        }
      }
    } //for next
    mysql_close(conn);
  } else write_logfile(logfile,(char *) "Error open mysql.");
  return(1);
}


// ****************************************************************************************
//
// get url
//
// ****************************************************************************************

int rss_stream_class::set_stream_url(int nr,char *url) {
  if (nr>=rss_source_feed_vector.size()) {
    rss_stream_struct new_rss_source_feed;
    new_rss_source_feed.stream_name="";
    new_rss_source_feed.stream_url="";
    rss_source_feed_vector.push_back(new_rss_source_feed);
  }
  rss_source_feed_vector[nr].stream_url=url;
  return(1);
}


// ****************************************************************************************
//
// update name
//
// ****************************************************************************************

int rss_stream_class::set_stream_name(int nr,char *name) {
  if (nr>=rss_source_feed_vector.size()) {
    rss_stream_struct new_rss_source_feed;
    new_rss_source_feed.stream_name="";
    new_rss_source_feed.stream_url="";
    rss_source_feed_vector.push_back(new_rss_source_feed);
  }
  rss_source_feed_vector[nr].stream_name=name;
  return(1);
}


// ****************************************************************************************
//
// Setup screen config
//
// ****************************************************************************************

void show_setup_screen() {
  char resl[1024];
  int winsizx=1200;
  int winsizy=800;
  int xpos=0;
  int ypos=0;
  // background

  renderer.AddTextureRect(0,setupscreenback, 300, 300, 850, 500,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 600, 700, 188, 81,1,1,1,1);

  switch(screen_size) {
      case 1: sprintf(resl,"1024 x 768 (720p)  ");
              break;
      case 2: sprintf(resl,"1280 x 1024 (720p) ");
              break;
      case 3: sprintf(resl,"1920 x 1080 (1080p)");
              break;                
      case 4: sprintf(resl,"1360 x 768         ");
              break;
      default:sprintf(resl,"1024 x 768 (720p)  ");
              break;
  }
  renderer.AddText(&myfont,450 ,450+(0*18) ,"Screen size ",1,1,1,1);
  renderer.AddText(&myfont,550 ,450+(0*18) ,resl,1,1,1,1);

  renderer.AddText(&myfont,450 ,450+(1*18) ,"Language                 ",1,1,1,1);
  switch(configland) {
    case 0: strcpy(keybuffer,"English");
    break;
    case 1: strcpy(keybuffer,"Dansk");
    break;
    case 2: strcpy(keybuffer,"France");
    break;
    case 3: strcpy(keybuffer,"Tysk");
    break;
    case 4: strcpy(keybuffer,"Arabi");
    break;
    default: strcpy(keybuffer,"English");
    break;
  }
  renderer.AddText(&myfont,550 ,450+(1*18) ,keybuffer,1,1,1,1);
  renderer.AddText(&myfont,450 ,450+(2*18) ,"Saver timeout",1,1,1,1);
  if (full_screen)
    renderer.AddText(&myfont,450 ,450+(4*18) ,"Full screen mode: NO ",1,1,1,1);
  else renderer.AddText(&myfont,450 ,450+(4*18) ,"Full screen mode: YES",1,1,1,1);
  sprintf(resl,"Hardware %s",glGetString(GL_RENDERER));
  renderer.AddText(&myfont,450 ,450+(6*18) ,resl,1,1,1,1);
  sprintf(resl," Render   %s",glGetString(GL_VENDOR));
  renderer.AddText(&myfont,450 ,450+(7*18) ,resl,1,1,1,1);
  sprintf(resl," Version  %s",glGetString(GL_VERSION));
  renderer.AddText(&myfont,450 ,450+(8*18) ,resl,1,1,1,1);
}



// ****************************************************************************************
//
// Setup video play config
//
// ****************************************************************************************


void show_setup_video() {
  std::string text;
  int winsizx=750;
  int winsizy=550;
  int xpos=300;
  int ypos=300;
  // background
  renderer.AddTextureRect(0,setupscreenback, 300, 300, winsizx, winsizy,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, xpos+300, ypos + 400, 188, 81,1,1,1,1);
  renderer.AddText(&myfont2,xpos + 270 ,ypos + 50 ,"Player settings",1,1,1,1);
  // setup
  renderer.AddText(&myfont,xpos + 100 ,ypos + 150 ,"Videoplayer",1,1,1,1);
  renderer.AddText(&myfont,xpos + 240 ,ypos + 150 ,(char *) configdefaultplayer,1,1,1,1);
  renderer.AddText(&myfont,xpos + 100 ,ypos + 150+(1*18) ,"Player resolution",1,1,1,1);
  renderer.AddText(&myfont,xpos + 100 ,ypos + 150+(2*18) ,"UV Meter mode",1,1,1,1);
  switch(configdefaultplayer_screenmode) {
      case 1: text = "1024 x 768 (720p)";
              break;
      case 2: text = "1280 x 1024 (720p)";
              break;
      case 3: text = "1920 x 1080 (1080p)";
              break;
      case 4: text = "1360 x 768";
              break;
      default:text = "1024 x 768 (720p)";
              break;
  }
  renderer.AddText(&myfont,xpos + 240 ,ypos + 150 + (1*18) ,text,1,1,1,1);

  if (configuvmeter==0) text = "None";
  if (configuvmeter==1) text = "Simple";
  if (configuvmeter==2) text = "Dual";  
  if (configuvmeter>2) text = "None";
  renderer.AddText(&myfont,xpos + 240 ,ypos + 150+(2*18) ,text,1,1,1,1);
}


// ****************************************************************************************
//
// Setup tema config
//
// ****************************************************************************************


void show_setup_tema() {
  char temptxt[80];
  int winsizx=100;
  int winsizy=200;
  int xpos=0;
  int ypos=0;
  std::string tema_string;
  if (tema==1) tema_string="Tema 1";
  if (tema==2) tema_string="Tema 2";
  if (tema==3) tema_string="Tema 3";
  if (tema==4) tema_string="Tema 4";
  if (tema==5) tema_string="Tema 5";
  if (tema==6) tema_string="Tema 6";
  if (tema==7) tema_string="Tema 7";
  if (tema==8) tema_string="Tema 8";
  if (tema==9) tema_string="Tema 9";
  if (tema==10) tema_string="Tema 10";
  renderer.AddTextureRect(42,setupscreenback, 300, 300, 800, 650,1,1,1,1);
  renderer.AddText(&myfont2, 650, 330, tema_string ,1,1,1,1);
  switch(tema) {
    case 1:
          renderer.AddTextureRect(41,screenshot1, 350, 330, 650, 500,1,1,1,1);
          break;
    case 2:
          renderer.AddTextureRect(41,screenshot2, 350, 330, 650, 500,1,1,1,1);
          break;
    case 3:
          renderer.AddTextureRect(41,screenshot3, 350, 330, 650, 500,1,1,1,1);
          break;
    case 4:
          renderer.AddTextureRect(41,screenshot4, 350, 330, 650, 500,1,1,1,1);
          break;
    case 5:
          renderer.AddTextureRect(41,screenshot5, 350, 330, 650, 500,1,1,1,1);
          break;
    case 6:
          renderer.AddTextureRect(41,screenshot6, 350, 330, 650, 500,1,1,1,1);
          break;
    case 7:
          renderer.AddTextureRect(41,screenshot7, 350, 330, 650, 500,1,1,1,1);
          break;
    case 8:
          renderer.AddTextureRect(41,screenshot8, 350, 330, 650, 500,1,1,1,1);
          break;
    case 9:
          renderer.AddTextureRect(41,screenshot9, 350, 330, 650, 500,1,1,1,1);
          break;
    case 10:
          renderer.AddTextureRect(41,screenshot10, 350, 330, 650, 500,1,1,1,1);
          break;
  }
  // close
  renderer.AddTextureRect(42,_textureclose, 600, 840, 188, 81,1,1,1,1);
}



// ****************************************************************************************
//
// Setup font
//
// ****************************************************************************************

void show_setup_font(int aktiv) {
  int i;
  std::string temptxt;
  int visantal;
  static Font SampleFont;
  int startofset=0;
  if (aktiv>18) startofset=(aktiv-18);
  renderer.AddTextureRect(0,setupfontback, 300, 300, 800, 650,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 700, 800, 188, 81,1,1,1,1);
  renderer.AddTextureRect(41,_textureselect, 500, 800, 188, 81,1,1,1,1);
  visantal=aktivfont.mastercount;
  if (visantal>20) visantal=20;
  for (i=0;i<visantal;i++) {
    temptxt=aktivfont.typeinfo[i+startofset].fontname;
    if (i==aktiv) renderer.AddText(&myfont, 350, 430+(i*18), temptxt ,1,1,1,1);
    else renderer.AddText(&myfont, 350, 430+(i*18), temptxt ,0.5f,0.5f,0.5f,1);
  }
  aktivfont.selected_font_nr=aktiv;
  
  SampleFont.Load(aktivfont.typeinfo[aktiv].fontpath.c_str(),14);
  renderer.AddText(&SampleFont, 750, 430, "The quick brown fox jumps over..." ,1,1,1,1);
  renderer.AddText(&SampleFont, 750, 430+(1*18), "abcdefghijklmnopqrstuvxyz" ,1,1,1,1);
  renderer.AddText(&SampleFont, 750, 430+(2*18), "1234567890-.," ,1,1,1,1);
}




// ****************************************************************************************
//
// Network
//
// ****************************************************************************************

void show_wlan_networks(int valgtnr) {
  int i;
  int si;
  char tmptxt[80];
  int winsizx=100;
  int winsizy=200;
  int xpos=0;
  int ypos=0;
  // background
  /*
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupnetworkwlanback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
  glEnd();
  glPopMatrix();
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=400;
  i=0;
  while ((i<wifinets.antal()) && (i<12)) {
    if (i==valgtnr) {
      glPushMatrix();
      glTranslatef(600.0f, 400.0f, 0.0f);
      glRasterPos2f(0.0f, 0.0f+(i*20.0f));
      glColor3f(0.0f, 0.0f, 1.0f);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
    }
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(600.0f, 400.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f+(i*20.0f));
    wifinets.get_networkid(i,tmptxt);        	// get wlan name
    myglprint4((char *) tmptxt);
    glPopMatrix();
    glPushMatrix();
    wifinets.get_networksign(i,&si);        	// get wlan signal styrke
    sprintf(tmptxt,"%d db",si);
    //glRasterPos2f(6.0f, 0.8-(i*0.4f));
    glTranslatef(600.0f, 400.0f, 0.0f);
    glRasterPos2f(250.0f, 0.0f+(i*20.0f));
    myglprint4((char *) tmptxt);
    glPopMatrix();
    winsizx=20;
    winsizy=20;
    xpos=0;
    ypos=0;
    if (wifinets.get_encryptstatus(i)==true) {
      //glLoadIdentity();
      glPushMatrix();
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,_texturelock);
      //glTranslatef(1.2f, 2.4f-(i*0.4f),-14.8f);
      glTranslatef(550.0f, 260.0f+(i*20), 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);
      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
      glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
      glEnd();
      glPopMatrix();
    }
    i++;
  }
  if (wifinets.antal()==0) {
    glDisable(GL_TEXTURE_2D);
    myglprint4((char *) "No wifi networks avable.");
  }
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=430;
  ypos=90;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  */
}

// ****************************************************************************************
//
// network setup window
//
// ****************************************************************************************

void show_setup_network() {
  char tmptxt[1024];
  char id[80];			// wlan id
  int si;				// wlan signal
  int winsizx=100;
  int winsizy=200;
  int xpos=0;
  int ypos=0;
  // background
  renderer.AddTextureRect(0,setupnetworkback, 300, 300, 800, 650,1,1,1,1);
  // close button
  renderer.AddTextureRect(40,_textureclose, 600, 840, 188, 81,1,1,1,1);

  renderer.AddText(&myfont,340, 500,"Hostname",1,1,1,1);
  renderer.AddText(&myfont,340+120, 500,confighostname,1,1,1,1);

  renderer.AddText(&myfont,340, 500+(1*18),"IP adress",1,1,1,1);
  renderer.AddText(&myfont,340+120, 500+(1*18),confighostip,1,1,1,1);

  renderer.AddText(&myfont,340, 500+(2*18),"WLAN",1,1,1,1);
  renderer.AddText(&myfont,340+120, 500+(2*18),confighostwlanname,1,1,1,1);

  renderer.AddText(&myfont,340, 500+(3*18),"Signal",1,1,1,1);
  renderer.AddText(&myfont,340+120, 500+(3*18),confighostwlanname,1,1,1,1);

  renderer.AddText(&myfont,340, 500+(4*18),"Password",1,1,1,1);
  renderer.AddText(&myfont,340+120, 500+(4*18),confighostwlanname,1,1,1,1);

  /*
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupnetworkback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=430;
  ypos=80;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // text input background
  glPushMatrix();
  winsizx=200;
  winsizy=40;
  xpos=450;
  ypos=500;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // text input background
  glPushMatrix();
  winsizx=200;
  winsizy=40;
  xpos=450;
  ypos=450;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f,1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
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
  winsizx=200;
  winsizy=40;
  xpos=450;
  ypos=400;
  // text input background
//    glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f,1.0f, 1.0f);
//    glTranslatef(-3.6f, 2.1f-1.5f,-14.8f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
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
  winsizx=200;
  winsizy=40;
  xpos=450;
  ypos=350;    // text input background wlan essid's password
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f,1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
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
  // Buttons
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f,1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glLoadName(41);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-0.6, -0.2f, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-0.6,  0.2f, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f( 0.8,  0.2f, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f( 0.8, -0.2f, 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glTranslatef(800,650, 0.0f);
  glRasterPos2f(20.0f, 0.0f);
  if (do_show_setup_select_linie==3) glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
  myglprint4((char *) confighostname);
  glPopMatrix();
  glPushMatrix();
  // here start input
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(700,650, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Hostname ");
  glRasterPos2f(120.0f, 0.0f);
  if (do_show_setup_select_linie==0) {
//        glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glColor3f(0.5f,0.5f,0.5f);
      myglprint4((char *) keybuffer);
  } else {
      myglprint4((char *) confighostname);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==0) showcoursornow(450,500,strlen(confighostname));
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(700,600, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "IP adress");
  glRasterPos2f(120.0f, 0.0f);
  if (do_show_setup_select_linie==1) {
    strcpy(keybuffer,confighostip);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
      myglprint4((char *) confighostip);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==1) showcoursornow(450,450,strlen(confighostip));
  glPushMatrix();
  //  show wlan network id
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(700,550, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "WLAN");
  glRasterPos2f(120.0f, 0.0f);
  wifinets.get_networkid(setupwlanselectofset,id);
  if (do_show_setup_select_linie==2) {
    strcpy(keybuffer,confighostwlanname);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) confighostwlanname);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==2) showcoursornow(450,400,strlen(confighostwlanname));
  // get wifi signal db
  glPushMatrix();
  wifinets.get_networksign(setupwlanselectofset,&si);
  sprintf(tmptxt,"%d db",si);
  glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
  glTranslatef(1020,550, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Signal");
  glRasterPos2f(60.0f, 0.0f);
  myglprint4((char *) tmptxt);
  glPopMatrix();
  glPushMatrix();
  //  show wlan network password
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(700,500, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Password");
  wifinets.get_wifipass(setupwlanselectofset,tmptxt);			// get password for aktive wlan
  if (wifinets.antal()>0) strcpy(tmptxt,""); else {
    // no wifi network avable
    strcpy(tmptxt,"No wifi");
    if (do_show_setup_select_linie==3) strcpy(keybuffer,"No wifi");
  }
  glRasterPos2f(120.0f, 0.0f);
  if (do_show_setup_select_linie==3) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) tmptxt);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==3) showcoursornow(450,350,strlen(tmptxt));
  */
}




// ****************************************************************************************
//
// Setup sound config
//
// ****************************************************************************************

void show_setup_sound() {
   renderer.AddTextureRect(0,setupsoundback, 400, 300, 800, 500,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 600, 700, 188, 81,1,1,1,1);
  renderer.AddText(&myfont,500,400+(0*18),"Output device",1,1,1,1);
  renderer.AddText(&myfont,650,400+(0*18),configmythsoundsystem,1,1,1,1);
  if (do_show_setup_select_linie==0) {
    strcpy(keybuffer,configmythsoundsystem);
  }
  renderer.AddText(&myfont,500,400+(1*18),"Output",1,1,1,1);
  renderer.AddText(&myfont,650,400+(1*18),configsoundoutport,1,1,1,1);
  if (do_show_setup_select_linie==1) {
    strcpy(keybuffer,configsoundoutport);
  }
  renderer.AddText(&myfont,500,400+(2*18),"Sound system",1,1,1,1);
  renderer.AddText(&myfont,650,400+(2*18),"FMOD",1,1,1,1);
  if (do_show_setup_select_linie==2) {
    strcpy(keybuffer,"FMOD");
  }
}


// ****************************************************************************************
//
// db mythtv or xbmc ok
// Backend
//
// ****************************************************************************************

void show_setup_sql() {
  char text[200];
  float mythver;
  int winsizx=100;
  int winsizy=200;  
  int xpos=0;
  int ypos=0;
  // background
  renderer.AddTextureRect(0,setupsqlback, 400, 300, 800, 500,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 700, 700, 188, 81,1,1,1,1);
  if (strcmp(configbackend,"mythtv")==0) {
    if (configmythtvver==1264) mythver=0.24f;
    if (configmythtvver) {
      sprintf(text,"Mythtv version   : %2.2f - Database ver : %d",mythver,configmythtvver);
    } else {
      if (global_use_internal_music_loader_system) sprintf(text,"No mythtv backend found. Internal db in use."); else sprintf(text,"Mythtv/Database config error no connection.");
    }
    renderer.AddText(&myfont, 500, 400 ,text,1,1,1,1);
  } else if ((strcmp(configbackend,"xbmc")==0) || (strcmp(configbackend,"kodi")==0)) {
    if (configxbmcver) {
      if (configxbmcver==75) sprintf(text,"XBMC version     : 12 found"); else sprintf(text,"XBMC version     : %d found",configxbmcver);
    } else sprintf(text,"NO XBMC version found");
    renderer.AddText(&myfont, 500, 650+(1*18),text,1,1,1,1);
  }
  
  if (strcmp(configbackend,"mythtv")==0) renderer.AddText(&myfont, 500, 400+(2*18),"backend",1,1,1,1);
  else renderer.AddText(&myfont, 500, 400+(2*18),"buildin",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(2*18),configbackend,1,1,1,1);

  if (do_show_setup_select_linie==0) {
    float strlength=renderer.GetTextWidth(&myfont, configbackend);
    showcoursornow(600+120.0f+strlength, 400+(1*18),0);
  }
  
  if (strcmp(configbackend,"mythtv")==0) renderer.AddText(&myfont, 500, 400+(3*18),"Host",1,1,1,1);
  else renderer.AddText(&myfont, 500, 400+(3*18),"Host",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(3*18),configmysqlhost,1,1,1,1);

  if (do_show_setup_select_linie==1) {
    float strlength=renderer.GetTextWidth(&myfont, configmysqlhost);
    showcoursornow(600+120.0f+strlength, 400+(2*18),0);
  }
  
  if (strcmp(configbackend,"mythtv")==0) renderer.AddText(&myfont, 500, 400+(4*18),"User",1,1,1,1);
  else renderer.AddText(&myfont, 500.0f, 400+(4*18),"User",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(4*18),configmysqluser,1,1,1,1);

  if (do_show_setup_select_linie==2) {
    float strlength=renderer.GetTextWidth(&myfont, configmysqluser);
    showcoursornow(600+120.0f+strlength, 400+(3*18),0);
  }

  if (strcmp(configbackend,"mythtv")==0) renderer.AddText(&myfont, 500, 400+(5*18),"Password",1,1,1,1);
  else renderer.AddText(&myfont, 500.0f+120.0f, 400+(5*18),"Password",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(5*18),configmysqlpass,1,1,1,1);
  
  if (do_show_setup_select_linie==3) {
    float strlength=renderer.GetTextWidth(&myfont, configmysqlpass);
    showcoursornow(600+120.0f+strlength, 400+(4*18),0);
  }
  
  renderer.AddText(&myfont, 500.0f, 400+(6*18),"Music path",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(6*18),configmusicpath,1,1,1,1);

  if (do_show_setup_select_linie==4) {
    float strlength=renderer.GetTextWidth(&myfont, configmusicpath);
    showcoursornow(600+120.0f+strlength, 400+(5*18),0);
  }

  
  renderer.AddText(&myfont, 500.0f, 400+(7*18),"Movie path",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(7*18),configmoviepath,1,1,1,1);  
  
  if (do_show_setup_select_linie==5) {
    float strlength=renderer.GetTextWidth(&myfont, configmoviepath);
    showcoursornow(600+120.0f+strlength, 400+(6*18),0);
  }

  renderer.AddText(&myfont, 500.0f, 400+(8*18),"Picture path",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(8*18),configpicturepath,1,1,1,1);
  
  if (do_show_setup_select_linie==6) {
    float strlength=renderer.GetTextWidth(&myfont, configpicturepath);
    showcoursornow(600+120.0f+strlength, 400+(7*18),0);
  }

  renderer.AddText(&myfont, 500.0f, 400+(9*18),"Record path",1,1,1,1);
  renderer.AddText(&myfont, 600.0f+120.0f, 400+(9*18),configrecordpath,1,1,1,1);
  
  if (do_show_setup_select_linie==7) {
    float strlength=renderer.GetTextWidth(&myfont, configrecordpath);
    showcoursornow(600+120.0f+strlength, 400+(8*18),0);
  }
}



// ****************************************************************************************
//
// select file for functions keys setup
//
// ****************************************************************************************

void select_exe_functions_keys_name() {
  char filenamepath[16380];
  strcpy(filenamepath,"");
  std::string filenamepath1;
  std::string filename="";
  std::string tmp;
  std::string dest_file="";
  std::string realpath="";
  std::ofstream torrentfile;
  FILE *f = popen("/usr/bin/zenity --file-selection --file-filter=*.* --modal --title=\"Select torrent file.\" 2> /dev/null", "r");
  fgets(filenamepath, 16380, f);
  if (!(f)) {
    return;
  }  
  fclose(f);
  if (strlen(filenamepath)>0) {
    filename = filenamepath;
    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.cend());
    switch (do_show_setup_select_linie) {
      case 0: strcpy(configkeyslayout[0].cmdname, filename.c_str());
              break;
      case 2: strcpy(configkeyslayout[1].cmdname, filename.c_str());
              break;
      case 4: strcpy(configkeyslayout[2].cmdname, filename.c_str());
              break;
      case 6: strcpy(configkeyslayout[3].cmdname, filename.c_str());
              break;
      case 8: strcpy(configkeyslayout[4].cmdname, filename.c_str());
              break;
      case 10: strcpy(configkeyslayout[5].cmdname, filename.c_str());
              break;
      case 12: strcpy(configkeyslayout[6].cmdname, filename.c_str());
              break;
      case 14: strcpy(configkeyslayout[7].cmdname, filename.c_str());
              break;
      case 16: strcpy(configkeyslayout[8].cmdname, filename.c_str());
              break;
      case 18: strcpy(configkeyslayout[9].cmdname, filename.c_str());
              break;
      case 20: strcpy(configkeyslayout[10].cmdname, filename.c_str());
              break;
    }
  }
}




// ****************************************************************************************
//
// Setup keys
//
// ****************************************************************************************


void show_setup_keys() {
  std::string tmptxt;
  std::string fkeysname[]={"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12"};
  static Font SampleFont;
  // Load font.
  SampleFont.Load("/usr/share/fonts/truetype/ubuntu/UbuntuMono[wght].ttf",18);
  renderer.AddTextureRect(0,setupkeysback, 400, 300, 800, 630,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 670, 800, 188, 81,1,1,1,1);
  renderer.AddText(&myfont,520, 450,"Command to execute.                                                   ScrNr",1,1,1,1);
  for(int i=0;i<12;i++) {
    tmptxt=fmt::format("{:>7}  - {:<70} ",fkeysname[i], configkeyslayout[i].cmdname);
    renderer.AddTextureRect(0,setupkeysbar1, 474+60, 468+((i)*22), 23*18, 20 ,1,1,1,1);
    renderer.AddText(&myfont,478, 465+((i+1)*22),tmptxt,1,1,1,1);
    tmptxt=fmt::format("{:<4}",configkeyslayout[i].scrnr);
    renderer.AddText(&myfont,508+(26*18), 465+((i+1)*22),tmptxt,1,1,1,1);
  }
  switch (do_show_setup_select_linie) {
    case 0: showcoursornow(578,465+(0*22),strlen(keybuffer));
            break;
    case 1: showcoursornow(578,465+(1*22),strlen(keybuffer));
            break;
    case 2: showcoursornow(578,465+(2*22),strlen(keybuffer));
            break;
    case 3: showcoursornow(578,465+(3*22),strlen(keybuffer));
            break;
    case 4: showcoursornow(578,465+(4*22),strlen(keybuffer));
            break;
    case 5: showcoursornow(578,465+(5*22),strlen(keybuffer));
            break;
    case 6: showcoursornow(578,465+(6*22),strlen(keybuffer));
            break;
    case 7: showcoursornow(578,465+(7*22),strlen(keybuffer));
            break;
    case 8: showcoursornow(578,465+(8*22),strlen(keybuffer));
            break;
    case 9: showcoursornow(578,465+(9*22),strlen(keybuffer));
            break;
    case 10: showcoursornow(578,465+(10*22),strlen(keybuffer));
            break;
    case 11: showcoursornow(578,465+(11*22),strlen(keybuffer));
            break;
    case 12: showcoursornow(578,465+(12*22),strlen(keybuffer));
            break;
  }
  
  /*  
  // start af input felter
  // text input background F3
  glPushMatrix();
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=500;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=450;
  // text input background F4
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F5
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=400;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F6
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=350;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F7
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=300;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=250;
  // text input background F8
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F9
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=200;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F10
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=150;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F11
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=100;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // text input background F12
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=150;
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
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
  /*
  glPushMatrix();
  xpos=810;
  ypos=100;
  winsizx=50;
  winsizy=30;
  
  for(int i=0;i<9;i++) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,setupkeysbar2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+(i*50)+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+(i*50)+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+(i*50)+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
    glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+(i*50)+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
  }
  glPopMatrix();
  
  
  
  // line 0
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(680 , 650 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);

  std::string tmpstr=configkeyslayout[0].cmdname;

  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  if ((onlyfname.length() > 0) && (!(fs::path(tmpstr).empty()))) onlyfname = "/.../" + onlyfname;
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==0) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==0) showcoursornow(311,500,strlen(keybuffer));
  // line 0
  glPushMatrix();
  glTranslatef(1180 , 650 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==1) {
    sprintf(keybuffer,"%d",configkeyslayout[0].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[0].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==1) showcoursornow(812,500,strlen(keybuffer));


  // line 1
  glPushMatrix();
  glTranslatef(680 , 600 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);

  tmpstr=configkeyslayout[1].cmdname;

  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  if ((onlyfname.length() > 0) && (!(fs::path(tmpstr).empty()))) onlyfname = "/.../" + onlyfname;
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==2) {
    if (strlen(configkeyslayout[1].cmdname) > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    if (strlen(configkeyslayout[1].cmdname) > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==2) showcoursornow(311,450,strlen(keybuffer));
  // line 1
  glPushMatrix();
  glTranslatef(1180 , 600 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==3) {
    sprintf(keybuffer,"%d",configkeyslayout[1].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[1].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==3) showcoursornow(812,450,strlen(keybuffer));
  // line 2
  glPushMatrix();
  glTranslatef(680 , 550 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);

  tmpstr=configkeyslayout[2].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  if ((onlyfname.length() > 0) && (!(fs::path(tmpstr).empty()))) onlyfname = "/.../" + onlyfname;
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==4) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==4) showcoursornow(311,400,strlen(keybuffer));
  // line 2
  glPushMatrix();
  glTranslatef(1180 , 550 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==5) {
    sprintf(keybuffer,"%d",configkeyslayout[2].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[2].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==5) showcoursornow(812,400,strlen(keybuffer));
  // line 3
  glPushMatrix();
  glTranslatef(680 , 500 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[3].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  if (fs::path(tmpstr).has_parent_path()) onlyfname = "/.../" + onlyfname;
  strcpy(keybuffer, onlyfname.c_str());
  keybufferindex=onlyfname.length();
  if (do_show_setup_select_linie==6) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==6) showcoursornow(311,350,strlen(keybuffer));
  // line 3
  glPushMatrix();
  glTranslatef(1180 , 500 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==7) {
    sprintf(keybuffer,"%d",configkeyslayout[3].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[3].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==7) showcoursornow(812,350,strlen(keybuffer));
  // line 4
  glPushMatrix();
  glTranslatef(680 , 450 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[4].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  onlypname=fs::path(tmpstr).parent_path();      // return path without filename.
  if ((onlyfname.length() > 0) && (!onlypname.empty())) onlyfname = "/.../" + onlyfname;
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==8) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==8) showcoursornow(311,300,strlen(keybuffer));
  // line 4
  glPushMatrix();
  glTranslatef(1180 , 450 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==9) {
    sprintf(keybuffer,"%d",configkeyslayout[4].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[4].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==9) showcoursornow(812,300,strlen(keybuffer));
  // line 5
  glPushMatrix();
  glTranslatef(680 , 400 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[5].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  onlypname=fs::path(tmpstr).parent_path();      // return path without filename.
  if ((onlyfname.length() > 0) && (!onlypname.empty())) onlyfname = "/.../" + onlyfname;  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==10) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==10) showcoursornow(311,250,strlen(keybuffer));
  // line 5
  glPushMatrix();
  glTranslatef(1180 , 400 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==11) {
    sprintf(keybuffer,"%d",configkeyslayout[5].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[5].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==11) showcoursornow(812,250,strlen(keybuffer));
  // line 6
  glPushMatrix();
  glTranslatef(680 , 350 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[6].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  onlypname=fs::path(tmpstr).parent_path();      // return path without filename.
  if ((onlyfname.length() > 0) && (!onlypname.empty())) onlyfname = "/.../" + onlyfname;  strcpy(keybuffer, onlyfname.c_str());
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==12) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==12) showcoursornow(311,200,strlen(keybuffer));
  // line 6
  glPushMatrix();
  glTranslatef(1180 , 350 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==13) {
    sprintf(keybuffer,"%d",configkeyslayout[6].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[6].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==13) showcoursornow(812,200,strlen(keybuffer));
  // line 7
  glPushMatrix();
  glTranslatef(680 , 300 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[7].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  onlypname=fs::path(tmpstr).parent_path();      // return path without filename.
  if ((onlyfname.length() > 0) && (!onlypname.empty())) onlyfname = "/.../" + onlyfname;  strcpy(keybuffer, onlyfname.c_str());
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==14) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==14) showcoursornow(311,150,strlen(keybuffer));
  // line 7
  glPushMatrix();
  glTranslatef(1180 , 300 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==15) {
    sprintf(keybuffer,"%d",configkeyslayout[7].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[7].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==15) showcoursornow(812,150,strlen(keybuffer));
  // line 8
  glPushMatrix();
  glTranslatef(680 , 250 , 0.0f);
  glRasterPos2f(0.8f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  tmpstr=configkeyslayout[8].cmdname;
  onlyfname=fs::path(tmpstr).filename();          // return filename with extension.
  onlypname=fs::path(tmpstr).parent_path();      // return path without filename.
  if ((onlyfname.length() > 0) && (!onlypname.empty())) onlyfname = "/.../" + onlyfname;  strcpy(keybuffer, onlyfname.c_str());
  strcpy(keybuffer, onlyfname.c_str());
  if (do_show_setup_select_linie==16) {
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    if (onlyfname.length() > 0) myglprint4((char *) onlyfname.c_str());
  }
  glPopMatrix();
  if (do_show_setup_select_linie==16) showcoursornow(311,100,strlen(keybuffer));
  // line 9
  glPushMatrix();
  glTranslatef(1180 , 250 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==17) {
    sprintf(keybuffer,"%d",configkeyslayout[8].scrnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    sprintf(text,"%d",configkeyslayout[8].scrnr);
    myglprint4((char *) text);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==17) {
    sprintf(text,"%d",configkeyslayout[8].scrnr);
    showcoursornow(812,100,strlen(text));
  }
  */
}

// ****************************************************************************************
//
// show rss list
//
// ****************************************************************************************


void showrss_list() {
  printf("\n");
  for(int t=0;t<99;t++) {
    if (strcmp(rssstreamoversigt.get_stream_name_std(t).c_str(),"")!=0) printf("nr %d feedname %s url %s \n",t,rssstreamoversigt.get_stream_name_std(t).c_str(),rssstreamoversigt.get_stream_url_std(t).c_str());
  }
  printf("\n");
}



// ****************************************************************************************
//
// setup rss
//
// ****************************************************************************************
void show_setup_rss(unsigned int startofset) {
  int winsizx=1300;
  int winsizy=975;
  int xpos=0;
  int ypos=0;
  char text[200];
  char temptxt[200];
  int n;
  std::string showtxt;
  float r_color,g_color,b_color;
  renderer.AddTextureRect(0,setuprssback, 300, 100, 1300, 850,1,1,1,1);
  // close
  renderer.AddTextureRect(40,_textureclose, 670, 800, 188, 81,1,1,1,1);
  for (int n=0;n<19;n++) {
    if (n+startofset==rssstreamoversigt.setup_select_linie/2+startofset) {
      r_color=1.0;
      g_color=1.0;
      b_color=1.0;
    } else {
      r_color=0.7f;
      g_color=0.7f;
      b_color=0.7f;
    }
    showtxt=fmt::format("{:3} ",n+startofset);
    renderer.AddText(&myfont,450 ,350+(n*18) ,showtxt,r_color,g_color,b_color,1);
    if ((startofset+n)<=rssstreamoversigt.streamantal()) {
      showtxt=rssstreamoversigt.get_stream_name_std(startofset+n);
    } else {
      showtxt=" BLANK";
    }
    renderer.AddText(&myfont,510 ,350+(n*18) ,showtxt,r_color,g_color,b_color,1);
    if ((startofset+n)<=rssstreamoversigt.streamantal()) {
      showtxt=rssstreamoversigt.get_stream_url_std(startofset+n);
    } else {
      showtxt=" BLANK";    
    }
    renderer.AddText(&myfont,850 ,350+(n*18) ,showtxt,r_color,g_color,b_color,1);
  }

  // std::cout << "setupline " << rssstreamoversigt.setup_select_linie << " setupline / 2 = " << (int) rssstreamoversigt.setup_select_linie / 2  <<  "\n";

  float ll=renderer.GetTextWidth(&myfont, rssstreamoversigt.get_stream_name_std(rssstreamoversigt.setup_select_linie /2));
  float ll2=renderer.GetTextWidth(&myfont, rssstreamoversigt.get_stream_url_std(rssstreamoversigt.setup_select_linie /2));

  switch(rssstreamoversigt.setup_select_linie) {
    case 0: showcoursornow(510+ll,330+(0*18),0);
            break;
    case 1: showcoursornow(850+ll2,330+(0*18),0);
            break;
    case 2: showcoursornow(510+ll,330+(1*18),0);
            break;
    case 3: showcoursornow(850+ll2,330+(1*18),0);
            break;    
    case 4: showcoursornow(510+ll,330+(2*18),0);
            break;
    case 5: showcoursornow(850+ll2,330+(2*18),0);
            break;
    case 6: showcoursornow(510+ll,330+(3*18),0);
            break;
    case 7: showcoursornow(850+ll2,330+(3*18),0);
            break;
    case 8: showcoursornow(510+ll,330+(4*18),0);
            break;
    case 9: showcoursornow(850+ll2,330+(4*18),0);
            break;
    case 10:showcoursornow(510+ll,330+(5*18),0);
            break;
    case 11:showcoursornow(850+ll2,330+(5*18),0);
            break;
    case 12:showcoursornow(510+ll,330+(6*18),0);
            break;
    case 13:showcoursornow(850+ll2,330+(6*18),0);
            break;
    case 14:showcoursornow(510+ll,330+(7*18),0);
            break;
    case 15:showcoursornow(850+ll2,330+(7*18),0);
            break;
    case 16:showcoursornow(510+ll,330+(8*18),0);
            break;
    case 17:showcoursornow(850+ll2,330+(8*18),0);
            break;
    case 18:showcoursornow(510+ll,330+(9*18),0);
            break;
    case 19:showcoursornow(850+ll2,330+(9*18),0);
            break;
    case 20:showcoursornow(510+ll,330+(10*18),0);
            break;
    case 21:showcoursornow(850+ll2,330+(10*18),0);
            break;
    case 22:showcoursornow(510+ll,330+(11*18),0);
            break;
    case 23:showcoursornow(850+ll2,330+(11*18),0);
            break;
    case 24:showcoursornow(510+ll,330+(12*18),0);
            break;
    case 25:showcoursornow(850+ll2,330+(12*18),0);
            break;
    case 26:showcoursornow(510+ll,330+(13*18),0);
            break;
    case 27:showcoursornow(850+ll2,330+(13*18),0);
            break;
    case 28:showcoursornow(510+ll,330+(14*18),0);
            break;
    case 29:showcoursornow(850+ll2,330+(14*18),0);
            break;
    case 30:showcoursornow(510+ll,330+(15*18),0);
            break;
    case 31:showcoursornow(850+ll2,330+(15*18),0);
            break;
    case 32:showcoursornow(510+ll,330+(16*18),0);
            break;
    case 33:showcoursornow(850+ll2,330+(16*18),0);
            break;
    case 34:showcoursornow(510+ll,330+(17*18),0);
            break;
    case 35:showcoursornow(850+ll2,330+(17*18),0);
            break;
  }

  /*  
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setuprssback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // background
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( 200,100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( 200,975 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( 200+1300,975 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( 200+1300,100 , 0.0);
  glEnd();
  glPopMatrix();
  // close buttons
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=400;
  ypos=-10;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // overskrift
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(260, 680, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) " NR RSS Feed Name.");
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(684, 680, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Podcast URL");
  glPopMatrix();

  // printf("do_show_setup_select_linie=%d startofset=%d streamantal=%d select name %s \n",(rssstreamoversigt.setup_select_linie/2)+startofset,startofset,rssstreamoversigt.streamantal(),rssstreamoversigt.get_stream_name_std(rssstreamoversigt.setup_select_linie/2+startofset).c_str());

  for (int n=0;n<19;n++) {
    glPushMatrix();
    if (n+startofset==rssstreamoversigt.setup_select_linie/2+startofset) glColor3f(1.0f,1.0f,0.0f); else glColor3f(.7f,0.7f,0.7f);
    glTranslatef(260 , 660-(n*20) , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    showtxt=fmt::format("{:3} ",n+startofset);
    myglprint4((char *) showtxt.c_str());
    glPopMatrix();

    glPushMatrix();
    glTranslatef(300 , 660-(n*20) , 0.0f);
    glRasterPos2f(0.0f, 0.0f);   
    if ((startofset+n)<=rssstreamoversigt.streamantal()) {
      showtxt=rssstreamoversigt.get_stream_name_std(startofset+n);
    } else {
      showtxt=" BLANK";
    }
    myglprint4((char *) showtxt.c_str());
    if ((startofset+n)<=rssstreamoversigt.streamantal()) {
      showtxt=rssstreamoversigt.get_stream_url_std(startofset+n);
    } else {
      showtxt="BLANK";
    }
    glRasterPos2f(392.0f, 0.0f);
    myglprint4((char *) showtxt.c_str());
    glPopMatrix();
  }
  switch(rssstreamoversigt.setup_select_linie) {
      case 0: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(0*20),strlen(rssstreamoversigt.get_stream_name_std(0+startofset).c_str()));
              else showcoursornow(-70,510-(0*20),0);
              break;
      case 1: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(0*20),strlen(rssstreamoversigt.get_stream_url_std(0+startofset).c_str()));
              else showcoursornow(320,510-(0*20),0);
              break;
      case 2: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+1)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(1*20),strlen(rssstreamoversigt.get_stream_name_std(1+startofset).c_str()));
              else showcoursornow(-70,510-(1*20),0);
              break;
      case 3: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+1)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(1*20),strlen(rssstreamoversigt.get_stream_url_std(1+startofset).c_str()));
              else showcoursornow(320,510-(1*20),0);
              break;
      case 4: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+2)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(2*20),strlen(rssstreamoversigt.get_stream_name_std(2+startofset).c_str()));
              else showcoursornow(-70,510-(2*20),0);
              break;
      case 5: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+2)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(2*20),strlen(rssstreamoversigt.get_stream_url_std(2+startofset).c_str()));
              else showcoursornow(320,510-(2*20),0);
              break;
      case 6: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+3)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(3*20),strlen(rssstreamoversigt.get_stream_name_std(3+startofset).c_str()));
              else showcoursornow(-70,510-(3*20),0);
              break;
      case 7: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+3)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(3*20),strlen(rssstreamoversigt.get_stream_url_std(3+startofset).c_str()));
              else showcoursornow(320,510-(3*20),0);
              break;
      case 8: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+4)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(4*20),strlen(rssstreamoversigt.get_stream_name_std(4+startofset).c_str()));
              else showcoursornow(-70,510-(4*20),0);
              break;
      case 9: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+4)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(4*20),strlen(rssstreamoversigt.get_stream_url_std(4+startofset).c_str()));
              else showcoursornow(320,510-(4*20),0);
              break;
      case 10:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+5)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(5*20),strlen(rssstreamoversigt.get_stream_name_std(5+startofset).c_str()));
              else showcoursornow(-70,510-(5*20),0);
              break;
      case 11:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+5)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(5*20),strlen(rssstreamoversigt.get_stream_url_std(5+startofset).c_str()));
              else showcoursornow(320,510-(5*20),0);
              break;
      case 12:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+6)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(6*20),strlen(rssstreamoversigt.get_stream_name_std(6+startofset).c_str()));
              else showcoursornow(-70,510-(6*20),0);
              break;
      case 13:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+6)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(6*20),strlen(rssstreamoversigt.get_stream_url_std(6+startofset).c_str()));
              else showcoursornow(320,510-(6*20),0);
              break;
      case 14:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+7)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(7*20),strlen(rssstreamoversigt.get_stream_name_std(7+startofset).c_str()));
              else showcoursornow(-70,510-(7*20),0);
              break;
      case 15:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+7)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(7*20),strlen(rssstreamoversigt.get_stream_url_std(7+startofset).c_str()));
              else showcoursornow(320,510-(7*20),0);
              break;
      case 16:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+8)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(8*20),strlen(rssstreamoversigt.get_stream_name_std(8+startofset).c_str()));
              else showcoursornow(-70,510-(8*20),0);
              break;
      case 17:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+8)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(8*20),strlen(rssstreamoversigt.get_stream_url_std(8+startofset).c_str()));
              else showcoursornow(320,510-(8*20),0);
              break;
      case 18: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+9)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(9*20),strlen(rssstreamoversigt.get_stream_name_std(9+startofset).c_str()));
              else showcoursornow(-70,510-(9*20),0);
              break;
      case 19: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+9)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(9*20),strlen(rssstreamoversigt.get_stream_url_std(9+startofset).c_str()));
              else showcoursornow(320,510-(9*20),0);
              break;
      case 20: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+10)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(10*20),strlen(rssstreamoversigt.get_stream_name_std(10+startofset).c_str()));
              else showcoursornow(-70,510-(10*20),0);
              break;
      case 21: glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+10)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(10*20),strlen(rssstreamoversigt.get_stream_url_std(10+startofset).c_str()));
              else showcoursornow(320,510-(10*20),0);
              break;
      case 22:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+11)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(11*20),strlen(rssstreamoversigt.get_stream_name_std(11+startofset).c_str()));
              else showcoursornow(-70,510-(11*20),0);
              break;
      case 23:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+11)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(11*20),strlen(rssstreamoversigt.get_stream_url_std(11+startofset).c_str()));
              else showcoursornow(320,510-(11*20),0);
              break;
      case 24:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+12)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(12*20),strlen(rssstreamoversigt.get_stream_name_std(12+startofset).c_str()));
              else showcoursornow(-70,510-(12*20),0);
              break;
      case 25:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+12)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(12*20),strlen(rssstreamoversigt.get_stream_url_std(12+startofset).c_str()));
              else showcoursornow(320,510-(12*20),0);
              break;
      case 26:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+13)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(13*20),strlen(rssstreamoversigt.get_stream_name_std(13+startofset).c_str()));
              else showcoursornow(-70,510-(13*20),0);
              break;
      case 27:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+13)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(13*20),strlen(rssstreamoversigt.get_stream_url_std(13+startofset).c_str()));
              else showcoursornow(320,510-(13*20),0);
              break;
      case 28:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+14)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(14*20),strlen(rssstreamoversigt.get_stream_name_std(14+startofset).c_str()));
              else showcoursornow(-70,510-(14*20),0);
              break;
      case 29:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+14)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(14*20),strlen(rssstreamoversigt.get_stream_url_std(14+startofset).c_str()));
              else showcoursornow(320,510-(14*20),0);
              break;
      case 30:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+14)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(15*20),strlen(rssstreamoversigt.get_stream_name_std(15+startofset).c_str()));
              else showcoursornow(-70,510-(15*20),0);
              break;
      case 31:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+15)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(15*20),strlen(rssstreamoversigt.get_stream_url_std(15+startofset).c_str()));
              else showcoursornow(320,510-(15*20),0);
              break;
      case 32:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+16)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(16*20),strlen(rssstreamoversigt.get_stream_name_std(16+startofset).c_str()));
              else showcoursornow(-70,510-(16*20),0);
              break;
      case 33:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+16)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(16*20),strlen(rssstreamoversigt.get_stream_url_std(16+startofset).c_str()));
              else showcoursornow(320,510-(16*20),0);
              break;
      case 34:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+17)<=rssstreamoversigt.streamantal()) showcoursornow(-70,510-(17*20),strlen(rssstreamoversigt.get_stream_name_std(17+startofset).c_str()));
              else showcoursornow(-70,510-(17*20),0);
              break;
      case 35:glColor3f(1.0f,1.0f,1.0f);
              if ((startofset+17)<=rssstreamoversigt.streamantal()) showcoursornow(320,510-(17*20),strlen(rssstreamoversigt.get_stream_url_std(17+startofset).c_str()));
              else showcoursornow(320,510-(17*20),0);
              break;
  }
  */
}


// ****************************************************************************************
// Not in use
// call tv_graber create defaut config and do auto config if posible
// will try to make list of all channels from tv_graber
// by pipe the command in shell
//
// ****************************************************************************************

int txmltvgraber_createconfig() {
  char path[1024];
  char exebuffer[1024];
  int sysresult;
  // delete old config from dir
  strcpy(exebuffer,"rm ");
  //getuserhomedir(path);
  strcpy(path,localuserhomedir);

  strcat(path,"/.xmltv/");
  strcat(path,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  strcat(exebuffer,path);
  strcat(exebuffer,".conf");
  //sysresult=system(exebuffer);
  // delete old config from dir
  sprintf(debuglogdata,"Graber in use: %d",aktiv_tv_graber.graberaktivnr);
  write_logfile(logfile,(char *) debuglogdata);


  if ((aktiv_tv_graber.graberaktivnr>0) && (aktiv_tv_graber.graberaktivnr<aktiv_tv_graber.graberantal)) {
    // create new config
    // make kommand do do in shell first
    switch (aktiv_tv_graber.graberaktivnr) {
              // create new config for tv_grab_na_dd
      case 0: sprintf(exebuffer,"echo -e '0\n\n\all\n' |");
              break;
      case 1: sprintf(exebuffer,"'\n\nall\n' |");
              break;
              // create new config for tv_grab_nl
      case 2: sprintf(exebuffer,"'all\n' |");
              break;
              // create new config for tv_grab_es_laguiatv
      case 3: sprintf(exebuffer,"'\n\nall\nno\nall\n' |");
              break;
              // create new config for tv_grab_il
      case 4: sprintf(exebuffer,"'all\n' |");
              break;
              // create new config for tv_grab_na_tvmedia
              // not work now
      case 5: sprintf(exebuffer,"'all\n' |");
              break;
              // create new config for tv_grab_dtv_la
              // not work
      case 6: sprintf(exebuffer,"'all\n' |");
              break;
              // create new config for tv_grab_fi
      case 7: sprintf(exebuffer,"'all\n' |");
              break;
              /*
              0: Austria
              1: Croatia
              2: Denmark
              3: Estonia
              4: Finland
              5: Germany
              6: Hungary
              7: Latvia
              8: Lithuania
              9: Netherlands
              10: Norway
              11: Poland
              12: Sweden
              13: Switzerland
              14: All Radio stations
              15: All TV Channels
              */
              // create new config for tv_grab_eu_dotmedia
      case 8: sprintf(exebuffer,"echo -e '2\n\nall\n' |");
              write_logfile(logfile,(char *) "XMLTv update from denmark");
              break;
              // create new config for tv_grab_se_swedb
      case 9: sprintf(exebuffer,"echo -e '\n\nall\n' |");
              write_logfile(logfile,(char *) "XMLTv update from Sweden");
              break;
              // create new config for tv_grab_fr
              // take some time to finish
      case 10: sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_uk_bleb
      case 11:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_huro
      case 12:sprintf(exebuffer,"echo -e '1\nall\n' |");
              break;
              // create new config for tv_grab_ch_search
      case 13:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_it
      case 14:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_is
      case 15:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_fi_sv
      case 16:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_na_dtv
              // do not work for now
              // need more work on time zones
      case 17:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_tr
      case 18:sprintf(exebuffer,"echo -e '\nall\n' |");
              break;
              // create new config for tv_grab_eu_egon
      case 19:sprintf(exebuffer,"echo -e '\n\nall\n' |");
              break;
              // create new config for tv_grab_dk_dr
      case 20:sprintf(exebuffer,"echo -e '1\n0\n\nyes\nall\n' |");
              break;
              // create new config for tv_grab_se_tvzon
      case 21:sprintf(exebuffer,"echo -e '\n\nall\n' |");
              break;
              // create new config for tv_grab_ar
              //
      case 22:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_fr_kazer
              // do not work
      case 23:sprintf(exebuffer,"echo -e 'all\n' |");
              break;
              // create new config for tv_grab_uk_tvguide
      case 24:sprintf(exebuffer,"echo -e '\nall\n' |");
              break;
      case 25:// tv_grab_zz_sdjson
              exebuffer[0]='\0';
              break;
      default: sprintf(exebuffer,"echo -e '\nall\n' |");
              break;
    }
    // get grabercmd command
    // add configure paramters to tv_grab_*
    strcat(exebuffer,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
    strcat(exebuffer, " --configure");
    // do not work
    //printf("xml config command : %s \n",exebuffer);
    //sysresult=system(exebuffer);
    switch(aktiv_tv_graber.graberaktivnr) {
      case 8: //sysresult=system("cp /opt/mythtv-controller/xmltv_config/tv_grab_eu_dotmedia.conf ~/.xmltv/");
              write_logfile(logfile,(char *) "cp /opt/mythtv-controller/xmltv_config/tv_grab_eu_dotmedia.conf ~/.xmltv/ (disabled)");
              break;
      default:
              //sysresult=system(exebuffer);
              //write_logfile("cp /opt/mythtv-controller/xmltv_config/tv_grab_eu_dotmedia.conf ~/.xmltv/");
              //printf("cp /opt/mythtv-controller/xmltv_config/tv_grab_eu_dotmedia.conf ~/.xmltv/");
              break;

    }
    // sysresult = -1 if error else command return value
    return(1);
  } else return(0);
}


// ****************************************************************************************
//
// create/update from old config file to new xmltv grabber config file. from tvgude config in array channel_list
//
// ****************************************************************************************

int txmltvgraber_updateconfigfile() {
  char path[1024];
  char buffer[1024];
  char filename[1024];
  char outfilename[1024];
  FILE *filin;
  FILE *filout;
  int sysresult;
  int cnr;
  // source file
  //getuserhomedir(filename);                                                 // get user homedir
  strcpy(filename,localuserhomedir);

  strcat(filename,"/.xmltv/");
  strcat(filename,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]); // grabber name
  strcat(filename,".conf");
  // create new config file
  // dest file
  //getuserhomedir(outfilename);                                                 // get user homedir
  strcpy(outfilename,localuserhomedir);

  strcat(outfilename,"/tmp_");
  strcat(outfilename,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]); // grabber name
  strcat(outfilename,".conf");
  filin=fopen(filename,"r");
  filout=fopen(outfilename,"w");
  // copy header from in file to out file
  if ((filout) && (filin)) {
    switch (aktiv_tv_graber.graberaktivnr) {
      case 8: fgets(buffer,512,filin);
              fputs(buffer,filout);
              fgets(buffer,512,filin);
              fputs(buffer,filout);
              break;
      case 9: fgets(buffer,512,filin);
              fputs(buffer,filout);
              fgets(buffer,512,filin);
              fputs(buffer,filout);
              break;
      default:
              break;
    }
    cnr=0;

    while((cnr<PRGLIST_ANTAL) && (strcmp(channel_list.channel_list[cnr].id,"")!=0)) {
      strcpy(buffer,"");
      switch (aktiv_tv_graber.graberaktivnr) {
        case 8: if (channel_list.channel_list[cnr].selected) {
                  strcpy(buffer,"channel=");
                } else {
                  strcpy(buffer,"channel!");
                }
                strcat(buffer,channel_list.channel_list[cnr].id);
                fputs(buffer,filout);
                fputs("\n",filout);
                break;
        defaut: if (channel_list.channel_list[cnr].selected) {
                  strcpy(buffer,"channel=");
                } else {
                  strcpy(buffer,"channel!");
                }
                strcat(buffer,channel_list.channel_list[cnr].id);
                fputs(buffer,filout);
                fputs("\n",filout);
                break;
      }
      cnr++;
    }
  }
  fclose(filin);
  fclose(filout);
  // copy new config file to xmltv homedir ~/.xmltv
  //getuserhomedir(path);                                                     // get user homedir
  strcpy(path,localuserhomedir);
  strcpy(filename,"cp ");
  strcat(filename,path);
  strcat(filename,"/tmp_");
  strcat(filename,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  strcat(filename,".conf");
  strcat(filename," ~/.xmltv/");
  strcat(filename,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  strcat(filename,".conf");
  //sysresult=system(filename);
  return(1);
}


channel_list_type::channel_list_type() {
  channel_list.reserve(10);
}


// ****************************************************************************************
//
// constructor for channel_list array
//
// ****************************************************************************************

channel_configfile::channel_configfile() {
  /*
  for(int n=0;n<MAXCHANNEL_ANTAL-1;n++) {
    channel_list[n].selected=false;                                             // is program channel active (default)
    channel_list[n].ordernr=0;                                                  // show ordernr
    channel_list[n].changeordernr=false;                                        // used change ordernr in cobfig setup screen
    strcpy(channel_list[n].name,"");                                            // channel name
    strcpy(channel_list[n].id,"");                                              // internal dbid
  }
  */
}


channel_configfile::~channel_configfile() {

}

// ****************************************************************************************
// NOT in use
// Read xmltv config file
//
// ****************************************************************************************

int channel_configfile::readgraber_configfile() {
  char path[1024];
  char buffer[1024];
  char filename[1024];
  bool errors=false;
  FILE *fil;
  int line=0;
  //getuserhomedir(path);
  strcpy(path,localuserhomedir);
  strcat(path,"/.xmltv/");
  strcat(path,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  strcat(path,".conf");
  strcpy(filename,path);
  fil=fopen(filename,"r");
  if (fil) {
    while(!(feof(fil))) {
      if (line<maxconfigfilesize) {
        fgets(buffer,512,fil);
        strcpy(configtext[line],buffer);
        line++;
      }
    }
    if (line>0) configfilesize=line-1;
    fclose(fil);
  } else errors=true;
  if (errors==false) return(1); else return(0);
}





//
// write xmltv config file and save it
//
/*
int channel_configfile::writegraber_configfile() {
  char path[1024];
  char buffer[1024];
  char filename[1024];
  bool errors=false;
  FILE *fil;
  int line=0;
  getuserhomedir(path);
  strcat(path,"/.xmltv/");
  strcat(path,aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  strcat(path,".conf");
  strcpy(filename,path);
  fil=fopen(filename,"w");
  if (fil) {
    while(line<configfilesize) {
      fputs(configtext[line],fil);
      line++;
    }
    fclose(fil);
  } else errors=true;
  if (errors==false) return(1); else return(0);
}

*/

// ****************************************************************************************
//
// kill running graber
//
// ****************************************************************************************

int killrunninggraber() {
  int sysresult;
  char processname[1024];
  sprintf(processname,"killall -9 %s",aktiv_tv_graber.grabercmd[aktiv_tv_graber.graberaktivnr]);
  sysresult=system(processname);
  if (sysresult) return(sysresult); else return(0);
}


// ****************************************************************************************
// NOT IN USE
// parse channel info from xmltvguide reader channel overview xmlfile
// load in to channel_list array
//
// ****************************************************************************************

int load_channel_list_from_graber() {
  FILE *fil;
  int x;
  char buffer[1024];
  char buffer1[1024];
  unsigned int cnr=0;
  xmlChar *tmpdat;
  xmlDoc *document;
  xmlNode *root, *first_child, *node, *node1 ,*subnode;
  xmlChar *xmltvid;
  xmlChar *content;
  char *pointer;
  int sysresult;
  int channelnr=0;
  char result[1024];
  char exestring[2048];
  bool errors=false;
  char userhomedir[1024];
  char filename[1024];
  // contry xml graber
  const char *grabercmd[35]={"","tv_grab_na_dd","tv_grab_nl","tv_grab_es_laguiatv","tv_grab_il","tv_grab_na_tvmedia","tv_grab_dtv_la","tv_grab_fi","tv_grab_eu_dotmedia","tv_grab_se_swedb",
                            "tv_grab_pt_meo","tv_grab_fr","tv_grab_uk_bleb","tv_grab_huro","tv_grab_ch_search","tv_grab_it","tv_grab_is","tv_grab_fi_sv","tv_grab_na_dtv","tv_grab_tr",
                            "tv_grab_eu_egon","tv_grab_dk_dr","tv_grab_se_tvzon","tv_grab_ar","tv_grab_fr_kazer","tv_grab_uk_tvguide","tv_grab_zz_sdjson"};

  write_logfile(logfile,(char *) "Get channel list file from tv graber sub system.");
  //getuserhomedir(userhomedir);
  strcpy(userhomedir,localuserhomedir);
  strcpy(filename,userhomedir);
  strcat(filename,"/tvguide_channels.txt");
  // Er der en aktiv tv graber
  if (aktiv_tv_graber.graberaktivnr>0) {
    // get config tv graber
    strcpy(exestring,grabercmd[aktiv_tv_graber.graberaktivnr]);
    //strcat(exestring," --list-channels | grep '<display-name lang=' | cut -c29-300 | cut -f1 -d'<' > ~/tvguide_channels.txt");
    switch (aktiv_tv_graber.graberaktivnr) {
              // none
      case 0: break;
              // 1 tv_grab_na_dd (North America (Data Direct)) (need login)
      case 1: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 2 tv_grab_nl (Holland)
      case 2: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 3 tv_grab_es_laguiatv (Spain (laguiatv.com))
      case 3: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"es\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 4 tv_grab_il  (Israel (tv-guide.walla.co.il)) (graber NOT WORKING NOW)
      case 4: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 5 tv_grab_na_tvmedia (North America (XMLTVListings.com by TVMedia))
              // API Key found on your account dashboard page (https://www.xmltvlistings.com/account/)
      case 5: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 6 tv_grab_dtv_la (Latin America Direct TV listings)
              // Could not fetch http://directstage.directvla.com/, error: 403 Forbidden at /usr/bin/tv_grab_dtv_la line 322.
      case 6: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 7 tv_grab_fi Finland (foxtv.fi, iltapulu.fi, telkku.com, telvis.fi, yle.fi)
      case 7: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"fi\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 8 tv_grab_eu_dotmedia danish now can be other (Europe tv schedules for free)
              //
      case 8: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 9 tv_grab_se_swedb (Sweden (swedb/tvsajten))
      case 9: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"sv\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 10 tv_grab_pt_meo (Portugal)
              // MEED CHECK
      case 10:strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 11 tv_grab_fr (France)
              // NOT WORKING
      case 11:strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 12 tv_grab_uk_bleb (United Kingdom (bleb.org))
      case 12:strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name>).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 13 tv_grab_huro (Hungary/Romania)
              // --loc cz for Czech
		          // --loc hu for Hungary
		          // --loc ro for Romania
		          // --loc sk for Slovakia
      case 13: strcat(exestring," --list-channels --loc ro | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 14 tv_grab_ch_search (Switzerland (tv.search.ch))
      case 14: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"de\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 15 it tv_grab_it (Italy)
      case 15: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name>).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 16 tv_grab_is (Iceland)
      case 16: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name>|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 17 finland tv_grab_fi_sv (Finland (Swedish))
      case 17: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"sv\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 18 tv_grab_na_dtv  (North America using www.directv.com)
      case 18: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name>).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 19 tv_grab_tr (Turkiye - Digiturk (www.digiturk.com.tr))
      case 19: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"tr\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 20 tv_grab_eu_egon (German speaking area (Egon zappt))
      case 20: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"de\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 21 danish tv_grab_dk_dr (TV Oversigten fra Danmarks Radios (2012) (www.dr.dk/tjenester/programoversigt))
      case 21: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"da\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 22 tv_grab_se_tvzon  (Sweden (TVZon))
      case 22: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 23 tv_grab_ar
              // tv_grab_ar XMLTV module version 0.5.69
              // This is tv_grab_ar version 1.22, 2016/03/16 04:12:51
      case 23: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"es\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 24 tv_grab_fr_kazer
              // Your userhash at kazer.org. It is shown on http://www.kazer.org/my-channels.html together with your selection of channels.
              // need account
      case 24: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
              // 25 uk tv_grab_uk_tvguide
      case 25: strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
      default:
              // default for more (new contry)
              strcat(exestring," --list-channels | grep -oP '(?<=<channel id=\"|<display-name lang=\"en\">).*(?=\">|</display-name>)' > ~/tvguide_channels.txt");
              break;
    }
    //printf("Create channel list file from tv_graber_config \nexestring = %s\n",exestring);
    write_logfile(logfile,(char *) "Create channel list file from tv_graber_config.");
    write_logfile(logfile,(char *) exestring);
    switch (aktiv_tv_graber.graberaktivnr) {
        case 13:
              sysresult=system(exestring);
              break;
        default:
              sysresult=system(exestring);
              break;
    }
    if (sysresult) write_logfile(logfile,(char *) "Error create channel list file from tv_graber_config. Check xmltv is installed.");
    // read channel list into channel_list struct
    if (check_zerro_bytes_file(filename)!=0) {
      fil=fopen(filename,"r");
      if (fil) {
        PRGLIST_ANTAL=0;
        while(!(feof(fil))) {
          fgets(buffer,512,fil);                                                  // get id
          fgets(buffer1,512,fil);                                                 // get name
          if (cnr<MAXPRGLIST_ANTAL) {
            for(x=0;x<strlen(buffer);x++) {
              if ((buffer[x]!='\r') && (buffer[x]!='\n')) channel_list.channel_list[cnr].id[x]=buffer[x];
            }
            channel_list.channel_list[cnr].id[x]='\0';
            for(x=0;x<strlen(buffer1);x++) {
              if ((buffer1[x]!='\r') && (buffer1[x]!='\n')) channel_list.channel_list[cnr].name[x]=buffer1[x];
            }
            channel_list.channel_list[cnr].name[x]='\0';
            // set default new channel is not active
            channel_list.channel_list[cnr].selected=false;                                     // default
            channel_list.channel_list[cnr].ordernr=0;                                          // default
            cnr++;
            PRGLIST_ANTAL++;
          }
        }
        fclose(fil);
      }
      // remove temp file again
      sysresult=system("rm ~/tvguide_channels.txt");
      if (sysresult) printf("error remove file ~/tvguide_channels.txt \n ");
      //if (debugmode) printf("Done channel list file from web. found %2d channels\n",cnr);
    } else errors=true;
  } else {
    errors=true;
    write_logfile(logfile,(char *) "No tv graber selected in config file.");
  }
  if (errors) return(-1); else return(sysresult);
}



// ****************************************************************************************//
//
// load channel list in to channel_list array from tv_guide db
//
// ****************************************************************************************


int load_channel_list_from_tvguide() {
  // mysql vars
  bool done=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  int cnr=0;
  PRGLIST_ANTAL=0;
  channel_list_struct newchannel;
  // mysql stuf
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database and update
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,"SELECT distinct title,program.chanid,c.orderid from program inner join mythtvcontroller.channel c on mythtvcontroller.program.title = c.name  order by c.orderid");
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (cnr<MAXKANAL_ANTAL)) {
          strcpy(newchannel.id,row[1]);
          strcpy(newchannel.name,row[0]);
          newchannel.selected=true;                                     // default select channel
          newchannel.ordernr=atoi(row[2]);                              // default
          channel_list.channel_list.push_back(newchannel);
          cnr++;
          PRGLIST_ANTAL++;
        }
      }
      mysql_close(conn);
    }
  } else write_logfile(logfile,(char *) "Error connect to mysql.");
  return(1);
}



// ****************************************************************************************//
//
// save tvguide channel info to tvguide_channels.dat in homedir
//
// ****************************************************************************************

bool save_channel_list() {
  FILE *fil;
  char userhomedir[1024];
  char filename[1024];
  unsigned int cnr=0;
  bool errors=false;
  //getuserhomedir(userhomedir);                                                // get user homedir
  strcpy(userhomedir,localuserhomedir);
  strcpy(filename,userhomedir);
  strcat(filename,"/");
  strcat(filename,tvguide_dat_filename);                                     // add the filename
  if (PRGLIST_ANTAL>0) {
    fil=fopen(filename,"w");                                                 // open file for write
    if (fil) {
      //printf("PRGLIST_ANTAL = %d \n",PRGLIST_ANTAL);
      while(cnr<PRGLIST_ANTAL) {
        fwrite(&channel_list.channel_list[cnr],sizeof(channel_list_struct),1,fil);
        cnr++;
      }
      fclose(fil);
      // order_channel_list_in_tvguide_db();                                       // ret db liste til som i channel_list
    } else {
      errors=true;
      write_logfile(logfile,(char *) "Error save tvguide_channels.dat");
    }
  }
  if (cnr>0) return(true); else return(false);
}



// ****************************************************************************************
//
// load tvguide channel info from tvguide_channels.dat in homedir
// return antal loaded
//
// ****************************************************************************************

/*

int load_channel_list() {
  FILE *fil;
  char userhomedir[1024];
  char filename[1024];
  unsigned int cnr=0;
  bool errors=false;
  PRGLIST_ANTAL=0;
  //getuserhomedir(userhomedir);
  strcpy(userhomedir,localuserhomedir);
  strcpy(filename,userhomedir);
  strcat(filename,"/");
  strcat(filename,tvguide_dat_filename);                                        // filename
  for(int n=0;n<MAXCHANNEL_ANTAL-1;n++) {
    channel_list[n].selected=false;                                             // is program channel active (default)
    channel_list[n].ordernr=0;                                                  // show ordernr
    channel_list[n].changeordernr=false;                                        // used change ordernr in cobfig setup screen
    strcpy(channel_list[n].name,"");                                            // channel name
    strcpy(channel_list[n].id,"");                                              // internal dbid
  }
  fil=fopen(filename,"r");
  if (fil) {
    while((!(feof(fil))) && (cnr<MAXCHANNEL_ANTAL-1)) {
      fread(&channel_list[cnr],sizeof(channel_list_struct),1,fil);
      cnr++;
      PRGLIST_ANTAL++;                                                          // set nr of records loaded
    }
    fclose(fil);
  } else {
    errors=true;
    write_logfile(logfile,(char *) "Error loading tvguide_channels.dat");
  }
  if (errors==false) return(cnr); return(0);
}

*/


// ****************************************************************************************
//
// order tv channels in tvguide db (mysql)
// by order in channel_list array
//
// ****************************************************************************************

int order_channel_list_in_tvguide_db() {
  // mysql vars
  bool done=false;
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char sqlselect[1024];
  // mysql stuf
  char *database = (char *) "mythtvcontroller";
  conn=mysql_init(NULL);
  // Connect to database and update
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      sprintf(sqlselect,"update channel set channel.visible=0 where chanid>=0");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      // make channel active from config
      for(int n=0;n<MAXCHANNEL_ANTAL-1;n++) {
        if (channel_list.channel_list[n].selected) {
          sprintf(sqlselect,"update channel set channel.orderid=%d,channel.visible=1 where channel.name like '%s' limit 1",n,channel_list.channel_list[n].name);
          mysql_query(conn,sqlselect);
          res = mysql_store_result(conn);
          write_logfile(logfile,(char *) sqlselect);                                     // write to debug log
          done=true;
        }
      }
      mysql_close(conn);
    }
  } else write_logfile(logfile,(char *) "Error open mysql.");
  return(done);
}


// ****************************************************************************************
//
// tv guide setup tool
//
// ****************************************************************************************

void show_setup_tv_graber(int startofset) {
  int winxsize=950;
  int winysize=800;
  int winxpos=100;
  int winypos=200;
  int textxofset=260;
  int textyofset=400;
  const char *weekdaysdk[10]={"Mandag","Tirsdag","Onsdag","Torsdag","Fredag","lørdag","søndag"};
  const char *weekdaysuk[10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
  const char *weekdaysfr[10]={"Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samed","Dimanche"};
  const char *weekdaysgr[11]={"Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Sonnabend","Sonntag"};
  const char *weekdaysar[10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
  int channel_antal=0;
  struct tm *xmlupdatelasttime;
  int xpos=0;
  int ypos=0;
  char text[200];
  std::string showtxt;
  // update channel list before show it
  // channel list editor
  if (hent_tv_channels==false) {
    channel_antal=load_channel_list_from_tvguide();
    hent_tv_channels=true;
  }
  renderer.AddTextureRect(0,setuptvgraberback, winxpos, winypos, winxsize, winysize,1,1,1,1);
  // close button
  renderer.AddTextureRect(40,_textureclose, ((winxsize-188)/2)+(188/2), winysize+100, 188, 81,1,1,1,1);
  xmlupdatelasttime=localtime(&configtvguidelastupdate);
  //
  // Sprog struktur. (date format)
  //
  // English, danish, france, tysk, Arabic
  if (configtvguidelastupdate>0) {    
    switch (configland) {
      case 0: showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 1: showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysdk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 2: showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysfr[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 3: showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysgr[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 4: showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysar[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      default:
              showtxt=fmt::format("{} {:02}/{:02}/{:02} {:02}:{:02}",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              // sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
    }
  } else showtxt="Never.... ";
  renderer.AddText(&myfont,250 ,300+(0*18) ,"Last updated : ",1,1,1,1);
  renderer.AddText(&myfont,350 ,300+(0*18) ,showtxt,1,1,1,1);
  for (int n=0;n<24;n++) {
    if (n<channel_list.channel_list.size()) {
      if (channel_list.channel_list[(n)+startofset].selected) {
        if (channel_list.channel_list[(n)+startofset].changeordernr)
          renderer.AddText(&myfont,textxofset+20 ,textyofset+(n*18) ,"[x] ",1.0f,1.0f,1.0f,1);
        else
          renderer.AddText(&myfont,textxofset ,textyofset+(n*18) ,"[x] ",1.0f,1.0f,1.0f,1);
      } else {
        if (channel_list.channel_list[(n)+startofset].changeordernr)
          renderer.AddText(&myfont,textxofset+20 ,textyofset+(n*18) ,"[ ] ",1.0f,1.0f,1.0f,1);
        else 
          renderer.AddText(&myfont,textxofset ,textyofset+(n*18) ,"[ ] ",1.0f,1.0f,1.0f,1);
      }
      if ((do_show_setup_select_linie-1)==n) {
        if (channel_list.channel_list[(n)+startofset].changeordernr)
          renderer.AddText(&myfont,textxofset+30+20 ,textyofset+(n*18) ,channel_list.channel_list[(n)+startofset].name,1,1,1,1);
        else
          renderer.AddText(&myfont,textxofset+30 ,textyofset+(n*18) ,channel_list.channel_list[(n)+startofset].name,1,1,1,1);
      } else {
        if (channel_list.channel_list[(n)+startofset].changeordernr)
          renderer.AddText(&myfont,textxofset+30+20 ,textyofset+(n*18) ,channel_list.channel_list[(n)+startofset].name,0.6f,0.6f,0.6f,1);
        else 
          renderer.AddText(&myfont,textxofset+30 ,textyofset+(n*18) ,channel_list.channel_list[(n)+startofset].name,0.6f,0.6f,0.6f,1);
      }
    }
  }
  // tv graber select line
  // if (do_show_setup_select_linie==0) showcoursornow(111,500,strlen(keybuffer));
  // if (do_show_setup_select_linie>0) showcoursornow(311,368-((do_show_setup_select_linie-2)*20),0);
}



// ****************************************************************************************
//
// Setup torrent settings
//
// ****************************************************************************************

void show_setup_torrent() {
  renderer.AddTextureRect(0,setuptorrent_background, 400, 300, 750, 650,1,1,1,1);
  renderer.AddText(&myfont2,650, 350 ,"Torrent settings",1,1,1,1);
  // close button
  renderer.AddTextureRect(40,_textureclose, 670, 800, 188, 81,1,1,1,1);

  renderer.AddText(&myfont,450 , 400,"Trash torrent file.....................: ",1,1,1,1);
  if (torrent_downloader.automove_to_movie_path) {
    renderer.AddText(&myfont,450+230 , 400,"N ",1,1,1,1);
  } else {
    renderer.AddText(&myfont,450+230 , 400,"Y ",1,1,1,1);
  }

  renderer.AddText(&myfont,450, 400 + (18*2),"Auto move file to movie db..: ",1,1,1,1);
  if (torrent_downloader.trash_torrent==true) {
    renderer.AddText(&myfont,450+230, 400 + (18*2),"Y ",1,1,1,1);
  } else {
    renderer.AddText(&myfont,450+230, 400 + (18*2),"N ",1,1,1,1);
  }

  renderer.AddText(&myfont,450, 400 + (18*4),"Torrent Download path.........: ",1,1,1,1);
  renderer.AddText(&myfont,450+230, 400 + (18*4),torrent_downloader.downloadpath.c_str(),1,1,1,1);
}


// ****************************************************************************************
//
// Setup start interface * select sub menu
//
// ****************************************************************************************

void show_setup_interface() {
  int ii=0;
  int winsizx=1200;
  int winsizy=800;
  int xpos=200+0;
  int ypos=200+0;
  int tabelofset=0;
  // load setings
  if (rssstreamoversigt.streamantal()==0) {
    rssstreamoversigt.load_rss_data();
  }
  // background
  renderer.AddTextureRect(0,setuptexture, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // sound setup
  winsizx=200;
  winsizy=200;
  xpos=300+200;
  ypos=200+500;
  renderer.AddTextureRect(30,_texturesoundsetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // buttons
  xpos=300+400;
  ypos=200+500;
  renderer.AddTextureRect(31,_texturesourcesetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // buttons
  xpos=300+600;
  ypos=200+500;
  renderer.AddTextureRect(32,_textureimagesetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // buttons
  xpos=300+800;
  ypos=200+500;
  renderer.AddTextureRect(33,_texturetemasetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // button for torrent
  xpos=300+200;
  ypos=200+300;
  renderer.AddTextureRect(34,_texturemythtvsql, xpos, ypos, winsizx, winsizy,1,1,1,1);
  //***********************************************************************************************
  // buttons
  xpos=300+400;
  ypos=200+300;
  renderer.AddTextureRect(35,_texturesetupfont, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // *************************************************************************************************
  xpos=300+600;
  ypos=200+300;
  renderer.AddTextureRect(36,_texturekeyssetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup video player button
  xpos=300+800;
  ypos=200+300;
  renderer.AddTextureRect(38,_texturevideoplayersetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup tv graber to use
  xpos=300+200; // 400
  ypos=200+100;
  renderer.AddTextureRect(39,_texturetvgrabersetup, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup rss
  xpos=300+400; // 600 
  ypos=200+100;
  renderer.AddTextureRect(42,_texturekeysrss, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup spotify
  xpos=300+600; // 800
  ypos=200+100;
  renderer.AddTextureRect(43,_texturespotify, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup tidal
  xpos=300+800; // 1000
  ypos=200+100;
  renderer.AddTextureRect(44,_texturetidal, xpos, ypos, winsizx, winsizy,1,1,1,1);
  // setup backend
  xpos=300+0; // 1000
  ypos=200+100;
  renderer.AddTextureRect(45,setupbackend, xpos, ypos, winsizx, winsizy,1,1,1,1);

  // close button
  xpos=200+400;
  ypos=900+10;
  winsizx = 355;
  winsizy = 81;
  renderer.AddTextureRect(37,_texturesetupclose, xpos, ypos, winsizx, winsizy,1,1,1,1);
}


