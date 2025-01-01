#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdarg.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/Intrinsic.h>    /* Display, Window */
#include <GL/glx.h>           /* GLXContext */
#include <GL/glc.h>       		// danish ttf support
// mysql support
#include <mysql.h>
#include <sstream>
#include <iostream>
#include "text3d.h"
#include "readjpg.h"
#include "utility.h"
#include "myctrl_recorded.h"
#include "myctrl_storagedef.h"
#include "myctrl_glprint.h"

extern FILE *logfile;
extern char debuglogdata[1024];                                  // used by log system

extern int fonttype;
extern char configmysqluser[256];                              //
extern char configmysqlpass[256];                              //
extern char configmysqlhost[256];                              //
extern char configmusicpath[256];
extern char configrecordpath[256];
extern recorded_overigt recordoversigt;
extern bool reset_recorded_texture;
extern storagedef configstoragerecord[];      // storage array (for recorded programs)
extern GLuint _textureId22;    // movie open info box1
extern GLuint _textureId23;    // movie open info box2
extern GLuint _textureId24;    // movie open info box3
extern int screen_size;
extern int visvalgtnrtype;

// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

recorded_oversigt_type::recorded_oversigt_type() {
  strcpy(title,"");
  strcpy(subtitle,"");
  strcpy(recordedpath,"");
  strcpy(startdato,"");
  strcpy(enddato,"");
  strcpy(description,"");
  start_prg_image="";
}

// ****************************************************************************************
//
// ****************************************************************************************

void recorded_oversigt_type::put_recorded(char *tit,char *subtit,char *startdag,char *enddag,char *filepath,char *desc)

{
  strcpy(title,tit);
  strcpy(subtitle,subtit);
  strcpy(recordedpath,filepath);
  strcpy(startdato,startdag);
  strcpy(enddato,enddag);
  strncpy(description,desc,200);
  description[199]=0;
}

// ****************************************************************************************
//
// ****************************************************************************************

void recorded_oversigt_type::get_recorded(int recnr,char *tit,char *subtit,char *startdag,char *enddag,char *desc)

{
  strcpy(tit,title);
  strcpy(subtit,subtitle);
  strcpy(startdag,startdato);
  strcpy(enddag,enddato);
  strcpy(desc,description);
}

// ****************************************************************************************
//
// ****************************************************************************************

void recorded_oversigt_type::get_recorded_filepath(char *filepath)

{
  strcpy(filepath,recordedpath);
}

// ****************************************************************************************
//
// constructor
//
// ****************************************************************************************

recorded_top_type::recorded_top_type()

{
  strcpy(title,"");
  prg_antal=0;
}

// ****************************************************************************************
//
// ****************************************************************************************

void recorded_top_type::put_recorded_top(char *title)

{
  strcpy(this->title,title);
}


// ****************************************************************************************
//
// ****************************************************************************************

void recorded_top_type::get_recorded_top(char *title)

{
    strcpy(title,this->title);
}

// ****************************************************************************************
//
// ****************************************************************************************

void draw_ccover() {
  int zofset=0;
  int xofset=0;
  glBegin(GL_QUADS); //Begin quadrilateral coordinates
  glTexCoord2f(0.0, 0.0); glVertex3f(-3.5+xofset, -3.5, 0.0-zofset);
  glTexCoord2f(0.0, 1.0); glVertex3f(-3.5+xofset, 3.5, 0.0-zofset);
  glTexCoord2f(1.0, 1.0); glVertex3f(3.5+xofset, 3.5, 0.0-zofset);
  glTexCoord2f(1.0, 0.0); glVertex3f(3.5+xofset, -3.5, 0.0-zofset);
  glEnd(); //End quadrilateral coordinates
}


// ****************************************************************************************
//
// ****************************************************************************************

void recorded_overigt::get_recorded_filepath(char *filepath,int valgtnr,int subvalgtnr)

{
  char temptxt[255];
  programs[valgtnr].recorded_programs[subvalgtnr].get_recorded_filepath(temptxt);
  strcpy(filepath,temptxt);
}


// ****************************************************************************************
//
// ****************************************************************************************

int find_storagegroupfile(char *filename) {
    bool fundet=false;
    char filepath[512];
    int storagegrpnr=0;
    while((!(fundet)) && (storagegrpnr<storagegroupantal)) {
      strcpy(filepath,configstoragerecord[storagegrpnr].path);
      strcat(filepath,filename);
      if (file_exists(filepath)) {
        fundet=true;
        strcpy(filename,filepath);
      }
      storagegrpnr++;
    }
    if (fundet) return(1); else return(0);
}


// ****************************************************************************************
//
// ****************************************************************************************


int recorded_overigt::opdatere_recorded_oversigt() {
  char sqlselect[1024];
  char title[128];
  int n,nn;
  // mysql vars
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *database = (char *) "mythtvcontroller";                                          // old ver mythconverg
  char filename[512];
  int storagegroupused=0;
  //gotoxy(10,17);
  // Connect to database
  try {
    conn=mysql_init(NULL);
    if (conn) {
      write_logfile(logfile,(char *) "Update recorded programs from database.");
      mysql_real_connect(conn, configmysqlhost,configmysqluser, configmysqlpass, database, 0, NULL, 0);
      mysql_query(conn,"set NAMES 'utf8'");
      strcpy(sqlselect,"create table IF NOT EXISTS recorded(chanid int, starttime datetime, endtime datetime, title varchar(128), subtitle varchar(128), description text, season int,episode int ,category varchar(64), hostname varchar(255), bookmark int,editing int, cutlist int,autoexpire int, commflagged int,recgroup varchar(32), recordid int, seriesid varchar(64), inetref varchar(64), lastmodified datetime, filesize int,stars float, previouslyshown int, originalairdate date, preserve int, findid int, deletepending int, transcoder int, timestretch float, recpriority int,basename  varchar(255), progstart datetime, progend datetime, playgroup varchar(32), profile varchar(32), duplicate int, transcoded int, watched int, storagegroup varchar(32), bookmarkupdate datetime)");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      strcpy(sqlselect,"create table IF NOT EXISTS recordedprogram(chanid int(10) unsigned,starttime datetime,endtime datetime,title varchar(128),subtitle varchar(128),description text,category varchar(64),category_type varchar(64),airdate year(4),stars float unsigned,previouslyshown tinyint(4),title_pronounce varchar(128),stereo tinyint(1),subtitled tinyint(1),hdtv tinyint(1),closecaptioned tinyint(1),partnumber int,parttotal int,seriesid varchar(12),originalairdate date,showtype varchar(30),colorcode  varchar(20),syndicatedepisodenumber varchar(20),programid varchar(64),manualid int(10) unsigned,generic tinyint(1),listingsource int(11),first tinyint(1),last tinyint(1),audioprop varchar(20),subtitletypes varchar(20),videoprop varchar(20))");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);

      strcpy(sqlselect,"select title,subtitle,starttime,endtime,basename,description from recorded order by title,starttime desc");
      mysql_query(conn,sqlselect);
      res = mysql_store_result(conn);
      n=-1;
      nn=0;
      strcpy(title,"");
      if (res) {
        while ((row = mysql_fetch_row(res)) != NULL) {
          if (strcmp(title,row[0])!=0) {
            n++;
            nn=0;
            strcpy(title,row[0]);
          } else {
            if (n==-1) n++;
            nn++;
          }
          // printf("title=%s n=%d nn=%d  \n",title,n,nn);
          if (row[4]) strcpy(filename,row[4]); else strcpy(filename,"");
          storagegroupused=find_storagegroupfile(filename);						// hent storage group info
          if (storagegroupused==0) strcpy(filename,row[4]);
          if ((n<40) && (nn<200)) {
            recordoversigt.programs[n].put_recorded_top(title);
            recordoversigt.programs[n].recorded_programs[nn].put_recorded(title,row[1],row[2],row[3],filename,row[5]);
            recordoversigt.programs[n].prg_antal++;
          }
        }        	// end while
      } else {
        write_logfile(logfile,(char *) "SQL Database error.");
        n=0;
      }
      set_top_antal(n);					// sætter hvor mange der er i array
      if (n>0) {
        //printf("Fundet %d recorded programs.                                                \n",n);
      }
      mysql_close(conn);
    }
  }
  catch (...) {
    write_logfile(logfile,(char *) "Error connect to mysql..");
  }
  return(n);
}

// ****************************************************************************************
//
// bitmap font
//
// ****************************************************************************************


void myglprint3(char *string)
{
  int len,i;
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
  }
}

// ****************************************************************************************
//
// glut print func
//
// ****************************************************************************************


void stroke_output1(GLfloat x, GLfloat y, char *format,...) {
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
// In use
//
// Viser recorded programer
// valgtnr= den valgte som skal vises med mark
// subvalgtnr = den valgte som skal vises i under liste
//
// ****************************************************************************************

void recorded_overigt::show_recorded_oversigt(int valgtnr,int subvalgtnr) {
  static GLuint texture=0;
  unsigned int i=0;
  int ii,iii;
  unsigned int startofset=0;
  unsigned int substartofset=0;
  float glprintyofset=0.0f;
  float screen_zofset;
  char title[128];
  char subtitle[128];
  char startdato[40];
  char slutdato[40];
  char desc[200];			// desc
  float yofset=0.5f;
  float yof=0.0f;
  char temptxt[256];
  char temptxt1[256];
  int xpos,ypos;
  static unsigned int flipflop=0;
  if (valgtnr>10) {
    startofset=valgtnr-10;
    valgtnr=10;
  } else startofset=0;
  // box2 mask
  glTranslatef(0.0f, 0.0f,0);
  xpos=100;
  ypos=250;
  // box2
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_textureId23);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+600, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+500, ypos+600, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+500, ypos+100, 0.0);
  glEnd();
  xpos=510;
  ypos=250;
  // box2
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D,_textureId24);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+600, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+800, ypos+600, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+800, ypos+100, 0.0);
  glEnd();
  //box3 mask
  xpos=100;
  ypos=40;
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_DST_COLOR, GL_ZERO);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+300, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+1210, ypos+300, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+1210, ypos+100, 0.0);
  glEnd();
  // box3
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_ONE, GL_ONE);
  glBindTexture(GL_TEXTURE_2D,_textureId22);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(xpos+100, ypos+100, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(xpos+100, ypos+300, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(xpos+1210, ypos+300, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(xpos+1210, ypos+100, 0.0);
  glEnd();
  // main start off text in window
  while ((i<=recordoversigt.top_antal()) && (i<11)) { 				// vis max
    recordoversigt.programs[i+startofset].recorded_programs[0].get_recorded(i,title,subtitle,startdato,slutdato,desc);
    // hvis valgte
    if (i+startofset==valgtnr+startofset) {
      strcpy(temptxt,title);
      if (visvalgtnrtype==1) drawText(temptxt, 220.0f, 800.0f-(i*25.0f), 0.4f,2);
        else drawText(temptxt, 220.0f, 800.0f-(i*25.0f), 0.4f,3);
    } else {
      strcpy(temptxt,title);
      drawText(temptxt, 220.0f, 800.0f-(i*25.0f), 0.4f,1);
    }
    i++;
  }
  i=0;
  // show sub (epsiode)
  while ((i<recordoversigt.programs[valgtnr+startofset].prg_antal) && (i<11)) { 				// vis max
    recordoversigt.programs[valgtnr+startofset].recorded_programs[i+substartofset].get_recorded(i+substartofset,title,subtitle,startdato,slutdato,desc);
    strcpy(temptxt,subtitle);					// get sub title
    if (strcmp(temptxt,"")==0) strcpy(temptxt,desc);                // Hvis der ikke er nogle subtitle bruge description i stedet
    if (i+startofset==subvalgtnr+startofset) {
      drawText(temptxt, 620.0f, 800.0f-(i*25.0f), 0.4f,2);
    } else {
      drawText(temptxt, 620.0f, 800.0f-(i*25.0f), 0.4f,1);
    }
    i++;
  }
  recordoversigt.programs[valgtnr+startofset].recorded_programs[subvalgtnr+substartofset].get_recorded(subvalgtnr,title,subtitle,startdato,slutdato,desc);
  // show prg start date
  strcpy(temptxt,startdato);
  temptxt[10]=0;
  drawText("Date...:", 220.0f, 310.0f, 0.4f,1);
  drawText(temptxt, 220.0f+70, 310.0f, 0.4f,1);           // show date
  drawText("KL : ", 220.0f+190.0f, 310.0f, 0.4f,1);
  strcpy(temptxt,startdato+11);
  temptxt[5]=0;
  drawText(temptxt, 220.0f+230.0f, 310.0f, 0.4f,1);       // show kl
  static time_t tm;				// this time (now)
  struct tm *nutid;
  struct tm prgstarttid;
  struct tm prgsluttid;
  char dagsdag[40];
  tm=time(0);				// get time
  nutid=localtime(&tm);
  strftime(dagsdag, 40, "%Y-%m-%d %H:%M:%S", nutid );
  strptime(startdato,"%Y-%m-%d %H:%M:%S",&prgstarttid);
  strptime(slutdato,"%Y-%m-%d %H:%M:%S",&prgsluttid);
  prgstarttid.tm_isdst=1;
  prgsluttid.tm_isdst=1;
  time_t st=mktime(&prgstarttid);
  time_t et=mktime(&prgsluttid);
  if ((tm>st) && (tm<et)) {				// er tiden inden for nu så hvis at vi optager live nu
    flipflop++;
    if (flipflop<24) drawText("Recording now",  220.0f+290.0f, 310.0f, 0.4f,1);
    if (flipflop>48) flipflop=0;  
  }  
  // show desc
  int subtitlelength=strlen(desc);                       // get desc length
  float linof=0.0f;
  int maxWidth=70;
  int sted=0;
  int ll=0;
  std::istringstream stream(desc);
  std::string word, line;
  while ((stream >> word) && (linof>-120.0f)) {
    if (line.length() + word.length() + 1 > maxWidth) {
      drawText(line.c_str(), 220.0f+480.0f, 290.0f+linof, 0.4f,1);
      line = word;
      linof-=20.0f;
      ll++;
    } else {
      if (!line.empty()) line += ' ';
      line += word;
    }
  }
  if (!line.empty()) {
    drawText(line.c_str(), 220.0f+480.0f, 290.0f+linof, 0.4f,1);
    linof-=20.0f;
    ll++;
  }
  // show image from recorded if exist
  if (programs[valgtnr].recorded_programs->start_prg_image.length()>0) {
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D,_textureId23);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos+120, ypos+120, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos+120, ypos+260, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+310, ypos+260, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+310, ypos+120, 0.0);
    glEnd();
  }
}


