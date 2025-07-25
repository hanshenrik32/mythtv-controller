//
// All setup functions and windows
//
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdarg.h>
// opengl stuf
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Intrinsic.h>    /* Display, Window */
#include <GL/glx.h>           /* GLXContext */
// glc true type font system
#include <GL/glc.h>
#include <libxml/parser.h>
#include <fstream>
#include <fmt/format.h>

#include <iostream>

// mysql support
#include <mysql.h>
// program include
#include "text3d.h"
#include "readjpg.h"
#include "myth_ttffont.h"
#include "myth_setup.h"
#include "checknet.h"
#include "utility.h"
#include "myctrl_tvprg.h"

using namespace std;

extern FILE *logfile;
extern char debuglogdata[1024];                                                 // used by log system
extern float configdefaultradiofontsize;                                        // font size in overviews
extern float configdefaulttvguidefontsize;
extern float configdefaultmusicfontsize;
extern float configdefaultstreamfontsize;
extern float configdefaultmoviefontsize;

extern int configuvmeter;
extern int debugmode;
extern int vis_nyefilm_oversigt;
extern char localuserhomedir[4096];


extern const char *dbname;                                                      // internal database name in mysql (music,movie,radio)

// controll rss
//
rss_stream_class rssstreamoversigt;
//

struct configkeytype {
  char cmdname[200];
  unsigned int scrnr;
};

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
extern char configclosemythtvfrontend[256];			// close mythtvfront end on startup
extern char configaktivescreensavername[256];		// screen saver name
extern char configsoundoutport[256];				    // sound output port (hdmi/spdif/analog)
extern char configfontname[200];				        // default ttf font name to load and use
extern long configtvguidelastupdate;            // last xmltv update
extern char configbackend_tvgraber[256];        // internal tv graber to use
extern char configbackend_tvgraberland[256];    // internal tv graber to use country
extern configkeytype configkeyslayout[12];			// functions keys startfunc
extern char configuse3deffect[20];
// extern char configvideoplayer[200];             // default video player
extern char configdefaultplayer[200];
extern int configdefaultplayer_screenmode;
extern int configland;
extern char *configlandsprog[];
extern int configxbmcver;
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
extern GLuint setupnetworkback;
extern GLuint setupnetworkwlanback;
extern GLuint setupscreenback;
extern GLuint setuptemaback;                        //
extern GLuint setupfontback;                        //
extern GLuint setupkeysback;                        //
extern GLuint setuprssback;                         //
extern GLuint _texturevideoplayersetup;             // setup
extern GLuint _texturemythtvsql;
extern GLuint _textureclose;
extern GLuint _texturetvgrabersetup;                //
extern GLuint setupupdatebutton;
extern GLuint screenshot1,screenshot2,screenshot3,screenshot4,screenshot5,screenshot6,screenshot7,screenshot8,screenshot9,screenshot10;
extern unsigned int do_show_setup_select_linie;
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

extern channel_list_struct channel_list[];                                      // channel_list array used in setup graber
extern char keybuffer[];                                    // keyboard buffer


// ****************************************************************************************
//
// Denne som bruges
//
// ****************************************************************************************


void myglprint4(char *string)
{
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}


// ****************************************************************************************
//
// print string
//
// ****************************************************************************************

void myglprint5(char *string)
{
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
  }
}


// ****************************************************************************************
//
// print string
//
// ****************************************************************************************

void stroke_output2(GLfloat x, GLfloat y, char *format,...) {
  va_list args;
  char buffer[200], *p;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  glPushMatrix();
  glTranslatef(x, y, 0);
  glLineWidth(2.0);
  glScalef(0.003, 0.003, 0.003);
  for (p = buffer; *p; p++) glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}


// ****************************************************************************************
//
// draw cursor on screen at pos
//
// ****************************************************************************************

void showcoursornow(int cxpos,int cypos,int txtlength) {
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  cxpos+=4+(txtlength*9);
  cypos+=6;
  struct timespec timer;
  long lasttimer=0;
  static bool showcursor=false;
  clock_gettime(CLOCK_REALTIME, &timer);
  if (timer.tv_nsec>lasttimer+243600692) showcursor=true; else showcursor=false;
  if (showcursor) {
    lasttimer=timer.tv_nsec;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glTexCoord2f(0, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+6,cypos+((orgwinsizey/2)-(800/2))+20, 0.0);
    glTexCoord2f(1, 1); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2))+20 , 0.0);
    glTexCoord2f(1, 0); glVertex3f(cxpos+((orgwinsizex/2)-(1200/2))+14,cypos+((orgwinsizey/2)-(800/2)) , 0.0);
    glEnd();
  }
  glPopMatrix();

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
    rss_source_feed[n].stream_name=new char[namemaxlength];
    rss_source_feed[n].stream_url=new char[urlmaxlength];
    strcpy(rss_source_feed[n].stream_name,"");
    strcpy(rss_source_feed[n].stream_url,"");
  }
}

// ****************************************************************************************
//
// destructor
//
// ****************************************************************************************

rss_stream_class::~rss_stream_class() {
  for(int n=0;n<maxantal;n++) {
    delete[] rss_source_feed[n].stream_name;
    delete[] rss_source_feed[n].stream_url;
  }
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
  char *database = (char *) dbname;
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,"select feedtitle,title,url from internetcontentarticles where mediaURL is NULL order by feedtitle asc");
    res = mysql_store_result(conn);
    if (res) {
      while (((row = mysql_fetch_row(res)) != NULL) && (antal<100)) {
        if (row[0]) strcpy(rss_source_feed[antal].stream_name,row[0]);
        if (row[2]) strcpy(rss_source_feed[antal].stream_url,row[2]);
        antal++;
      }
    }
    mysql_close(conn);
  } else write_logfile(logfile,(char *) "Error connect to mysql.");
  return(1);
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
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res,*res1;
  MYSQL_ROW row,row1;
  int n=0;
  char *database = (char *) dbname;
  conn=mysql_init(NULL);
  // Connect to database
  if (conn) {
    mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    for(n=0;n<maxantal;n++) {
      // find record
      doexist=false;
      strcpy(ftitle,"");
      strcpy(ftitle2,"");
      strcpy(furl,"");
      if ((rss_source_feed[n].stream_name) && (strcmp(rss_source_feed[n].stream_name,"")!=0)) {
        sprintf(sqlstring,"select feedtitle,url from internetcontentarticles where feedtitle like '%s' limit 1",rss_source_feed[n].stream_name);
        mysql_query(conn,sqlstring);
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res)) != NULL) {
          doexist=true;
          strcpy(ftitle,row[0]);
          strcpy(furl,row[0]);
        }
        if (doexist) {
          // find record in db abd do the change and update record again
          if (ftitle) {
            if (strcmp(ftitle,rss_source_feed[n].stream_name)==0) {
              sprintf(sqlstring,"update internetcontentarticles set url='%s' where title like '%s' limit 1",rss_source_feed[n].stream_url,rss_source_feed[n].stream_name);
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
            } else if (strcmp(furl,rss_source_feed[n].stream_url)==0) {
              //
              // update name
              //
              // save old name
              sprintf(sqlstring,"select title from internetcontentarticles where url like '%s' limit 1",rss_source_feed[n].stream_url);
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              while (((row1 = mysql_fetch_row(res1)) != NULL) && (antal<100)) {
                strcpy(ftitle2,row1[0]);
              }
              // update db record with new name
              sprintf(sqlstring,"update internetcontentarticles set name='%s' where url like '%s' limit 1",rss_source_feed[n].stream_name,rss_source_feed[n].stream_url);
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              if (res1) {
                // if okay update
                // chnage/update name
                sprintf(sqlstring,"update internetcontent set name='%s' where name like '%s'",rss_source_feed[n].stream_name,ftitle2);
                mysql_query(conn,sqlstring);
                res1 = mysql_store_result(conn);
              }
            } else {
              //
              // no update of name of url create new
              //
              sprintf(sqlstring,"insert into internetcontentarticles (feedtitle,title,url) values('%s','%s','%s')",rss_source_feed[n].stream_name,rss_source_feed[n].stream_name,rss_source_feed[n].stream_url);
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
              sprintf(sqlstring,"insert into internetcontent (name,type) values('%s')",rss_source_feed[n].stream_name);
              mysql_query(conn,sqlstring);
              res1 = mysql_store_result(conn);
            }
          }
        } else {
          // no update of name of url create new
          sprintf(sqlstring,"insert into internetcontentarticles (feedtitle,title,url) values('%s','%s','%s')",rss_source_feed[n].stream_name,rss_source_feed[n].stream_name,rss_source_feed[n].stream_url);
          mysql_query(conn,sqlstring);
          res1 = mysql_store_result(conn);
          sprintf(sqlstring,"insert into internetcontent (name,type) values('%s')",rss_source_feed[n].stream_name);
          mysql_query(conn,sqlstring);
          res1 = mysql_store_result(conn);
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
  strcpy(rss_source_feed[nr].stream_url,url);
  return(1);
}


// ****************************************************************************************
//
// update name
//
// ****************************************************************************************

int rss_stream_class::set_stream_name(int nr,char *name) {
  strcpy(rss_source_feed[nr].stream_name,name);
  return(1);
}


// ****************************************************************************************
//
// Setup video config
//
// ****************************************************************************************

void show_setup_screen() {
  char resl[1024];
  int winsizx=1200;
  int winsizy=800;
  int xpos=0;
  int ypos=0;
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupscreenback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=550;
  ypos=50;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  winsizx=300;
  winsizy=40;
  xpos=250;
  ypos=600;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  winsizx=300;
  winsizy=40;
  xpos=250;
  ypos=550;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd(); //End quadrilateral coordinates
  glPopMatrix();
  // screen saver time out
  glPushMatrix();
  winsizx=50;
  winsizy=40;
  xpos=250;
  ypos=500;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // use 3d
  glPushMatrix();
  winsizx=40;
  winsizy=40;
  xpos=250;
  ypos=450;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  xpos=250;
  ypos=400;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // full screen box
  glPushMatrix();
  winsizx=40;
  winsizy=40;
  xpos=250;
  ypos=350;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  //
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 750.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Video mode                ");
  if (do_show_setup_select_linie==0) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) resl);
  } else {
    myglprint4((char *) resl);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==0) showcoursornow(250,600,strlen(resl));
  glPushMatrix();
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 700.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Screen saver             ");
  myglprint4((char *) " ");
  if (do_show_setup_select_linie==1) {
    strcpy(keybuffer,configaktivescreensavername);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) configaktivescreensavername);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==1) showcoursornow(250,550,strlen(configaktivescreensavername));
  glPushMatrix();
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 650.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Saver timeout            ");
  myglprint4((char *) " ");
  if (do_show_setup_select_linie==2) {
      strcpy(keybuffer,configscreensavertimeout);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
//        glRasterPos2f(2.2f, 0.0f);
      myglprint4((char *) keybuffer);
  } else {
      myglprint4((char *) configscreensavertimeout);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==2) showcoursornow(250,500,strlen(configscreensavertimeout));
  glPushMatrix();
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 600.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Use 3D                   ");
  myglprint4((char *) " ");
  if (do_show_setup_select_linie==3) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    if (strcmp(configuse3deffect,"yes")==0) myglprint4((char *) "yes"); else  myglprint4((char *) "no ");
  } else {
    if (strcmp(configuse3deffect,"yes")==0) myglprint4((char *) "yes"); else  myglprint4((char *) "no ");
  }
  if (do_show_setup_select_linie==3) showcoursornow(250,450,strlen("yes"));
  glPopMatrix();
  glPushMatrix();
  // show sprog valg
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 550.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Language                 ");
  myglprint4((char *) " ");
  if (do_show_setup_select_linie==4) {
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
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    if ((configland>=0) && (configland<5)) {
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
        myglprint4(keybuffer);
    }
  }
  glPopMatrix();
  if (do_show_setup_select_linie==4) showcoursornow(250,400,strlen(keybuffer));
  glPushMatrix();
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 500.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Full screen mode          ");
  if (do_show_setup_select_linie==5) {
    if (full_screen) strcpy(keybuffer,"yes"); else strcpy(keybuffer,"no ");
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    if (full_screen) myglprint4((char *) "yes"); else myglprint4((char *) "no ");
    if (full_screen) strcpy(keybuffer,"yes"); else strcpy(keybuffer,"no ");
  }
  glPopMatrix();
  if (do_show_setup_select_linie==5) showcoursornow(250,350,strlen(keybuffer));
  glPushMatrix();
  sprintf(resl,"Hardware %s",glGetString(GL_RENDERER));
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 450.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint5((char *) resl);
  glRasterPos2f(0.0f, -20.0f);
  sprintf(resl," Render   %s",glGetString(GL_VENDOR));
  myglprint5((char *) resl);
  glRasterPos2f(0.0f, -40.0f);
  sprintf(resl," Version  %s",glGetString(GL_VERSION));
  myglprint5((char *) resl);
  glPopMatrix();
}



// ****************************************************************************************
//
// Setup video config
//
// ****************************************************************************************


void show_setup_video() {
  char resl[1024];
  int winsizx=1200;
  int winsizy=600;
  int xpos=0;
  int ypos=0;
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupscreenback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 800.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Video play options.");   // keybuffer
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=550;
  ypos=150;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  winsizx=300;
  winsizy=40;
  xpos=250;
  ypos=600;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  winsizx=300;
  winsizy=40;
  xpos=250;
  ypos=550;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 750.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Videoplayer                ");
  if (do_show_setup_select_linie==0) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) configdefaultplayer);
  } else {
    myglprint4((char *) configdefaultplayer);
  }
  glPopMatrix();
  // show cursor
  if (do_show_setup_select_linie==0) showcoursornow(250,600,strlen(configdefaultplayer));
  glPushMatrix();
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 700.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Player resolution         ");
  myglprint4((char *) " ");
  switch(configdefaultplayer_screenmode) {
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
  if (do_show_setup_select_linie==1) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) resl);
  } else {
    myglprint4((char *) resl);
  }
  glPopMatrix();
  // debug mode
  glPushMatrix();
  winsizx=100;
  winsizy=40;
  xpos=250;
  ypos=500;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
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
  glDisable(GL_TEXTURE_2D);
  // show close on exit type
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 650.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Debug mode               ");
  myglprint4((char *) " ");
  if (debugmode==2) sprintf(resl,"Music");
  else if (debugmode==4) sprintf(resl,"radio");
  else if (debugmode==8) sprintf(resl,"Keyboard/mouse");
  else if (debugmode==16) sprintf(resl,"Movie");
  else if (debugmode==32) sprintf(resl,"Not def");
  else if (debugmode==64) sprintf(resl,"Not def");
  else if (debugmode==128) sprintf(resl,"Stream");
  else if (debugmode==256) sprintf(resl,"TV guide stuf");
  else if (debugmode==512) sprintf(resl,"media importer");
  else sprintf(resl,"%d",debugmode);
  if (do_show_setup_select_linie==2) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) resl);
  } else {
    myglprint4((char *) resl);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==1) showcoursornow(250,550,strlen(resl));
  glPushMatrix();
  winsizx=300;
  winsizy=40;
  xpos=250;
  ypos=450;
  // here start input
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // show uv mode
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(380.0f, 600.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "UV Meter mode             ");
  if (configuvmeter==0) strcpy(resl,"None");
  if (configuvmeter==1) strcpy(resl,"Simple");
  if (configuvmeter==2) strcpy(resl,"Dual");
  if (configuvmeter>2) strcpy(resl,"None");
  sprintf(resl,"%d",configuvmeter);
  if (do_show_setup_select_linie==3) {
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) resl);
  } else {
    myglprint4((char *) resl);
  }
  glPopMatrix();
  // show cursor
  if (do_show_setup_select_linie==2) showcoursornow(250,500,strlen(resl));
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
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupscreenback);                   //setuptemaback);
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
  // show tema
  winsizx=550;
  winsizy=350;
  xpos=250;
  ypos=220;
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  switch(tema) {
      case 1:	glBindTexture(GL_TEXTURE_2D,screenshot1);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 2:	glBindTexture(GL_TEXTURE_2D,screenshot2);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 3:	glBindTexture(GL_TEXTURE_2D,screenshot3);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 4:	glBindTexture(GL_TEXTURE_2D,screenshot4);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 5:	glBindTexture(GL_TEXTURE_2D,screenshot5);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 6:	glBindTexture(GL_TEXTURE_2D,screenshot6);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 7:	glBindTexture(GL_TEXTURE_2D,screenshot7);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 8:	glBindTexture(GL_TEXTURE_2D,screenshot8);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 9:	glBindTexture(GL_TEXTURE_2D,screenshot9);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              break;
      case 10:glBindTexture(GL_TEXTURE_2D,screenshot10);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      default:
              glBindTexture(GL_TEXTURE_2D,screenshot10);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  glLoadName(41);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(840 , 740 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glScalef(25.0f, 25.0f, 1.00f);
  sprintf(temptxt,"Tema:%d ",tema);
  glcRenderString(temptxt);
  glEnable(GL_TEXTURE_2D);
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
  ypos=100;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
}



// ****************************************************************************************
//
// Setup font
//
// ****************************************************************************************

void show_setup_font(int startofset) {
    int i;
    char temptxt[200];
    int winsizx=100;
    int winsizy=200;
    int xpos=0;
    int ypos=0;
    // background
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,setupfontback);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),200 , 0.0);
    glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
    glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
    glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,200 , 0.0);
    glEnd();
    glPopMatrix();
    for (i=0;i<8;i++) {
      if (i==2) {
        winsizx=600;
        winsizy=20;
        xpos=180;
        ypos=360+40;
        glDisable(GL_BLEND);//aktivfont.selectfont(aktivfont.typeinfo[i+startofset].fontname);
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
      }
      glPushMatrix();
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0f, 1.0f, 1.0f);
      glTranslatef(540 , 600-(i*20) , 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      glScalef(25.0f, 25.0f, 1.00f);
      if ((i+startofset)-3>=0) strcpy(temptxt,(char *) aktivfont.typeinfo[(i+startofset)-3].fontname);
      else sprintf(temptxt,"                                     ");
      //aktivfont.selectfont(aktivfont.typeinfo[i+startofset].fontname);
      glcRenderString(temptxt);
      glEnable(GL_TEXTURE_2D);
      glPopMatrix();
    }
    // show sample
    glPushMatrix();
    glTranslatef(540 , 400 , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    glScalef(25.0f, 25.0f, 1.00f);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    //aktivfont.selectfont(aktivfont.typeinfo[2].fontname);
    //aktivfont.selectfont("ani");
    aktivfont.selectfont((char *) aktivfont.typeinfo[(i+startofset)-3].fontname);
    glcRenderString("Sample string...");
    aktivfont.selectfont(configfontname);
    glPopMatrix();

/*
    // select selected font
    if ((startofset>0) && (fontselected==false)) {
        fontselected=true;
        printf("Select font %s \n",aktivfont.typeinfo[startofset].fontname);
        fontnr=startofset;
        aktivfont.selectfont(aktivfont.typeinfo[startofset].fontname);
//        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(400.0f, 200.f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(25.0f, 25.0f, 1.00f);
        glcRenderString("Sample");
        //myglprint4((char *) "This is a demo of the font");
         // restore select font
        //    aktivfont.selectfont(configfontname);
    } else {
//        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(200.0f, 200.0f, 0.0f);
        glRasterPos2f(0.0f, 0.0f);
        glScalef(25.0f, 25.0f, 1.00f);
        glcRenderString("Sample");
    }
    if ((strcmp(aktivfont.typeinfo[startofset].fontname,configfontname)!=0) && (fontnr!=startofset)) {
        fontselected=false;
    }
*/
    glPushMatrix();
    // close buttons
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D,_textureclose);                            // _texturesetupclose
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
}




// ****************************************************************************************
//
// Setup sound config
//
// ****************************************************************************************

void show_setup_sound() {
  // mask
  int winsizx=100;
  int winsizy=200;
  int xpos=0;
  int ypos=0;
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupsoundback);
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
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=430;
  ypos=120;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=500;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
//    glTranslatef(-2.0f, 2.1f-0.5f,-14.8f);
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=450;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=400;
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
  // show sound system in use
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(500.0f, 650.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Output device");
  glRasterPos2f(40.0f, 0.0f);
  myglprint4((char *) ":");
  if (do_show_setup_select_linie==0) {
    strcpy(keybuffer,configmythsoundsystem);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) configmythsoundsystem);
  }
  glPopMatrix();
  glPushMatrix();
  // show sound output device
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(500.0f, 600.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Output");
  glRasterPos2f(30.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==1) {
    strcpy(keybuffer,configsoundoutport);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) configsoundoutport);
  }
  glPopMatrix();
  glPushMatrix();
  // show sound system
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(500.0f, 550.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Sound system");
  glRasterPos2f(30.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==2) {
    strcpy(keybuffer,"FMOD");
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) "FMOD");
  }
  glPopMatrix();
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
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupsqlback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  //glBlendFunc(GL_ONE, GL_ONE);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=430;
  ypos=-10;
  glLoadName(40);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=400;
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd(); //End quadrilateral coordinates
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=350;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=200;
  winsizy=30;
  xpos=300;
  ypos=300;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=250;
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd(); //End quadrilateral coordinates
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=200;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=150;
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
  // text input background
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  winsizx=500;
  winsizy=30;
  xpos=300;
  ypos=100;
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,setupkeysbar1);                 // setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd(); //End quadrilateral coordinates
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  if (strcmp(configbackend,"mythtv")==0) {
    if (configmythtvver==1264) mythver=0.24f;
    if (configmythtvver) {
      sprintf(text,"Mythtv version   : %2.2f - Database ver : %d",mythver,configmythtvver);
    } else {
      if (global_use_internal_music_loader_system) sprintf(text,"No mythtv backend found. Internal db in use."); else sprintf(text,"Mythtv/Database config error no connection.");
    }
    glTranslatef(500.0f, 650.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) text);
  } else if ((strcmp(configbackend,"xbmc")==0) || (strcmp(configbackend,"kodi")==0)) {
    if (configxbmcver) {
      if (configxbmcver==75) sprintf(text,"XBMC version     : 12 found"); else sprintf(text,"XBMC version     : %d found",configxbmcver);
    } else sprintf(text,"NO XBMC version found");
    glTranslatef(500.0f, 650.0f, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) text);
  }
  // here start input
  glPushMatrix();
  glTranslatef(10.0f, -50.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "mythtv/xbmc");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  if (do_show_setup_select_linie==0) {
      strcpy(keybuffer,configbackend);
      glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) keybuffer);
  } else {
      glColor3f(1.0f,1.0f,1.0f);
      glRasterPos2f(160.0f, 0.0f);
      myglprint4((char *) configbackend);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==0) showcoursornow(-200,-200,strlen(configbackend));
  // here start input
  glPushMatrix();
  glColor3f(1.0f,1.0f,1.0f);
  glTranslatef(10.0f, -100.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Mythtv server");
  else myglprint4((char *) "XBMC/KODI server");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  if (do_show_setup_select_linie==1) {
    if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqlhost);
    else strcpy(keybuffer,configxbmchost);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    glRasterPos2f(160.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqlhost);
    else myglprint4((char *) configxbmchost);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==1) showcoursornow(-200,-250,strlen(configmysqlhost));
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(10.0f, -150.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Mythtv user");
  else myglprint4((char *) "XBMC user");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  if (do_show_setup_select_linie==2) {
    if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqluser);
    else strcpy(keybuffer,configxbmcuser);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    glRasterPos2f(160.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqluser);
    else myglprint4((char *) configxbmcuser);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==2) showcoursornow(-200,-300,strlen(configmysqluser));
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(10.0f, -200.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) "Password");
  else myglprint4((char *) "Password");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  if (do_show_setup_select_linie==3) {
    if (strcmp(configbackend,"mythtv")==0) strcpy(keybuffer,configmysqlpass);
    else strcpy(keybuffer,configxbmcpass);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    glRasterPos2f(160.0f, 0.0f);
    if (strcmp(configbackend,"mythtv")==0) myglprint4((char *) configmysqlpass);
    else myglprint4((char *) configxbmcpass);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==3) showcoursornow(-200,-350,strlen(configmysqlpass));
  // show config music path
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(10.0f, -250.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Music path");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==4) {
    strcpy(keybuffer,configmusicpath);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configmusicpath);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==4) showcoursornow(-200,-400,strlen(configmusicpath));
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(10.0f, -300.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Picture path");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==5) {
    strcpy(keybuffer,configpicturepath);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configpicturepath);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==5) showcoursornow(-200,-450,strlen(configpicturepath));
  // show config movie path
  glPushMatrix();
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(10.0f, -350.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Movie path");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==6) {
    strcpy(keybuffer,configmoviepath);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    glRasterPos2f(160.0f, 0.0f);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4((char *) configmoviepath);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==6) showcoursornow(-200,-500,strlen(configmoviepath));
  glPushMatrix();
  glColor3f(1.0f,1.0f,1.0f);
  glTranslatef(10.0f, -400.0f, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Recorded path");
  glRasterPos2f(140.0f, 0.0f);
  myglprint4((char *) ":");
  glRasterPos2f(160.0f, 0.0f);
  if (do_show_setup_select_linie==7) {
    strcpy(keybuffer,configrecordpath);
    glColor3f(selectcolor[0],selectcolor[1],selectcolor[2]);
    myglprint4((char *) keybuffer);
  } else {
    myglprint4(configrecordpath);
  }
  glPopMatrix();
  if (do_show_setup_select_linie==7) showcoursornow(-200,-550,strlen(configrecordpath));
}

// ****************************************************************************************
//
// Setup keys
//
// ****************************************************************************************
void show_setup_keys() {
  int winsizx=100;
  int winsizy=300;
  int xpos=0;
  int ypos=0;
  char text[200];
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupkeysback);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4),100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4),800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+800,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+800,100 , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=430;
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
  glTranslatef(680, 680, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Command to execute.                                                              ScrNr");
  glPopMatrix();
  glPushMatrix();
  // F3
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 650, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F3");
  glPopMatrix();
  glPushMatrix();
  // F4
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 600, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F4");
  glPopMatrix();
  glPushMatrix();
  // F5
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 550, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F5");
  glPopMatrix();
  glPushMatrix();
  // F6
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 500, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F6");
  glPopMatrix();
  glPushMatrix();
  // F7
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 450, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F7");
  glPopMatrix();
  glPushMatrix();
  // F8
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 400, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F8");
  glPopMatrix();
  glPushMatrix();
  // F9
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 350, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F9");
  glPopMatrix();
  glPushMatrix();
  // F10
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 300, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F10");
  glPopMatrix();
  glPushMatrix();
  // F11
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(610, 250, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "F11");
  glPopMatrix();
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
  if (do_show_setup_select_linie==0) {
    strcpy(keybuffer,configkeyslayout[0].cmdname);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    myglprint4((char *) configkeyslayout[0].cmdname);
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
  if (do_show_setup_select_linie==2) {
    strcpy(keybuffer,configkeyslayout[1].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    sprintf(text,"%s",configkeyslayout[1].cmdname);
    myglprint4((char *) text);
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
  if (do_show_setup_select_linie==4) {
    strcpy(keybuffer,configkeyslayout[2].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[2].cmdname);
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
  if (do_show_setup_select_linie==6) {
    strcpy(keybuffer,configkeyslayout[3].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[3].cmdname);
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
  if (do_show_setup_select_linie==8) {
    strcpy(keybuffer,configkeyslayout[4].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[4].cmdname);
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
  if (do_show_setup_select_linie==10) {
    strcpy(keybuffer,configkeyslayout[5].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[5].cmdname);
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
  if (do_show_setup_select_linie==12) {
    strcpy(keybuffer,configkeyslayout[6].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[6].cmdname);
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
  if (do_show_setup_select_linie==14) {
    strcpy(keybuffer,configkeyslayout[7].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[7].cmdname);
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
  if (do_show_setup_select_linie==16) {
    strcpy(keybuffer,configkeyslayout[8].cmdname);
    myglprint4((char *) keybuffer);
  } else {
    glColor3f(1.0f,1.0f,1.0f);
    myglprint4((char *) configkeyslayout[8].cmdname);
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
}




void showrss_list() {
  printf("\n");
  for(int t=0;t<99;t++) {
    if (strcmp(rssstreamoversigt.get_stream_name(t),"")!=0) printf("nr %d feedname %s url %s \n",t,rssstreamoversigt.get_stream_name(t),rssstreamoversigt.get_stream_url(t));
  }
  printf("\n");
}



// ****************************************************************************************
//
// setuo rss
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
    glTranslatef(300, 680, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "RSS Feed Name.");
    glPopMatrix();
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(684, 680, 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    myglprint4((char *) "Podcast URL");
    glPopMatrix();
    for (int n=0;n<18;n++) {
      glPushMatrix();
      glTranslatef(300 , 660-(n*20) , 0.0f);
      glRasterPos2f(0.0f, 0.0f);
      if (n==do_show_setup_select_linie) glColor3f(1.0f,1.0f,0.0f); else glColor3f(.7f,0.7f,0.7f);
      if ((startofset+n+1)<rssstreamoversigt.streamantal()) {
        showtxt=rssstreamoversigt.get_stream_name(0+startofset+n);
        // strcpy(keybuffer,rssstreamoversigt.get_stream_name(0+startofset+n));
      } else {
        strcpy(keybuffer,"");
        showtxt=keybuffer;
      }
      myglprint4((char *) showtxt.c_str());
      if (n==do_show_setup_select_linie) glColor3f(1.0f,1.0f,0.0f); glColor3f(.7f,0.7f,0.7f);
      if ((startofset+n+1)<rssstreamoversigt.streamantal()) {
        strcpy(keybuffer,rssstreamoversigt.get_stream_url(0+startofset+n));
        showtxt=rssstreamoversigt.get_stream_url(0+startofset+n);
      } else {
        strcpy(keybuffer,"");
        showtxt=keybuffer;
      }
      glRasterPos2f(392.0f, 0.0f);
      myglprint4((char *) showtxt.c_str());
      glPopMatrix();
    }
    switch(do_show_setup_select_linie) {
        case 0: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(0*20),strlen(rssstreamoversigt.get_stream_name(0+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 1: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(0*20),strlen(rssstreamoversigt.get_stream_url(0+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 2: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+1)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(1*20),strlen(rssstreamoversigt.get_stream_name(1+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 3: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+1)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(1*20),strlen(rssstreamoversigt.get_stream_url(1+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 4: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+2)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(2*20),strlen(rssstreamoversigt.get_stream_name(2+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 5: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+2)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(2*20),strlen(rssstreamoversigt.get_stream_url(2+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 6: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+3)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(3*20),strlen(rssstreamoversigt.get_stream_name(3+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 7: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+3)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(3*20),strlen(rssstreamoversigt.get_stream_url(3+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 8: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+4)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(4*20),strlen(rssstreamoversigt.get_stream_name(4+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 9: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+4)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(4*20),strlen(rssstreamoversigt.get_stream_url(4+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 10:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+5)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(5*20),strlen(rssstreamoversigt.get_stream_name(5+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 11:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+5)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(5*20),strlen(rssstreamoversigt.get_stream_url(5+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 12:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+6)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(6*20),strlen(rssstreamoversigt.get_stream_name(6+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 13:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+6)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(6*20),strlen(rssstreamoversigt.get_stream_url(6+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 14:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+7)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(7*20),strlen(rssstreamoversigt.get_stream_name(7+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 15:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+7)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(7*20),strlen(rssstreamoversigt.get_stream_url(7+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 16:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+8)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(8*20),strlen(rssstreamoversigt.get_stream_name(8+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 17:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+8)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(8*20),strlen(rssstreamoversigt.get_stream_url(8+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 18: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+9)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(9*20),strlen(rssstreamoversigt.get_stream_name(9+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 19: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+9)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(9*20),strlen(rssstreamoversigt.get_stream_url(9+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 20: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+10)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(10*20),strlen(rssstreamoversigt.get_stream_name(10+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 21: glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+10)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(10*20),strlen(rssstreamoversigt.get_stream_url(10+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 22:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+11)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(11*20),strlen(rssstreamoversigt.get_stream_name(11+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 23:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+11)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(11*20),strlen(rssstreamoversigt.get_stream_url(11+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 24:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+12)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(12*20),strlen(rssstreamoversigt.get_stream_name(12+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 25:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+12)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(12*20),strlen(rssstreamoversigt.get_stream_url(12+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 26:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+13)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(13*20),strlen(rssstreamoversigt.get_stream_name(13+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 27:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+13)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(13*20),strlen(rssstreamoversigt.get_stream_url(13+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 28:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+14)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(14*20),strlen(rssstreamoversigt.get_stream_name(14+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 29:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+14)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(14*20),strlen(rssstreamoversigt.get_stream_url(14+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 30:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+14)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(15*20),strlen(rssstreamoversigt.get_stream_name(15+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 31:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+15)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(15*20),strlen(rssstreamoversigt.get_stream_url(15+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 32:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+16)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(16*20),strlen(rssstreamoversigt.get_stream_name(16+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 33:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+16)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(16*20),strlen(rssstreamoversigt.get_stream_url(16+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 34:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+17)<rssstreamoversigt.streamantal()) showcoursornow(-70,510-(17*20),strlen(rssstreamoversigt.get_stream_name(17+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
        case 35:glColor3f(1.0f,1.0f,1.0f);
                if ((startofset+17)<rssstreamoversigt.streamantal()) showcoursornow(320,510-(17*20),strlen(rssstreamoversigt.get_stream_url(17+startofset)));
                else showcoursornow(-70,510-(0*20),0);
                break;
    }
}


// ****************************************************************************************
//
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

    while((cnr<PRGLIST_ANTAL) && (strcmp(channel_list[cnr].id,"")!=0)) {
      strcpy(buffer,"");
      switch (aktiv_tv_graber.graberaktivnr) {
        case 8: if (channel_list[cnr].selected) {
                  strcpy(buffer,"channel=");
                } else {
                  strcpy(buffer,"channel!");
                }
                strcat(buffer,channel_list[cnr].id);
                fputs(buffer,filout);
                fputs("\n",filout);
                break;
        defaut: if (channel_list[cnr].selected) {
                  strcpy(buffer,"channel=");
                } else {
                  strcpy(buffer,"channel!");
                }
                strcat(buffer,channel_list[cnr].id);
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



// ****************************************************************************************
//
// constructor for channel_list array
//
// ****************************************************************************************

channel_configfile::channel_configfile() {
  for(int n=0;n<MAXCHANNEL_ANTAL-1;n++) {
    channel_list[n].selected=false;                                             // is program channel active (default)
    channel_list[n].ordernr=0;                                                  // show ordernr
    channel_list[n].changeordernr=false;                                        // used change ordernr in cobfig setup screen
    strcpy(channel_list[n].name,"");                                            // channel name
    strcpy(channel_list[n].id,"");                                              // internal dbid
  }
}


channel_configfile::~channel_configfile() {

}

// ****************************************************************************************
//
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
//
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
              if ((buffer[x]!='\r') && (buffer[x]!='\n')) channel_list[cnr].id[x]=buffer[x];
            }
            channel_list[cnr].id[x]='\0';
            for(x=0;x<strlen(buffer1);x++) {
              if ((buffer1[x]!='\r') && (buffer1[x]!='\n')) channel_list[cnr].name[x]=buffer1[x];
            }
            channel_list[cnr].name[x]='\0';
            // set default new channel is not active
            channel_list[cnr].selected=false;                                     // default
            channel_list[cnr].ordernr=0;                                          // default
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
  // mysql stuf
  char *database = (char *) dbname;
  conn=mysql_init(NULL);
  // Connect to database and update
  if (conn) {
    if (mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0)) {
      mysql_query(conn,"set NAMES 'utf8'");
      res = mysql_store_result(conn);
      mysql_query(conn,"SELECT c.name,c.chanid,c.orderid FROM program inner join channel c where c.chanid=program.chanid group by program.chanid");
      res = mysql_store_result(conn);
      if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (cnr<MAXKANAL_ANTAL)) {
          strcpy(channel_list[cnr].id,row[1]);
          strcpy(channel_list[cnr].name,row[0]);
          channel_list[cnr].selected=true;                                     // default select channel
          channel_list[cnr].ordernr=atoi(row[2]);                              // default
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
        fwrite(&channel_list[cnr],sizeof(channel_list_struct),1,fil);
        cnr++;
      }
      fclose(fil);
      order_channel_list_in_tvguide_db();                                       // ret db liste til som i channel_list
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


// ****************************************************************************************
//
// sort record after selected (struct)
//
// ****************************************************************************************

void order_channel_list() {
  struct channel_list_struct tmpchannel;
  int n;
  bool swap=true;
  while(swap) {
    n=0;
    swap=false;
    while(n<MAXCHANNEL_ANTAL-1) {
      if ((channel_list[n].selected==false) && (channel_list[n+1].selected)) {
        swap=true;
        tmpchannel.selected=channel_list[n].selected;
        tmpchannel.ordernr=channel_list[n].ordernr;
        tmpchannel.changeordernr=channel_list[n].changeordernr;
        strcpy(tmpchannel.name,channel_list[n].name);
        strcpy(tmpchannel.id,channel_list[n].id);
        channel_list[n].selected=channel_list[n+1].selected;                    // is program channel active
        channel_list[n].ordernr=channel_list[n+1].ordernr;                      // show ordernr
        channel_list[n].changeordernr=channel_list[n+1].changeordernr;          // used change ordernr in cobfig setup screen
        strcpy(channel_list[n].name,channel_list[n+1].name);                    // channel name
        strcpy(channel_list[n].id,channel_list[n+1].id);                        // internal dbid
        channel_list[n+1].selected=tmpchannel.selected;                         // is program channel active
        channel_list[n+1].ordernr=tmpchannel.ordernr;                           // show ordernr
        channel_list[n+1].changeordernr=tmpchannel.changeordernr;               // used change ordernr in cobfig setup screen
        strcpy(channel_list[n+1].name,tmpchannel.name);                         // channel name
        strcpy(channel_list[n+1].id,tmpchannel.id);                             // internal dbid
      }
      n++;
    }
  }
}


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
  char *database = (char *) dbname;
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
        if (channel_list[n].selected) {
          sprintf(sqlselect,"update channel set channel.orderid=%d,channel.visible=1 where channel.name like '%s' limit 1",n,channel_list[n].name);
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
  const char *weekdaysdk[10]={"Mandag","Tirsdag","Onsdag","Torsdag","Fredag","lørdag","søndag"};
  const char *weekdaysuk[10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
  const char *weekdaysfr[10]={"Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samed","Dimanche"};
  const char *weekdaysgr[11]={"Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Sonnabend","Sonntag"};
  const char *weekdaysar[10]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
  int channel_antal;
  int winsizx=100;
  struct tm *xmlupdatelasttime;
  int winsizy=300;
  int xpos=0;
  int ypos=0;
  char text[200];
  // update channel list before show it
  // channel list editor
  if (hent_tv_channels==false) {
    // try to load struct channel info first time
    // tvguide_channels.dat
    channel_antal=load_channel_list();
    if (channel_antal==0) {
      // load channel names from tvguide grapper and save it to internal db
      // it is a first time program thing
      // crete mew config file
      printf("Create config file for xmltv first time. (disabled)\n");
      //write_logfile((char *) "Create config file for xmltv first time. (disabled).");
      /*
      if (txmltvgraber_createconfig()==0) {
        printf("\nError xmltv create graber confg. Set to %s \n",configbackend_tvgraber);
      }
      */
      //
      // load all channels name from tv_graber

      //load_channel_list_from_graber();                                        // get channel list from graber
      load_channel_list_from_tvguide();

      // save channel list to struct db file
      // struct channel_list
      order_channel_list();                                                   // Order data
      save_channel_list();                                                    // save to db file
    } else {
      // the channel list is loaded from db file.
      // set flag to load channel list
      order_channel_list();
      //save_channel_list();
      // update conf file to xmltv grabber
      //txmltvgraber_updateconfigfile();
      //
      // this func create new config and make all channel's active
      // txmltvgraber_createconfig();
      //
    }
    hent_tv_channels=true;
  }
  // background
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(0.6f, 0.6f, 0.6f);
  glBindTexture(GL_TEXTURE_2D,setuptvgraberback);
  //glBindTexture(GL_TEXTURE_2D,setuptexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f( (orgwinsizex/4)-50,100 , 0.0);
  glTexCoord2f(0, 1); glVertex3f( (orgwinsizex/4)-50,800 , 0.0);
  glTexCoord2f(1, 1); glVertex3f( (orgwinsizex/4)+950,800 , 0.0);
  glTexCoord2f(1, 0); glVertex3f( (orgwinsizex/4)+950,100 , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  // close buttons
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D,_textureclose);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  winsizx=188;
  winsizy=81;
  xpos=475;
  ypos=-10;
  glLoadName(40);
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd(); //End quadrilateral coordinates
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(520, 650, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Use TV graber ");
  glPopMatrix();
  // update button
  glPushMatrix();
  winsizx=100;
  winsizy=50;
  xpos=820;
  ypos=490;
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setupupdatebutton);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(45);                                                             // update button name
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glTranslatef(540, 600, 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  myglprint4((char *) "Last update.");
  glPopMatrix();
  // start af input felter
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
  // line 0 input
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(680 , 650 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (do_show_setup_select_linie==0) {
    if (aktiv_tv_graber.graberaktivnr==1) sprintf(keybuffer,"%s (reg is required on homepage) %d",aktiv_tv_graber.graberland[aktiv_tv_graber.graberaktivnr],aktiv_tv_graber.graberaktivnr);
    else sprintf(keybuffer,"%s %d",aktiv_tv_graber.graberland[aktiv_tv_graber.graberaktivnr],aktiv_tv_graber.graberaktivnr);
    myglprint4((char *) keybuffer);   // keybuffer
  } else {
    if (aktiv_tv_graber.graberaktivnr==1) sprintf(keybuffer,"%s (reg is required on homepage)",aktiv_tv_graber.graberland[aktiv_tv_graber.graberaktivnr]);
    else sprintf(keybuffer,"%s",aktiv_tv_graber.graberland[aktiv_tv_graber.graberaktivnr]);
    myglprint4((char *) keybuffer);
  }
  glPopMatrix();
  // line 1 show last update date
  glPushMatrix();
  winsizx=250;
  winsizy=30;
  xpos=300;
  ypos=450;
  glEnable(GL_TEXTURE_2D);
  glColor3f(0.7f, 0.7f, 0.7f);
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
  glDisable(GL_TEXTURE_2D);
  glTranslatef(680 , 600 , 0.0f);
  glRasterPos2f(0.0f, 0.0f);
  glColor3f(1.0f,1.0f,1.0f);
  if (configtvguidelastupdate>0) {
    // get last time from running config
    xmlupdatelasttime=localtime(&configtvguidelastupdate);
    //
    // Sprog struktur. (date format)
    //
    // English, danish, france, tysk, Arabic
    switch (configland) {
      case 0: sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 1: sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysdk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 2: sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysfr[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 3: sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysgr[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      case 4: sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysar[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
              break;
      default:
              sprintf(keybuffer,"%s %d/%d/%d %02d:%02d",weekdaysuk[xmlupdatelasttime->tm_wday],xmlupdatelasttime->tm_mday,xmlupdatelasttime->tm_mon+1,xmlupdatelasttime->tm_year+1900,xmlupdatelasttime->tm_hour,xmlupdatelasttime->tm_min);
    }
  } else sprintf(keybuffer,"Never.... ");
  myglprint4((char *) keybuffer);   // keybuffer
  glPopMatrix();
  //
  // show channel names to select active channel and order
  //
  glPushMatrix();
  winsizx=450;
  winsizy=280;
  xpos=300;
  ypos=150;
  glEnable(GL_TEXTURE_2D);
  glColor3f(0.2f, 0.2f, 0.2f);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,0);			// setupkeysbar1
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  glPopMatrix();
  for (int n=0;n<14;n++) {
    glPushMatrix();
    if (channel_list[(n-1)+startofset].changeordernr) glTranslatef(692 , 560-(n*20) , 0.0f); else glTranslatef(672 , 560-(n*20) , 0.0f);
    glRasterPos2f(0.0f, 0.0f);
    if ((do_show_setup_select_linie-1)==n) glColor3f(1.0f,1.0f,1.0f); else glColor3f(.7f,0.7f,0.7f);
    if (channel_list[(n-1)+startofset].selected) myglprint4((char *) "[x] "); else myglprint4((char *) "[ ] ");
    myglprint4((char *) channel_list[(n-1)+startofset].name);
    glPopMatrix();
  }
  // tv graber select line
  if (do_show_setup_select_linie==0) showcoursornow(111,500,strlen(keybuffer));
  if (do_show_setup_select_linie>0) showcoursornow(311,368-((do_show_setup_select_linie-2)*20),0);
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
  int xpos=0;
  int ypos=0;
  int tabelofset=0;
  // load setings
  if (rssstreamoversigt.streamantal()==0) {
    rssstreamoversigt.load_rss_data();
  }
  // background
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,setuptexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2)),((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(((orgwinsizex/2)-(winsizx/2))+winsizx,((orgwinsizey/2)-(winsizy/2)) , 0.0);
  glEnd();
  glPopMatrix();
  //***********************************************************************************************
  // sound setup
  // buttons
  winsizx=200;
  winsizy=200;
  xpos=200;
  ypos=500;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturesoundsetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(30);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  //***********************************************************************************************
  // buttons
  xpos=400;
  ypos=500;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturesourcesetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(31);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  //***********************************************************************************************
  // buttons
  xpos=600;
  ypos=500;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_textureimagesetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(32);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  //***********************************************************************************************
  // buttons
  xpos=800;
  ypos=500;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturetemasetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(33);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  //***********************************************************************************************
  // buttons
  xpos=200;
  ypos=300;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturemythtvsql);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(34);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  //***********************************************************************************************
  // buttons
  xpos=400;
  ypos=300;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturesetupfont);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(35);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // *************************************************************************************************
  xpos=600;
  ypos=300;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturekeyssetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(36);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();

  // setup video player button
  xpos=800;
  ypos=300;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturevideoplayersetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(38);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();

  // setup tv graber to use
  xpos=200; // 400
  ypos=100;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturetvgrabersetup);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(39);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();

  // setup rss
  xpos=400; // 600 
  ypos=100;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturekeysrss);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(42);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();

  // setup spotify
  xpos=600; // 800
  ypos=100;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturespotify);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(43);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // setup tidal
  xpos=800; // 1000
  ypos=100;
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturetidal);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(44);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
  // close button
  xpos=400;
  ypos=10;
  winsizx = 355;
  winsizy = 81;
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_texturesetupclose); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glLoadName(37);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glTexCoord2f(0, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2)),ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 1); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2))+winsizy , 0.0);
  glTexCoord2f(1, 0); glVertex3f(xpos+((orgwinsizex/2)-(1200/2))+winsizx,ypos+((orgwinsizey/2)-(800/2)) , 0.0);
  glEnd();
}
